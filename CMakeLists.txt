#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#

cmake_minimum_required(VERSION 2.6)

SET(CMAKE_CXX_FLAGS "-g")

PROJECT(qpid-xsink)
FIND_PACKAGE(Qt4 REQUIRED)
FIND_LIBRARY(qmf2 qmf2)

SET(qpid-xsink_FORMS
    dialogdeclareoptions.ui
    dialogobjects.ui
    dialogopen.ui
    mainwindow.ui
    sinkcomponent.ui
    )

SET(qpid-xsink_HEADERS
    dialogdeclareoptions.h
    dialogobjects.h
    dialogopen.h
    msg_tool.h
    mainwindow.h
    object-details.h
    object-model.h
    optiontextedit.h
    qmf-thread.h
    sink.h
    sinkcomponent.h
    whatsthisbutton.h
)

SET(qpid-xsink_SOURCES
    dialogdeclareoptions.cpp
    dialogobjects.cpp
    dialogopen.cpp
    main.cpp
    mainwindow.cpp
    msg_tool.cpp
    object-details.cpp
    object-model.cpp
    optiontextedit.cpp
    qmf-thread.cpp
    sinkcomponent.cpp
    sink.cpp
    whatsthisbutton.cpp
)

SET(qpid-xsink_RESOURCES sink.qrc)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI(qpid-xsink_FORMS_HEADERS ${qpid-xsink_FORMS})
QT4_WRAP_CPP(qpid-xsink_HEADERS_MOC ${qpid-xsink_HEADERS})
QT4_ADD_RESOURCES(qpid-xsink_RESOURCES_RCC ${qpid-xsink_RESOURCES})

ADD_EXECUTABLE(qpid-gsink ${qpid-xsink_SOURCES} ${qpid-xsink_HEADERS_MOC} ${qpid-xsink_FORMS_HEADERS} ${qpid-xsink_RESOURCES_RCC})
TARGET_LINK_LIBRARIES(qpid-gsink ${QT_LIBRARIES} qmf2 qpidmessaging qpidtypes)

INSTALL (TARGETS qpid-gsink RUNTIME DESTINATION bin)
