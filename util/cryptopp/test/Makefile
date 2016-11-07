MODNAME = test_cryptopp 
DEV_PATH_ROOT = ../../../../..

BIN_PATH = $(DEV_PATH_ROOT)/bin
INCLUDE_PATH = $(DEV_PATH_ROOT)/include
LIB_PATH = /usr/local/lib $(DEV_PATH_ROOT)/lib /usr/lib/

CP = /bin/cp -f
RM = /bin/rm -f
MV = /bin/mv -f
MKDIR = /bin/mkdir -p

CC = g++ -Wall -ggdb

INCLUDES = -I$(DEV_PATH_ROOT)/include 
LIBS=-L/root/svnroot/cc_dev/lib/ -lganji_util_cryptopp -L/usr/lib -lcryptopp

CFLAGS=-D_LINUX_

CXXFILES= main.cc

TARGET1=$(MODNAME)
all: $(TARGET1) 


$(TARGET1): main.cc
	$(CC) $(INCLUDES) -c main.cc -o main.o
	$(CC) main.o $(LIBS) -o $(TARGET1) 
    
clean:
	$(RM) *.o $(TARGET1) 
