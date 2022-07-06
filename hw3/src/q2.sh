#!/bin/sh

OUT_DIR=./out

mkdir ./out
rm -f $OUT_DIR/*
openssl genrsa -out $OUT_DIR/key.pem 1024
openssl rsa -in $OUT_DIR/key.pem -noout -text
openssl rsa -in $OUT_DIR/key.pem -out $OUT_DIR/public.pem -pubout 

make hw3
./hw3 -q34 -f $OUT_DIR/key.pem
rm -f $OUT_DIR/*