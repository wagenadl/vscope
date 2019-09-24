# vscope/rois.py - functions dealing with regions of interest

# This module contains:

# - roicoords_cam

import numpy as np
from .types import *

def roicoords_cam(x, cam):
    '''ROICOORD_CAM - Return coordinates for all ROIs on a given camera
    coords = ROICOORDS_CAM(x, cam), where X is a VScopeFile from LOAD and
    CAM is a camera name, returns a dict mapping ROI numbers to (xx, yy) 
    tuples of pixel coordinates.
    Only ROIs that are defined on the given camera will appear in the dict.'''
    
    coords = {}
    for id in x.rois.keys():
        cc = x.rois[id]['cams']
        if len(cc)==0 or cam in cc:
            # Very old vscope files have no cam names stored in -rois.xml
            # More recent versions have a colon-separated list
            if 'x' in x.rois[id]:
                xx = x.rois[id]['x']
                yy = x.rois[id]['y']
            else:
                # XYRRA style
                pass
            
        
