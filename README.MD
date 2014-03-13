This is a clone of the linphone repo,
With a patch that enable a callback after playing a wave file.

The new function is play_file_with_cb

I've included a  dialer program that make the use of the new function.
It calls a number, wait for the answer, plays a wave file and hangup.

Dialer is a command line program.


Here's how to build it (statically for easy portability).



git clone https://github.com/codingtony/linphone.git  --recursive

cd linphone/
LINPATH=$PWD
export PKG_CONFIG_PATH=$LINPATH/install/lib/pkgconfig/
git tag -a 3.7.0 HEAD -m "3.7.0"
cd $LINPATH/antlr3/runtime/C/dist/
tar xvzf libantlr3c-3.4-beta3.tar.gz
cd libantlr3c-3.4-beta3
./configure  --prefix=$LINPATH/install   --enable-shared=false --enable-static=true  && make & make install

#cd $LINPATH/oRTP
#./autogen.sh && ./configure  LDFLAGS="-static" --prefix=$LINPATH/install   --enable-shared=false --enable-static=true& & make && make install

#cd $LINPATH/mediastreamer2
#./autogen.sh && ./configure  LDFLAGS="-static"  --prefix=$LINPATH/install   --enable-shared=false --enable-static=true  --with-gnu-ld --disable-speex --disable-video && make && make install

cd $LINPATH/belle-sip
./autogen.sh && ./configure  --prefix=$LINPATH/install  --enable-shared=false --enable-static=true  --with-gnu-ld  && make && make install

cd  $LINPATH 
./autogen.sh
./configure LDFLAGS="-static"   --enable-external-ortp --enable-external-mediastreamer  --prefix=$LINPATH/install --disable-x11 --enable-gtk_ui=no   --disable-video  --enable-static=true  --disable-speex --with-gnu-ld --enable-console_ui=no --enable-shared=false --enable-tools=no

make && make install

