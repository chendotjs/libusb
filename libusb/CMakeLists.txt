CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)
PROJECT(libusb)
SET(CMAKE_CXX_STANDARD 11)
INCLUDE_DIRECTORIES(..)
INCLUDE_DIRECTORIES(.)

SET(DIR_LIB_SRCS
  core.c
  descriptor.c
  hotplug.c
  io.c
  strerror.c
  sync.c
)

SET(LINUX_SRC_LIST
  os/poll_posix.c
  os/threads_posix.c
  os/linux_usbfs.c
  os/linux_udev.c
)

message("src: ${DIR_LIB_SRCS} ${LINUX_SRC_LIST}")

ADD_LIBRARY(usb SHARED ${DIR_LIB_SRCS} ${LINUX_SRC_LIST})
target_link_libraries(usb
  PRIVATE pthread
  PRIVATE udev
)
