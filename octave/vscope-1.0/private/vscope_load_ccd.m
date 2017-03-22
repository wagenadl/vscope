function dat=vscope_load_ccd(ifn,wht,frmno,camno)
% VSCOPE_LOAD_CCD - Load ccd data from vscope
%    dat = VSCOPE_LOAD_CCD(ifn,info), where IFN is a "-ccd.dat" file and INFO
%      is the "ccd.info" field from a previous VSCOPE_LOAD, loads the raw
%      data and returns it in the proper shape.
%    dat = VSCOPE_LOAD_CCD(ifn,info,frmno) loads CCD data from a single frame, 
%    dat = VSCOPE_LOAD_CCD(ifn,info,frmno, camno) loads CCD data from a single
%      camera and from a single frame. Note that frames are counted from 
%      zero, but cameras are counted from one. 
%    The images are properly flipped for Daniel's setup. That is, the data 
%    for camera 1 are upside downed. For John's setup, this is obviously
%    less than helpful.

% This file is part of VScope. (C) Daniel Wagenaar 2008-1017.

% VScope is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% VScope is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with VScope.  If not, see <http://www.gnu.org/licenses/>.

ifd=fopen(ifn,'r');
if nargin>=3
  % read just one frame, one or more cameras
  C=wht.ncams;
  F=wht.nframes;
  Xser=wht.pix(1);
  Xpar=wht.pix(2);
  frmpix = C*Xser*Xpar;
  fseek(ifd,frmpix*2*frmno,'bof');
  if nargin>=4
    % read just one camera
    fseek(ifd,Xser*Xpar*2*(camno-1),'cof');
    dat = fread(ifd,[Xser Xpar],'uint16');
    if camno==1
      dat = fliplr(dat);
    end
  else
    dat=fread(ifd,[1 frmpix],'uint16');
    dat = reshape(dat,[Xser Xpar C]);
    dat(:,:,1) = dat(:,end:-1:1,1);
  end
  dat = permute(dat,[2 1 3 4]);
else
  % read all frames, all cameras
  dat=fread(ifd,[1 inf],'uint16');
  L=length(dat);
  if nargin>=2
    C=wht.ncams;
    F=wht.nframes;
    Xser=wht.pix(1);
    Xpar=wht.pix(2);
    dat = reshape(dat,[Xser Xpar C F]);
    dat(:,:,1) = dat(:,end:-1:1,1);
    dat = permute(dat,[2 1 3 4]);
  else
    dat = reshape(dat,[L 1]);
  end
end
fclose(ifd);
