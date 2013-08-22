#!/bin/bash
resDir=res
srcFonts=(dk-code.ttf dk-fangsong.ttf dk-heiti.ttf dk-kaiti.ttf dk-serif.ttf dk-songti.ttf dk-xiaobiaosong.ttf dk-xiheiti.ttf)
dstFonts=(userfonts/Inconsolata.ttf userfonts/hyfs.ttf sysfonts/fzlth_gb18030.ttf userfonts/hwkt.ttf userfonts/GenBkBasR.ttf userfonts/fzssjt.ttf userfonts/fzxbsjt.ttf userfonts/fzltkh_gbk.ttf)
#srcFonts=(1 2 3 4 5 6 7 8 9 10)
lenSrcFonts=${#srcFonts[@]}
i=0
while [ $i -lt $lenSrcFonts ]
do
srcFont=${srcFonts[$i]}
dstFont=$resDir/${dstFonts[$i]}

if diff $srcFont $dstFont 
then
echo $srcFont $dstFont are same
else
tf co $dstFont && cp $srcFont $dstFont
fi
let "i=$i+1"
done
