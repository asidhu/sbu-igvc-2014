CAMERA_L=/dev/video1
CAMERA_R=/dev/video2

echo "Checking for cameras..."

if [ -e $CAMERA_L ]
then
	echo "Found Left Camera"
	v4l2-ctl -d$CAMERA_L --set-ctrl focus_auto=0
	v4l2-ctl -d$CAMERA_L --set-ctrl white_balance_temperature_auto=0
	v4l2-ctl -d$CAMERA_L --set-ctrl exposure_auto=3
	v4l2-ctl -d$CAMERA_L --set-ctrl exposure_absolute=5
	v4l2-ctl -d$CAMERA_L --set-ctrl white_balance_temperature=4000
	v4l2-ctl -d$CAMERA_L --set-ctrl brightness=35
else
	echo "Camera Not Found $CAMERA_L"
	exit 2
fi


if [ -e $CAMERA_R ]
then
	echo "Found Right Camera"
	v4l2-ctl -d$CAMERA_R --set-ctrl focus_auto=0
	v4l2-ctl -d$CAMERA_R --set-ctrl white_balance_temperature_auto=0
	v4l2-ctl -d$CAMERA_R --set-ctrl exposure_auto=3
	v4l2-ctl -d$CAMERA_R --set-ctrl exposure_absolute=5
	v4l2-ctl -d$CAMERA_R --set-ctrl white_balance_temperature=4000
	v4l2-ctl -d$CAMERA_R --set-ctrl brightness=35
else
	echo "Camera Not Found $CAMERA_R"
	exit 2
fi





