#INSTALL SCRIPT
set -e
sudo echo "GET SUPER POWERS!!!"
OPENCV_LOCATION=/usr/src/opencv/
X264_LOCATION=/usr/src/x264/
V4L_LOCATION=/usr/src/v4l/
echo "Updating package manager..."
sudo apt-get update

echo "Getting Build Essentials..."
sudo apt-get -y install build-essential

echo "Getting git (if you don't already have it?)..."
sudo apt-get -y install git

echo "Getting libusb..."
sudo apt-get -y install libusb-dev

echo "Getting OpenCV requirements..."
sudo apt-get -y install cmake libgtk2.0-0 libgtk2.0-dev pkg-config python-dev python-numpy ffmpeg libavcodec-dev libavformat-dev libswscale-dev libdc1394-22 libdc1394-22-dev libjpeg8 libjpeg8-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libfaac-dev libjack-jackd2-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libsdl1.2-dev libtheora-dev libva-dev libvdpau-dev libvorbis-dev libx11-dev libxfixes-dev libxvidcore-dev texi2html yasm zlib1g-dev

echo "Getting Gstreamer..."
sudo apt-get -y install libgstreamer0.10-0 libgstreamer0.10-dev gstreamer0.10-tools gstreamer0.10-plugins-base libgstreamer-plugins-base0.10-dev gstreamer0.10-plugins-good gstreamer0.10-plugins-ugly gstreamer0.10-plugins-bad gstreamer0.10-ffmpeg


echo "Getting x264..."
sudo mkdir -p $X264_LOCATION
sudo chown $USER $X264_LOCATION
cd $X264_LOCATION
wget ftp://ftp.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-20120528-2245-stable.tar.bz2
tar xvf x264-snapshot-20120528-2245-stable.tar.bz2
cd x264-snapshot-20120528-2245-stable
./configure --enable-shared --enable-pic
make
sudo make install

echo "Getting V4L..."
sudo mkdir -p $V4L_LOCATION
sudo chown $USER $V4L_LOCATION
cd $V4L_LOCATION
wget http://www.linuxtv.org/downloads/v4l-utils/v4l-utils-0.8.8.tar.bz2
tar xvf v4l-utils-0.8.8.tar.bz2
cd v4l-utils-0.8.8
make
sudo make install
sudo apt-get -y install v4l-utils libv4l-0 libv4l-dev



echo "Installing opencv..."
sudo mkdir -p $OPENCV_LOCATION
sudo chown $USER $OPENCV_LOCATION
cd $OPENCV_LOCATION
if [ -e .git ]
then
	echo "already have opencv!"
else	
	git clone https://github.com/Itseez/opencv.git .
fi
git checkout 2.4.9 #switch to stable release
mkdir -p build
cd build 
cmake -D CMAKE_BUILD_TYPE=RELEASE .. #-D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
