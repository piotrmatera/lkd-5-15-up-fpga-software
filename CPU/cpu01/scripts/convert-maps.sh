#!/bin/bash

echo Konwertowanie map na pliki inc wlaczane do zrodel
#echo uruchamiac w katalogu map 
#echo 'np. 0108/$ ../../../scripts/convert-maps.sh'

rootscr=../../../scripts

file_info=file_info.inc
echo Skasowanie starych plikow
rm -rf *.inc *.zip *.gz

#set -x 

gzip -k -f modbus_map_small.txt
mv modbus_map_small.txt.gz modbus_map_small.zip

for f in modbus_map_small.txt MODBUS_word_small.txt modbus_map_small.zip
do
	echo przetwarzanie $f w $f.inc
	py $rootscr/conv_txt_to_C.py $f > $f.inc
done

for f in modbus_map_small.txt MODBUS_word_small.txt modbus_map_small.zip
do
	file_inc=$f.inc
	#echo "+++ $f"
	echo -n '{ '  >> $file_info
	awk '/size:/{printf("%s, ", $3)}' $file_inc  >> $file_info
	awk '/md5sum-part:/{printf("0x%s, ", $3)}' $file_inc  >> $file_info
	case $f in
		modbus_map_small.txt ) echo -n '"modbus_map.txt"' >> $file_info;;
		MODBUS_word_small.txt ) echo -n '"MODBUS_word.txt"' >> $file_info;;
		modbus_map_small.zip ) echo -n '"modbus_map.txt.zip"' >> $file_info;;
	esac
	echo "},"  >> $file_info
done
	
cat $file_info
	