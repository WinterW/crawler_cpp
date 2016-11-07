MODNAME=libganji_util
PROJECT_HOME_PATH=..

LIB_PATH=$(PROJECT_HOME_PATH)/lib

TARGET=$(LIB_PATH)/$(MODNAME).a

#CC=g++ -O2 
CC=g++ -ggdb

CP=/bin/cp -f
RM=/bin/rm -f
MV=/bin/mv -f
MKDIR=/bin/mkdir -p


$(TARGET): subdirs
	rm -f *.a
	ar rcv $(TARGET)  thread/*.o text/*.o log/*.o xml/*.o encoding/*.o net/*.o file/*.o tree/*.o db/*.o config/*.o tool/*.o system/*.o compress/*.o
	ranlib $(TARGET)

subdirs:
	make -C time
	make -C text
	make -C log
	make -C file
	make -C xml
	make -C thread
	make -C db
	make -C encoding
	make -C net
	make -C tree
	make -C config 
	make -C tool
	make -C system
	make -C utime
	make -C compress 
	#make -C cache/redis

clean:
	$(RM) $(TARGET)
	make clean -C log
	make clean -C tree
	make clean -C file
	make clean -C net
	make clean -C encoding
	make clean -C db
	make clean -C text
	make clean -C thread
	make clean -C config
	make clean -C tool
	make clean -C time
	make clean -C system
	make clean -C utime
	make clean -C compress 
	#make clean -C cache/redis
