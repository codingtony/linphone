This is a clone of the linphone repo with a patch that enable a callback after playing a wave file.

The new function is linphone_core_set_play_file_with_cb
The change is backward compatible.

I've included a  dialer program that make the use of the new function.
It calls a number, wait for the answer, plays a wave file and hangup.

Dialer is a command line program.


Here's how to build it mostly static  for less dependencies (except for glibc)

```
git clone https://github.com/codingtony/linphone.git  --recursive

cd linphone/
LINPATH=$PWD
export PKG_CONFIG_PATH=$LINPATH/install/lib/pkgconfig/

# commit 29663afd68a5f5657042fc42b12ec5207194c6a8
cd $LINPATH/osip
./autogen.sh && ./configure  --prefix=$LINPATH/install  --disable-shared --enable-static=true  && make clean && make && make install

cd $LINPATH/libspeex 
./autogen.sh && ./configure --enable-fixed-point  --prefix=$LINPATH/install  --disable-shared --enable-static=true && make && make install


# commit 8c977dba97549ae1643cef1c29b860f5fb0358b9
# on some system you might have to add : --enable-openssl=no
cd $LINPATH/exosip
./autogen.sh && ./configure  --prefix=$LINPATH/install  --disable-shared --enable-static=true --enable-openssl=no && make clean && make && make install


cd  $LINPATH 
./autogen.sh
./configure   --prefix=$LINPATH/install --disable-x11 --enable-gtk_ui=no   --disable-video  --enable-static=true  --enable-console_ui=no --enable-shared=false --enable-tools=no 

make && make clean && make && make install
```
