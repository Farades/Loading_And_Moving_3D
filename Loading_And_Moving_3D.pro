#-------------------------------------------------
#
# Project created by QtCreator 2013-04-04T16:10:09
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

LIBS     +=-lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32 -ldevil
LIBS     +=-lilu  -lilut

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Loading_And_Moving_3D
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=
