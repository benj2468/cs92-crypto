LDFLAGS="-L/opt/homebrew/opt/openssl@3/lib"
CPPFLAGS="-I/opt/homebrew/opt/openssl@3/include"

rsa: rsa.o crypto.o
	cc rsa.o crypto.o -o $@

elgamal: elgamal.o crypto.o
	$(CC) -o $@ `pkg-config --libs openssl` $^

cbc: cbc.o des.o
	$(CC) -o $@ $^

ctr: ctr.o des.o
	$(CC) -o $@ $^

glitch: glitch.o
	$(CC) -o $@ $^

cbc_mac: cbc_mac.o des.o
	$(CC) -o $@ $^

cbc_mac.o: cbc_mac.c includes/des.h
	$(CC) -c cbc_mac.c -o $@

cbc.o: cbc.c
	$(CC) -c $^

ctr.o: ctr.c
	$(CC) -c $^

elgamal.o: elgamal.c 
	$(CC) -o $@ -c `pkg-config --cflags openssl` $^

rsa.o: rsa.c includes/crypto.h
	$(CC) -c rsa.c

crypto.o: crypto.c includes/crypto.h
	$(CC) -c crypto.c

des.o: des.c includes/des.h
	$(CC) -c des.c

glitch.o: glitch_des.c includes/des.h
	$(CC) -c glitch_des.c -o $@

clean:
	$(RM) -f cbc ctr rsa elgamal glitch cbc_mac *.o