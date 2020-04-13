# vscope/rois.py - functions dealing with regions of interest

# This module contains:

# - roicoords
# - roioutlines
# - fillpoly
# - allroimask

import pyqplot as qp
import numpy as np
from .types import *
from . import xyrra
try:
    import cairo
    havecairo = True
except:
    havecairo = False
    print('Could not import cairo --- polygon functions will be slower')

if havecairo:
    def fillpoly(Y, X, xx, yy):
        '''FILLPOLY - Create an image with a polygonal area filled
        img = FILLPOLY(Y, X, xx, yy) creates an YxX image in which the
        polygonal area enclosed by the vertices (xx, yy) is filled with 
        True on a background of False.'''
        def pad(w, n):
            d = w % n
            if d:
                return w + n - d
            else:
                return w

        surface = cairo.ImageSurface(cairo.FORMAT_A1, X, Y)
        ctx = cairo.Context(surface)
        ctx.set_source_rgb(1,1,1)
        ctx.move_to(xx[0], yy[0])
        N = len(xx)
        for k in range(1,N):
            ctx.line_to(xx[k], yy[k])
        ctx.close_path()
        ctx.fill()
        W = pad(X, 32)
        buf = surface.get_data()
        data = np.unpackbits(np.frombuffer(buf, np.uint8))
        data = np.reshape(data, (Y, W//8, 8))
        data = np.flip(data, 2)
        data = np.reshape(data, (Y, W))
        return data[:,:X]
else:
    def point_inside_polygon(x,y,poly):
        # Found on the internet. Great, but not good for points on edges
        n = len(poly)
        inside = False
        p1x,p1y = poly[0]
        for i in range(n+1):
            p2x,p2y = poly[i % n]
            if y > min(p1y,p2y):
                if y <= max(p1y,p2y):
                    if x <= max(p1x,p2x):
                        if p1y != p2y:
                            xinters = (y-p1y)*(p2x-p1x)/(p2y-p1y)+p1x
                        if p1x == p2x or x <= xinters:
                            inside = not inside
            p1x,p1y = p2x,p2y
        return inside
    
    def fillpoly(Y, X, xx, yy):
        '''FILLPOLY - Create an image with a polygonal area filled
        img = FILLPOLY(Y, X, xx, yy) creates an YxX image in which the
        polygonal area enclosed by the vertices (xx, yy) is filled with 
        True on a background of False.'''
        vertices = np.stack(((xx-(X/2))*1.0001 + X/2,
                             (yy-(Y/2))*1.0001 + Y/2), 1)
        img = np.zeros((Y,X), bool)
        for y in range(Y):
            for x in range(X):
                img[y,x] = point_inside_polygon(x, y, vertices)
        return img

def _outlines_poly(roi, info):
    xx = roi['x']
    yy = roi['y']
    if len(xx)==0:
        return xx, yy
    return info.transform.inverse().apply(xx, yy)
    
def _coords_poly(roi, info):
    xx, yy = _outlines_poly(roi, info)
    if len(xx)==0):
        return xx, yy
    xmin = int(np.floor(np.min(xx))-1)
    ymin = int(np.floor(np.min(yy))-1)
    xmax = int(np.ceil(np.max(xx))+1)
    ymax = int(np.ceil(np.max(yy))+1)
    xmin = max(xmin, 0)
    ymin = max(ymin, 0)
    xmax = min(xmax, info.serpix)
    ymax = min(ymax, info.parpix)
    xx0 = xx - xmin
    yy0 = yy - ymin
    X = xmax - xmin + 5
    Y = ymax - ymin + 5
    if not havecairo:
        if info.transform.ax*info.transform.ay < 0:
            xx0 = np.flip(xx0,0)
            yy0 = np.flip(yy0,0)
    img = fillpoly(Y, X, xx0, yy0)
    yy, xx = np.nonzero(img)
    return xx+xmin, yy+ymin

def _outline_xyrra(roi, info):
    '''UNTESTED'''
    el = xyrra.XYRRA(roi)
    xx, yy = el.stroke()
    return info.transform.inverse().apply(xx, yy)

def _coords_xyrra(roi, info):
    '''UNTESTED'''
    el = xyrra.XYRRA(roi)
    r = max(el.R, el.r)
    x0, y0 = info.transform.inverse().apply(el.x-r, el.y-r)
    x1, y1 = info.transform.inverse().apply(el.x+r, el.y+r)
    x0 = np.floor(x0)
    y0 = np.floor(y0)
    x1 = np.ceil(x1)
    y1 = np.ceil(y1)
    x0 = max(x0, 0);
    y0 = max(y0, 0);
    x1 = min(x1, info.serpix)
    y1 = min(y1, info.parpix)
    X = x1 - x0
    Y = y1 - y0
    xx = np.arange(x0, x1)
    yy = np.arange(y0, y1)
    xx_, yy_ = info.transform.apply(xx, yy)
    xx_ -= el.x
    yy_ -= el.y
    xx_ = np.reshape(xx_, [1, X])
    yy_ = np.reshape(yy_, [Y, 1])
    xi = np.cos(el.phi)*xx_ + np.sin(el.phi)*yy_;
    eta = -np.sin(el.phi)*xx_ + np.cos(el.phi)*yy_;
    yy, xx = np.nonzero(xi**2/el.R**2 + eta**2/el.r**2 <= 1)
    return xx+x0, yy+y0
    
def roicoords(x, cam):
    '''ROICOORDS - Return coordinates for all ROIs on a given camera
    coords = ROICOORDS(x, cam), where X is a VScopeFile from LOAD and
    CAM is a camera name, returns a dict mapping ROI numbers to (xx, yy) 
    tuples of pixel coordinates that are inside the ROI.
    Only ROIs that are defined on the given camera will appear in the dict.'''
    info = x.ccd.info(cam)
    coords = {}
    for id in x.rois.keys():
        cc = x.rois[id]['cams']
        if len(cc)==0 or cam in cc:
            # Very old vscope files have no cam names stored in -rois.xml
            # More recent versions have a colon-separated list
            if 'x' in x.rois[id]:
                coords[id] = _coords_poly(x.rois[id], info)
            else:
                coords[id] = _coord_xyrra(x.rois[id], info)
    return coords
        
def roioutlines(x, cam):
    '''ROIOUTLINES - Returns polygons specifying the edge of ROIs
    polys = ROIOUTLINES(x, camid), where X is a VScopeFile from LOAD
    and CAM is a camera name, returns a dict mapping ROI numbers to (xx, yy)
    tuples defining the boundaries of each ROI.'''
    info = x.ccd.info(cam)
    polys = {}
    for id in x.rois.keys():
        cc = x.rois[id]['cams']
        if len(cc)==0 or cam in cc:
            # Very old vscope files have no cam names stored in -rois.xml
            # More recent versions have a colon-separated list
            if 'x' in x.rois[id]:
                polys[id] = _outlines_poly(x.rois[id], info)
            else:
                polys[id] = _outlines_xyrra(x.rois[id], info)
    return polys
    
def allroimask(x, cam, marg=None, margx=None, margy=None):
    '''VSCOPE_ALLROIMASK - Return mask corresponding to all ROIs
    msk = VSCOPE_ALLROIMASK(x, cam) returns a binary mask 
    corresponding to the joint area of all ROIs defined for
    camera CAM. 
    X must be a VScopeFile from LOAD. 
    CAM must be a camera name (not a number).
    Optional:
      - MARG: grow the mask by given number of pixels in each 
          direction.
      - MARGX, MARGY: specify margin for x- and y-directions
          separately.'''

    if margx is None:
        margx = marg
    if margy is None:
        margy = marg

    xyxy = roicoords(x, cam)
    T, Y, X = x.ccd.data(cam).shape
    msk = np.zeros((Y, X))
    for id, roi in xyxy.items():
        msk[(roi[1],roi[0])] = 1
    if margx is not None:
        krn = np.ones((1,int(np.ceil(margx)*2 + 1)))
        msk = scipy.signal.convolve2d(msk, krn, 'same')
    if margy is not None
        krn = np.ones((int(np.ceil(margy)*2 + 1), 1))
        msk = scipy.signal.convolve2d(msk, krn, 'same')
    return msk>0
