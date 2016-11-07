MODNAME=libganji_util_encoding
PROJECT_HOME_PATH=../..

LIB_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb -fPIC

LIBS=-L$(LIB_PATH)
INCLUDES=-I/usr/include -I$(INCLUDE_PATH)

CFLAGS=-D_LINUX_

CXXFILES=encoding.cc utf8conv.cc md5.cc md5_generator.cc crcsign.cc encryption.cc gbk_utf8.cc

OBJS=$(CXXFILES:.cc=.o)

TARGET=$(LIB_PATH)/$(MODNAME).a

all: preexec $(TARGET) afterexec

$(TARGET): $(OBJS)
	ar r $(TARGET) $(OBJS) 
	ranlib $(TARGET)

.SUFFIXES: .o .cc
.cc.o:
	$(CC) -c $(INCLUDES) $(LIBS) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/util/encoding
	$(MKDIR) $(LIB_PATH)
	$(CP) *.h $(INCLUDE_PATH)/util/encoding

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp
	$(RM) $(INCLUDE_PATH)/util/encoding/*_imp.h

.PHONY: clean
clean:
	$(RM) *.o $(TARGET)
	$(RM) -r $(INCLUDE_PATH)/util/encoding
