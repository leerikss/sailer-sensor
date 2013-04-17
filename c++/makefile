CPP		:= g++ -g
CPPFLAGS	:= -Wall
LIBS		:= -lconfig++ -lboost_thread -lboost_system -lgps -lsqlite3
OBJECTS		:=  sailersensor.o gpspoller.o dao.o lsmpoller.o lsm303dlhc/lsm303dlhc.o \
	lsm303dlhc/I2C_bus.o lsm303dlhc/smbus.o

sailersensor: $(OBJECTS)
	$(CPP) $(LIBS) $(OBJECTS) -o sailersensor.bin

sailersensor.o: sailersensor.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c sailersensor.cpp -o sailersensor.o

gpspoller.o: gpspoller.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c gpspoller.cpp -o gpspoller.o

dao.o: dao.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c dao.cpp -o dao.o

lsmpoller.o: lsmpoller.cpp
	$(CPP) $(LIBS) $(CPPFLAGS) -c lsmpoller.cpp -o lsmpoller.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
