# build.sh - build the ems arduino code and flash it

# Project Variables
INO_NAME=ems
PROJECT_PATH=/home/pi/ems/Arduino

# Local System variables
PORT=ttyACM0
ARDUINO_PATH=/home/pi/arduino-1.6.12
PACKAGES_PATH=/home/pi/.arduino15/packages

# Arduino compile tools are here:
TOOL_PATH=$PACKAGES_PATH/arduino/tools
HARDWARE_PATH=$PACKAGES_PATH/arduino/hardware
TOOLS="-tools $ARDUINO_PATH/tools-builder -tools $ARDUINO_PATH/hardware/tools/avr -tools $PACKAGES_PATH"
HARDWARE="-hardware $HARDWARE_PATH -hardware $PACKAGES_PATH"
BUILD_PATH=$PROJECT_PATH/build
CMSIS_PATH=$PACKAGES_PATH/arduino/tools/CMSIS/4.0.0-atmel
GCC_PATH=$PACKAGES_PATH/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1
BOSSAC_PATH=$PACKAGES_PATH/arduino/tools/bossac/1.7.0
OPENOCD_PATH=$PACKAGES_PATH/arduino/tools/openocd/0.9.0-arduino
SOURCE_PATH=$PROJECT_PATH/

ARDUINO=/home/pi/arduino-1.6.12/arduino-builder
TOOLS_BUILDER=/home/pi/arduino-1.6.12/tools-builder

# make sure our build folder exists
mkdir -p $BUILD_PATH

# compile and install
$ARDUINO -dump-prefs -logger=machine $HARDWARE $TOOLS -built-in-libraries $ARDUINO_PATH/libraries -libraries $PROJECT_PATH/libraries -fqbn=adafruit:samd:adafruit_feather_m0 -vid-pid=0X239A_0X800B -ide-version=10612 -build-path $BUILD_PATH -warnings=all -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.CMSIS.path=$CMSIS_PATH -prefs=runtime.tools.arm-none-eabi-gcc.path=$GCC_PATH -prefs=runtime.tools.bossac.path=$BOSSAC_PATH -prefs=runtime.tools.openocd.path=$OPENOCD_PATH -verbose $PROJECT_PATH/$INO_NAME/$INO_NAME.ino
$ARDUINO -compile -logger=machine $HARDWARE $TOOLS -built-in-libraries $ARDUINO_PATH/libraries -libraries $PROJECT_PATH/libraries -fqbn=adafruit:samd:adafruit_feather_m0 -vid-pid=0X239A_0X800B -ide-version=10612 -build-path $BUILD_PATH -warnings=all -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.CMSIS.path=$CMSIS_PATH -prefs=runtime.tools.arm-none-eabi-gcc.path=$GCC_PATH -prefs=runtime.tools.bossac.path=$BOSSAC_PATH -prefs=runtime.tools.openocd.path=$OPENOCD_PATH -verbose $PROJECT_PATH/$INO_NAME/$INO_NAME.ino

# load to the feather if possible
$BOSSAC_PATH/bossac -i -d --port=$PORT -U true -i -e -w -v $BUILD_PATH/$INO_NAME.ino.bin -R 

