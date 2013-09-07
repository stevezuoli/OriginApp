#!/bin/bash


#cp -rf cheat_prod.conf /mnt/us/DK_System/xKindle/cheat.conf
cd  /DuoKan

SYS_LITE_PATH=$(pwd)
cd lib

ln -s ./libasound.so ./libasound.so.2

ln -s ./libcharset.so.1.0.0 ./libcharset.so
ln -s ./libcharset.so.1.0.0 ./libcharset.so.1

ln -s ./libcurl.so.4.2.0 ./libcurl.so
ln -s ./libcurl.so.4.2.0 ./libcurl.so.4

ln -s ./libfreetype.so.6.5.0 ./libfreetype.so
ln -s ./libfreetype.so.6.5.0 ./libfreetype.so.6

ln -s ./libiconv.so.2.5.0 ./libiconv.so
ln -s ./libiconv.so.2.5.0 ./libiconv.so.2

ln -s ./libxml2.so.2.7.4 ./libxml2.so
ln -s ./libxml2.so.2.7.4 ./libxml2.so.2 

ln -s ./libz.so.1.2.5 ./libz.so
ln -s ./libz.so.1.2.5 ./libz.so.1 

ln -s ./libzip.so.2.1.0 ./libzip.so
ln -s ./libzip.so.2.1.0 ./libzip.so.2

ln -s ./libjson-c.so.2.0.0 ./libjson-c.so.2
ln -s ./libjson-c.so.2.0.0 ./libjson-c.so
sync
cd ..
LD_LIBRARY_PATH=.:$SYS_LITE_PATH/lib/:../lib/:${LD_LIBRARY_PATH}
FONTCONFIG_PATH=$SYS_LITE_PATH/res/fontconfig
export LD_LIBRARY_PATH
export FONTCONFIG_PATH

YEAR=$(date +%Y)
MONTH=$(date +%m)
DAY=$(date +%d)
HOUR=$(date +%H)
MINUTE=$(date +%M)
SECOND=$(date +%S)
TIME_STR=$YEAR$MONTH${DAY}_$HOUR$MINUTE$SECOND
#valgrind --tool=massif ./KindleApp
#valgrind --tool=memcheck --track-origins=yes --leak-check=full --num-callers=20 ./KindleApp 2>val.$TIME_STR.log
pkill KindleApp
if [ -n "$1" ]
then
    echo "set args -d $1">gdb.info
else
    echo "no args"
    echo "">gdb.info
fi
gdb ./KindleApp -x gdb.info
#./KindleApp
