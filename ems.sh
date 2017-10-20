# sesh.sh - rpi startup script for ems
#    To Activate: 1) Edit the file ~/.config/lxsession/LXDE-pi/autostart
#                 2) Add the path to the script to the top of the file as in the line below:
#
#                        @/home/pi/ems/ems.sh
#
#                 3) Save the file and restart
#
xinput --set-prop 'Microchip Technology Inc. AR1100 HID-MOUSE' 'Evdev Axes Swap' 0
xinput --set-prop 'Microchip Technology Inc. AR1100 HID-MOUSE' 'Evdev Axis Inversion' 1 1


cd /home/pi/ems
export PYTHONPATH=/home/pi/ems:$PYTHONPATH
python EMSConsole/EMSConsole.py >> /home/pi/ems/ems.log 2>&1 &
