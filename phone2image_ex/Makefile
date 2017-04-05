PROJECT_HOME_PATH=../
THIRD_LIBS_PATH=$(PROJECT_HOME_PATH)/third_libs

#THRIFT_HOME=/usr/local/
#THRIFT=$(THRIFT_HOME)/bin/thrift 
#THRIFT_FILE_PATH=../anti_spider/thrift

MODNAME=phone2image_ex
CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include/
#THRIFT_INCLUDE_PATH=$(THRIFT_HOME)/include/thrift/
BIN_PATH=$(PROJECT_HOME_PATH)/bin

INCLUDES=-I$(INCLUDE_PATH) -I$(THIRD_LIBS_PATH)/include/ -I$(THIRD_LIBS_PATH)/fcgi/include/ `Magick++-config --cppflags`
LIB_PATH=$(PROJECT_HOME_PATH)/lib $(THIRD_LIBS_PATH)/fcgi/lib $(THIRD_LIBS_PATH)/lib 
LIBS=event ganji_util ganji_util_thread_log ganji_util_thread fcgi++ fcgi gd freetype png16 pthread
STATIC_LIBS =  $(THIRD_LIBS_PATH)/lib/libssl.a $(THIRD_LIBS_PATH)/lib/libcrypto.a 

CC=g++ -g -ggdb -Wall -std=c++0x
#CC=g++ -pg -ggdb -Wall -std=c++0x
CFLAGS = -D_LINUX_

# CXX Files
COMMON_FILES=img_config.cc img_builder.cc enc_dec.cc des_decoder.cc phone_img.cc enc_url.cc img_util.cc
PHONE_IMG_SVR_FILES=phone_img_svr.cc
IMAGE_URL_SVR_FILES=img_url_svr.cc
TEST_DES_FILES=test_des.cc
#PASS_SVR_GEN_FILES=anti_spider_constants.cpp anti_spider_types.cpp Pass.cpp

# Objs
COMMON_OBJS = $(COMMON_FILES:.cc=.o)
PHONE_IMG_SVR_OBJS = $(PHONE_IMG_SVR_FILES:.cc=.o)
IMAGE_URL_SVR_OBJS = $(IMAGE_URL_SVR_FILES:.cc=.o)
TEST_DES_OBJS = $(TEST_DES_FILES:.cc=.o)
#PASS_SVR_GEN_OBJS = $(PASS_SVR_GEN_FILES:.cc=.o)

# Elfs
PHONE_IMG_SVR = phone_img_svr
IMAGE_URL_SVR = img_url_svr
TEST_DES = test_des

all: preexec $(PHONE_IMG_SVR) $(IMAGE_URL_SVR) $(TEST_DES) afterexec

ok: clean all

.SUFFIXES: .o .cc .cpp
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.cpp.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)
	$(MKDIR) $(INCLUDE_PATH)/$(MODNAME)
	$(CP) *.h $(INCLUDE_PATH)/$(MODNAME)
	#$(THRIFT) --gen cpp $(THRIFT_FILE_PATH)/anti_spider.tft
	#$(CP) gen-cpp/*.h $(INCLUDE_PATH)/$(MODNAME)

.PHONY: afterexec
afterexec:
	#$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o
	$(RM) $(PHONE_IMG_SVR) $(PROJECT_HOME_PATH)/bin/$(PHONE_IMG_SVR)
	$(RM) $(IMAGE_URL_SVR) $(PROJECT_HOME_PATH)/bin/$(IMAGE_URL_SVR)
	$(RM) $(TEST_DES) $(PROJECT_HOME_PATH)/bin/$(TEST_DES)
	$(RM) -r $(INCLUDE_PATH)/$(MODNAME)

$(PHONE_IMG_SVR):$(COMMON_OBJS) $(PHONE_IMG_SVR_OBJS)
	$(CC) $(INCLUDES) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) 
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(IMAGE_URL_SVR):$(COMMON_OBJS) $(IMAGE_URL_SVR_OBJS)
	$(CC) $(INCLUDES) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) 
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)

$(TEST_DES):$(COMMON_OBJS) $(TEST_DES_OBJS)
	$(CC) $(INCLUDES) $^ -o $@ $(addprefix -L,$(LIB_PATH)) $(addprefix -l,$(LIBS)) $(STATIC_LIBS) 
	$(MKDIR) $(BIN_PATH)/$(MODNAME)
	$(CP) $@ $(BIN_PATH)/$(MODNAME)
