OPENSSL=openssl
OUT_DIR=./out
ASSETS_DIR=../assets
PRIV_FILE=$OUT_DIR/key.pem
PUB_FILE=$OUT_DIR/public.pem
SIGOPTS=rsa_padding_mode:pss
DGST_FILE=$ASSETS_DIR/fatherwilliam.txt
OUT_FILE=$OUT_DIR/father.sign

$OPENSSL dgst -sign $PRIV_FILE -out $OUT_FILE -sigopt $SIGOPTS $DGST_FILE
$OPENSSL dgst -verify $PUB_FILE -signature $OUT_FILE -sigopt $SIGOPTS $DGST_FILE
rm -f $OUT_FILE