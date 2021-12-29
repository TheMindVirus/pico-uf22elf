data = b""

data += b"\x7F\x45\x4C\x46" # ELF
data += b"\x01\x02\x01" # ELF32
data += b"\x00\x00" # ABI
data += b"\x00\x00\x00\x00\x00\x00\x00" # Pad
data += b"\x00\x02\x00\x28\x00\x00\x00\x01" # Type Machine Version
data += b"\x00\x00\x00\x00" # Start Address
data += b"\x00\x00\x00\x34" # Program Header
data += b"\x00\x00\x00\x54" # Section Header
data += b"\x00\x00\x00\x00" # Flags
data += b"\x00\x34" # Size of This Header
data += b"\x00\x20" # Size of Program Header
data += b"\x00\x01" # Number of Program Headers
data += b"\x00\x28" # Size of Section Headers
data += b"\x00\x02" # Number of Section Headers
data += b"\x00\x01" # Section Index Containing Labels

data += b"\x00\x00\x00\x01" # Segment Type (LOAD)
data += b"\x00\x00\x00\x00" # Offset
data += b"\x00\x00\x00\x00" # Virtual Address
data += b"\x00\x00\x00\x00" # Physical Address
data += b"\x00\x00\x00\x00" # Size in File
data += b"\x00\x00\x00\x00" # Size in Memory
data += b"\x00\x00\x00\x00" # Flags (None)
data += b"\x00\x00\x00\x00" # Alignment

data += b"\x00\x00\x00\x01" # Section Name in STRTAB
data += b"\x00\x00\x00\x01" # PROGBITS
data += b"\x00\x00\x00\x07" # Flags (WAX)
data += b"\x00\x00\x00\x00" # Virtual Address
data += b"\x00\x00\x01\x00" # Offset in File
data += b"\x00\x00\x00\x00" # Size in File
data += b"\x00\x00\x00\x00" # Section Index
data += b"\x00\x00\x00\x00" # Extra Information
data += b"\x00\x00\x00\x00" # Alignment
data += b"\x00\x00\x00\x00" # Size of Entry or Zero

data += b"\x00\x00\x00\x07" # Section Name in STRTAB
data += b"\x00\x00\x00\x03" # STRTAB
data += b"\x00\x00\x00\x00" # Flags (None)
data += b"\x00\x00\x00\x00" # Virtual Address
data += b"\x00\x00\x00\xA4" # Offset in File
data += b"\x00\x00\x00\x0C" # Size in File
data += b"\x00\x00\x00\x00" # Section Index
data += b"\x00\x00\x00\x00" # Extra Information
data += b"\x00\x00\x00\x00" # Alignment
data += b"\x00\x00\x00\x00" # Size of Entry or Zero

data += b"\x00.text\x00.data\x00"

data += b"\x00" * (0x100 - len(data))

data += b"\x48\x65\x6C\x6C\x6F\x20\x57\x6F\x72\x6C\x64\x21\x0A\x00" # Hello World!

file = open("sample3.elf", "wb")
file.write(data)
file.close()
print("Done!")
