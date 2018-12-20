#!/bin/bash
FLASH_DISK=$1

if [ ! -b $FLASH_DISK ];then
  echo $1" is not a usb flash disk...exit..."
  exit -1
fi

wget https://raw.githubusercontent.com/git/git/master/Documentation/user-manual.txt -O git-user-manual.txt
sudo dd if=/dev/zero of=$FLASH_DISK bs=1M count=10 status=progress # clear flash disk
sudo dd if=git-user-manual.txt of=$FLASH_DISK status=progress

