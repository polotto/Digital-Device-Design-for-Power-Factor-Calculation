function [ t ] = getSubTime( t_val, V_val )
%getSubTime Adiquire os dados de somente um período
%   Usado para retirar um período de dados do vetor t_val com base no
%   cruzamento de zero de V_val, ou seja, cruzamentos de zero da tensão.

[t_down, t_up] = zeroCross(V_val);
disp(['Bordas de descida: ' num2str(t_down)]);
disp(['Bordas de subida: ' num2str(t_up)]);

if mod(length(t_down),2)==0
    t = subVector(t_val, t_down(1), t_down(2));
elseif mod(length(t_up),2) == 0
    t = subVector(t_val, t_up(1), t_up(2));
end

end

