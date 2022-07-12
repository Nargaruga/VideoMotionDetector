CC = g++
CFLAGS = -O3 -std=c++20 -Wall -pthread -DTRACE_FASTFLOW
LDFLAGS = -pthread
LIBS =  `pkg-config --libs opencv4`
INCLUDE = -isystem /usr/local/include/ff/ `pkg-config --cflags opencv4`
MODULES = . threads ff
SRCDIR = $(addprefix src/,$(MODULES)) 
OBJDIR = $(addprefix build/,$(MODULES))
SRC = $(foreach sdir,$(SRCDIR),$(wildcard $(sdir)/*.cpp))
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC)) 

vpath %.cpp $(SRCDIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $$< -o $$@
endef

.PHONY: clean checkdirs all

all: checkdirs motionDetector 

motionDetector: $(OBJ)  
	$(CC) $(LDFLAGS) $(LIBS) $^ -o $@ 
 
checkdirs: $(OBJDIR)

$(foreach odir,$(OBJDIR),$(eval $(call make-goal,$(odir))))

clean:
	rm -f $(OBJ) 
