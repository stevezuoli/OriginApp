#!/bin/bash
DISC_BASE=/mnt/us
DK_SYSTEM=DK_TouchSystem
[ -d $DISC_BASE ] || mkdir -p $DISC_BASE
[ -h /mnt/base-us ] && rm /mnt/base-us
ln -s $DISC_BASE /mnt/base-us
for i in DK_Documents DK_Download DK_News DK_ScreenShots DK_Sync documents music system tts
do
[ -d $DISC_BASE/$i ] || mkdir -p $DISC_BASE/$i
done

[ -d $DISC_BASE/$DK_SYSTEM/ ] || mkdir -p $DISC_BASE/$DK_SYSTEM
cp -rf ../../KindleSetup/DK_System/* $DISC_BASE/$DK_SYSTEM
[ -h $DISC_BASE/DK_System ] && rm $DISC_BASE/DK_System
ln -s $DISC_BASE/$DK_SYSTEM $DISC_BASE/DK_System 
apt-get install python python-tk python-imaging python-imaging-tk valgrind

DUOKAN_ROOT=/DuoKanTouch
[ -d $DUOKAN_ROOT ] || mkdir -p $DUOKAN_ROOT
for i in Resource res
do
[ -d $DUOKAN_ROOT/$i ] || mkdir -p $DUOKAN_ROOT/$i
[ -d $DISC_BASE/$DK_SYSTEM/xKindle/$i ] || mkdir -p $DISC_BASE/$DK_SYSTEM/xKindle/$i
cp -rf ../$i/* $DUOKAN_ROOT/$i
cp -rf ../$i/* $DISC_BASE/$DK_SYSTEM/xKindle/$i
done

cp ../../KindleSetup/DK_System/xKindle/LayoutDemo.txt $DUOKAN_ROOT

[ -h /DuoKan ] && rm /DuoKan
ln -s $DUOKAN_ROOT /DuoKan
touch /DuoKan/gdb.info

chmod 777 -R /mnt
chmod 777 -R $DUOKAN_ROOT

[ -f /DuoKan/LoggerCfg.ini ] || cp LoggerCfg.ini.sample /DuoKan/LoggerCfg.ini
chmod +w /DuoKan/LoggerCfg.ini
