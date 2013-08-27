CPP		:= g++ -g
CPPFLAGS	:= -Wall -pedantic
LIBS		:= -lconfig++ -lgps -lsqlite3 -lpthread
OBJECTS		:=  sailersensord.o  Log.o mathutil.o gpspoller.o dao.o socketclient.o \
	 	    lsmpoller.o lsm303dlhc/lsm303dlhc.o lsm303dlhc/I2C_bus.o lsm303dlhc/smbus.o

sailersensord: $(OBJECTS)
	$(CPP) $(LIBS) $(OBJECTS) -o sailersensord

sailersensord.o: sailersensord.cpp
	$(CPP) $(CPPFLAGS) -c sailersensord.cpp -o sailersensord.o

Log.o: Log.cpp
	$(CPP) $(CPPFLAGS) -c Log.cpp -o Log.o


mathutil.o: mathutil.cpp
	$(CPP) $(CPPFLAGS) -c mathutil.cpp -o mathutil.o

gpspoller.o: gpspoller.cpp
	$(CPP) $(CPPFLAGS) -c gpspoller.cpp -o gpspoller.o

dao.o: dao.cpp
	$(CPP) $(CPPFLAGS) -c dao.cpp -o dao.o

lsmpoller.o: lsmpoller.cpp
	$(CPP) $(CPPFLAGS) -c lsmpoller.cpp -o lsmpoller.o

socketclient.o: socketclient.cpp
	$(CPP) $(CPPFLAGS) -c socketclient.cpp -o socketclient.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
