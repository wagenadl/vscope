function dat = vscope_loadrois(dat, ifn)
% VSCOPE_LOADROIS - Add ROI data to a loaded trial
%    dat = VSCOPE_LOADROIS(dat, ifn), where DAT is the result of VSCOPE_LOAD,
%    replaces the ROIs in DAT with the contents of the named file IFN, 
%    which must be a "-rois.xml" file.
%    rois = VSCOPE_LOADROIS(ifn) loads only the ROIs and ROICAMs data

if nargin==2
  x = vscope_load(ifn);
  dat.rois = x.rois;
  dat.roicams = x.roicams;
else
  dat = vscope_load(dat);
end

