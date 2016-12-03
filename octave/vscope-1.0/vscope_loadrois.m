function dat = vscope_loadrois(dat, ifn)
% VSCOPE_LOADROIS - Add ROI data to a loaded trial
%    dat = VSCOPE_LOADROIS(dat, ifn), where DAT is the result of VSCOPE_LOAD,
%    replaces the ROIs in DAT with the contents of the named file IFN, 
%    which must be a "-rois.xml" file.

x = vscope_load(ifn);

dat.rois = x.rois;
dat.roicams = x.roicams;
