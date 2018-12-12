CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)
PROJECT(libusb_parent)
SET(CMAKE_CXX_STANDARD 11)
## \ref https://cmake.org/Wiki/CMake_RPATH_handling
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)


ADD_SUBDIRECTORY(libusb)
ADD_SUBDIRECTORY(examples)
