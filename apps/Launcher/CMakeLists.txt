
# Copyright (c) 2013-2015, EPFL/Blue Brain Project
#                     Ahmet Bilgili <ahmet.bilgili@epfl.ch>

set(LAUNCHER_MOC_HEADERS
  Launcher.h
  FileBrowser.h)

set(LAUNCHER_SOURCES
  main.cpp
  Launcher.cpp
  FileBrowser.cpp
  ImageProvider.cpp)

set(LAUNCHER_RESOURCES
  resources.qrc)

set(LAUNCHER_LINK_LIBRARIES dccore DeflectQt)

common_application(launcher)
