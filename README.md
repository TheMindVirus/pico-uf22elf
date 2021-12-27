# pico-uf22elf
WIP reverse codec of elf2uf2 back-converting uf2 files to compressed elf files for memory usage analysis

#### Example Analysis of ELF files in 7zip

![stat](https://github.com/TheMindVirus/pico-uf22elf/blob/main/stat.png)

Top Left: avr8 LUFA USB-to-Serial with micropython embedded
Bottom Left: avr8 LUFA USB-to-Serial (no python, fits in 16KB)
Middle: Micropython Debian x64 Unix Minimal Port
Right: CircuitPython 7.0.x Adafruit Macropad RP2040 Port

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