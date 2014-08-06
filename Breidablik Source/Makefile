TARGET = main

CC = g++
INCPATH=-I./PS2Framework/include
CFLAGS = -g -Wall $(INCPATH) 

.SUFFIXES: .cpp .o .vcl

OBJS = 	PS2Framework/src/primitives.o \
		PS2Framework/src/ps2maths.o \
		PS2Framework/src/ps2matrix4x4.o \
		PS2Framework/src/ps2vector4.o \
		PS2Framework/src/pad.o \
		PS2Framework/src/dma.o \
		PS2Framework/src/sps2wrap.o \
		PS2Framework/src/texture.o \
		PS2Framework/src/font.o \
		PS2Framework/src/audio.o \
		MyPS2Application.o \
		XYCoords.o \
		GameObject.o \
		Character.o \
		Player.o \
		Enemy.o \
		Object.o \
		Cursor.o \
		LevelData.o\
		Map.o \
		Menu.o \
		PathController.o \
		ScreenViewController.o \
		GameTextureController.o \
		GameStateController.o \
		SoundController.o \
		main.o

LIBS = -lm -lsps2util

LIBPATH=-L./

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBPATH) $(LIBS)

.cpp.o:
	$(CC) -c $<  $(CFLAGS) -o $@

.vcl.o:
	vcl -g -o$*.vsm $*.vcl
	ee-dvp-as -o $*.vo_ $*.vsm
	objcopy -Obinary $*.vo_ $*.bin_
	./bin2as $* $*.bin_ > $*.a_
	as -mcpu=r5900 -KPIC -mips2 -o $*.o $*.a_
	rm $*.vo_ $*.bin_ $*.a_

*.vcl: bin2as
bin2as:  bin2as.cpp
	cc -o bin2as bin2as.cpp
	
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f depend.mk
	
submission: $(TARGET)
	rm -f $(OBJS)
	rm -f depend.mk
	
run: $(TARGET)
	./$(TARGET)

# create the dependancy file	
depend:
	$(CC) $(CFLAGS) -MM $(patsubst %.o,%.cpp,$(OBJS)) > depend.mk

#include the dependencies into the build
-include depend.mk