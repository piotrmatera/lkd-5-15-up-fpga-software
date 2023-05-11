#!/bin/bash
#
# autor: Piotr 
# 4-08-2016
# 
# do Pre-build event command line wpisac:
# $(shell pwd)/scripts/version.bat
#

echo 'Skrypt generujacy opis wersji (@bash)'
H=../Software/version-id.h
rm -f $H

SHA_LONG=$(git log -1 |awk '/^commit[ ]/{print $2}')
SHA_SHORT=$(echo $SHA_LONG | awk '//{print substr($1,1,6)}')
REPO_CLEAN=$(git status |grep -c -E "nothing to commit, working ((tree)|(directory)) clean" )

echo 'Identyfikacja wersji zrodel z repo GIT:'
echo "#define REPO_CLEAN $REPO_CLEAN"
echo "#define SHA_LONG $SHA_LONG"
echo "#define SHA_SHORT $SHA_SHORT"

cat <<EOT >>$H
//plik version-id.h
//plik automatycznie generowany przez skrypt scripts/version.sh na poczatku kazdego budowania
//nie edytowac go, nie commitowac do repo
  
  
/**@brief wskazuje czy repozytorium mialo skomittowane wszystkie zmiany */
#define REPO_CLEAN $REPO_CLEAN
  
/**@brief pelne SHA z ostatniego commita */
#define SHA_LONG $SHA_LONG
  
/**@brief krotnie SHA z ostatniego commita */
#define SHA_SHORT $SHA_SHORT
 
EOT


