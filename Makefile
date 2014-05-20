DEFINES=-std=c++0x
INCS=-I/usr/include/freetype2 -Iframeworks -I/usr/include/jsoncpp -I/usr/include/curl
LDLIBS=-lconfig++ -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_objdetect -lopencv_imgproc -lopencv_calib3d -lglut -lGL -lGLU -lSDL -lftgl -lboost_filesystem -lboost_system -lpthread -lflycapture -ljsoncpp -lcurl
DEBUG=-g -pg
WARNINGS=-Wall -Wextra -pedantic -Wno-long-long #-O3 -Weffc++
BUILDDIR=build
BINDIR=dist

MAINCXX=${shell find -name "*.cxx"}
TARGETS=${MAINCXX:%.cxx=%}
CXXFLAGS=${DEBUG} ${WARNINGS} ${DEFINES} ${INCS}
LDFLAGS=${DEBUG} ${WARNINGS} ${DEFINES}
include IDEconfigs/Makefile/generic.mk
