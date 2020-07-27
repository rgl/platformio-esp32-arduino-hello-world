![Build](https://github.com/rgl/platformio-esp32-arduino-hello-world/workflows/Build/badge.svg)

[AZ-Delivery ESP-32 Dev Kit C V4 board](https://www.az-delivery.de/en/products/esp-32-dev-kit-c-v4) Hello World

This board connects to the computer as a USB serial device, as such, you should add your user to the `dialout` group:

```bash
# add ourselfs to the dialout group to be able to write to the USB serial
# device without root permissions.
sudo usermod -a -G dialout $USERNAME
# enter a new shell with the dialout group enabled.
# NB to make this permanent its easier to reboot the system with sudo reboot
#    or to logout/login your desktop.
newgrp dialout
```

Create a python virtual environment in the current directory:

```bash
sudo apt-get install -y --no-install-recommends python3-pip python3-venv
python3 -m venv venv
```

Install [esptool](https://github.com/espressif/esptool):

```bash
source ./venv/bin/activate
# NB this pip install will display several "error: invalid command 'bdist_wheel'"
#    messages, those can be ignored.
# see https://pypi.org/project/esptool/
# see https://github.com/espressif/esptool
# TODO use a requirements.txt file and then python3 -m pip install -r requirements.txt
esptool_version='2.8'
python3 -m pip install esptool==$esptool_version
esptool.py version
```

Dump the information about the esp attached to the serial port:

```bash
esptool.py -p /dev/ttyUSB0 chip_id
```

You should see something alike:

```plain
esptool.py v2.8
Serial port /dev/ttyUSB0
Connecting........___
Detecting chip type... ESP32
Chip is ESP32D0WDQ6 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 24:62:ab:e0:29:3c
Uploading stub...
Running stub...
Stub running...
Warning: ESP32 has no Chip ID. Reading MAC instead.
MAC: 24:62:ab:e0:29:3c
Hard resetting via RTS pin...
```

Dump the information about the flash:

```bash
esptool.py -p /dev/ttyUSB0 flash_id
```

You should see something alike:

```plain
esptool.py v2.8
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... ESP32
Chip is ESP32D0WDQ6 (revision 1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: 24:62:ab:e0:29:3c
Uploading stub...
Running stub...
Stub running...
Manufacturer: 5e
Device: 4016
Detected flash size: 4MB
Hard resetting via RTS pin...
```

This flash device Manufacturer and Device ID is described at [coreboot flashrom repository](https://review.coreboot.org/cgit/flashrom.git/tree/flashchips.h), in this case, its:

```c
#define WINBOND_NEX_ID          0xEF    /* Winbond (ex Nexcom) serial flashes */
#define WINBOND_NEX_W25Q32_V	0x4016	/* W25Q32BV; W25Q32FV in SPI mode (default) */
```

This SPI flash supports QPI mode and as such we can use the qio flashing mode.

## platformio

Install platformio:

```bash
# active our isolated python environment.
source ./venv/bin/activate
# install platformio.
# see https://docs.platformio.org/en/latest/core/installation.html
# see https://docs.platformio.org/en/latest/projectconf/section_platformio.html#core-dir
# see https://pypi.org/project/platformio/
export PLATFORMIO_CORE_DIR="$VIRTUAL_ENV/platformio"
python3 -m pip install platformio==4.3.4
platformio --version
```

List the available boards:

```bash
platformio boards
```

Then search for `az-delivery`, you should see something alike:

```
Platform: espressif32
================================================================================
ID                               MCU    Frequency    Flash    RAM     Name
-------------------------------  -----  -----------  -------  ------  --------------------------------
az-delivery-devkit-v4            ESP32  240MHz       16MB     520KB   AZ-Delivery ESP-32 Dev Kit C V4
```

Create the project skeleton:

```bash
platformio \
  project init \
  --board az-delivery-devkit-v4 \
  --project-option 'platform=espressif32' \
  --project-option 'framework=arduino' \
  --project-option 'board_build.flash_mode=qio' \
  --project-option 'monitor_speed=115200'
```

For more information see the PlatformIO documentation:

* https://docs.platformio.org/en/latest/core/quickstart.html
* https://docs.platformio.org/en/latest/platforms/espressif32.html
* https://docs.platformio.org/en/latest/boards/espressif32/az-delivery-devkit-v4.html

Add the hello world source code:

```bash
cat >src/main.cpp <<'EOF'
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.printf("%lu Hello World!\r\n", millis()/1000);
  delay(1000);
}
EOF
```

Build the project:

```bash
platformio run
```

Flash it into the board:

```bash
firmware_image=.pio/build/az-delivery-devkit-v4/firmware.bin
ls -laF $firmware_image
esptool.py --chip esp32 image_info $firmware_image
# flash the device.
platformio run --target upload --upload-port /dev/ttyUSB0
```

Automatically build the firmware in a GitHub Actions Workflow:

See [build.yml](.github/workflows/build.yml).
