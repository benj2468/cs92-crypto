crack: crack.o
	$(CC) -o $@ `pkg-config --libs openssl` $^

crack.o: crack.c  key.h
	$(CC) -o $@ -c `pkg-config --cflags openssl` crack.c


sample: sample.o
	$(CC) -o $@ `pkg-config --libs openssl` $^

sample.o: sample.c  key.h
	$(CC) -o $@ -c `pkg-config --cflags openssl` sample.c

clean:
	rm -f *.o crack sample