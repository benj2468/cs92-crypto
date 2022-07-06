OPENSSL=openssl
OUT_DIR=./out
ASSETS_DIR=../assets
PUB_FILE=$ASSETS_DIR/pub-key.pem
SIGOPTS=rsa_padding_mode:pss
DGST1=$ASSETS_DIR/jabber1.txt
DGST2=$ASSETS_DIR/jabber2.txt
SIG_FILE=$ASSETS_DIR/jabber.sign

$OPENSSL dgst -verify $PUB_FILE -signature $SIG_FILE -sigopt $SIGOPTS $DGST1 > /dev/null 2>&1
if [ "$?" = "0" ];
then
    echo "You Signed $DGST1"
fi
$OPENSSL dgst -verify $PUB_FILE -signature $SIG_FILE -sigopt $SIGOPTS $DGST2 > /dev/null 2>&1
if [ "$?" = "0" ];
then
    echo "You Signed $DGST2"
fi
