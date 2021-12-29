#https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

file = open("firmware.uf2", "rb")
text = file.read()
file.close()

elf_header = b""

elf_header += b"\x7F\x45\x4C\x46" #.ELF
elf_header += b"\x02\x01\x01" #ELF64
elf_header += b"\x00\x00" #ABI Version
elf_header += b"\x00\x00\x00\x00\x00\x00\x00" #Pad
elf_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Type Machine Version
elf_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Start Address (64-bit)
elf_header += b"\x00\x00\x00\x00\x00\x00\x00\x40" #Start of Program Header (PH Table)
elf_header += b"\x00\x00\x00\x00\x00\x00\x00\x78" #Start of Section Header (SH Table)
elf_header += b"\x00\x00\x00\x00" #Flags
elf_header += b"\x00\x40" #Size of This Header (64 bytes for 64-bit)
elf_header += b"\x00\x38" #Size of Program Header (PH Table) Entry
elf_header += b"\x00\x01" #Number of Program Header (PH Table) Entries
elf_header += b"\x00\x40" #Size of Section Header (SH Table) Entry
elf_header += b"\x00\x02" #Number of Section Header (SH Table) Entries
elf_header += b"\x00\x00" #Section Header (SH Table) Index containing Section Names

program_header = b""

program_header += b"\x00\x00\x00\x01" #Segment Type (LOAD)
program_header += b"\x00\x00\x00\x00" #Flags (64-bit)
program_header += b"\x00\x00\x00\x00\x00\x00\x01\x00" #Offset in File
program_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Virtual Address
program_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Physical Address
program_header += b"\x00\x00\x00\x00\x00\x00\x01\x00" #Size in File
program_header += b"\x00\x00\x00\x00\x00\x00\x01\x00" #Size in Memory
program_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Alignment

section_header = b""

section_header += b"\x00\x00\x00\x00" #Section Name in .data
section_header += b"\x00\x00\x00\x01" #PROGBITS
section_header += b"\x00\x00\x00\x00\x00\x00\x01\x05" #Non-Standard Writable Executable
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Virtual Address in Memory
section_header += b"\x00\x00\x00\x00\x00\x00\x01\x00" #Offset in File
section_header += b"\x00\x00\x00\x00\x00\x00\x01\x00" #Size in File
section_header += b"\x00\x00\x00\x00" #Section Index
section_header += b"\x00\x00\x00\x00" #Extra Information
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Alignment
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Size of Entry or Zero

section_header += b"\x00\x00\x00\x06" #Section Name in .data
section_header += b"\x00\x00\x00\x03" #STRTAB
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x32" #Merged Allocated Null Terminated Information Strings
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Virtual Address in Memory
section_header += b"\x00\x00\x00\x00\x00\x00\x00\xF8" #Offset in File
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x0C" #Size in File
section_header += b"\x00\x00\x00\x00" #Section Index
section_header += b"\x00\x00\x00\x00" #Extra Information
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Alignment
section_header += b"\x00\x00\x00\x00\x00\x00\x00\x00" #Size of Entry or Zero

strtab_header = b".text\x00.data\x00"

headers = elf_header + program_header + section_header + strtab_header
pad = b"\x00" * (256 - len(headers))

data = headers + pad + text

file = open("firmware.elf", "wb")
file.write(data)
file.close()

print("Done!")
