#!/bin/bash
#
# wyciaga aktualny numer mapy modbus z Software/version.h 
# przechodzi do katalogu mapy
# uruchamia tam wlasciwy skrypt convert-maps.sh

echo "Okreslenie numeru wersji mapy modbus ze zrodel..."
modbus_version=$( awk '/^#define[ ]*MODBUS_ID/{printf $3}' ../Software/version.h )
echo "Znaleziony numer wersji mapy: $modbus_version"

cd ../doc/modbus-maps/$modbus_version

../../../scripts/convert-maps.sh

