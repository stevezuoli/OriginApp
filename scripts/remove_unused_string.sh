#!/bin/bash
src="inc/I18n/StringManager.h"
english="res/language/English.txt"
for i in `cat $src |sed -n -e  "/MACRO_NAME/!d;s/.*MACRO_NAME( *\([^,]*\),.*/\1/;/LOGIN_ERROR/d;/STRING_LIST/d;p"`
do
    if  ! grep -q $i **/*.cpp 
    then
        sed -i "/$i/d" $src
    fi
done

for i in `cat $english |sed -n -e  "s/ *\([^=]*\)=.*/\1/;/LOGIN_ERROR/d;/STRING_LIST/d;p"`
do
    if  ! grep -q $i **/*.cpp 
    then
        sed -i "/$i/d" $english
    fi
done
