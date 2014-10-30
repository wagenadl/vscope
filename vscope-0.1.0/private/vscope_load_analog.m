function dat = vscope_load_analog(ifn, info, channel)
% VSCOPE_LOAD_ANALOG - Load analog data from vscope
%    dat = VSCOPE LOAD_ANALOG(ifn, info) loads analog data from 
%    a '-analog.dat' file. INFO must be result of previous VSCOPE_LOAD_INFO.
%    Result is TxC array of analog data, in millivolts.
%    dat = VSCOPE_LOAD_ANALOG(ifn, info, channel) loads data from a single
%    channel only. CHANNEL may be a string, specifying a channel ID, or a
%    number (counting from 1).
%    dat = VSCOPE_LOAD_ANALOG(ifn) simply loads the raw data without
%    scaling or reshaping.

ifn = strrep(ifn,'.xml','-analog.dat'); % for ease of use.

ifd = fopen(ifn,'rb');
if nargin==1
  dat = fread(ifd,[1 inf],'int16')';
elseif nargin==2
  C = length(info.channo);
  dat = fread(ifd,[1 inf],'int16');
  L = length(dat);
  dat = reshape(dat,[C L/C]);
  for k=1:length(info.chunk)
    i0 = info.chunk(k).istart;
    i1 = min(info.chunk(k).iend,L/C);
    for c=1:C
      dat(c,i0:i1) = dat(c,i0:i1) * info.chunk(k).scale(c) ...
	  + info.chunk(k).offset(c);
    end
  end
  dat = dat';
elseif nargin==3
  C = length(info.channo);
  if ischar(channel)
    c = strmatch(channel,info.chanid,'exact');
    if isempty(c)
      error('Unknown channel ID');
    end
  else
    c = channel;
  end
  dat = fread_interleaved(ifd,c,C,'int16');
  L = length(dat);
  for k=1:length(info.chunk)
    i0 = info.chunk(k).istart;
    i1 = min(info.chunk(k).iend,L);
    dat(i0:i1) = dat(i0:i1) * info.chunk(k).scale(c) ...
	+ info.chunk(k).offset(c);
  end
else
  error 'Bad argument count'
end
fclose(ifd);
