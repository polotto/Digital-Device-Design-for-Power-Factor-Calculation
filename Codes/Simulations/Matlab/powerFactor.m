function [ fp, S, P, Vrms, Irms ] = powerFactor(t, V, I)
%powerFactor Função para calculo do fator de potência
%   Essa função retorna o fator de potência (fp), potência ativa(P) e
%   potêcia aparente(S) com base no tempo(t), tensão(V) e corrente(I). 
%   Todos os valores de entrada devem ser vetores vindos da amostragem.
%   Esses valores devem também serem correspondentes a um período de
%   amostra, para estimativa correta dos resultados.

if length(V) == length(I) && length(V) == length(t)
    T = t(end)-t(1);
    P = myTrapz(t, V.*I)/T;
    Vrms = sqrt(myTrapz(t, V.^2)/T);
    Irms = sqrt(myTrapz(t, I.^2)/T);
    S = Vrms.*Irms;
    fp = P/S;
    %disp(['P = ' num2str(P) ' W']);
    %disp(['S = ' num2str(S) ' VA - Vrms = ' num2str(Vrms) ' V - Irms = ' num2str(Irms) ' A']);
    %disp(['fp = ' num2str(fp)]);
else
    error('V, I e t devem possuir o mesmo tamanho.');
end
end

