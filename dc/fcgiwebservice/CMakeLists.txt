
# Copyright (c) 2013-2015, EPFL/Blue Brain Project
#                     Raphael Dumusc <raphael.dumusc@epfl.ch>

set(DCWEBSERVICE_PUBLIC_HEADERS
  DefaultHandler.h
  FastCGIWrapper.h
  Handler.h
  Mapper.h
  Request.h
  RequestBuilder.h
  Response.h
  Server.h
  types.h
)

set(DCWEBSERVICE_SOURCES
  DefaultHandler.cpp
  FastCGIWrapper.cpp
  Mapper.cpp
  RequestBuilder.cpp
  Response.cpp
  Server.cpp
)

set(DCWEBSERVICE_LINK_LIBRARIES ${Boost_LIBRARIES} ${FCGI_LIBRARIES})

set(DCWEBSERVICE_INCLUDE_NAME dc/webservice)
set(DCWEBSERVICE_OMIT_LIBRARY_HEADER ON)
common_library(dcwebservice)
