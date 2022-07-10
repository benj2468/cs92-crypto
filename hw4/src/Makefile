dex: dex.o
	$(CC) -o $@ `pkg-config --libs openssl` $^

dex.o: dex.c
	$(CC) -o $@ -c `pkg-config --cflags openssl` $^

clean:
	rm -f *.o dex dex2 out/*