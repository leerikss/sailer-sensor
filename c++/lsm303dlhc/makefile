lsmout: lsmout.o lsm303dlhc.o I2C_bus.o smbus.o
	g++ -lconfig++ lsmout.o lsm303dlhc.o I2C_bus.o smbus.o -o lsmout.bin

lsm303dlhc.o: lsm303dlhc.o
	g++ -c lsm303dlhc.cpp -o lsm303dlhc.o

I2C_bus.o: I2C_bus.o
	g++ -c I2C_bus.cpp -o I2C_bus.o

smbus.o: smbus.o
	g++ -c smbus.c -o smbus.o

clean:
	rm -f *.o
	rm -f *~
	rm -f \#*
