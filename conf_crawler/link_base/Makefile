PROJECT_HOME_PATH=../..
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs
THRIFT=$(THIRD_LIBS_PATH)/bin/thrift 
THRIFT_FILE_PATH=../thrift
CONF_CRAWLER_COMMON_PATH=../common/

CONF_CRAWLER=conf_crawler
MODNAME=$(CONF_CRAWLER)/link_base
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include
MONGO_PATH=$(THIRD_LIBS_PATH)/include/mongo
THRIFT_INCLUDE_PATH=$(THRIFT_HOME)/include/thrift/
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH) -I$(THIRD_LIBS_PATH)/include -I$(MONGO_PATH)
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/lib
#LIBS=thrift thriftnb ganji_util ganji_util_thread_log ganji_util_thread mysqlclient mongoclient boost_thread boost_system boost_program_options boost_filesystem bz2
LIBS=thrift thriftnb ganji_util ganji_util_thread_log ganji_util_thread mysqlclient mongoclient boost_thread boost_system boost_program_options boost_filesystem bz2
STATIC_LIBS =  $(PROJECT_HOME_PATH)/third_libs/libthriftnb.a  $(PROJECT_HOME_PATH)/third_libs/libthrift.a   $(PROJECT_HOME_PATH)/third_libs/libevent.a $(PROJECT_HOME_PATH)/third_libs/libboost_filesystem.a $(PROJECT_HOME_PATH)/third_libs/libboost_system.a $(PROJECT_HOME_PATH)/third_libs/libboost_thread.a
# XXX 选项-Wno-deprecated目的是去掉:warning: ‘auto_ptr’ is deprecated 
CC=g++ -Wall -Wno-deprecated -ggdb -std=c++0x
#CC=g++ -O2 -Wall -ggdb -std=c++0x
#CC=g++ -pg -Wall -ggdb -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=link_config.cc base64.cc mongo_storage.cc base_link.cc link_util.cc
LINK_BASE_FILES=link_base_svr.cc link_base.cc
LINK_BASE_SVR_GEN_FILES=conf_crawler_constants.cpp LinkBaseService.cpp DCService.cpp ExtractorService.cpp DedupService.cpp conf_crawler_types.cpp

DEV_LINK_BASE_FILES=dev_link_base_svr.cc dev_link_base.cc
DEV_LINK_BASE_SVR_GEN_FILES=conf_crawler_constants.cpp DCService.cpp ExtractorService.cpp DevLinkBaseService.cpp conf_crawler_types.cpp

STATIC_LINK_BASE_FILES=static_link_base_svr.cc static_link_base.cc
STATIC_LINK_BASE_SVR_GEN_FILES=conf_crawler_constants.cpp DCService.cpp ExtractorService.cpp DedupService.cpp StaticLinkBaseService.cpp conf_crawler_types.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
LINK_BASE_OBJS = $(LINK_BASE_FILES:.cc=.o)
DEV_LINK_BASE_OBJS = $(DEV_LINK_BASE_FILES:.cc=.o)
STATIC_LINK_BASE_OBJS = $(STATIC_LINK_BASE_FILES:.cc=.o)

# Elfs
LINK_BASE_SVR = link_base_svr
DEV_LINK_BASE_SVR = dev_link_base_svr
STATIC_LINK_BASE_SVR = static_link_base_svr

#all: preexec $(LINK_BASE_SVR) $(DEV_LINK_BASE_SVR) $(STATIC_LINK_BASE_SVR) afterexec
all: preexec $(LINK_BASE_SVR) $(STATIC_LINK_BASE_SVR) afterexec

ok: clean
	make

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
	$(RM) $(LINK_BASE_SVR) $(PROJECT_HOME_PATH)/bin/$(LINK_BASE_SVR)
	$(RM) $(DEV_LINK_BASE_SVR) $(PROJECT_HOME_PATH)/bin/$(DEV_LINK_BASE_SVR)
	$(RM) $(STATIC_LINK_BASE_SVR) $(PROJECT_HOME_PATH)/bin/$(STATIC_LINK_BASE_SVR)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(LINK_BASE_SVR):$(COMMON_OBJS) $(LINK_BASE_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(LINK_BASE_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(DEV_LINK_BASE_SVR):$(COMMON_OBJS) $(DEV_LINK_BASE_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(DEV_LINK_BASE_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(STATIC_LINK_BASE_SVR):$(COMMON_OBJS) $(STATIC_LINK_BASE_OBJS)
	$(CC) $(INCLUDES) $(addprefix gen-cpp/,$(STATIC_LINK_BASE_SVR_GEN_FILES)) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) -lrt
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
