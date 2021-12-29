#include <cstdio>
#include <map>
#include <vector>
#include <cstring>
#include <cstdarg>
#include <algorithm>
#include "elf.h"
#include "uf2.h"

#define LOG2_PAGE_SIZE 8u
#define PAGE_SIZE (1u << LOG2_PAGE_SIZE)

static char error_msg[512] = {};
static bool verbose = false;
typedef unsigned int uint;

//https://github.com/raspberrypi/pico-sdk/blob/master/tools/elf2uf2/main.cpp
//https://github.com/microsoft/uf2/tree/master/utils

int usage();
int uf22elf(FILE* in, FILE* out);

int main(int argc, char **argv)
{
    int arg = 1;
    if (arg < argc && !strcmp(argv[arg], "-v")) { verbose = true; ++arg; }
    if (argc < arg + 2) { return usage(); }

    const char* in_filename = argv[arg++];
    FILE* in = fopen(in_filename, "rb");
    if (!in)
    {
        fprintf(stderr, "Error: Can't open input file '%s'\n", in_filename);
        return -1;
    }

    const char* out_filename = argv[arg++];
    FILE* out = fopen(out_filename, "wb");
    if (!out)
    {
        fprintf(stderr, "Error: Can't open output file '%s'\n", out_filename);
        return -1;
    }

    int rc = uf22elf(in, out);
    fclose(in);
    fclose(out);
    if (rc)
    {
        remove(out_filename);
        if (error_msg[0]) { fprintf(stderr, "Error: %s\n", error_msg); }
    }
    return rc;
}

int usage()
{
    fprintf(stderr, "Usage: uf22elf (-v) <input UF2 file> <output ELF file>\n");
    return -1;
}

int uf22elf(FILE* in, FILE* out)
{
    if (verbose) { fprintf(stderr, "Warning: Partially Implemented\n"); }

    //fread();
    //fwrite();

    if (verbose) { printf("Info: Done!"); }
    return 0;
}
/*
struct address_range
{
    enum type
    {
        CONTENTS,     // may have contents
        NO_CONTENTS,  // must be uninitialized
        IGNORE        // will be ignored
    };
    address_range(uint32_t from, uint32_t to, type type) : from(from), to(to), type(type) {}
    address_range() : address_range(0, 0, IGNORE) {}
    type type;
    uint32_t to;
    uint32_t from;
};

typedef std::vector<address_range> address_ranges;

#define MAIN_RAM_START        0x20000000u
#define MAIN_RAM_END          0x20042000u
#define FLASH_START           0x10000000u
#define FLASH_END             0x15000000u
#define XIP_SRAM_START        0x15000000u
#define XIP_SRAM_END          0x15004000u
#define MAIN_RAM_BANKED_START 0x21000000u
#define MAIN_RAM_BANKED_END   0x21040000u

const address_ranges rp2040_address_ranges_flash
{
    address_range(FLASH_START, FLASH_END, address_range::type::CONTENTS),
    address_range(MAIN_RAM_START, MAIN_RAM_END, address_range::type::NO_CONTENTS),
    address_range(MAIN_RAM_BANKED_START, MAIN_RAM_BANKED_END, address_range::type::NO_CONTENTS)
};

const address_ranges rp2040_address_ranges_ram
{
    address_range(MAIN_RAM_START, MAIN_RAM_END, address_range::type::CONTENTS),
    address_range(XIP_SRAM_START, XIP_SRAM_END, address_range::type::CONTENTS),
    address_range(0x00000000u, 0x00004000u, address_range::type::IGNORE) // for now we ignore the bootrom if present
};

struct page_fragment
{
    page_fragment(uint32_t file_offset, uint32_t page_offset, uint32_t bytes) : file_offset(file_offset), page_offset(page_offset), bytes(bytes) {}
    uint32_t file_offset;
    uint32_t page_offset;
    uint32_t bytes;
};

//This will have to be reversed into a uf2 read function
static int read_and_check_uf2(FILE* in, elf32_header& eh_out)
{
    if (1 != fread(&eh_out, sizeof(eh_out), 1, in))
    {
        fprintf(stderr, "Unable to read UF2\n");
        return -1;
    }
    if (eh_out.common.magic != ELF_MAGIC)
    {
        fprintf(stderr, "Not an UF2 file\n");
        return -1;
    }
    if (eh_out.common.version != 1 || eh_out.common.version2 != 1)
    {
        fprintf(stderr, "Unrecognised UF2 version\n");
        return -1;
    }
    if (eh_out.common.arch_class != 1 || eh_out.common.endianness != 1)
    {
        fprintf(stderr, "Require 32 bit little-endian UF2\n");
        return -1;
    }
    if (eh_out.eh_size != sizeof(struct elf32_header))
    {
        fprintf(stderr, "Invalid UF2 format\n");
        return -1;
    }
    if (eh_out.common.machine != EM_ARM)
    {
        fprintf(stderr, "Not an ARM executable\n");
        return -1;
    }
    if (eh_out.common.abi != 0)
    {
        fprintf(stderr, "UnrecognisedABI\n");
        return -1;
    }
    if (eh_out.flags & EF_ARM_ABI_FLOAT_HARD)
    {
        fprintf(stderr, "HARD-FLOAT not supported\n");
        return -1;
    }
    return 0;
}

int check_address_range(const address_ranges& valid_ranges, uint32_t addr, uint32_t vaddr, uint32_t size, bool uninitialized, address_range &ar)
{
    for (const auto& range : valid_ranges)
    {
        if ((range.from <= addr) && (range.to >= (addr + size)))
        {
            if ((range.type == address_range::type::NO_CONTENTS) && (!uninitialized))
            {
                fprintf(stderr, "UF2 contains memory contents for uninitialized memory\n");
                return -1;
            }
            ar = range;
            if (verbose)
            {
                printf("%s segment %08x->%08x (%08x->%08x)\n",
                    uninitialized ? "Uninitialized" : "Mapped", addr,
                    addr + size, vaddr, vaddr+size);
            }
            return 0;
        }
    }
    fprintf(stderr, "Memory segment %08x->%08x is outside of valid address range for device\n", addr, addr + size);
    return -1;
}

//Same for this
int read_and_check_uf2_ph_entries(FILE* in, const elf32_header &eh, const address_ranges& valid_ranges, std::map<uint32_t, std::vector<page_fragment>>& pages)
{
    if (eh.ph_entry_size != sizeof(elf32_ph_entry))
    {
        fprintf(stderr, "Invalid UF2 program header\n");
        return -1;
    }
    if (eh.ph_num)
    {
        std::vector<elf32_ph_entry> entries(eh.ph_num);
        if ((fseek(in, eh.ph_offset, SEEK_SET))
        ||  (eh.ph_num != fread(&entries[0], sizeof(struct elf32_ph_entry), eh.ph_num, in)))
        {
            fprintf(stderr, "fail read error\n");
            return -1;
        }
        for (uint i = 0 ; i < eh.ph_num; ++i)
        {
            elf32_ph_entry& entry = entries[i];
            if (entry.type == PT_LOAD && entry.memsz)
            {
                address_range ar;
                int rc;
                uint mapped_size = std::min(entry.filez, entry.memsz);
                if (mapped_size)
                {
                    rc = check_address_range(valid_ranges, entry.paddr, entry.vaddr, mapped_size, false, ar);
                    if (rc) { return rc; }
                    // we don't download uninitialized, generally it is BSS and should be zero-ed by crt0.S, or it may be COPY areas which are undefined
                    if (ar.type != address_range::type::CONTENTS)
                    {
                        if (verbose) { printf("  ignored\n"); }
                        continue;
                    }
                    uint addr = entry.paddr;
                    uint remaining = mapped_size;
                    uint file_offset = entry.offset;
                    while (remaining)
                    {
                        uint off = addr & (PAGE_SIZE - 1);
                        uint len = std::min(remaining, PAGE_SIZE - off);
                        auto &fragments = pages[addr - off]; // list of fragments
                        // note if filesz is zero, we want zero init which is handled because the
                        // statement above creates an empty page fragment list
                        // check overlap with any existing fragments
                        for (const auto &fragment : fragments)
                        {
                            if ((off < fragment.page_offset + fragment.bytes)
                            != ((off + len) <= fragment.page_offset))
                            {
                                fprintf(stderr, "In memory segments overlap\n");
                            }
                        }
                        fragments.push_back(page_fragment{ file_offset, off, len});
                        addr += len;
                        file_offset += len;
                        remaining -= len;
                    }
                }
                if (entry.memsz > entry.filez)
                {
                    // we have some uninitialized data too
                    rc = check_address_range(valid_ranges, entry.paddr + entry.filez, entry.vaddr + entry.filez, entry.memsz - entry.filez, true, ar);
                    if (rc) { return rc; }
                }
            }
        }
    }
    return 0;
}

int realize_page(FILE* in, const std::vector<page_fragment> &fragments, uint8_t* buf, uint buf_len)
{
    assert(buf_len >= PAGE_SIZE);
    for (auto& frag : fragments)
    {
        assert(frag.page_offset >= 0);
        assert(frag.page_offset < PAGE_SIZE);
        assert(frag.page_offset + frag.bytes <= PAGE_SIZE);
        if ((fseek(in, frag.file_offset, SEEK_SET))
        ||  (1 != fread(buf + frag.page_offset, frag.bytes, 1, in)))
        {
            fprintf(stderr, "fail read error\n");
            return -1;
        }
    }
    return 0;
}

static bool is_address_valid(const address_ranges& valid_ranges, uint32_t addr)
{
    for (const auto& range : valid_ranges)
    {
        if (range.from <= addr && range.to > addr) { return true; }
    }
    return false;
}

static bool is_address_initialized(const address_ranges& valid_ranges, uint32_t addr)
{
    for (const auto& range : valid_ranges)
    {
        if ((range.from <= addr)
        &&  (range.to > addr))
        {
            return address_range::type::CONTENTS == range.type;
        }
    }
    return false;
}

static bool is_address_mapped(const std::map<uint32_t, std::vector<page_fragment>>& pages, uint32_t addr)
{
    uint32_t page = addr & ~(PAGE_SIZE - 1);
    if (!pages.count(page)) { return false; }
    // todo check actual address within page
    return true;
}

//And this...
int elf2uf2(FILE *in, FILE *out)
{
    elf32_header eh;
    std::map<uint32_t, std::vector<page_fragment>> pages;
    int rc = read_and_check_uf2(in, eh);
    bool ram_style = false;
    address_ranges valid_ranges = {};
    if (!rc)
    {
        ram_style = is_address_initialized(rp2040_address_ranges_ram, eh.entry);
        if (verbose)
        {
            if (ram_style) { printf("Detected RAM binary\n"); }
            else { printf("Detected FLASH binary\n"); }
        }
        valid_ranges = ram_style ? rp2040_address_ranges_ram : rp2040_address_ranges_flash;
        rc = read_and_check_uf2_ph_entries(in, eh, valid_ranges, pages);
    }
    if (rc) { return rc; }
    if (pages.empty())
    {
        fprintf(stderr, "The input file has no memory pages\n");
        return -1;
    }
    uint page_num = 0;
    if (ram_style)
    {
        uint32_t expected_ep_main_ram = UINT32_MAX;
        uint32_t expected_ep_xip_sram = UINT32_MAX;
        for (auto& page_entry : pages)
        {
            if (((page_entry.first >= MAIN_RAM_START) && (page_entry.first < MAIN_RAM_END))
            &&   (page_entry.first < expected_ep_main_ram))
            {
                expected_ep_main_ram = page_entry.first | 0x1;
            }
            else if (((page_entry.first >= XIP_SRAM_START) && (page_entry.first < XIP_SRAM_END))
            &&   (page_entry.first < expected_ep_xip_sram))
            {
                expected_ep_xip_sram = page_entry.first | 0x1;
            }
        }
        uint32_t expected_ep = (UINT32_MAX != expected_ep_main_ram) ? expected_ep_main_ram : expected_ep_xip_sram;
        if (eh.entry == expected_ep_xip_sram)
        {
            fprintf(stderr, "B0/B1 Boot ROM does not support direct entry into XIP_SRAM\n");
            return -1;
        }
        else if (eh.entry != expected_ep)
        {
            fprintf(stderr, "A RAM binary should have an entry point at the beginning: %08x (not %08x)\n", expected_ep, eh.entry);
            return -1;
        }
        static_assert(0 == (MAIN_RAM_START & (PAGE_SIZE - 1)), "");
        // currently don't require this as entry point is now at the start, we don't know where reset vector is
    }
    uf2_block block;
    block.magic_start0 = UF2_MAGIC_START0;
    block.magic_start1 = UF2_MAGIC_START1;
    block.flags = UF2_FLAG_FAMILY_ID_PRESENT;
    block.payload_size = PAGE_SIZE;
    block.num_blocks = (uint32_t)pages.size();
    block.file_size = RP2040_FAMILY_ID;
    block.magic_end = UF2_MAGIC_END;
    for (auto& page_entry : pages)
    {
        block.target_addr = page_entry.first;
        block.block_no = page_num++;
        if (verbose) { printf("Page %d / %d %08x\n", block.block_no, block.num_blocks, block.target_addr); }
        memset(block.data, 0, sizeof(block.data));
        rc = realize_page(in, page_entry.second, block.data, sizeof(block.data));
        if (rc) { return rc; }
        if (1 != fwrite(&block, sizeof(uf2_block), 1, out))
        {
            fprintf(stderr, "fail write error\n");
            return -1;
        }
    }
    return 0;
}
*/