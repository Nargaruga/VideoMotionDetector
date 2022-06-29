CC = g++
CFLAGS = -g -O3 -std=c++20 -Wall -pthread
OCV = `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
INCLUDE = -I /usr/include/opencv4/ -I lib/fastflow/ff
MODULES = . threads 
SRCDIR = $(addprefix src/,$(MODULES)) 
OBJDIR = $(addprefix build/,$(MODULES))
SRC = $(foreach sdir,$(SRCDIR),$(wildcard $(sdir)/*.cpp))
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC)) 

vpath %.cpp $(SRCDIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(CFLAGS) -c $$< -o $$@
endef

.PHONY: clean checkdirs all

all: checkdirs motionDetector 

motionDetector: $(OBJ)  
	$(CC) $(CFLAGS) $(OCV) $^ -o $@ 
 
checkdirs: $(OBJDIR)

$(foreach odir,$(OBJDIR),$(eval $(call make-goal,$(odir))))

clean:
	rm -f $(OBJ) 
