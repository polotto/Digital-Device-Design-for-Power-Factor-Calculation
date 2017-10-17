L=0.001
R=1
f=60
omega=2*%pi*f
t=0:0.000001:0.02
Vp=127*sqrt(2)
V_in=Vp*sin(omega*t)
V=Vp+%i*0
I=V/(R+%i*omega*L)
rho=abs(I)
phi=atan(imag(I),real(I))
I_in=rho*sin(omega*t+phi)
close()
plot(t,V_in,t,I_in,'--')
xtitle('Curvas de Tensao e Corrente (Entrada)','Tempo (segundos)','Tensao (Volts) / Corrente (Amperes)')
legend(['v(t)- Tensao';'i(t) - Corrente'])
