This is a clone of the linphone repo,
With a patch that enable a callback after playing a wave file.

The new function is linphone_core_set_play_file_with_cb
The change is backward compatible.

I've included a  dialer program that make the use of the new function.
It calls a number, wait for the answer, plays a wave file and hangup.

Dialer is a command line program.


Here's how to build it. 
Mostly static for less dependencies

```

git clone https://github.com/codingtony/linphone.git  --recursive

cd linphone/
LINPATH=$PWD
export PKG_CONFIG_PATH=$LINPATH/install/lib/pkgconfig/
git tag -a 3.7.0 HEAD -m "3.7.0"
cd $LINPATH/antlr3/runtime/C/
./autogen.sh && ./configure  --prefix=$LINPATH/install  --disable-shared --enable-64bit --with-pic  --enable-static=true  && make && make install
mkdir -p $LINPATH/install/share/java/
ln -s $LINPATH/antlr3/antlr-3.4-complete.jar $LINPATH/install/share/java/antlr.jar

cd $LINPATH/belle-sip
./autogen.sh && ./configure  --with-antlr=$LINPATH/install --prefix=$LINPATH/install  --enable-shared=false --enable-static=true && make && make install

cd  $LINPATH 
./autogen.sh
./configure   --prefix=$LINPATH/install --disable-x11 --enable-gtk_ui=no   --disable-video  --enable-static=true  --disable-speex --enable-console_ui=no --enable-shared=false --enable-tools=no

make && make install
```
