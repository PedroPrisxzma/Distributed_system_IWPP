CC = mpic++
CXXFLAGS = -Wall
CVFLAGS = `pkg-config --cflags --libs opencv` 
TFLAGS = -lpthread 

#M_SRCS := $(wildcard *.cpp) 	
#INC_SRCS := $(wildcard */*.cpp)
#SRC = $(M_SRCS) $(INC_SRCS)
SRCS = main.cpp 	
INC_SRCS = classes/vertices.cpp image_handling/ImageSeparator.cpp morphological_algorithms/MorphologicOperations.cpp 
INC = -I/classes/ -I/image_handling/ -I/morphological_algorithms/
BINS = main 

main : ${SRCS} ${INC_SRCS}
	@echo "Checking.."
	# $(CC) $(CXXFLAGS) $(INC) $(SRC) $(CVFLAGS) -o main
	$(CC) $(CXXFLAGS) $(INC) $(SRCS) $(INC_SRCS) $(CVFLAGS) -o main

clean :
	rm -rvf *.o ${BINS}


.PHONY: all run clean val tests fresh