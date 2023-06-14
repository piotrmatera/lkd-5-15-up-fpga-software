close all
clear all
SCOPE_BUFFER = 800;
Ts = 25e-6;
time = (0:1:SCOPE_BUFFER-1)*Ts*1000;
[file,path] = uigetfile('*scope.bin');
fid = fopen([path file],'r');
data = fread(fid, [SCOPE_BUFFER 12], 'single');
fclose(fid);

h = figure;
%plot(time, data(:,1:3) - [data(:,2) data(:,3) data(:,1)]);
plot(time, data(:,1:3));
title('Grid voltages');
xlim([0 time(end)+time(2)]);
xlabel('t [ms]') 
ylabel('U [V]') 
legend({'L_1','L_2','L_3'},'Location','southwest')

h = figure;
plot(time, data(:,4:6));
title('Grid currents');
xlim([0 time(end)+time(2)]);
xlabel('t [ms]') 
ylabel('I [A]') 
legend({'L_1','L_2','L_3'},'Location','southwest')

h = figure;
plot(time, data(:,9:12));
title('Converter currents');
xlim([0 time(end)]);
xlabel('t [ms]') 
ylabel('I [A]') 
legend({'L_1','L_2','L_3','N'},'Location','southwest')

h = figure;
plot(time, data(:,7:8));
title('DClink voltage');
xlim([0 time(end)+time(2)]);
xlabel('t [ms]') 
ylabel('U [V]') 
legend({'U_d_c','U_d_c_n'},'Location','southwest')

uiwait(msgbox('Press OK to close figures'));
close all