clear all;
close all;

fileID = fopen('cpu01/Software/Kalman_gains.h','w');
fprintf(fileID, '#pragma once\n\n');
fprintf(fileID, '#ifndef Kalman_gains_H_\n');
fprintf(fileID, '#define Kalman_gains_H_\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_10US \\\n');
Calculate_gain_dc(fileID, 10e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 10e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_11US \\\n');
Calculate_gain_dc(fileID, 1/96e3);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 1/96e3);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_14US \\\n');
Calculate_gain_dc(fileID, 1/72e3);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 1/72e3);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_16US \\\n');
Calculate_gain_dc(fileID, 16e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 16e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_20US \\\n');
Calculate_gain_dc(fileID, 20e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 20e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_21US \\\n');
Calculate_gain_dc(fileID, 1/48e3);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 1/48e3);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_25US \\\n');
Calculate_gain_dc(fileID, 25e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 25e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_28US \\\n');
Calculate_gain_dc(fileID, 1/36e3);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 1/36e3);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_31_25US \\\n');
Calculate_gain_dc(fileID, 31.25e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 31.25e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_32US \\\n');
Calculate_gain_dc(fileID, 32e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 32e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_42US \\\n');
Calculate_gain_dc(fileID, 1/24e3);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 1/24e3);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_50US \\\n');
Calculate_gain_dc(fileID, 50e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 50e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_62_5US \\\n');
Calculate_gain_dc(fileID, 62.5e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 62.5e-6);
fprintf(fileID,'\n');

fprintf(fileID, '\n#define KALMAN_GAIN_INIT_100US \\\n');
Calculate_gain_dc(fileID, 100e-6);
fprintf(fileID, ',\\\n');
Calculate_gain_ac(fileID, 100e-6);
fprintf(fileID,'\n');

fprintf(fileID, '#endif /* Kalman_gains_H_ */\n');
fclose(fileID);
clear fileID;

function Calculate_gain_dc(fileID, Ts)
omega = 2*pi*50;
harmonics = [0 2:1:50];

A = [1 0;
     0 0];
for k = harmonics(2:end)
A_diag = [cos(omega*Ts*k) -sin(omega*Ts*k);
          sin(omega*Ts*k)  cos(omega*Ts*k)];
A = blkdiag(A,A_diag);
end

Q = eye(2*length(harmonics));
R = 0.0000125/(Ts*Ts);

H = [];
for k = 1:length(harmonics)
    H = [H [1 0]];      
end

pp = idare(A',H',Q,R);
K_ricatti = pp*H'*(H*pp*H'+R)^(-1);

for i=1:size(K_ricatti,1)
    if i~=size(K_ricatti,1)
        fprintf(fileID, 'Kalman_gain_dc');
        fprintf(fileID,'[%u] = %d,\\\n', i-1, K_ricatti(i));
    else
        fprintf(fileID, 'Kalman_gain_dc');
        fprintf(fileID,'[%u] = %d', i-1, K_ricatti(i));
    end
end
end

function Calculate_gain_ac(fileID, Ts)
omega = 2*pi*50;
harmonics = [0 1:2:49];

A = [1 0;
     0 0];
for k = harmonics(2:end)
A_diag = [cos(omega*Ts*k) -sin(omega*Ts*k);
          sin(omega*Ts*k)  cos(omega*Ts*k)];
A = blkdiag(A,A_diag);
end

Q = eye(2*length(harmonics));
R = 0.005/(Ts*Ts);

H = [];
for k = 1:length(harmonics)
    H = [H [1 0]];      
end

pp = idare(A',H',Q,R);
K_ricatti = pp*H'*(H*pp*H'+R)^(-1);

for i=1:size(K_ricatti,1)
    if i~=size(K_ricatti,1)
        fprintf(fileID, 'Kalman_gain');
        fprintf(fileID,'[%u] = %d,\\\n', i-1, K_ricatti(i));
    else
        fprintf(fileID, 'Kalman_gain');
        fprintf(fileID,'[%u] = %d', i-1, K_ricatti(i));
    end
end
end