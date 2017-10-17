resolutionBits=4
maxInput=5
f=60
omega=2*%pi*f
t=0:0.000001:0.02
X=5*sin(omega*t)
X_int=zeros(1,length(X))
stepSize = (2^resolutionBits-1)/maxInput
for i=1:length(X)
    X_int(i)=round(X(i)*stepSize);
end
Y=zeros(1,length(X));
for i=1:length(X_int)
    Y(i)=X_int(i)/stepSize;
end
E=zeros(1,length(X));
for i=1:length(X)
    E(i)=X(i)-Y(i);
end
plot(t,X,t,Y,t,E)
xtitle('Erro de Quantizacao - Resol.=4bits; Vmax=5V','Tempo (segundos)','X[n]/Y[n]/E[n]')
legend(['X[n] - Sinal Amostrado';'Y[n] - Sinal Quantizado'; 'E[n] - Erro'])
