# source files
# no need to explain a wildcard in name and sign
files = $(wildcard *.c)
# object files instead of listing them.
# this way we have our make file always working,
# so long as files are in the same folder
# then substitute the .cpp part of $(files) with .o
objects = $(patsubst %.c,%.o,$(files))
# executable file
BIN = matrix
#g++ commands
# g++
CFLAG = gcc
# C++ version to compile to
VFLAG = -std=c++2a
# error handling flags
WFLAG = -Wall -Werror

# rules
all: $(BIN)

# create executable from object files
$(BIN): $(objects)
	$(CFLAG) $(VFLAG) $(WFLAG) -g -o $(BIN) $(objects)

# create object files from source files
%.o: %.cpp
	$(CFLAG) $(VFLAG) $(WFLAG) -g -c $< -o $@

# run the program
run: $(BIN)
	./${BIN}
# clean
.PHONY:clean
clean:
	rm -f $(BIN) *.o