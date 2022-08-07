#!/bin/bash

openssl s_client -servername $1 -connect $2:443 < /dev/null 2>&1 |  sed -n '/-----BEGIN/,/-----END/p' > ./certificate.pem

mkdir ./.scurl_tmp

openssl s_client -showcerts -servername $1 -connect $2:443 < /dev/null 2>&1 |  sed -n '/-----BEGIN/,/-----END/p' > ./.scurl_tmp/certs.txt

python3 ./build_chain.py ./.scurl_tmp/certs.txt ./chain.pem

OSCP_URL=$(openssl x509 -noout -ocsp_uri -in ./certificate.pem)

openssl ocsp -issuer chain.pem -cert certificate.pem -text -url $OSCP_URL


