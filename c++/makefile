CPP		:= g++ -g
CPPFLAGS	:= -Wall
LIBS		:= -lconfig++ -lgps -lsqlite3
OBJECTS		:=  sailersensor.o  structs.o mathutil.o gpspoller.o dao.o \
	lsmpoller.o lsm303dlhc/lsm303dlhc.o \
	lsm303dlhc/I2C_bus.o lsm303dlhc/smbus.o

sailersensor: $(OBJECTS)
	$(CPP) $(LIBS) $(OBJECTS) -o sailersensor.bin

scructs.o: structs.cpp
	$(CPP) $(CPPFLAGS) -c structs.cpp -o structs.o

mathutil.o: mathutil.cpp
	$(CPP) $(CPPFLAGS) -c mathutil.cpp -o mathutil.o

sailersensor.o: sailersensor.cpp
	$(CPP) $(CPPFLAGS) -c sailersensor.cpp -o sailersensor.o

gpspoller.o: gpspoller.cpp
	$(CPP) $(CPPFLAGS) -c gpspoller.cpp -o gpspoller.o

dao.o: dao.cpp
	$(CPP) $(CPPFLAGS) -c dao.cpp -o dao.o

lsmpoller.o: lsmpoller.cpp
	$(CPP) $(CPPFLAGS) -c lsmpoller.cpp -o lsmpoller.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
