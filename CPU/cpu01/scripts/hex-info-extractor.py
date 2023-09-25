#!/usr/bin/python
'''
Created on 22.04.2022

@author: Piotr
'''


import sys
from builtins import quit

def ascii_digit2hex( d: str ) -> int:
    #Konwersja znaku hex na wartosc
    if d>= '0' and d <= '9' :
        return ord(d)-ord('0')
    if d>= 'a' and d <= 'f' :
        return ord(d)-ord('a')+10
    if d>= 'A' and d <= 'F' :
        return ord(d)-ord('A')+10
    return 0

def ascii2hex( d:str ) -> int:
    # Konwersja dwoch znakow hez na wartosc
    value = ascii_digit2hex( d[0] )*16
    value += ascii_digit2hex( d[1])
    return value

def ntohs( x: int ) -> int:
    #zamiana kolejnosci bajtow w liczbie 2 bajtowej
    return ((x&0xFF)<<8) | ((x&0xFF00)>>8);

def ntoh( x: int ) -> int:
    #zamiana kolejnosci bajtow w liczbie 4 bajtowej
    return (ntohs( x &0xFFFF )) |  ((ntohs(x&0xFFFF0000)>>16)<<16)

def list_ntoh( s: str ) -> int:
    #konwersja listy zawierajacej 4 bajty na wartosc BE
    v = 0
    for i in range(4):
        v = v*256 + s[i]
    return v

def list_ntos( s: str ) -> int:
    #konwersja listy zawierajacej 2 bajty na wartosc
    v = 0
    for i in range(2):
        v = v*256 + s[i]
    return v

def list_h( s ):
    #konwersja listy zawierajacej 4 bajty na wartosc LE
    v = 0
    for i in range(4):
        v = v*256 + s[3-i]
    return v


def str16_print( prefix:str, buffer:list, startix:int, endix:int):
    #wypisanie tekstu TI (16bitow = 1 znak)
    print(prefix,end='')
    for i in range( startix,endix,2):
        if buffer[i] >= 32 :
            print( chr(  buffer[i] ), end='')
        else:
            print( ' ', end='' )
    




print( 'LKD hex description extractor')
#print( 'build: " __DATE__ "/" __TIME__"\n"');
print(' ')

if len(sys.argv) != 2 :
    print('usage: hex-id cpu01.hex')
    quit()
    
if sys.argv[1] == '--help' or  sys.argv[1] == '-h' :
    print("usage: hex-id cpu01.hex");
    quit()        
        
#print( sys.argv )
#print( len(sys.argv) )


f = open( sys.argv[1] )



pattern = [ 0xCA, 0xFE, 0x00, 0x00, 0xDE, 0xAD, 0x00, 0x00 ]
match_index = 0
buffer = [ ]
data_buff = [ ]
dsc_buffer = [ ]

IHEX_DATA_OFFSET = 9
IHEX_TYPE_OFFSET = 7
IHEX_LEN_OFFSET = 1
                

line = ' '
ix = 0
while line != '':
    line = f.readline()
    if( line == '' or len(line)<IHEX_TYPE_OFFSET+2 ):
        print('short')
        break
    #print( line )
    #print(ix)
    
    if ascii2hex( line[ IHEX_TYPE_OFFSET:IHEX_TYPE_OFFSET+2 ]) != 0:
        line = f.readline()
        continue
    
    data_buff_len = ascii2hex( line[ IHEX_LEN_OFFSET:IHEX_DATA_OFFSET+2])
    
    #print(ix,end='')
    #ix=ix+1    
    #print(match_index)
    
    data_buff.clear()
    for i in range( data_buff_len ):
        data_buff.append( ascii2hex( line[IHEX_DATA_OFFSET+i*2:IHEX_DATA_OFFSET+i*2+2]))
    
    for i in range( data_buff_len ):
        #print(data_buff[i])
        #print(pattern[match_index])
        if match_index < len(pattern) :
            if data_buff[i] == pattern[match_index]:
                #print('match')
                dsc_buffer.append( data_buff[i] )
                match_index = match_index+1
            else:
                dsc_buffer.clear()
                match_index = 0
        else:
            if match_index <100 :
                dsc_buffer.append( data_buff[i] )
    if len(dsc_buffer)>=100:
        break            

#print('[['      )
#print(match_index)    
#print( len(dsc_buffer))

if len(dsc_buffer)>=100:
    for i in range(0,len(dsc_buffer),8):
        for j in range(8):
            print(''.join('{:02x}'.format(dsc_buffer[i+j])), end='')
            print(' ',end='')
        print('')
    
marker = [ 0, 0]
marker[0] = list_ntoh( dsc_buffer[ 0: 4])
marker[1] = list_ntoh( dsc_buffer[ 4: 8])

start_address = list_h( dsc_buffer[8:12 ])

board_id = list_ntos( dsc_buffer[12:14])
modbus_id = list_ntos( dsc_buffer[14:16])
sw_id = list_ntos( dsc_buffer[16:18])
        
print()
print('marker = ', end='')
print(''.join('{:02x}'.format(marker[0])), end=', ')    
print(''.join('{:02x}'.format(marker[1])))
    

print('start addr = ',end='')
print(''.join('{:02x}'.format(start_address)))
                
print('board_id  = ',end='')
print(''.join('{:02x}'.format( board_id)))
      
print('modbus_id = ',end='')
print(''.join('{:02x}'.format(modbus_id)))
print('sw_id     = ',end='')
print(''.join('{:02x}'.format(sw_id)))
        

        
str16_print('dev_type  = ', dsc_buffer, 18+1,18+16*2)
print('')
         
print('fw_type   = ',end='')
print( chr(dsc_buffer[18+16*2+1]));
        
        
str16_print('sha_id    =', dsc_buffer, 18+17*2+1,18+16*2+15*2+2 )
        
    
f.close()
