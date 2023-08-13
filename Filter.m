% First-order Low-Pass Filter Discretization
%
% Control Systems Academy team
%
% 6/25/2017

clc; format compact
close all
clear all

% Continuous time filter
Ts = 6e-3;
N=128;
alfa = 1/N;
Hd = tf([alfa],[1 alfa-1],Ts)
bodeplot(Hd);

% Recursive representation of the CIC filter
% G(Z)=(1/N)*(1-z^(-N))/(1-z^-1)=(1/N)*B(z)/A(z)
B = [1,zeros(1,N-1),-1]/N;
A = [1,-1];

hold on
Hd2 = tf(B,A,Ts)
h = bodeplot(Hd2);
p = getoptions(h);
p.FreqUnits = 'Hz';
setoptions(h,p);
xlim([0.1 1/Ts])
