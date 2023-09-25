#!/bin/bash
# Skrypt do przygotowania zbudowanego firmawaru dla kompensatora
#
# skrypt uruchamiac w katalogu cpu01 projektu przez
# ./scripts/prepare_fw.sh confirm
#
# wynik w podkatalogu wzgl. powyzszego
#  ../../../fw/<data>/<sha>
#

set -e 

#include_fpga_bin="no"
include_fpga_bin="yes"

if [ "$include_fpga_bin" = "yes" ];then
        echo_txt="z bitstreamem dla FPGA"
else
        echo_txt=" >>> BEZ BITSTREAMu dla FPGA <<<"
fi
        
        
if [ $# -ne 1 ]; then
        echo "prepare_fw.sh - skrypt do przygotowania zaszyfowanego pakietu firmwaru kompensatora"
	echo ' uzycie:'
        echo '  ./scripts/prepare_fw.sh confirm'
        echo "obecna konfiguracja: $echo_txt"
        echo 'aby dodac bitsteram dla fpga wskaz w tym skrypcie lokalizacje fpga.bin'
        echo ' '
        echo "Kolejnosc operacji:"
        echo "1. zatwierdzenie do repozytorium zmian w zrodlach"
        echo "2. zbudowanie plikow FW (hexy)"
        echo "3. uruchomienie tego skryptu do przygotowania pakietu z FW"
        echo "UWAGA! Ten skrypt ni ewykryje jesli bedzie wykonane [1] ale nie [2]!"
	exit 1
fi

if [ ! -f ./scripts/prepare_fw.sh ]; then
	echo BLAD! "Skrypt nalezy uruchomic z katalogu glownego projektu"
        echo "jako ./scripts/prepare_fw ..."
	exit 3
fi

fw_file="lkd.fw"
files_to_copy="./Debug-for-encrypted-fw/$fw_file"
file_cpu01="Debug-for-encrypted-fw/cpu01.hex "
file_cpu02="../cpu02/Debug-for-encrypted-fw/cpu02.hex"
if [ "$include_fpga_bin" = "yes" ];then
        file_fpga="../../../../fw-lkd-5-15-up-fpga-software/2023-07-22/fpga/FPGA.BIN"
else
        file_fpga="./scripts/empty-fpga.bin"
fi


dt=$( date +%Y-%m-%d )
tm=$( date +%H:%M:%S )

publish_prefix=../../../../fw-lkd-5-15-up-fpga-software
publish_dir_day=$publish_prefix/$dt

if [ ! -d "$publish_dir_day" ]; then 
	mkdir $publish_dir_day
fi



sha_short=$(awk '/SHA_SHORT/{printf($3)}' Software/version-id.h )

rt_sha_long=$(git log -1 |grep commit |awk '//{print $2}')
echo SHA@version-id=$rt_sha_long
rt_sha_short=$(echo $rt_sha_long | awk '//{print substr($1,1,6)}')
echo "SHA@version-id=$sha_short"
echo "SHA@git       =$rt_sha_short"
repo_clean=$(git status |awk '/nothing to commit, working ((tree)|(directory)) clean/{printf 1}')

#echo "++${repo_clean}++"
if [ "$repo_clean" == "" ]; then
        echo "Repo not clean"
        repo_clean="0"
else
        echo "Repo clean"                
fi
if [ "$rt_sha_short" != "$sha_short" ]; then
        echo BLAD! wykryto niezgodne wersje SHA w version-id.h ze zglaszanym przez GIT
        exit 4
fi

if [ "$repo_clean" == "0" ]; then
	echo BLAD! wykryto pliki niezatwierdzone do repozytorium, obraz moze byc zbudowany z tymczasowych zrodel
	echo Nalezy wykonac commit/ignore i przebudowac firmware
	exit 2	
fi



publish_dir=$publish_dir_day/$rt_sha_short

if [ ! -d "$publish_dir" ]; then 
	mkdir $publish_dir
fi

echo $publish_dir

echo "Budowanie pakietu LKD.FW $echo_txt"
echo "===================================================================="
../../../bootloader/cpu01/scripts/create-encrypted-fw.sh $file_cpu01 $file_cpu02 $file_fpga |tee $publish_dir/pkg-build.log

if [ $? -ne 0 ]; then 
        echo "BLAD !!!"
        exit 0; 
fi
echo "===================================================================="

mkdir -p $publish_dir/
cp $files_to_copy   $publish_dir/
ls -sla $publish_dir/$fw_file
echo 

readme_file=$publish_dir/gitlog.txt 

echo "-----------------------------" >> $readme_file
echo "$dt $tm" >> $readme_file
echo "gitlog:" >> $readme_file
git log -1 >> $readme_file
echo  >> $readme_file
echo -n "branch: " >> $readme_file
git branch|grep '*'|awk '//{print $2}' >> $readme_file

echo finished ok
