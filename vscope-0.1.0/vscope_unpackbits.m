function y = vscope_unpackbits(x)
% VSCOPE_UNPACKBITS - Unpack a bit array stored in uint32 to booleans
%   y = VSCOPE_UNPACKBITS(x) where X is an Nx1 or 1xN vector of uint32s
%   returns a Nx32 or 32xN matrix of booleans with the bits of X
%   unpacked.

N=length(x);
S=size(x);
x=x(:);

y=logical(zeros(N,32));
msk=1;
for k=1:32
  y(:,k) = bitand(x,uint32(msk))>0;
  msk=msk*2;
end
if S(1)==1
  y=y';
end
