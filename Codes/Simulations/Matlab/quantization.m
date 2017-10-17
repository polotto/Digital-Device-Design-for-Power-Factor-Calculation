function [ Y, Ysample, E ] = quantization(X, resolutionBits, maxInput, Tsample )
%quantization Faz a quantização do vetor X
%   Detailed explanation goes here
if mod(resolutionBits,2)~=0
    error('resolutionBits deve ser multiplo de 2');
end

X_int = zeros(1,length(X));
stepSize = (2^resolutionBits-1)/maxInput;

%classification stage (forward quantization stage)
for i=1:length(X)
    X_int(i) = round(X(i)*stepSize);
end

%reconstruction stage (inverse quantization stage)
Y = zeros(1,length(X));
for i=1:length(X_int)
    Y(i) = X_int(i)/stepSize;
end

%Erro de quantização
E = [];
for i=1:length(X)
    E(end+1) = X(i)-Y(i);
end

%Amostragem do sinal
Ysample = decimate(Y, Tsample);