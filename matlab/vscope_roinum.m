function n = vscope_roinum(str)
% VSCOPE_ROINUM - Convert ROI string to number
%    n = VSCOPE_ROIID(str) returns the number for named ROI str (e.g., 2
%    for 'b', 27 for 'aa'). If n is a cell array, str will be a numeric array.

if iscell(str)
  n  = zeros(size(str));
  for k = 1:prod(size(str))
    n(k) = vscope_roinum(str{k});
  end
else
  if length(str)==1
    n = 1 + str - 'a';
  else
    n = 27 + 26*(str(1) - 'a') + str(2) - 'a';
  end
end
