
# Copyright (c) 2013-2015, EPFL/Blue Brain Project
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>

include_directories(${PROJECT_BINARY_DIR}) ### for config.h ###

### for override on pre-C++11 compilers ###
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI ${PROJECT_BINARY_DIR}/config.h")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include config.h")
endif()

set(DISPLAYCLUSTER_MOC_HEADERS
  BackgroundWidget.h
  DisplayGroupView.h
  DisplayGroupListWidget.h
  MasterApplication.h
  MasterWindow.h
  WallApplication.h
  WebbrowserWidget.h
)

set(DISPLAYCLUSTER_SOURCES
  main.cpp
  BackgroundWidget.cpp
  CommandLineParameters.cpp
  ContentWindowListWidgetItem.cpp
  DisplayGroupView.cpp
  DisplayGroupListWidget.cpp
  MasterApplication.cpp
  MasterWindow.cpp
  WallApplication.cpp
  WebbrowserWidget.cpp
)

set(DISPLAYCLUSTER_LINK_LIBRARIES dccore dcwebservice)

if(ENABLE_TUIO_TOUCH_LISTENER)
  list(APPEND DISPLAYCLUSTER_MOC_HEADERS MultiTouchListener.h)
  list(APPEND DISPLAYCLUSTER_SOURCES MultiTouchListener.cpp)
  list(APPEND DISPLAYCLUSTER_LINK_LIBRARIES ${X11_LIBRARIES} TUIO)
endif()

set(DISPLAYCLUSTER_RESOURCES resources/displaycluster.qrc)

common_application(displaycluster)
install(PROGRAMS startdisplaycluster DESTINATION bin COMPONENT apps)

# Copy the startup script and default config to the bin folder to be able to
# launch after building without requiring an install folder and 'make install'
file(COPY startdisplaycluster DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${PROJECT_SOURCE_DIR}/examples/configuration_1x3.xml
  DESTINATION ${CMAKE_BINARY_DIR}/share/DisplayCluster/examples
)
