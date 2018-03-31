CC = gcc
CFLAGS = -Wall -std=c11 -g -Iinclude -fPIC
LDFLAGS = -L. 

#UNAME Shell Variable
UNAME_S := $(shell uname -s)

# OS X Dependant Variables
ifeq ($(UNAME_S), Darwin)
    LIB := sharedLib.dylib
# GNU/Linux Depedant Variables
else ifeq ($(UNAME_S), Linux)
    LIB := sharedLib.so
endif

all: $(LIB)

$(LIB) :
	$(CC) $(CFLAGS) $(LDFLAGS) src/LinkedListAPI.c src/GEDCOMparser.c src/GEDCOMutilities.c -shared -o ../$(LIB)

main:
	$(CC) $(CFLAGS) $(LDFLAGS) ./src/*.c -Iinclude -o ./bin/main
clean :
	rm ../$(LIB)

