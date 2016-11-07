MODNAME=libganji_util_redis
PROJECT_HOME_PATH=../../..

#LIB_PATH=$(UTIL_PROJECT_PATH)/lib
TARGET_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -fPIC 

INCLUDES=-I$(INCLUDE_PATH) 
STATIC_LIBS = 

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
	$(MKDIR) $(INCLUDE_PATH)/util/cache/redis
	#$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/util/cache/redis

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) -r $(INCLUDE_PATH)/util/cache/redis
$(GTESTMAIN):$(GTEST_OBJS)
	$(CC) $(INCLUDES) $(STATIC_LIBS) $(GTESTFILES) $^ -o $@ 
