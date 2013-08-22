KindleDir=/media/Kindle

if [ ! -d $KindleDir ]
then 
KindleDir=/media/KINDLE
fi

if [ ! -d $KindleDir ]
then
    echo "Kindle not mounted"
#    exit
fi



CONFIGURATION=rtm
[ -z $1 ] || CONFIGURATION=$1
[ -d $KindleDir ] && cp -rf ../KindleSetup/DK_System/* $KindleDir/DK_System
[ -d $KindleDir ] && cp -rf _bin/Kindle/$CONFIGURATION/KindleApp $KindleDir/DK_System/xKindle/
ls -l _bin/Kindle/$CONFIGURATION/KindleApp
#[ -d $KindleDir ] && cp -rf _bin/Kindle/$CONFIGURATION/UsbSignal.bin $KindleDir/DK_System/xKindle/
#myscp ../KindleSetup/DK_System/install/DuoKanInstall.sh root@192.168.15.244:/mnt/us/DK_System/install
#myscp  _bin/Kindle/$CONFIGURATION/KindleApp root@192.168.15.244:/mnt/us/DK_System/xKindle
#myscp  _bin/Kindle/$CONFIGURATION/lib/libdkkernel.so root@192.168.15.244:/mnt/us/DK_System/xKindle/lib
#myscp -r _bin/Kindle/$CONFIGURATION/*  root@192.168.15.244:/mnt/us/DK_System/xKindle

