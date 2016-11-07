
PROJECT_HOME_PATH=../..
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs

THRIFT_HOME=$(THIRD_LIBS_PATH)/thrift
THRIFT=$(THIRD_LIBS_PATH)/bin/thrift 
THRIFT_FILE_PATH=../thrift
CONF_CRAWLER_COMMON_PATH = ../common/

CONF_CRAWLER=conf_crawler
MODNAME=$(CONF_CRAWLER)/downloader
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH)/include -I$(THIRD_LIBS_PATH)/include/thrift
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/lib
#LIBS=thrift thriftnb ganji_util_thread_log ganji_util_thread util_config ganji_util_net ganji_util_encoding ganji_util_text ganji_util_system
LIBS=thrift thriftnb ganji_util
STATIC_LIBS = $(THIRD_LIBS_PATH)/lib/libevent.a

CC=g++ -Wall -ggdb -std=c++0x
#CC=g++ -O2 -Wall -ggdb -std=c++0x
#CC=g++ -pg -Wall -ggdb -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=downloader_config.cc downloader_util.cc net_checker.cc
DOWNLOADER_FILES=downloader.cc curl_downloader.cc main.cc
DOWNLOADER_GEN_FILES=conf_crawler_constants.cpp DCService.cpp HeaderFieldsService.cpp conf_crawler_types.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
DOWNLOADER_OBJS = $(DOWNLOADER_FILES:.cc=.o)

# Elfs
DOWNLOADER = downloader

all: preexec $(DOWNLOADER) afterexec

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
	$(RM) $(DOWNLOADER) $(PROJECT_HOME_PATH)/bin/$(DOWNLOADER)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(DOWNLOADER):$(COMMON_OBJS) $(DOWNLOADER_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(DOWNLOADER_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) `curl-config --static-libs`
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
