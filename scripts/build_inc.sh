#/bin/bash
[ -d ../Bin/K3_inc ] && rm -rf ../Bin/K3_inc
[ -d ../Bin/K3_inc_final ] && rm -rf ../Bin/K3_inc_final
ls -1 KindleRelease |sed -e "s/^/KindleRelease\//" | xargs /usr/Python27/python.exe KindleApp/scripts/build_increase_package.py ../Bin/K3
mkdir ../Bin/K3_inc_final/
mv ../Bin/K3_inc/DK_System ../Bin/K3_inc_final
rm -rf ../Bin/K3_inc
mv ../Bin/K3_inc_final ../Bin/K3_inc

