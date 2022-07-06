hw3: hw3.o
	$(CC) -o $@ `pkg-config --libs openssl` $^


hw3.o: hw3.c
	$(CC) -o $@ -c `pkg-config --cflags openssl` $^


clean:
	rm -f *.o hw3