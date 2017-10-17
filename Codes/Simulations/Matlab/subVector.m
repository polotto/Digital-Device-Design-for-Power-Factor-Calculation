function [ Y ] = subVector( X, i1, i2 )
%subVector Retorna um sub-vetor
%   Retorna um sub-vetor compreendido entre dois indices.
if i2 > length(X)
    error('i2 deve possuir valor menor/igual ao de X');
elseif i1 >= i2
    error('i1 deve ser menor que i2');
end
Y=[];
for i = i1:i2
    Y(end+1)=X(i);
end
end
