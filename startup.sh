CAMERA_L=/dev/video0
CAMERA_R=/dev/video1

echo "Checking for cameras..."

if [ -e $CAMERA_L ]
then
	echo "Found Left Camera"
	v4l2-ctl -d$CAMERA_L --set-ctrl focus_auto=0
else
	echo "Camera Not Found $CAMERA_L"
	exit 2
fi


if [ -e $CAMERA_R ]
then
	echo "Found Right Camera"
	v4l2-ctl -d$CAMERA_R --set-ctrl focus_auto=0
else
	echo "Camera Not Found $CAMERA_R"
	exit 2
fi



# run the file

./robot


