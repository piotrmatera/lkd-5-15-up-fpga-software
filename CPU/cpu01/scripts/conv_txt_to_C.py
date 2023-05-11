# conv_txt_to_C.py
import sys
import os
import hashlib
from datetime import datetime


counter = 0;

def process_bytes( b ):
	global counter;	
	global byte_prev;
	if counter == 0:
		byte_prev = b;
		counter = 1
	else:
		counter = 0;
		value = byte_prev*256+b;
		print(f"{hex(value)},");



print(f"//Nie edytowac, automatycznie wygenerowane przez conv_txt_to_C.py");
today = datetime.today()
# dd/mm/YY
d1 = today.strftime("%Y-%m-%d %H:%M")
print("//", d1);
print(f"// ");
#if __name__ == "__main__":
#    print(f"//Arguments count: {len(sys.argv)}")
#    for i, arg in enumerate(sys.argv):
#        print(f"//Argument {i:>6}: {arg}")
        
filename = sys.argv[1];         
print(f"// File: {filename}");

with open( filename, "rb") as f:
	bytes_read = f.read()
	f.seek(0, os.SEEK_END)
	size = f.tell()

print( f"// size: {size} B  {size/2} W" );
  
md5sum = hashlib.md5(bytes_read).hexdigest();    
print(f"// md5sum: {md5sum}")
print(f"// md5sum-part: {md5sum[0:4]}")

print(f" ");

b_last = 0
for b in bytes_read:
    #print(f":{b}, {hex(b)}")
    process_bytes(b)
    b_last = b*256;     
    
if counter == 1 :
	print(f"{hex(b_last)},");    
       