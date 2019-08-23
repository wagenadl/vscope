function num = vscope_stimtypenumber(name)
% VSCOPE_STIMTYPENUMBER - Lookup stimulus type enum
%   num = VSCOPE_STIMTYPENUMBER(name) returns numeric representation of 
%   stimulus type given a string. 
%   See also VSCOPE_STIMTYPENAME.

% Keep aligned with base/enums.h!
switch name
  case 'TTL'
    num = 0;
  case 'Square'
    num = 1;
  case 'Biphasic'
    num = 2;
  case 'Ramp'
    num = 3;
  case 'Sine'
    num = 4;
  otherwise
    error('Illegal stimulus type name')
end
