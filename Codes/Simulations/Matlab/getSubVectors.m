function [ V, I, t ] = getSubVectors( V_val, I_val, t_val )
%getSubVectors Adiquire os dados de somente um período
%   Usado para retirar um período de dados dos vetores medidos com base no
%   cruzamento de zero de V_val, ou seja, cruzamentos de zero da tensão.

[t_down, t_up] = zeroCross(V_val);
if mod(length(t_down),2)==0
    V = subVector(V_val, t_down(1), t_down(2));
    I = subVector(I_val, t_down(1), t_down(2));
    t = subVector(t_val, t_down(1), t_down(2));
elseif mod(length(t_up),2) == 0
    V = subVector(V_val, t_up(1), t_up(2));
    I = subVector(I_val, t_up(1), t_up(2));
    t = subVector(t_val, t_up(1), t_up(2));
end

end

