from vcd import VCDWriter

f = open('SCOPE.BIT')
lines = f.read().splitlines()
lines = [int(x, 16) for x in lines]

def get_bits(num, start, end):
    mask = 2**(end-start+1)-1
    return (num >> start) & mask

file = open("demofile.vcd", "w")
writer = VCDWriter(file, timescale='1 ns')
# assign Scope_data_in = {PWM_i, FLT_REG_O[15:0], FLT_bus[15:0], sync_phase, Scope_trigger_r, timestamp_diff}; 
vars = [writer.register_var('top', 'timestamp_diff', 'integer', size=32)]
vars.append(writer.register_var('top', 'Scope_trigger_r', 'integer', size=1))
vars.append(writer.register_var('top', 'sync_phase', 'integer', size=1))
vars.append(writer.register_var('top', 'FLT_bus', 'integer', size=16))
vars.append(writer.register_var('top', 'FLT_REG_O', 'integer', size=16))
vars.append(writer.register_var('top', 'PWM_i', 'integer', size=4))
i=0
for integer in lines:
    bits = 0
    for var in vars:
        writer.change(var, i, get_bits(integer, bits, bits+var.size-1))
        bits += var.size
    i+=1

bits = 0
for var in vars:
    writer.change(var, i, get_bits(0, bits, bits+var.size-1))
    bits += var.size

writer.close()