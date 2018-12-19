CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)
PROJECT(libusb_example)
SET(CMAKE_CXX_STANDARD 11)
INCLUDE_DIRECTORIES(../libusb)

# find_library(USB_LIBRARY usb HINTS libusb)
add_executable(usb_pixelx usb_example.cc)
target_link_libraries(usb_pixelx
  PRIVATE usb
)

add_executable(xusb xusb.c)
target_link_libraries(xusb
  PRIVATE usb
)