clc
clear
Fs = 1000;                    % Sampling frequency
T = 1/Fs;                     % Sample time
L = 1000;                     % Length of signal
t = (0:L-1)*T;                % Time vector
% Sum of a 50 Hz sinusoid and a 120 Hz sinusoid
x = 127*sqrt(2)*sin(2*pi*60*t) + 20*sin(2*pi*120*t) + 10*sin(2*pi*220*t);
% x = 127*sqrt(2)*sin(2*pi*50*t);
% NFFT = 2^nextpow2(L); % Next power of 2 from length of y
NFFT=2^ceil(log2(L));
Y=myFFT(x, NFFT)/L;
f = Fs/2*linspace(0,1,NFFT/2+1);

% Plot single-sided amplitude spectrum.
plot(f,2*abs(Y(1:NFFT/2+1)))
title('Single-Sided Amplitude Spectrum of y(t)')
xlabel('Frequency (Hz)')
ylabel('|Y(f)|')