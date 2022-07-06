q6: q6.o
	$(CC) -o $@ `pkg-config --libs openssl` $^

q6.o: q6.c 
	$(CC) -o $@ -c `pkg-config --cflags openssl` $^

clean:
	rm -f *.o grad q6