function [ z ] = myFFT( x, nfft )
%myFFT Summary of this function goes here
%   Detailed explanation goes here
%
% nfft=2^ceil(log2(L));
N=length(x);
z=zeros(1,nfft);
Sum=0;
for k=1:nfft
    for jj=1:N
        Sum=Sum+x(jj)*exp(-2*pi*1i*(jj-1)*(k-1)/nfft);
    end
z(k)=Sum;
Sum=0;% Reset
end
return

end

