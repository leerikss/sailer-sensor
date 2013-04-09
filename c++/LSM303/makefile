marine: marine.o lsm303.o I2C_bus.o smbus.o
	g++ marine.o lsm303.o I2C_bus.o smbus.o -o marine.bin

serial: serial.o lsm303.o I2C_bus.o smbus.o
	g++ serial.o lsm303.o I2C_bus.o smbus.o -o serial.bin

calibrate: calibrate.o lsm303.o I2C_bus.o smbus.o
	g++ calibrate.o lsm303.o I2C_bus.o smbus.o -o calibrate.bin

serial.o: serial.o
	g++ -c serial.cpp -o serial.o

calibrate.o: calibrate.o
	g++ -c calibrate.cpp -o calibrate.o

lsm303.o: lsm303.o
	g++ -c lsm303.cpp -o lsm303.o

I2C_bus.o: I2C_bus.o
	g++ -c I2C_bus.cpp -o I2C_bus.o

smbus.o: smbus.o
	g++ -c smbus.c -o smbus.o

clean:
	rm -f *.o




