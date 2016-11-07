PROJECT_HOME_PATH=../..
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs
THRIFT=$(THIRD_LIBS_PATH)/bin/thrift 
THRIFT_FILE_PATH=../thrift
CONF_CRAWLER_COMMON_PATH = ../common/

CONF_CRAWLER=conf_crawler
MODNAME=$(CONF_CRAWLER)/url_dedup
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH)/include -I$(THIRD_LIBS_PATH)/include/thrift
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/lib
LIBS=thrift thriftnb ganji_util ganji_util_thread_log ganji_util_thread ssl
#STATIC_LIBS = /usr/lib/libevent.a /usr/lib/libboost_regex.a /usr/lib/libssl.a /usr/lib/libcrypto.a
STATIC_LIBS = $(THIRD_LIBS_PATH)/lib/libevent.a  $(THIRD_LIBS_PATH)/lib/libboost_regex.a

CC=g++ -Wall -ggdb -std=c++0x
#CC=g++ -O2 -Wall -ggdb -std=c++0x
#CC=g++ -pg -Wall -ggdb -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=dedup_config.cc
STATIC_DEDUP_FILES=static_dedup_svr.cc static_dedup.cc
DEDUP_FILES=dedup_svr.cc md5_dedup.cc
DEDUP_SVR_GEN_FILES=conf_crawler_constants.cpp conf_crawler_types.cpp DedupService.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
STATIC_DEDUP_OBJS = $(STATIC_DEDUP_FILES:.cc=.o)
DEDUP_OBJS = $(DEDUP_FILES:.cc=.o)

# Elfs
STATIC_DEDUP_SVR = static_dedup_svr
DEDUP_SVR = url_dedup_svr

all: preexec $(STATIC_DEDUP_SVR) $(DEDUP_SVR) afterexec

ok: clean
	make

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/$(MODNAME)
	$(CP) *.h $(INCLUDE_PATH)/$(MODNAME)
	$(CP) *.hpp $(INCLUDE_PATH)/$(MODNAME)
	$(THRIFT) --gen cpp $(THRIFT_FILE_PATH)/conf_crawler.tft
	$(CP) gen-cpp/*.h $(INCLUDE_PATH)/$(MODNAME)

.PHONY: afterexec
afterexec:
	#$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) $(STATIC_DEDUP_SVR) $(PROJECT_HOME_PATH)/bin/$(STATIC_DEDUP_SVR)
	$(RM) $(DEDUP_SVR) $(PROJECT_HOME_PATH)/bin/$(DEDUP_SVR)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(STATIC_DEDUP_SVR):$(COMMON_OBJS) $(STATIC_DEDUP_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(DEDUP_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(DEDUP_SVR):$(COMMON_OBJS) $(DEDUP_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(DEDUP_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
