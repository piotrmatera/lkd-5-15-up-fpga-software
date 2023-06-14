close all
clear all
[file,path] = uigetfile('*logs.bin');
fid = fopen([path file],'r');
data = fread(fid, [18 Inf], 'single')';
data = data(2:end,:);
frewind(fid);
data2 = fread(fid, [18 Inf], 'uint32=>uint32')';
data2 = data2(2:end,:);

time = data2(:,1);
Y = bitand(bitshift(time,-25) ,127 , 'uint32') + 1980;
M = bitand(bitshift(time,-21) ,15 , 'uint32');
D = bitand(bitshift(time,-16) ,31 , 'uint32');
H = bitand(bitshift(time,-11) ,31 , 'uint32');
MI = bitand(bitshift(time,-5) ,63 , 'uint32');
S = bitand(bitshift(time,1) ,63 , 'uint32');
t = datetime(Y,M,D,H,MI,S);
fclose(fid);

xlimits = [t(2) t(end)];
figure
plot(t, data(:,2:4));
title('Grid voltage abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('U [V]') 
xlim(xlimits);
figure
plot(t, data(:,5:7));
title('Load Q power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('Q [var]')
xlim(xlimits);
figure
plot(t, data(:,5:7)-data(:,11:13));
title('Grid Q power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('Q [var]')
xlim(xlimits);
figure
plot(t, data(:,11:13));
title('Converter Q power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('Q [var]')
xlim(xlimits);
figure
plot(t, data(:,8:10));
title('Load P power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('P [W]')
xlim(xlimits);
figure
plot(t, data(:,8:10)-data(:,14:16));
title('Grid P power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('P [W]')
xlim(xlimits);
figure
plot(t, data(:,14:16));
title('Converter P power abc');
legend({'L_1','L_2','L_3'},'Location','southwest')
ylabel('P [W]')
xlim(xlimits);
figure
plot(t, data(:,17));
title('Temperatures');
xlim(xlimits);
figure
plot(t, data(:,18));
title('Operatin');
xlim(xlimits);

uiwait(msgbox('Press OK to close figures'));
close all
