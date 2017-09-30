function vscope_stacktrial(expt, trialnos, outno, args)
% VSCOPE_STACKTRIAL - Create a mock trial from focusstack of snapshots
%    VSCOPE_STACKTRIAL(expt, trialnos, outno) loads the trials
%    mentioned in the vector TRIALNOS from the experiment EXPT, performs
%    focus stacking, and saves the result as a new virtual trial with
%    number OUTNO.
%    VSCOPE_STACKTRIAL(expt, trialnos) uses OUTNO=999.
%    Any nonexisting trials mentioned in TRIALNOS are quietly ignored as
%    are trials that are not snapshots.
%    As a special case, leaving out TRIALNOS entirely causes VSCOPE_STACKTRIAL
%    to load all snapshots.
%    Leaving out EXPT causes the current directory to be scanned.
%    VSCOPE_STACKTRIAL(expt, trialnos, outno, {args}) specifies arguments
%    for FOCUSSTACK.
%    OUTNO may also be a string, in which case it specifies a .xml to write.
%    TRIALNOS may be a cell array of vectors, in which case different
%    sets of trials are used for different cameras.

if nargin<4
  args = {25, 10, 1};
end
if nargin<3
  outno = 999;
end
if nargin<2
  trialnos = [];
end
if nargin<1
  expt = '.';
end

if nargin<3 && isnumeric(expt)
  outno = trialnos;
  trialnos = expt;
  expt = '.';
end

if ischar(outno)
  ofn = outno;
  outno = 999;
else
  ofn = sprintf('%s/%03i.xml', expt, outno);
end

if isempty(trialnos)
  potfns = glob(sprintf('%s/???.xml', expt));
  P = length(potfns);
  fns = {};
  for p=1:P
    fn = potfns{p};
    fprintf(1, 'Scanning %i/%i: %s\n', p,P,fn);
    idx = find(fn=='/');
    lf = fn(idx(end)+1:end);
    idx = find(lf=='.', 1);
    tri = atoi(lf(1:idx-1));
    if tri<900
      x = vscope_load(fn, '-'); % Load XML only
      if isfield(x, 'ccd') && ~isfield(x, 'analog')
        fns{end+1} = fn;
      end
    end
  end
else
  if iscell(trialnos)
    C = length(trialnos);
    fns = cell(C,1);
    for c=1:C
      fns{c} = {};
      P = length(trialnos{c});
      for p=1:P 
        tri=trialnos{c}(p);
        fn = sprintf('%s/%03i.xml', expt, tri);
        if exist(fn)
          fprintf(1, 'Scanning %i/%i %i/%i: %s\n', c,C,p,P,fn);
          x = vscope_load(fn,'');% XML only
          toc
          if isfield(x, 'ccd') && ~isfield(x, 'analog')
            fns{c}{end+1} = fn;
          end
        end
        toc
      end
    end
  else
    fns = {};
    P = length(trialnos);
    for p=1:P
      tri=trialnos(p);
      fn = sprintf('%s/%03i.xml', expt, tri);
      if exist(fn)
        fprintf(1, 'Scanning %i/%i: %s\n', p,P,fn);
        x = vscope_load(fn, ''); % Load XML only
        if isfield(x, 'ccd') && ~isfield(x, 'analog')
          fns{end+1} = fn;
        end
      end
    end
  end
end

if iscell(trialnos)
  for c=1:C
    if isempty(fns{c})
      error(sprintf('No input files for camera %i', c));
    end
  end
else
  if isempty(fns)
    error('No input files');
  end
end

F = length(fns);
if iscell(trialnos)
  x = vscope_load(fns{1}{1}, '');
else
  x = vscope_load(fns{1}, '');
  C = x.ccd.info.ncams;
end
S = x.ccd.info.pix{1};

imgs = cell(C,1);
for c=1:C
  imgs{c} = zeros([S F]);
end

if iscell(trialnos)
  for c=1:C
    F = length(fns{c});
    for f=1:F
      fn = fns{c}{f};
      fprintf('Loading trial %i/%i %i/%i: %s\n', c,C,f,F,fn);
      fn = strrep(fn, '.xml', '-ccd.dat');
      fd = fopen(fn, 'rb');
      if fd<0
        error(sprintf('Cannot read "%s"', fn));
      end
      fseek(fd, prod(S)*2*(c-1), 'cof'); % Skip earlier images
      rw = fread(fd, S, 'uint16');
      rw = double(rw) ./ 65535;
      rw = rw ./percentile(sort(rw(:)), 0.5);
      imgs{c}(:,:,f) = rw;
    
      fclose(fd);
    end
  end
else
  for f=1:F
    fn = fns{f};
    fprintf('Loading trial %i/%i: %s\n', f,F,fn);
    fn = strrep(fn, '.xml', '-ccd.dat');
    fd = fopen(fn, 'rb');
    if fd<0
      error(sprintf('Cannot read "%s"', fn));
    end
    
    for c=1:C
      rw = fread(fd, S, 'uint16');
      rw = double(rw) ./ 65535;
      rw = rw ./percentile(sort(rw(:)), 0.5);
      imgs{c}(:,:,f) = rw;
    end
    
    fclose(fd);
  end
end

stack = cell(C,1);
for c=1:C
  fprintf(1, 'Stacking camera %i/%i\n', c,C);
  img = focusstack(imgs{c}, args{:});
  img = img - min(img(:));
  img = img ./ max(img(:));
  img = uint16(img*65535);
  stack{c} = img;
end

% Read .xml of first trial
if iscell(trialnos)
  fd = fopen(fns{1}{1}, 'rb');
else
  fd = fopen(fns{1}, 'rb');
end
txt = fread(fd, [1 inf], '*char');
fclose(fd);

% Muck it up
idx = strfind(txt, ' time=');
if ~isempty(idx)
  txt(idx+[7:12]) = '000000'; % Fake out the start time
end
idx = strfind(txt, ' endtime=');
if ~isempty(idx)
  txt(idx+3+[7:12]) = '000000'; % Fake out the end time
end
idx = strfind(txt, ' trial=');
if ~isempty(idx)
  txt(idx+[8:10]) = sprintf('%03i', outno); % Fake out the trial number
end

% Write .xml of stacked trial
fd = fopen(ofn, 'wb');
fwrite(fd, txt, 'char');
fclose(fd);

% Write images
dfn = strrep(ofn, '.xml', '-ccd.dat');
fd = fopen(dfn, 'wb');
for c=1:C
  fwrite(fd, stack{c}, 'uint16');
end
fclose(fd);

  