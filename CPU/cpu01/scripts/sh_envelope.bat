@echo off
echo "Wywolanie shella SH w Windows"

cd ..\scripts

echo "Poszukiwanie sh.exe..."

set SH="%ProgramFiles(x86)%\Git\bin\sh.exe"
echo "w katalogu: %SH%"
if exist %SH% goto L_run

set SH="%ProgramFiles(x86)%\Git\usr\bin\sh.exe"
echo "w katalogu: %SH%"
if exist "%SH%" goto L_run

set SH="%ProgramW6432%\Git\bin\sh.exe"
echo "w katalogu: %SH%"
if exist %SH% goto L_run

set SH="%ProgramW6432%\Git\usr\bin\sh.exe"
echo "w katalogu: %SH%"
if exist "%SH%" goto L_run

set SH="%UserProfile%\AppData\Local\Programs\Git\bin\sh.exe"
echo "w katalogu: %SH%"
if exist "%SH%" goto L_run

echo BLAD: Nie znaleziono programu Git (zainstaluj konsole git-bash)!
goto L_exit

:L_run

echo SH znaleziony w: %SH%

%SH% --login -i -c ". %1" 

@echo on
exit 0

:L_exit
@echo on
exit -1
