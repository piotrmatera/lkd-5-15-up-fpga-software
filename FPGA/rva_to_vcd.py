from vcd import VCDWriter

f = open('Reveal_ana_ACDC.trc')
lines = f.read().splitlines()

i=8
while i:
    lines.pop(0)
    i-=1

position = lines[0].find("-1")+3
lines = [x[position:] for x in lines]
lines = [x.replace(" ", "") for x in lines]
lines = [int(x[::-1], 2) for x in lines]

def get_bits(num, start, end):
    mask = 2**(end-start+1)-1
    return (num >> start) & mask

file = open("demofile.vcd", "w")
writer = VCDWriter(file, timescale='1 ns')
vars = [writer.register_var('top', 'sync_phase', 'integer', size=1)]
vars.append(writer.register_var('top', 'local_counter', 'integer', size=16))
vars.append(writer.register_var('top', 'local_free_counter', 'integer', size=16))
vars.append(writer.register_var('top', 'local_counter_timestamp_master', 'integer', size=16))
vars.append(writer.register_var('top', 'local_counter_timestamp_slave', 'integer', size=16))
i=0
for integer in lines:
    if integer == 0: break
    bits = 0
    for var in vars:
        read_bits = get_bits(integer, bits, bits+var.size-1)
        writer.change(var, i, read_bits)
        bits += var.size
    i+=1

writer.close()