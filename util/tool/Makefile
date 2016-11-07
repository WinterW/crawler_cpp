MODNAME=libutil_tool
PROJECT_HOME_PATH=../..
THIRD_LIB_PATH=$(PROJECT_HOME_PATH)/third_libs

libMemcached_path=$(THIRD_LIB_PATH)/libmemcached
LIB_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -std=c++0x

LIBS=-L$(LIB_PATH)
INCLUDES=-I/usr/include -I$(INCLUDE_PATH) -I$(libMemcached_path)

CFLAGS=-D_LINUX_ 

CXXFILES= formatteddatareader.cc shareresourcewrapper.cc d_bit_set.cc switch_hash_map.cc memcache.cc

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
	$(MKDIR) $(INCLUDE_PATH)/util/tool
	$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/util/tool

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) -r $(INCLUDE_PATH)/util/tool
