CC = g++
CFLAGS = -g -O3 -Wall
OCV = `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
OCVPATH = -I /usr/include/opencv4/
SRCDIR = src
OBJDIR = build
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

.PHONY: clean all

all: motionDetector 

motionDetector: $(OBJ)  
	$(CC) $(CFLAGS) $(OCV) $^ -o $@ 

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp 
	$(CC) -c $(CFLAGS) $^ -o $@ 

$(OBJDIR)/sequential_motion_detector.o: $(SRCDIR)/sequential_motion_detector.cpp
	$(CC) -c $(CFLAGS) $^ -o $@ 

clean:
	rm -f $(OBJ) 
