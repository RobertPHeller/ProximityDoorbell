#!/bin/bash
dirname=`dirname $0`
BUILDPATH="$dirname/BUILD"
if [ ! -d $BUILDPATH ] ; then
  mkdir -p $BUILDPATH
fi
CACHEPATH="$dirname/CACHE"
if [ ! -d $CACHEPATH ] ; then
  mkdir -p $CACHEPATH
fi
ino=$dirname/ProximityDoorbell.ino
ide=`which arduino`
realide=`readlink -f $ide`
ARDUINOINSTALLDIR=`dirname $realide`
python $HOME/.arduino15/packages/esp32/tools/esptool_py/3.3.0/esptool.py \
    --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset \
    --after hard_reset write_flash -z --flash_mode dio \
    --flash_freq 80m --flash_size detect 0xe000 \
    $HOME/.arduino15/packages/esp32/hardware/esp32/*/tools/partitions/boot_app0.bin \
    0x1000 \
    $HOME/.arduino15/packages/esp32/hardware/esp32/*/tools/sdk/esp32/bin/bootloader_qio_80m.bin \
    0x10000 $BUILDPATH/`basename $ino`.bin \
    0x8000 \
    $BUILDPATH/`basename $ino`.partitions.bin
