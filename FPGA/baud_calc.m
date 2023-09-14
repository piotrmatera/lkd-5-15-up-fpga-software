clear all
global_clock_freq = 20000000;
baud_rate = 64*10000000/8/32/256
baud_freq = 16*baud_rate / gcd(global_clock_freq, 16*baud_rate)
baud_limit = (global_clock_freq / gcd(global_clock_freq, 16*baud_rate)) - baud_freq