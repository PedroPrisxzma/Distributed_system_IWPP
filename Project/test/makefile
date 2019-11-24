CC = mpic++
CVFLAGS = `pkg-config --cflags --libs opencv` 
CXXFLAGS = -Wall
TFLAGS = -lpthread 

M_SRCS = imreconTest.cpp
INC_SRCS = ../src/morphological_algorithms/MorphologicOperations.cpp
SRC = $(M_SRCS) $(INC_SRCS)
INC = -I../src/morphological_algorithms/

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