# load.sh - flash your arduino using a locally installed bosac

# Project Variables
INO_NAME=ems
PROJECT_PATH=/home/pi/ems/Arduino

# Local System variables
PORT=ttyACM0
PACKAGES_PATH=/home/pi/BOSSA/bin
BUILD_PATH=$PROJECT_PATH/build

# Arduino compile tools are here:
BOSSAC_PATH=$PACKAGES_PATH

# load to the feather if possible
#$BOSSAC_PATH/bossac -i -d -U true -i -e -w -v $BUILD_PATH/$INO_NAME.ino.bin -R 
$BOSSAC_PATH/bossac -i -d --port=$PORT -U true -i -e -w -v $BUILD_PATH/$INO_NAME.ino.bin -R 

