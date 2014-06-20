function str = vscope_roiid(n)
% VSCOPE_ROIID - Convert ROI number to string
%    str = VSCOPE_ROIID(n) returns the ID for ROI n (e.g., 'b' for n=2; 
%    'aa' for n=27). If n is an array, str will be a cell array.

if prod(size(n))>1
  str = cell(size(n));
  for k = 1:prod(size(n))
    str{k}  = vscope_roiid(n(k));
  end
else
  n = n-1;
  if n<26
    str = char('a' + n);
  else
    str = char([ 'a' + floor(n/26-1), 'a' + mod(n, 26) ]);
  end
end
