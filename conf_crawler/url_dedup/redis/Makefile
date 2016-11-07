MODNAME=libganji_util_redis
GANJI_DEV_PATH=../../../../..
ROOT_DEV_PATH=../../../../../..

LIB_PATH=$(GANJI_DEV_PATH)/lib
TARGET_PATH=$(ROOT_DEV_PATH)/lib
INCLUDE_PATH=$(ROOT_DEV_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -fPIC 

INCLUDES=-I/usr/include -I$(INCLUDE_PATH) -I$(GANJI_DEV_PATH) -I/data/home/zhangdanian/software/gtest-1.7.0/include -I/usr/local
STATIC_LIBS = /data/home/zhangdanian/software/gtest-1.7.0/lib/.libs/libgtest.a /data/home/ganji/software/redis-2.8.6/deps/hiredis/libhiredis.a 

CFLAGS=-D_LINUX_

CXXFILES= redis_config.cc redis.cc #gtest/main.cc

GTESTFILES = gtest/main.cc

GTESTMAIN = gtest

OBJS=$(CXXFILES:.cc=.o)

GTEST_OBJS=$(GTESTFILES:.cc=.o)

TARGET=$(TARGET_PATH)/$(MODNAME).a

all: preexec $(TARGET) afterexec

$(TARGET): $(OBJS)
	ar r $(TARGET) $(OBJS) 
	ranlib $(TARGET)

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $(addprefix -L,$(LIB_PATH)) $< -o $@ 

subdir:
	make -C gtest

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/ganji/util/cache/redis
	$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/ganji/util/cache/redis
	$(CP) *.h /data/home/zhangdanian/src-backup/cc_dev/trunk/src/ganji/crawler/octopus_crawler/seo_proxy/seoproxy

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) -r $(INCLUDE_PATH)/ganji/util/cache/redis
$(GTESTMAIN):$(GTEST_OBJS)
	$(CC) $(INCLUDES) $(STATIC_LIBS) $(GTESTFILES) $^ -o $@ 
	$(CP) $(TARGET) /data/home/zhangdanian/src/cc_dev/trunk/src/lib/
