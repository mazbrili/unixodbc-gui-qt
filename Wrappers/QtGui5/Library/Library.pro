TEMPLATE        = lib
VERSION 	= %V_MAJOUR%.%V_MINOR%.%V_RELEASE%
TARGET          = OQG5 
DESTDIR         = ../../../lib
include( ../../../common.pri )
include( ../../../config.pri )
include( ../../../defines.pri )
include( ../../../odbc.pri )

QT += gui
QT += widgets

CONFIG          += staticlib

INCLUDEPATH     += ../include  ../../QtCore5/include

HEADERS =       ../include/OQGConnection.h \
		        ../include/OQGEnvironment.h \
                ../include/OQGLogin.h \
		        ../include/OQGMessageOutput.h \
		        ../include/OQGPropertiesDelegate.h \
		        ../include/OQGPropertiesDialog.h \
                ../include/OQGPropertiesModel.h \
                ../include/OQGProperty.h \
		        ../include/OQGStatement.h \
#                ../include/OQGStatementClient.h \
		        ../include/OQGSystem.h
SOURCES =       OQGConnection.cpp \
		        OQGEnvironment.cpp \
                OQGLogin.cpp \
		        OQGMessageOutput.cpp \
		        OQGPropertiesDelegate.cpp \
		        OQGPropertiesDialog.cpp \
                OQGPropertiesModel.cpp \
                OQGProperty.cpp \
		        OQGStatement.cpp \
#                OQGStatementClient.cpp \
		        OQGSystem.cpp

