CC = mpic++
CFLAGS = -Wall 
TFLAGS = -lpthread 
Vflags = valgrind --leak-check=full

SRCS = main.cpp ImageSeparator.cpp
BINS = main 

# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

all : ${BINS}

main : main.cpp
	@echo "Checking.."
	$(CC) $(CFLAGS) $(SRCS) `pkg-config opencv --libs` -o main

run : 
	$(foreach b, ${BINS}, \
	mpiexec -n 5 ./$b < input )

val :
	$(Vflags) ./${RUN_ARGS}

tests :
	$(Vflags) ./${RUN_ARGS} < \* insert entry tests here*\

clean :
	rm -rvf *.o ${BINS}


.PHONY: all run clean val tests fresh