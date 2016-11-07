MODNAME=libtinyxml
PROJECT_HOME_PATH=../..

LIB_PATH=$(PROJECT_HOME_PATH)/lib
INCLUDE_PATH=$(PROJECT_HOME_PATH)/include

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p

CC=g++ -ggdb
CXXFILES=tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
OBJS=$(CXXFILES:.cpp=.o)
TARGET=$(LIB_PATH)/$(MODNAME).a

all: preexec $(TARGET) afterexec

$(TARGET): $(OBJS)
	ar r $(TARGET) $(OBJS) tinystr.o tinyxmlerror.o tinyxmlparser.o tinyxml.o
	ranlib $(TARGET)

.SUFFIXES: .o .cpp
.cc.o:
	$(CC) -c $(INCLUDES) $< -o $@

.PHONY: preexec
preexec:
	$(MKDIR) $(INCLUDE_PATH)/util/xml
	$(CP) *.h $(INCLUDE_PATH)/util/xml

.PHONY: afterexec
afterexec:
	$(RM) *~ *.swp

.PHONY: clean
clean:
	$(RM) -r $(INCLUDE_PATH)/util/xml
	$(RM) *.o $(TARGET)
