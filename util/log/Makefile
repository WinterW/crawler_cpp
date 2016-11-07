MODNAME=libganji_util_log
MODNAME2=libganji_util_thread_log
MODNAME3=libganji_util_scribe_log
PROJECT_HOME_PATH=../..
THIRD_LIB_PATH=$(PROJECT_HOME_PATH)/third_libs

thrift_home = $(THIRD_LIB_PATH)/thrift
THRIFT = $(thrift_home)/bin/thrift
boost_home=$(THIRD_LIB_PATH)/boost

LIB_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -fPIC

LIBS=-L$(LIB_PATH)
INCLUDES=-I/usr/include -I$(INCLUDE_PATH) -I$(thrift_home)/include/thrift -I$(boost_home)/include

CFLAGS=-D_LINUX_

CXXFILES=fast_log.cc
CXXFILES2=thread_fast_log.cc
CXXFILES3=scribe_client_ex.cpp scribe_log.cpp ./gen-cpp/fb303_constants.cpp ./gen-cpp/fb303_types.cpp ./gen-cpp/scribe_constants.cpp ./gen-cpp/scribe_types.cpp ./gen-cpp/scribe.cpp ./gen-cpp/FacebookService.cpp 

OBJS=$(CXXFILES:.cc=.o)
OBJS2=$(CXXFILES2:.cc=.o)
OBJS3=$(CXXFILES3:.cpp=.o)

TARGET=$(LIB_PATH)/$(MODNAME).a
TARGET2=$(LIB_PATH)/$(MODNAME2).a
#TARGET3=$(LIB_PATH)/$(MODNAME3).a

THRIFTFILE1 = ./fb303/fb303.thrift
GENCXXFILE1 = ./gen-cpp/fb303_constants.cpp ./gen-cpp/fb303_types.cpp ./gen-cpp/FacebookService.cpp
THRIFTOBJ1=$(GENCXXFILE1:.cpp=.o)

THRIFTFILE2 = ./scribe/scribe.thrift
GENCXXFILE2 = ./gen-cpp/scribe_constants.cpp ./gen-cpp/scribe_types.cpp ./gen-cpp/scribe.cpp
THRIFTOBJ2=$(GENCXXFILE2:.cpp=.o)

#all: preexec $(TARGET2) afterexec
all: preexec $(TARGET) $(TARGET2) $(TARGET3) afterexec

$(TARGET): $(OBJS)
	ar r $(TARGET) $(OBJS) 
	ranlib $(TARGET)

$(TARGET2): $(OBJS2)
	ar r $(TARGET2) $(OBJS2) 
	ranlib $(TARGET2)

$(TARGET3): $(OBJS3)
	ar r $(TARGET3) $(OBJS3) 
	ranlib $(TARGET3)

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $(LIBS) $< -o $@

.SUFFIXES: .o .cpp
.cpp.o:
	$(CC) -c $(INCLUDES) $(LIBS) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/util/log
	$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/util/log
	#$(THRIFT) -o . -I $(thrift_home)/share --gen cpp --gen py --gen php $(THRIFTFILE1)
	#$(THRIFT) -o . -I $(thrift_home)/share --gen cpp --gen py --gen php $(THRIFTFILE2)
	#$(CP) gen-cpp/*.h $(INCLUDE_PATH)/ganji/util/log/
	
.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) *.o $(TARGET2)
	$(RM) *.o $(TARGET3)
	$(RM) -r $(INCLUDE_PATH)/util/log
