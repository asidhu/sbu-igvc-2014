#!/bin/sh
sudo killall xboxdrv
sudo rmmod xpad
sudo modprobe uinput
sudo modprobe joydev
sudo xboxdrv --daemon --dbus disabled --detach --detach-kernel-driver
