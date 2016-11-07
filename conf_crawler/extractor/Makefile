
PROJECT_HOME_PATH=../..
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs
THRIFT_HOME=$(THIRD_LIBS)/thrift
THRIFT=$(THIRD_LIBS_PATH)/bin/thrift 
THRIFT_FILE_PATH=../thrift
CONF_CRAWLER_COMMON_PATH=../common/

CONF_CRAWLER=conf_crawler
MODNAME=conf_crawler/extractor
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH)/include -I$(THIRD_LIBS_PATH)/include/thrift -I$(THIRD_LIBS_PATH)/include/hcxselect
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/lib 
LIBS=thrift thriftnb ganji_util ganji_util_thread_log ganji_util_thread ganji_util_thread_log pthread hcxselect
STATIC_LIBS = $(THIRD_LIBS_PATH)/lib/libboost_regex.a $(THIRD_LIBS_PATH)/lib/libevent.a $(THIRD_LIBS_PATH)/lib/libhtmlcxx.a

CC=g++ -Wall -ggdb -std=c++0x
#CC=g++ -O2 -Wall -ggdb -std=c++0x
#CC=g++ -pg -Wall -ggdb -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=base_extractor.cc plain_extractor.cc css_extractor.cc relay_extractor.cc extractor_config.cc extractor_util.cc
TEST_EXTRACTOR_FILES=test_extractor.cc
EXTRACTOR_SVR_FILES=conf_extractor_svr.cc
EXTRACTOR_SVR_GEN_FILES=conf_crawler_constants.cpp ExtractorService.cpp StaticLinkBaseService.cpp conf_crawler_types.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
TEST_EXTRACTOR_OBJS = $(TEST_EXTRACTOR_FILES:.cc=.o)
EXTRACTOR_SVR_OBJS = $(EXTRACTOR_SVR_FILES:.cc=.o)

# Elfs
TEST_EXTRACTOR = test_extractor
EXTRACTOR_SVR = conf_extractor_svr

all: preexec $(EXTRACTOR_SVR) $(TEST_EXTRACTOR) afterexec
#all: preexec $(TEST_EXTRACTOR) $(EXTRACTOR_SVR) afterexec

ok: clean
	make

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/$(MODNAME)
	$(CP) *.h $(INCLUDE_PATH)/$(MODNAME)
	$(CP) $(CONF_CRAWLER_COMMON_PATH)/*.h $(INCLUDE_PATH)/$(CONF_CRAWLER)/common
	$(THRIFT) --gen cpp $(THRIFT_FILE_PATH)/conf_crawler.tft
	$(CP) gen-cpp/*.h $(INCLUDE_PATH)/$(MODNAME)

.PHONY: afterexec
afterexec:
	#$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) $(TEST_EXTRACTOR) $(PROJECT_HOME_PATH)/bin/$(TEST_EXTRACTOR)
	$(RM) $(EXTRACTOR_SVR) $(PROJECT_HOME_PATH)/bin/$(EXTRACTOR_SVR)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(TEST_EXTRACTOR):$(COMMON_OBJS) $(TEST_EXTRACTOR_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(EXTRACTOR_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(EXTRACTOR_SVR):$(COMMON_OBJS) $(EXTRACTOR_SVR_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(EXTRACTOR_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
