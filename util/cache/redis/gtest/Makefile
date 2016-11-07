GANJI_DEV_PATH=../../../../../..
DEV_ROOT_PATH=../../../../..

LIB_PATH=$(GANJI_DEV_PATH)/lib
INCLUDE_PATH=$(GANJI_DEV_PATH)/include


CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

.SUFFIXES: .o .cc
CC=g++ -ggdb -std=c++0x

INCLUDES= -I/data/home/zhangdanian/software/gtest-1.7.0/include/ -I/usr/include/ -I/usr/include/c++/4.4/backward/  -I$(GANJI_DEV_PATH) -I$(DEV_ROOT_PATH)
LIB_PATH = /usr/lib/x86_64-linux-gnu/ /usr/local/lib/ 
LIBS = 
STATIC_LIBS = /data/home/zhangdanian/software/gtest-1.7.0/lib/.libs/libgtest.a /data/home/ganji/software/redis-2.8.6/deps/hiredis/libhiredis.a $(GANJI_DEV_PATH)/lib/libganji_util.a
GTESTFILES = main.cc pressure_test.cc

GTESTMAIN = gtest

GTEST_OBJS=$(GTESTFILES:.cc=.o)

.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

all: $(GTESTMAIN)

$(GTESTMAIN):$(GTEST_OBJS)
	$(CC) $(INCLUDES) -pthread $(addprefix -L,$(LIB_PATH))  $^ -o $@ $(STATIC_LIBS)
.PHONY: clean
clean:
	$(RM) *.o $(GTESTMAIN)
