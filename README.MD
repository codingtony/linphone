

git clone git@github.com:codingtony/linphone.git --recursive
cd linphone/
LINPATH=$PWD
git tag -a 3.7.0 HEAD -m "3.7.0"

#cd $LINPATH/oRTP
#./autogen.sh && ./configure  LDFLAGS="-static" --prefix=$LINPATH/install   --enable-static && make && make install

#cd $LINPATH/mediastreamer2
#./autogen.sh && ./configure  LDFLAGS="-static"  --prefix=$LINPATH/install    --enable-static  --with-gnu-ld --disable-speex --disable-video && make && make install

cd $LINPATH/belle-sip
./autogen.sh && ./configure  LDFLAGS="-static"  --prefix=$LINPATH/install  --enable-static  --with-gnu-ld  && make && make install


#BELLESIP_CFLAGS="-I$LINPATH/install/include"
cd  $LINPATH 
./autogen.sh
./configure PKG_CONFIG_PATH=$LINPATH/install/lib/pkgconfig/  LDFLAGS="-static"   --prefix=$LINPATH/install --disable-x11 --enable-gtk_ui=no   --disable-video  --enable-static   --disable-speex --with-gnu-ld --enable-console_ui=no
make && make install

