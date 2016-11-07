
PROJECT_HOME_PATH=../..
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs
#THRIFT_HOME=$(THIRD_LIBS_PATH)/lib/thrift
THRIFT=$(THIRD_LIBS_PATH)/bin/thrift
THRIFT_FILE_PATH=../thrift
CONF_CRAWLER_COMMON_PATH = ../common/
#THRIFT_SRC_PATH=$(THRIFT_HOME)/lib/cpp/src
#THRIFT_INCLUDE_PATH=$(THRIFT_HOME)/lib/cpp/src/thrift

CONF_CRAWLER=conf_crawler
MODNAME=$(CONF_CRAWLER)/dc
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH)/include/ -I$(THIRD_LIBS_PATH)/include/thrift
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/lib
#LIBS=thrift thriftnb  ganji_util_thread_log ganji_util_thread util_config ganji_util_net ganji_util_encoding ganji_util_text
LIBS=thrift thriftnb  ganji_util
STATIC_LIBS = $(THIRD_LIBS_PATH)/lib/libevent.a  $(THIRD_LIBS_PATH)/lib/libboost_regex.a $(THIRD_LIBS_PATH)/lib/libboost_thread.a $(THIRD_LIBS_PATH)/lib/libboost_filesystem.a
CC=g++ -Wall -ggdb -std=c++0x
#CC=g++ -O2 -Wall -ggdb -std=c++0x
#CC=g++ -pg -Wall -ggdb -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=dc_config.cc
DC_FILES=dc_svr.cc dc.cc
DC_SVR_GEN_FILES=conf_crawler_constants.cpp DCService.cpp conf_crawler_types.cpp LinkBaseService.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
DC_OBJS = $(DC_FILES:.cc=.o)

# Elfs
DC_SVR = dc_svr

all: preexec $(DC_SVR) afterexec

ok: clean all

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/$(MODNAME)
	$(CP) *.h $(INCLUDE_PATH)/$(MODNAME)
	$(MKDIR) $(INCLUDE_PATH)/$(CONF_CRAWLER)/common
	$(CP) $(CONF_CRAWLER_COMMON_PATH)/*.h $(INCLUDE_PATH)/$(CONF_CRAWLER)/common
	$(THRIFT) --gen cpp $(THRIFT_FILE_PATH)/conf_crawler.tft
	$(CP) gen-cpp/*.h $(INCLUDE_PATH)/$(MODNAME)

.PHONY: afterexec
afterexec:
	#$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) $(DC_SVR) $(PROJECT_HOME_PATH)/bin/$(DC_SVR)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(DC_SVR):$(COMMON_OBJS) $(DC_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(DC_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
