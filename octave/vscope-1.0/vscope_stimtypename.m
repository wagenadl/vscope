function name = vscope_stimtypename(num)
% VSCOPE_STIMTYPENAME - Lookup stimulus type enum
%   name = VSCOPE_STIMTYPENAME(num) returns string representation of 
%   stimulus type given a number. 
%   See also VSCOPE_STIMTYPENUMBER.

% Keep aligned with base/enums.h!
switch num
  case 0
    name = 'TTL';
  case 1
    name = 'Square';
  case 2
    name = 'Biphasic';
  case 3
    name = 'Ramp';
  case 4
    name = 'Sine';
  otherwise
    error('Illegal stimulus type number')
end
