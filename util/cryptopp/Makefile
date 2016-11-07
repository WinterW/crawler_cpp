MODNAME=libganji_util_cryptopp
PROJECT_HOME_PATH=../..
THIRD_LIB_PATH=$(PROJECT_HOME_PATH)/third_libs

LIB_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
CRYPTOPP_INCLUDE = $(THIRD_LIB_PATH)/
CRYPTOPP_LIB = $(THIRD_LIB_PATH)/

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -fPIC

LIBS=-L$(LIB_PATH) -L$(CRYPTOPP_LIB)
INCLUDES=-I/usr/include -I$(INCLUDE_PATH) -I$(CRYPTOPP_INCLUDE)

CFLAGS=-D_LINUX_

CXXFILES=cryptopp_util.cc

OBJS=$(CXXFILES:.cc=.o)

TARGET=$(LIB_PATH)/$(MODNAME).a

all: preexec $(TARGET) afterexec

$(TARGET): $(OBJS)
	ar r $(TARGET) $(OBJS) 
	ranlib $(TARGET)

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $(LIBS) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/util/cryptopp
	$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/util/cryptopp

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp
  
.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) -r $(INCLUDE_PATH)/util/cryptopp
