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

ide=`which arduino`
realide=`readlink -f $ide`
ARDUINOINSTALLDIR=`dirname $realide`
$ARDUINOINSTALLDIR/arduino-builder -compile \
    -hardware $ARDUINOINSTALLDIR/hardware \
    -hardware $HOME/.arduino15/packages \
    -tools $ARDUINOINSTALLDIR/tools-builder \
    -tools $HOME/.arduino15/packages \
    -built-in-libraries $ARDUINOINSTALLDIR/libraries \
    -libraries $HOME/Arduino/libraries \
    -fqbn=esp32:esp32:ttgo-t1:PartitionScheme=default,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=4M,UploadSpeed=921600,DebugLevel=none \
    -build-path $BUILDPATH \
    -warnings=none \
    -build-cache $CACHEPATH \
    -prefs=build.warn_data_percentage=75 \
    -prefs=runtime.tools.esptool_py.path=$HOME/.arduino15/packages/esp32/tools/esptool_py/3.3.0 \
    -prefs=runtime.tools.esptool_py-3.3.0.path=$HOME/.arduino15/packages/esp32/tools/esptool_py/3.3.0 \
    -prefs=runtime.tools.mkspiffs.path=$HOME/.arduino15/packages/esp32/tools/mkspiffs/0.2.3 \
    -prefs=runtime.tools.mkspiffs-0.2.3.path=$HOME/.arduino15/packages/esp32/tools/mkspiffs/0.2.3 \
    -prefs=runtime.tools.xtensa-esp32-elf-gcc.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3 \
    -prefs=runtime.tools.xtensa-esp32-elf-gcc-gcc8_4_0-esp-2021r2-patch3.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r2-patch3 \
    -verbose $dirname/ProximityDoorbell.ino
