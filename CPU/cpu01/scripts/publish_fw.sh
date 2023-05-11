#!/bin/bash

# Skrypt do publikacji zbudowanych firmaware'ow
#
# skrypt uruchamiac w katalogu glownym projektu przez
# ./scripts/publish_fw.sh typ-fw opis
#

set -e 

if [ $# -ne 2 ]; then
        echo "publish_fw.sh - skrypt do publikacji firmware'ow"
	echo ' uzycie:'
        echo '  ./scripts/publish_fw <typ-fw> <opis>'
        echo
	echo '   typ-fw - rodzaj FW: basic/default/loaded'
	echo '   opis   - opis ktory zostanie dodany do rejestru (README.txt)'	
	exit 1
fi

if [ ! -f ./scripts/publish_fw.sh ]; then
	echo BLAD! "Skrypt nalezy uruchomic z katalogu glownego projektu"
        echo "jako ./scripts/publish_fw ..."
	exit 3
fi

#if [ "$1" == "default" ]; then 
#	fw_type=default; 
#	ids="default"
#fi
#if [ "$1" == "loaded" ]; then 
#	fw_type=loaded; 
#	ids="loaded"
#fi

if [ "$1" == "basic" ]; then 
	fw_type=basic; 
	ids=basic
fi

if [ "$fw_type" == "" ]; then 
	echo 'Niewlasciwa wartosc argumentu, podaj basic/default/loaded'
	exit 1
fi
	
project=lamp-node
fw_basename=lamp-node
build_dir=_build_Debug


dt=$( date +%Y-%m-%d )
tm=$( date +%H:%M:%S )

publish_dir=../../firmware/$project/$dt

echo $publish_dir

sha_short=$(awk '/SHA_SHORT/{printf($3)}' inc/version-id.h )

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

if [ ! -d "$publish_dir" ]; then 
	mkdir $publish_dir
fi

fw_name=${fw_basename}-$sha_short-$ids.elf
fw_name_bin=${fw_basename}-$sha_short-$ids.bin

if [ -a $publish_dir/${fw_basename}-$sha_short-$ids.elf ]; then
	echo BLAD! "Proba opublikowania tego samego firmware'u powtornie"
	exit 3
fi



cp ${build_dir}/${fw_basename}.elf $publish_dir/$fw_name
#cp ${build_dir}/${fw_basename}.bin $publish_dir/$fw_name_bin

echo "$dt/$tm  $fw_name  |  $2" >> $publish_dir/../README.txt

cd $publish_dir
#git add $fw_name $fw_name_bin ../README.txt
git add $fw_name ../README.txt
git commit -m "publikacja pliku firmware'u $fw_name"
git svn dcommit

exit 0


