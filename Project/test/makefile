CC = g++
# CC = mpic++
CXXFLAGS = -Wall
CVFLAGS = `pkg-config --cflags --libs opencv` 
TFLAGS = -lpthread 

M_SRCS = $(wildcard *.cpp) 	
INC_SRCS = $(wildcard */*.cpp)
SRC = $(M_SRCS) $(INC_SRCS)
H_SRCS = $(wildcard */)
INC = $(H_SRCS:%=-I/%)

# SRCS = main.cpp 	
# INC_SRCS = classes/vertices.cpp image_handling/ImageSeparator.cpp morphological_algorithms/MorphologicOperations.cpp 
# INC = -I/classes/ -I/image_handling/ -I/morphological_algorithms/
BINS = imreconTest 

main : ${SRC}
	@echo "Checking.."
	$(CC) $(CXXFLAGS) $(INC) $(SRC) $(CVFLAGS) -o imreconTest

# $(CC) $(CXXFLAGS) $(INC) $(SRCS) $(INC_SRCS) $(CVFLAGS) -o main

clean :
	rm -rvf *.o ${BINS}


.PHONY: all run clean val tests fresh