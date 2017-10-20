Instructions for loading compiled binary images 
	from a raspberry pi to a feather M0.

First, update your pi
sudo apt-get update && apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install libreadline-dev wx2.8-headers libwxgtk2.8-0 libwxgtk2.8-dev

next visit the page https://github.com/shumatech/BOSSA.  
	clone this repo and note the folder name.

next follow the instructions on the page to make the bossac and bossash files

next create a script some thing like what is below:

-----------------------------------------------------
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
$BOSSAC_PATH/bossac -i -d --port=$PORT -U true -i -e -w -v $BUILD_PATH/$INO_NAME.ino.bin -R 
-------------------------------------------------------

Note that if you do not know the port of the device, you can remove the --port=PORT and bossac will attempt to find the port.



