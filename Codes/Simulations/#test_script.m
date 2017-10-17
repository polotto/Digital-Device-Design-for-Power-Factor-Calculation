disp('Script para cálculo do fator de potência pelo método numérico.');
disp('Autor: Ângelo Polotto');
%Dados vindos da simulação (Simulink
%T_MAX = 4*pi;
%Vsim = sin(0:0.1:T_MAX);
%Isim = sin(0:0.1:T_MAX);
%tsim = 0:0.1:T_MAX;
Vsim_val = Vsim.signals.values;
Isim_val = Isim.signals.values;
tsim_val = Vsim.time;
%*************************

[t_down, t_up] = zeroCross(Vsim_val);
if mod(length(t_down),2)==0
    V = subVector(Vsim_val, t_down(1), t_down(2));
    I = subVector(Isim_val, t_down(1), t_down(2));
    t = subVector(tsim_val, t_down(1), t_down(2));
elseif mod(length(t_up),2) == 0
    V = subVector(Vsim_val, t_down(1), t_down(2));
    I = subVector(Isim_val, t_down(1), t_down(2));
    t = subVector(tsim_val, t_down(1), t_down(2));
end
T = t(end)-t(1);

disp(['Bordas de descida: ' num2str(t_down)]);
disp(['Bordas de subida: ' num2str(t_up)]);
disp(['Período: ' num2str(T) 's - Frequencia: ' num2str(1/T) ' Hz']);
disp('********* Usando Trapz Matlab: *********');
P = trapz(t, V.*I)/T;
Vrms = sqrt(trapz(t, V.^2)/T);
Irms = sqrt(trapz(t, I.^2)/T);
S = Vrms.*Irms;
fp = P/S;
disp(['P = ' num2str(P) ' W']);
disp(['S = ' num2str(S) ' VA - Vrms = ' num2str(Vrms) ' V - Irms = ' num2str(Irms) ' A']);
disp(['fp = ' num2str(fp)]);

disp('********* Usando MyTrapz: *********');
P = myTrapz(t, V.*I)/T;
Vrms = sqrt(myTrapz(t, V.^2)/T);
Irms = sqrt(myTrapz(t, I.^2)/T);
S = Vrms.*Irms;
fp = P/S;
disp(['P = ' num2str(P) ' W']);
disp(['S = ' num2str(S) ' VA - Vrms = ' num2str(Vrms) ' V - Irms = ' num2str(Irms) ' A']);
disp(['fp = ' num2str(fp)]);

plot(t,V,t,I);