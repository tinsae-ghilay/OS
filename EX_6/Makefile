# source files
# no need to explain a wildcard in name and sign
files1 = client.c messagequeue.c
files2 = printserver.c messagequeue.c
# object files instead of listing them.
# this way we have our make file always working,
# so long as files are in the same folder
# then substitute the .cpp part of $(files) with .o
objects1 = client.o messagequeue.o
objects2 = printserver.o messagequeue.o
# executable file
CLIENT = client
SERVER = printserver
#g++ commands
# g++
CFLAG = gcc -O2
# C++ version to compile to
//VFLAG = -std=c++2a
# error handling flags
WFLAG = -Wall -Werror

# rules
all: $(CLIENT) $(SERVER)

# create executable from object files
$(CLIENT): $(objects1)
	$(CFLAG) $(WFLAG) -g -o $(CLIENT) $(objects1) -lrt

$(SERVER): $(objects2)
	$(CFLAG) $(WFLAG) -g -o $(SERVER) $(objects2) -lrt

# create object files from source files
%.o: %.c
	$(CFLAG) $(VFLAG) $(WFLAG) -g -c $< -o $@
# running the programs doesnt work here using -o2

# clean
.PHONY:clean
clean:
	rm -f $(CLIENT) $(SERVER) *.o