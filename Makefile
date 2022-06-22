CC = g++
CFLAGS = -O3 -Wall
OCV = `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
OCVPATH = -I /usr/include/opencv4/
SRCDIR = src
OBJDIR = build
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(wildcard $(OBJDIR)/*.o)

.PHONY: clean all

all: motionDetector 

motionDetector: $(OBJ) 
	$(CC) $(CFLAGS) $(OCV) -o $@ $< 

main.o: $(SRCDIR)/main.cpp $(SRCDIR)/sequential_motion_detector.h
	$(CC) $(CFLAGS) -c -o $@ $<

seq_mo_dec.o: $(SRCDIR)/sequential_motion_detector.* 
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	rm -f $(OBJ) 
