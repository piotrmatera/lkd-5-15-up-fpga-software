#!/bin/bash
# Skrypt do przygotowania zbudowanego firmawaru dla kompensatora
#
# skrypt uruchamiac w katalogu cpu01 projektu przez
# ./scripts/prepare_fw.sh confirm
#
# wynik w podkatalogu wzgl. powyzszego
#  ../../../fw/<data>/<sha>
#
echo Uzyj skryptu z katalogu kompensator-25kvar/CPU/scripts/prepare_fw.sh
exit 0

set -e 

if [ $# -ne 1 ]; then
        echo "prepare_fw.sh - skrypt do przygotowania firmwaru kompensatora"
	echo ' uzycie:'
        echo '  ./scripts/prepare_fw.sh confirm'
        echo	
	exit 1
fi

if [ ! -f ./scripts/prepare_fw.sh ]; then
	echo BLAD! "Skrypt nalezy uruchomic z katalogu glownego projektu"
        echo "jako ./scripts/prepare_fw ..."
	exit 3
fi

files_to_copyTI="./DebugTI/cpu01.hex ../cpu02/DebugTI/cpu02.hex "
files_to_copyINFI="./DebugINFI/cpu01.hex ../cpu02/DebugINFI/cpu02.hex "

dt=$( date +%Y-%m-%d )
tm=$( date +%H:%M:%S )

publish_prefix=../../../fw
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

mkdir -p $publish_dir/TI/
mkdir -p $publish_dir/INFI
cp $files_to_copyTI   $publish_dir/TI/
cp $files_to_copyINFI $publish_dir/INFI/
readme_file=$publish_dir/gitlog.txt 

echo "-----------------------------" >> $readme_file
echo "$dt $tm" >> $readme_file
echo "gitlog:" >> $readme_file
git log -1 >> $readme_file
echo  >> $readme_file
echo -n "branch: " >> $readme_file
git branch|grep '*'|awk '//{print $2}' >> $readme_file

echo finished ok
