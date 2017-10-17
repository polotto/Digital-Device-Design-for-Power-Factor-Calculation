function [ Int ] = myTrapz( X, Y )
%myTrapz Função para cálculo da integral numérica de uma função discreta
%   Efetua uma aproximação usando o método de soma de trapézios.
Int = 0;
if length(X) == length(Y)
    for i=1:length(Y)-1
        H = X(i+1)-X(i);
        B = Y(i);
        b = Y(i+1);
        Int = Int + (B+b)*H/2;
    end
else
    error('X e Y precisam possuir o mesmo tamanho.');
%disp(Int);
%Int = Int*(h/2);
end

