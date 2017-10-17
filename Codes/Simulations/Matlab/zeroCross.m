function [ t_down, t_up ] = zeroCross( X )
%zeroCross Detecção de cruzamentos por zero
%   Detecta cruzamentos e os divide em bordas de descidas e subidas do
%   vetor X
t_down=[];
t_up=[];
%for i=1:length(X)-1
%    if (X(i)>0 && X(i+1)<0) || (X(i)<0 && X(i+1)>0)
%        t(end+1) = i;
%    end
%end
for i=1:length(X)-1
    if (X(i)>=0 && X(i+1)<=0)
        t_down(end+1) = i;
    end
    if (X(i)<=0 && X(i+1)>=0)
        t_up(end+1) = i;
    end
end

end
