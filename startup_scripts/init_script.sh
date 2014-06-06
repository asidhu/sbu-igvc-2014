#!/bin/sh

rmmod xpad
modprobe uinput
modprobe joydev
xboxdrv --daemon --dbus disabled --detach --detach-kernel-driver
