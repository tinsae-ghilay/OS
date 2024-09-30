# simple make file for task 1
# no need to explain a wildcard in name and sign
# executable file
BIN = consumerproducer
#g++ commands
# g++
CFLAG = gcc
# C++ version to compile to
# VFLAG = -std=c++11
# error handling flags
WFLAG = -Wall -Werror

# rules
all: $(BIN)

consumerproducer.o : consumerproducer.c
	$(CFLAG) $(VFLAG) $(WFLAG) -g -c consumerproducer.c
    
    
buffer.o:  buffer.c buffer.h
	$(CFLAG) $(VFLAG) $(WFLAG) -g -c buffer.c
    
# create executable from object files
$(BIN): consumerproducer.o buffer.o
	$(CFLAG) $(WFLAG) -g -o $(BIN) consumerproducer.o buffer.o -pthread

# run the program
run: $(BIN)
	./${BIN}
# clean
.PHONY:clean
clean:
	rm -f $(BIN) consumerproducer.o buffer.o
