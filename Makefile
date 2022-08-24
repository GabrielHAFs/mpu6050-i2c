DEPS = i2c-accel.h smbus.h
OBJ = i2c-accel.o smbus.o
LIBS = -li2c

%.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

i2c-accel: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

install: i2c-accel
	mkdir -p $(WORKDIR)
	cp i2c-accel $(WORKDIR)/

clean:
	rm -f *.o
	rm -f i2c-accel
