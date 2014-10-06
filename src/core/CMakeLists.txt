
# Copyright (c) 2013-2014, EPFL/Blue Brain Project
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})

set(DCCORE_LINK_LIBRARIES dcwebservice
  ${Boost_LIBRARIES}
  ${QT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${FFMPEG_LIBRARIES}
  ${MPI_CXX_LIBRARIES}
  ${LibJpegTurbo_LIBRARIES}
)

if(ENABLE_TUIO_TOUCH_LISTENER)
  list(APPEND DCCORE_MOC_HEADERS MultiTouchListener.h)
  list(APPEND DCCORE_SOURCES MultiTouchListener.cpp)
  list(APPEND DCCORE_LINK_LIBRARIES ${TUIO_LIBRARIES})

  find_package(X11 REQUIRED)
  include_directories(${X11_INCLUDE_DIR})
  list(APPEND DCCORE_LINK_LIBRARIES ${X11_LIBRARIES})
endif()

if(ENABLE_PDF_SUPPORT)
  list(APPEND DCCORE_MOC_HEADERS
    PDFContent.h
  )
  list(APPEND DCCORE_SOURCES
    PDF.cpp
    PDFContent.cpp
    PDFInteractionDelegate.cpp
    thumbnail/PDFThumbnailGenerator.cpp
  )
  list(APPEND DCCORE_LINK_LIBRARIES ${POPPLER_LIBRARIES})
endif()

list(APPEND DCCORE_MOC_HEADERS
  CommandHandler.h
  Content.h
  ContentWindow.h
  DisplayGroup.h
  DisplayGroupRenderer.h
  EventReceiver.h
  Factories.h
  Markers.h
  MarkerRenderer.h
  MasterFromWallChannel.h
  MasterToWallChannel.h
  NetworkListener.h
  NetworkListenerThread.h
  Options.h
  PixelStream.h
  PixelStreamDispatcher.h
  PixelStreamWindowManager.h
  RenderController.h
  WallFromMasterChannel.h
  WallToMasterChannel.h
  WallToWallChannel.h
  WebbrowserCommandHandler.h
  localstreamer/AsyncImageLoader.h
  localstreamer/DockPixelStreamer.h
  localstreamer/PixelStreamer.h
  localstreamer/PixelStreamerLauncher.h
  localstreamer/Pictureflow.h
  localstreamer/WebkitAuthenticationHelper.h
  localstreamer/WebkitHtmlSelectReplacer.h
  localstreamer/WebkitPixelStreamer.h
  ws/TextInputDispatcher.h
  ws/TextInputHandler.h
  ws/WebServiceServer.h
)

list(APPEND DCCORE_SOURCES
  Command.cpp
  CommandHandler.cpp
  CommandType.cpp
  Content.cpp
  ContentFactory.cpp
  ContentLoader.cpp
  ContentType.cpp
  ContentInteractionDelegate.cpp
  ContentWindow.cpp
  ContentWindowRenderer.cpp
  DisplayGroup.cpp
  DisplayGroupRenderer.cpp
  DynamicTexture.cpp
  DynamicTextureContent.cpp
  ElapsedTimer.cpp
  Factory.hpp
  Factories.cpp
  FactoryObject.cpp
  FFMPEGMovie.cpp
  FFMPEGVideoFrameConverter.cpp
  FileCommandHandler.cpp
  FpsCounter.cpp
  FpsRenderer.cpp
  GLQuad.cpp
  GLTexture2D.cpp
  GLWindow.cpp
  globals.cpp
  gestures/DoubleTapGestureRecognizer.cpp
  gestures/PanGesture.cpp
  gestures/PanGestureRecognizer.cpp
  gestures/PinchGesture.cpp
  gestures/PinchGestureRecognizer.cpp
  ../Event.cpp
  ../log.cpp
  ../MessageHeader.cpp
  ImageJpegDecompressor.cpp
  Marker.cpp
  Markers.cpp
  MarkerRenderer.cpp
  MasterFromWallChannel.cpp
  MasterToWallChannel.cpp
  MetaTypeRegistration.cpp
  Movie.cpp
  MovieContent.cpp
  MPIChannel.cpp
  MPIContext.cpp
  NetworkListener.cpp
  NetworkListenerThread.cpp
  Options.cpp
  PixelStream.cpp
  PixelStreamBuffer.cpp
  PixelStreamContent.cpp
  PixelStreamDispatcher.cpp
  PixelStreamInteractionDelegate.cpp
  PixelStreamSegmentDecoder.cpp
  PixelStreamSegmentRenderer.cpp
  PixelStreamWindowManager.cpp
  RenderContext.cpp
  RenderController.cpp
  SessionCommandHandler.cpp
  State.cpp
  StatePreview.cpp
  StateSerializationHelper.cpp
  SVG.cpp
  SVGContent.cpp
  TestPattern.cpp
  Texture.cpp
  TextureContent.cpp
  WallFromMasterChannel.cpp
  WallToMasterChannel.cpp
  WallToWallChannel.cpp
  WebbrowserCommandHandler.cpp
  ZoomInteractionDelegate.cpp
  configuration/Configuration.cpp
  configuration/MasterConfiguration.cpp
  configuration/WallConfiguration.cpp
  localstreamer/AsyncImageLoader.cpp
  localstreamer/CommandLineOptions.cpp
  localstreamer/DockPixelStreamer.cpp
  localstreamer/DockToolbar.cpp
  localstreamer/PixelStreamer.cpp
  localstreamer/PixelStreamerFactory.cpp
  localstreamer/PixelStreamerLauncher.cpp
  localstreamer/PixelStreamerType.cpp
  localstreamer/Pictureflow.cpp
  localstreamer/WebkitAuthenticationHelper.cpp
  localstreamer/WebkitHtmlSelectReplacer.cpp
  localstreamer/WebkitPixelStreamer.cpp
  thumbnail/DefaultThumbnailGenerator.cpp
  thumbnail/FolderThumbnailGenerator.cpp
  thumbnail/ImageThumbnailGenerator.cpp
  thumbnail/MovieThumbnailGenerator.cpp
  thumbnail/PyramidThumbnailGenerator.cpp
  thumbnail/StateThumbnailGenerator.cpp
  thumbnail/ThumbnailGenerator.cpp
  thumbnail/ThumbnailGeneratorFactory.cpp
  ws/AsciiToQtKeyCodeMapper.cpp
  ws/DisplayGroupAdapter.cpp
  ws/TextInputDispatcher.cpp
  ws/TextInputHandler.cpp
  ws/WebServiceServer.cpp
)

set(DCCORE_RESOURCES resources/core.qrc)

# add OPTIONS hacks for Qt's MOC processing of Boost headers, for compiler errors with some versions of Boost
qt4_wrap_cpp(MOC_OUTFILES ${DCCORE_MOC_HEADERS} OPTIONS -DPOSIX_TIME_HPP___ -DPOSIX_TIME_SERIALIZE_HPP___ -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
qt4_add_resources(QRC_SOURCES ${DCCORE_RESOURCES})

add_library(dccore SHARED ${COMMON_SOURCES} ${DCCORE_SOURCES} ${MOC_OUTFILES}
            ${QRC_SOURCES} ${DISPLAYCLUSTER_SHARED_HEADERS})
target_link_libraries(dccore ${DCCORE_LINK_LIBRARIES})

set_target_properties(dccore PROPERTIES
  VERSION ${VERSION} SOVERSION ${VERSION_ABI} OUTPUT_NAME dccore)

install(TARGETS dccore
    ARCHIVE DESTINATION lib COMPONENT dev
    RUNTIME DESTINATION bin COMPONENT core
    LIBRARY DESTINATION lib COMPONENT cor
)
