
# Build the program that will determine the private key and save a keypair file
make dex

# Fun the program we just build...
./dex dsa_pub msg1 msg2

# Use the keypair we just made to sign and verify a new message
openssl dgst -sign ../out/fake_private.pem -out ../out/test.sign -sha1 ../assets/msg3.txt
openssl dgst -verify ../assets/dsa_pub.pem -signature ../out/test.sign -sha1 ../assets/msg3.txt

rm -f ../out/*