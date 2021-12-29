# pico-uf22elf
WIP reverse codec of elf2uf2 back-converting uf2 files to compressed elf files for memory usage analysis

#### Approach Notes for elf2uf2 and uf22elf

```
[elf2uf2]
 * Read the ELF header and detect whether this is a RAM binary or a Flash binary (???)
 * Check the Program Header Table (PH Table Entries) for their address ranges if it has any
 * Do some manual platform specific error correction and bug fixing on the image
 * Fail early if slight incompatibility detected, otherwise write the magic section to UF2
[uf22elf]
 * Read the UF2 and treat the entire contents as the .text GNU Section in the ELF
 * Optionally pick out strings and place them in the .data GNU Section in the ELF
```

#### Workspace in Visual Studio Code

![workspace](https://github.com/TheMindVirus/pico-uf22elf/blob/main/workspace.png)

#### Example Analysis of ELF files in 7zip

![stat](https://github.com/TheMindVirus/pico-uf22elf/blob/main/stat.png)

**Top Left:** avr8 LUFA USB-to-Serial with micropython embedded \
**Bottom Left:** avr8 LUFA USB-to-Serial (no python, fits in 16KB) \
**Middle:** Micropython Debian x64 Unix Minimal Port \
**Right:** CircuitPython 7.0.x Adafruit Macropad RP2040 Port

#### Sample Build Instructions for CircuitPython 7.0.x
```
sudo apt-get install git build-essential make gettext python3 python3-pip python3-jinja2
sudo pip3 install --upgrade pip
sudo pip3 install huffman
sudo git clone https://github.com/adafruit/circuitpython cp7
cd cp7
sudo git checkout 7.0.x
sudo git submodule init
sudo git submodule update --recursive
sudo make submodules BOARD=adafruit_macropad_rp2040
sudo make BOARD=adafruit_macropad_rp2040
cd build-adafruit_macropad_rp2040
sudo cp -rf ./* /mnt/d/cpy-debian-macropad/ #from Debian WSL
```
