function dat=vscope_load(ifn,wht,frmno,camno)
% VSCOPE_LOAD - Load data from vscope
%    dat = VSCOPE_LOAD(ifn) loads data from a single trial from vscope.
%    IFN may be an "-analog.dat" file, a "-digital.dat" file, or a 
%    "-ccd.dat" file, in which case raw data is read.
%    Alternatively, IFN may be a ".xml" file, in which case an entire
%    trial is read. In that case, DAT will contain fields:
%     - info: a structure containing overview info of this trial
%     - settings: a structure of trial settings
%     - analog: analog data
%     - digital: digital data
%     - ccd: vsd data
%    IFN may also be a number N, in which case the file NNN.xml is loaded,
%    or a cell array {dir, n} in which case the file DIR/NNN.XML is loaded.
%    dat = VSCOPE_LOAD(ifn,letters), where IFN is a ".xml" file and letters
%      is a sequence containing some of the letters 'a', 'd', 'c', or 'r',
%      loads the .xml and only some data (analog, digital, ccd, and/or rois).
%      Add the letter 'j' to flip the ccd images for John's scope.
%    dat = VSCOPE_LOAD(ifn,info), where IFN is an "-analog.dat" file and INFO
%      is the "analog.info" field from a previous VSCOPE_LOAD, loads the raw
%      data and returns it in the proper shape, correctly scaled.
%    dat = VSCOPE_LOAD(ifn,info,ch), where IFN is an "-analog.dat" file
%      and INFO is the "analog.info" field from a previous VSCOPE_LOAD, 
%      loads the raw data and returns one channel, correctly scaled.
%    dat = VSCOPE_LOAD(ifn,info), where IFN is a "-ccd.dat" file and INFO
%      is the "ccd.info" field from a previous VSCOPE_LOAD, loads the raw
%      data and returns it in the proper shape.
%    dat = VSCOPE_LOAD(ifn,info,frmno) loads CCD data from a single frame, 
%    dat = VSCOPE_LOAD(ifn,info,frmno, camno) loads CCD data from a single
%      camera and from a single frame. Note that frames are counted from 
%      zero, but cameras are counted from one. The images are properly 
%      flipped. That is, the data for camera 1 is upside downed.

if iscell(ifn)
  ifn = sprintf('%s/%03i.xml',ifn{1}, ifn{2});
elseif ~ischar(ifn)
  ifn = sprintf('%03i.xml',ifn);
end

ifd=fopen(ifn,'r');
if ifd<0
  error([ 'vscope_load: Cannot read "' ifn '"']);
end

if endswith(ifn,'-analog.dat')
  fclose(ifd);
  switch nargin
    case 1
      dat = vscope_load_analog(ifn);
    case 2
      dat = vscope_load_analog(ifn,wht);
    case 3
      dat = vscope_load_analog(ifn,wht,frmno);
  end
elseif endswith(ifn,'-digital.dat')
  dat=fread(ifd,[1 inf],'uint32');
  dat=dat(:);
  fclose(ifd);
elseif endswith(ifn,'-ccd.dat');
  fclose(ifd);
  switch nargin
    case 1
      dat = vscope_load_ccd(ifn);
    case 2
      dat = vscope_load_ccd(ifn,wht);
    case 3
      dat = vscope_load_ccd(ifn,wht,frmno);
    case 4
      dat = vscope_load_ccd(ifn,wht,frmno,camno);
  end
elseif endswith(ifn,'-roi.xml')
  dat = vsdl_rois(ifd);
  ifclose(ifd);
elseif endswith(ifn,'.xml') 
  % Structured
  if nargin<2
    wht='adc';
  elseif isempty(wht)
    wht=' '; % This prevents warning about empty==scalar comparisons
  end
  while 1
    str = fgetl(ifd);
    if ~ischar(str)
      break;
    end
    if ~isempty(strfind(str,'<settings>'))
      dat.settings = vsdl_settings(ifd);
    elseif ~isempty(strfind(str,'<info'))
      dat.info = vsdl_info(str, ifd);
    elseif ~isempty(strfind(str,'<analog'))
      dat.analog = vsdl_analog(str,ifd,ifn, any(wht=='a'));
    elseif ~isempty(strfind(str,'<digital'))
      dat.digital = vsdl_digital(str,ifd,ifn, any(wht=='d'));
    elseif ~isempty(strfind(str,'<ccd'))
      johnsscope = any(wht=='j');
      dat.ccd = vsdl_ccd(str,ifd,ifn, any(wht=='c'), johnsscope);
    elseif ~isempty(strfind(str,'<rois'))
      dat.rois = vsdl_rois(ifd); 
    end
  end
  fclose(ifd);
  roifn = strrep(ifn,'.xml','-rois.xml');
  if exist(roifn)
    ifd = fopen(roifn,'r');
    dat.rois = vsdl_rois(ifd);
    fclose(ifd);
  end
else
  fclose(ifd);
  error(['vscope_load: Unknown filetype "' ifn '"']);
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [a,b]=vsdl_splitstr(str,splt,gobble)
idx=strfind(str,splt);
if isempty(idx)
  a=str;
  b='';
else
  a=str(1:idx(1)-1);
  b=str(idx(1)+1:end);
end
if nargin>=3
  if gobble
    while ~isempty(b)
      if b(1)==splt
	b=b(2:end);
      else
	break;
      end
    end
  end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function kv = vsdl_params(str)
% VSDL_PARAMS - Return (key,value) pairs for xml parameters
% STR must be of the form '<elt key1="value1" key2="value2" ... >'
% STR may close with '>' or '/>'.
% The result is a structure:
%   kv.elt
%   kv.keys {Nx1}
%   kv.values {Nx1}
%   kv.single - 1 if STR closes with '/>', else 0.

while ~isempty(str)
  if str(1)==' '
    str=str(2:end);
  else
    break;
  end
end
while ~isempty(str)
  if str(end)==' '
    str=str(1:end-1);
  else
    break;
  end
end

kv.elt='';
kv.keys={};
kv.values={};
kv.single=nan;
if length(str>2)
  if str(end-1)=='/'
    kv.single=1;
  else
    kv.single=0;
  end
end

if isempty(str)
  return
end
if str(1)~='<'
  return;
end
if str(2)=='/'
  return;
end

while ~isempty(str)
  if str(1)=='<'
    [kv.elt,str] = vsdl_splitstr(str(2:end),' ',1);
  else
    [k,str] = vsdl_splitstr(str,'=');
    [d,str] = vsdl_splitstr(str,'"');
    if ~isempty(str)
      [v,str] = vsdl_splitstr(str,'"');
      [d,str] = vsdl_splitstr(str,' ');
      kv.keys{end+1}=k;
      kv.values{end+1}=v;
    end
  end
end
kv.keys=kv.keys';
kv.values=kv.values';

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function v = vsdl_getval(kv,k)
% VSDL_GETVAL
%   v = VSDL_GETVAL(kv, k) reads parameter K from the keyval structure KV.
idx = strmatch(k,kv.keys,'exact');
if isempty(idx)
  v='';
else
  v=kv.values{idx(1)};
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function t_ms = vsdl_time(str)
% VSDL_TIME - Converts a string to a time in milliseconds
t_ms = sscanf(str,'%g');
if endswith(str,'ns')
  t_ms = t_ms/1e6;
elseif endswith(str,'us')
  t_ms = t_ms/1e3;
elseif endswith(str,'ms')
  ;
elseif endswith(str,' s')
  t_ms = t_ms*1e3;
elseif endswith(str,' min')
  t_ms = t_ms*1e3*60;
else
  fprintf(1,'Warning: unsure that "%s" is a valid time.\n',str);
end

function f_hz = vsdl_freq(str)
% VSDL_FREQ - Converts a string to a frequency in hertz
f_hz = sscanf(str,'%g');
if endswith(str,'MHz')
  f_hz = f_hz*1e6;
elseif endswith(str,'kHz')
  f_hz = f_hz*1e3;
elseif endswith(str,' Hz')
  ;
else
  fprintf(1,'Warning: unsure that "%s" is a valid frequency.\n',str);
end

function v_mv = vsdl_mvolt(str)
% VSDL_MVOLT - Converts a string to a voltage in millivolts
v_mv = sscanf(str,'%g');
if endswith(str,'nV')
  v_mv = v_mv/1e6;
elseif endswith(str,'uV')
  v_mv = v_mv/1e3;
elseif endswith(str,'mV')
  ;
elseif endswith(str,'kV')
  v_mv = v_mv*1e6;
elseif endswith(str,' V')
  v_mv = v_mv*1e3;
else
  fprintf(1,'Warning: unsure that "%s" is a valid voltage.\n',str);
end

function i_na = vsdl_namp(str)
% VSDL_namp - Converts a string to a current in nanoamps
i_na = sscanf(str,'%g');
if endswith(str,'fA')
  i_na = i_na/1e6;
elseif endswith(str,'pA')
  i_na = i_na/1e3;
elseif endswith(str,'nA')
  ;
elseif endswith(str,'uA')
  i_na = i_na*1e3;
elseif endswith(str,'mA')
  i_na = i_na*1e6;
elseif endswith(str,' A')
  i_na = i_na*1e9;
else
  fprintf(1,'Warning: unsure that "%s" is a valid current.\n',str);
end

function b = vsdl_bool(str)
if strcmp(str,'true') || strcmp(str,'yes') || strcmp(str,'1')
  b=1;
else
  b=0;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function info = vsdl_info(str, ifd)
kv = vsdl_params(str);
info.dur_ms = vsdl_time(vsdl_getval(kv,'duration'));
info.stim = vsdl_bool(vsdl_getval(kv,'stim'));
info.expt = vsdl_getval(kv,'expt');
info.trial = atoi(vsdl_getval(kv,'trial'));
info.type = vsdl_getval(kv,'type');
info.starts = [vsdl_getval(kv,'date') '.' vsdl_getval(kv,'time')];
info.ends = [vsdl_getval(kv,'enddate') '.' vsdl_getval(kv,'endtime')];
if isempty(findstr(str,'/>'))
  % Multi line info element
  info.trials.id=[];
  info.trials.ascan=[];
  info.trials.dscan=[];
  while 1
    str = fgetl(ifd);
    if ~ischar(str)
      break;
    end
    if strfind(str,'</info')
      break;
    else
      kv = vsdl_params(str);
      if strcmp(kv.elt,'trial')
	id = vsdl_getval(kv,'id');
	ascan = vsdl_getval(kv,'ascan');
	dscan = vsdl_getval(kv,'dscan');
	info.trials.id(end+1) = atoi(id);
	info.trials.ascan(end+1) = atoi(ascan);
	info.trials.dscan(end+1) = atoi(dscan);
      end
    end
  end
end
  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function set = vsdl_settings(ifd)
set.vsdl_x=0;
while 1
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  end
  if strfind(str,'</settings')
    break;
  else
    kv = vsdl_params(str);
    if strcmp(kv.elt,'category')
      set = vsdl_addcatg(set,vsdl_getval(kv,'id'),kv.single, ifd, kv.elt);
    end
  end
end
set = rmfield(set,'vsdl_x');

function set = vsdl_addcatg(set,id,sngl, ifd, elt)
cls=['</' elt];
sub.vsdl_x=0;
while ~sngl
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  end
  if strfind(str,cls)
    break;
  else
    kv = vsdl_params(str);
    if strcmp(kv.elt,'category') || strcmp(kv.elt,'array') || ...
	  strcmp(kv.elt,'elt')
      sub = vsdl_addcatg(sub,vsdl_getval(kv,'id'),kv.single, ifd, kv.elt);
    elseif strcmp(kv.elt,'pval')
      sub = setfield(sub, vsdl_fieldname(vsdl_getval(kv,'id')), ...
	  vsdl_getval(kv,'value'));
    end
  end
end
sub = rmfield(sub,'vsdl_x');
set = setfield(set,id,sub);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function id = vsdl_fieldname(id)
id = strrep(id, '-', '_');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function ana = vsdl_analog(str,ifd,ifn,getdat)
kv = vsdl_params(str);
ntypebyt = atoi(vsdl_getval(kv,'typebytes'));
nchans  = atoi(vsdl_getval(kv,'channels'));
nscans  = atoi(vsdl_getval(kv,'scans'));
typ = vsdl_getval(kv,'type');
ana.info.rate_hz = vsdl_freq(vsdl_getval(kv,'rate'));
ana.info.channo = zeros(nchans,1)+nan;
ana.info.chanid = cell(nchans,1);
ana.info.typebytes = ntypebyt;
ana.info.chunk = struct('istart',1,'iend',inf,'scale',ones(nchans,1));
ichunk = 1;
while 1
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  end
  if ~isempty(findstr(str,'</analog'))
    break;
  end
  if ~isempty(findstr(str,'<channel'))
    kv = vsdl_params(str);
    idx = atoi(vsdl_getval(kv,'idx'));
    chn = atoi(vsdl_getval(kv,'chn'));
    id = vsdl_getval(kv,'id');
    if ~isempty(id)
      ana.info.channo(idx+1) = chn;
      ana.info.chanid{idx+1} = id;
    end
    [scl,uni] = vsdl_getscale(vsdl_getval(kv,'scale'));
    ana.info.units{idx+1} = uni;
    ana.info.chunk(ichunk).scale(idx+1) = scl;
  end
  if ~isempty(findstr(str,'<scale'))
    kv = vsdl_params(str);
    ss = atoi(vsdl_getval(kv,'startscan'));
    if ss>0
      ana.info.chunk(ichunk).iend = ss;
      ichunk=ichunk+1;
    end
    ana.info.chunk(ichunk).istart = ss+1;
    ana.info.chunk(ichunk).iend = inf;
    ana.info.chunk(ichunk).scale = ones(nchans,1);
  end
end

if getdat
  ana.dat = vscope_load_analog(ifn,ana.info);
else
  ana.dat = [];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function dig = vsdl_digital(str,ifd,ifn, getdat)
kv = vsdl_params(str);
ntypebyt = atoi(vsdl_getval(kv,'typebytes'));
nscans  = atoi(vsdl_getval(kv,'scans'));
typ = vsdl_getval(kv,'type');
dig.info.rate_hz = vsdl_freq(vsdl_getval(kv,'rate'));
dig.info.lineid = {};
dig.info.typebytes = ntypebyt;

while 1
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  end
  if ~isempty(findstr(str,'</digital'))
    break;
  end
  if ~isempty(findstr(str,'<line'))
    kv = vsdl_params(str);
    idx = atoi(vsdl_getval(kv,'idx'));
    id = vsdl_getval(kv,'id');
    dig.info.lineid{idx+1} = id;
  end
end
dig.info.lineid=dig.info.lineid';

if getdat
  ifd = fopen(strrep(ifn,'.xml','-digital.dat'),'rb');
  if ifd==0
    error('vscope_load: Cannot read digital data');
  end
  dig.dat = fread(ifd,[nscans 1],['*' typ]);
  fclose(ifd);
else
  dig.dat=[];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function ccd = vsdl_ccd(str,ifd,ifn, getdat, johnsscope)
if nargin<5
  johnsscope = 0;
end

kv = vsdl_params(str);
ntypebyt = atoi(vsdl_getval(kv,'typebytes'));
nframes  = atoi(vsdl_getval(kv,'frames'));
npar  = atoi(vsdl_getval(kv,'parpix'));
nser  = atoi(vsdl_getval(kv,'serpix'));
ncams  = atoi(vsdl_getval(kv,'cameras'));
rate = vsdl_getval(kv,'rate');

flipx = zeros(ncams,1); flipx(1) = johnsscope;
flipy = zeros(ncams,1); flipy(1) = ~johnsscope;
for c=1:ncams
  ccd.info.xform{c}.ax=1;
  ccd.info.xform{c}.ay=1;
  ccd.info.xform{c}.bx=0;
  ccd.info.xform{c}.by=0;
end

typ = vsdl_getval(kv,'type');
if isempty(rate)
    ccd.info.rate_hz = cell(ncams,1);
    ccd.info.delay_ms = cell(ncams,1);
else
    ccd.info.rate_hz = vsdl_freq(rate);
    ccd.info.delay_ms = vsdl_time(vsdl_getval(kv,'delay'));
end
ccd.info.camid = cell(ncams,1);
ccd.info.ncams = ncams;
if isnan(npar)
  ccd.info.pix=cell(ncams,1);
  ccd.info.nframes=cell(ncams,1);
else
  ccd.info.pix = [nser npar];
  ccd.info.nframes = nframes;
end

idx0=0;
while 1
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  elseif ~isempty(findstr(str,'</ccd'))
    break;
  elseif ~isempty(findstr(str,'<camera'))
    kv = vsdl_params(str);
    idx = atoi(vsdl_getval(kv,'idx'));
    if isnan(idx)
        idx=idx0;
        idx0 = idx0 + 1;
    end
    id = vsdl_getval(kv,'name');
    ccd.info.camid{idx+1} = id;
    if isnan(npar)
      % New style
      cnframes  = atoi(vsdl_getval(kv,'frames'));
      if cnframes==0
	ncams = ncams - 1;
	ccd.info.ncams = ccd.info.ncams - 1;
	idx0 = idx0 - 1;
	continue;
      end
      cnpar  = atoi(vsdl_getval(kv,'parpix'));
      cnser  = atoi(vsdl_getval(kv,'serpix'));
      ccd.info.pix{idx+1} = [cnser cnpar];
      ccd.info.nframes{idx+1} = cnframes;
      typ1 = vsdl_getval(kv,'type');
      if ~isempty(typ) && ~strcmp(typ,typ1)
        error('vscope_load: cannot deal with unequal pixel types');
      end
      typ=typ1;
    
      if isempty(rate)
          % Newer style (r100+)
          ccd.info.rate_hz{idx+1} = vsdl_freq(vsdl_getval(kv,'rate'));
          ccd.info.delay_ms{idx+1} = vsdl_time(vsdl_getval(kv,'delay'));
          while 1
              str = fgetl(ifd);
              if ~ischar(str)
                  break;
              elseif ~isempty(findstr(str,'</camera'))
                  break;
              elseif ~isempty(findstr(str,'transform'))
                kv = vsdl_params(str);
                flipx(idx+1) = atoi(vsdl_getval(kv,'ax'))<0;
                flipy(idx+1) = atoi(vsdl_getval(kv,'ay'))<0;
		ccd.info.xform{idx+1}.ax = atoi(vsdl_getval(kv,'ax'));
		ccd.info.xform{idx+1}.ay = atoi(vsdl_getval(kv,'ay'));
		ccd.info.xform{idx+1}.bx = atoi(vsdl_getval(kv,'bx'));
		ccd.info.xform{idx+1}.by = atoi(vsdl_getval(kv,'by'));
              end
          end
      else
	fx = vsdl_getval(kv,'flipx');
	if ~isempty(fx)
	  flipx(idx+1) = vsdl_bool(fx);
	end
	fy = vsdl_getval(kv,'flipy');
	if ~isempty(fy)
	  flipy(idx+1) = vsdl_bool(fy);
	end
	if flipx(idx+1)
	  ccd.info.xform{idx+1}.ax=-1;
	  ccd.info.xform{idx+1}.by=cnser;
	end
	if flipy(idx+1)
	  ccd.info.xform{idx+1}.ay=-1;
	  ccd.info.xform{idx+1}.by=cnpar;
	end
      end
    end
  end
end
ccd.info.flipx = flipx;
ccd.info.flipy = flipy;

if getdat && ncams>0
  ifd = fopen(strrep(ifn,'.xml','-ccd.dat'),'rb');
  if ifd==0
    error('vscope_load: Cannot read ccd data');
  end
  
  if isnan(npar)
    % New style, with information for individual cameras
    nser = ccd.info.pix{1}(1);
    npar = ccd.info.pix{1}(2);
    nframes = ccd.info.nframes{1};
    for k=2:ncams
      if ccd.info.pix{k}(1)~=nser || ccd.info.pix{k}(2)~=npar
	error('vscope_load: cannot deal with unequal frame sizes');
      end
      if ccd.info.nframes{k}~=nframes
	error('vscope_load: cannot deal with unequal frame counts');
      end
    end
    newstyle = 1;
    perm=[2 1 4 3];
    siz=[nser npar nframes ncams];
  else
    newstyle = 0;
    perm=[2 1 3 4];
    siz=[nser npar ncams nframes];
  end
  ccd.dat = fread(ifd,[nser*npar ncams*nframes],typ);
  ccd.dat = reshape(ccd.dat,siz);
  ccd.dat = permute(ccd.dat,perm);
  
  for k=1:ncams
    if flipy(k)
      ccd.dat(:,:,k,:) = ccd.dat(end:-1:1,:,k,:);
      ccd.info.xform{k}.ay=-ccd.info.xform{k}.ay;
      ccd.info.xform{k}.by=ccd.info.xform{k}.by ...
	  - ccd.info.xform{k}.ay*ccd.info.pix{k}(2);
    end
    if flipx(k)
      ccd.dat(:,:,k,:) = ccd.dat(:,end:-1:1,k,:);
      ccd.info.xform{k}.ax=-ccd.info.xform{k}.ax;
      ccd.info.xform{k}.bx=ccd.info.xform{k}.bx ...
	  - ccd.info.xform{k}.ax*ccd.info.pix{k}(1);
    end
  end
  
  fclose(ifd);
else
  ccd.dat=[];
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function rr = vsdl_rois(ifd)
rr=cell(0,0);
while 1
  str = fgetl(ifd);
  if ~ischar(str)
    break;
  end
  if ~isempty(strfind(str,'</rois>'))
    break;
  end
  if ~isempty(strfind(str,'<roi '))
    % Starting a new ROI
    kv = vsdl_params(str);
    id = atoi(vsdl_getval(kv,'id'));
    n = vsdl_getval(kv,'n');
    if isempty(n)
      % This is xyrra
      x0 = str2num(vsdl_getval(kv,'x0'));
      y0 = str2num(vsdl_getval(kv,'y0'));
      R = str2num(vsdl_getval(kv,'R'));
      r = str2num(vsdl_getval(kv,'r'));
      a = str2num(vsdl_getval(kv,'a'));
      rr{id} = [x0 y0 R r a];
      if ~isempty(strfind(str,'/>'))
        ;
      else
        while 1
          str = fgetl(ifn);
          if ~ischar(str)
            break;
          end
          if ~isempty(strfind(str,'</roi>'))
            break;
          end
        end
      end
    else
      idx = strfind(str,'>');
      str = str(idx+1:end);
      [x y] = sscanf(str,'%g %g');
      rr{id} = x';
      while 1
        str = fgetl(ifd);
        if ~ischar(str)
          break;
        end
        if ~isempty(strfind(str,'</roi>'))
          break;
        end
        [x y] = sscanf(str,'%g %g');
        rr{id}(end+1,:) = x';
      end
    end
  end
end

function [scl,uni] = vsdl_getscale(scl_u)
if endswith(scl_u,'V')
  scl = vsdl_mvolt(scl_u);
  uni = 'mV';
elseif endswith(scl_u,'A')
  scl = vsdl_namp(scl_u);
  uni = 'nA';
else
  [scl,uni] = sscanf(scl_u,'%g %s');
  fprintf(1,'Warning: scale is in odd units: %s\n',uni);
end
if scl==0
  scl = 1;
  uni = 'mV';
  fprintf(1,'Warning: assuming scale=1 mV.\n');
end
