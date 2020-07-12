#!/usr/bin/python3

# This module contains

# - bestshift
# - bestshiftrois
# - marginlessdiff
# - microshift
# - microperspective
# - microrotate
# - microscale
# - MICROCORRECT

# I am not copying microcorrectslow
import numpy as np
import scipy.interpolate

def bestshift(img, rf1, rf2, margx=None, margy=None):
    '''BESTSHIFT - Calculate best shift for image based on two reference images
    dx = BESTSHIFT(img, rf1, rf2) where RF1 and RF2 are two copies of a 
    reference image microshifted by 0.5 pixels in opposite directions, 
    returns the optimal microshift amount for IMG to match the original
    reference image.
 
    A margin of 16 pixels in X and 4 pixels in Y is ignored for the
    calculation if the images are 512x128. These defaults may be
    changed:

      dx = BESTSHIFT(img, rf1, rf2, margx, margy)'''

    Y, X = img.shape
    if margy is None:
        if margx is None:
            margy = max(1, np.round(Y/128))
        else:
            margy = margx
    if margx is None:
        margx = max(1, np.round(X/128))

    img = img[margy:Y-margy, margx:X-margx]
    rf1 = rf1[margy:Y-margy, margx:X-margx]
    rf2 = rf2[margy:Y-margy, margx:X-margx]
    drf = rf1 - rf2
    a = np.sum(drf * (img - rf2)) / np.sum(drf**2)
    return a - 0.5

def bestshiftrois(img, rf1, rf2, rois):
    '''BESTSHIFTROIS - Best shift for image based on two reference images
    dx = BESTSHIFT(img, rf1, rf2, rois) where RF1 and RF2 are two copies of a 
    reference image microshifted by 0.5 pixels in opposite directions, 
    returns the optimal microshift amounts for IMG to match the original
    reference image for each of the ROIs in ROIS, which much be from
    ROIS.ROICOORDS_CAM.'''

    Y,X = img.shape
    Y_, X_ = rf1.shape
    idx = {}
    if Y_<Y:
        img = img[1:Y-1, 1:X-1]
        for n in rois:
            use = np.nonzero(np.logical_and(np.logical_and(rois[n].x>=1,
                                                           rois[n].x<X-1),
                                            np.logical_and(rois[n].y>=1,
                                                           rois[n].x<Y-1)))
            idx[n] = (rois[n].x[use]-1, rois[n].y[use]-1)
    else:
        for n in rois:
            use = np.nonzero(np.logical_and(np.logical_and(rois[n].x>=0,
                                                           rois[n].x<X),
                                            np.logical_and(rois[n].y>=0,
                                                           rois[n].x<Y)))
            idx[n] = (rois[n].x[use], rois[n].y[use])

    dx = {}
    for n in rois:
        if len(idx[n])>0:
            drf = rf1[idx[n]] - rf2[idx[n]]
            a = np.sum(drf * (img[idx[n]] - rf2[idx[n]])) / np.sum(drf**2)
            dx[n] = a - .5
        else:
            dx[n] = 0
    return dx

def marginlessdiff(img1, img2, margx=None, margy=None):
    '''MARGINLESSDIFF - RMS difference between images sans margins
    rms = MARGINLESSDIFF(img1, img2) calculates the RMS difference between
    the two images IMG1 and IMG2.

    A margin of 16 pixels in X and 4 pixels in Y is ignored for the calculation
    if the images are 512x128. This default may be changed:

    rms = MARGINLESSDIFF(img1, img2, margx, margy)'''

    Y, X = img.shape
    if margy is None:
        if margx is None:
            margy = max(1, np.round(Y/128))
        else:
            margy = margx
    if margx is None:
        margx = max(1, np.round(X/128))

    img1 = img1[margy:Y-margy, margx:X-margx]
    img2 = img2[margy:Y-margy, margx:X-margx]
    Y,X = img1.shape
    return np.sqrt(np.sum((img1 - img2)**2)) / (Y*X)

def microshift(img, dx, dy):
    '''MICROSHIFT - Microshifted image
    img = MICROSHIFT(img, dx, dy) shifts the image IMG by DX and DY pixels
    in the horizontal (second) and vertical (first) dimensions. 
    The edge pixels are not trimmed, but they may become invalid (nan).'''

    if abs(dx)>1:
        print('Caution: microshift does not do shifts with |dx| > 1')
        dx = max(min(dx, 1), -1)
    if abs(dy)>1:
        print('Caution: microshift does not do shifts with |dy| > 1')
        dy = max(min(dy, 1), -1)

    res = img.copy()
    if dx>0:
        res[:,:-1] = (1-dx)*img[:,:-1] + dx*img[:,1:]
    elif dx<0:
        res[:,1:] = -dx*img[:,:-1] + (1+dx)*img[:,1:]
    if dy>0:
        res[:-1,:] = (1-dy)*res[:-1,:] + dy*res[1:,:]
    elif dy<0:
        res[1:,:] = -dy*res[:-1,:] + (1+dy)*res[1:,:]
    return res

def shapevectors(X, Y):
    xx = np.arange(X)
    yy = np.arange(Y)
    x0 = np.mean(xx)
    y0 = np.mean(yy)
    xx -= x0
    yy -= y0
    xx = np.reshape(xx, [1,X])
    yy = np.reshape(yy, [Y,1])
    return (xx, yy, x0, y0)


def microadjust(img, xfunc, yfunc):
    Y, X = img.shape
    xx = np.arange(X)
    yy = np.arange(Y)
    x0 = np.mean(xx)
    y0 = np.mean(yy)
    xx -= x0
    yy -= y0
    xx, yy = (xfunc(xx, yy), yfunc(xx, yy))
    xx += x0
    yy += y0
    xx = np.reshape(xx, [1,X])
    yy = np.reshape(yy, [Y,1])
    img = scipy.interpolate.RectBivariateSpline(xx, yy, img,
                                                bbox=[None, None, None, None],
                                                kx=1, ky=1)
    return img

def microperspective(img, dx, dy, trim=False, fill=None):
    '''MICROPERSPECTIVE - Microperspectived image
    img = MICROPERSPECTIVE(img, dx, dy) perspectivises the image IMG by
    DX pixels in x and DY pixels in y. That's at the extreme edge of
    the figure.
    Edge pixels may become nan.
    Optional argument TRIM specifies that one pixel be trimmed from all 
    edges.
    Optional argument FILL specifies an alternate fill value for edge 
    pixels.'''
    Y, X = img.shape
    RX = X/2
    RY = Y/2
    def xfunc(xx, yy):
        if dx!=0:
            return xx*(1+dx*yy/RX/RY)
        else:
            return xx
    def yfunc(xx, yy):
        if dy!=0:
            return yy*(1+dy*xx/RX/RY)
        else:
            return yy
    img = microadjust(img, xfunc, yfunc)
    if trim:
        img = img[1:Y-1,1:X-1]
    if fill is not None:
        img[np.nonzero(np.isnan(img))] = fill
    return img

def microrotate(img, dx):
    '''MICROROTATE - Microrotated image
    img = MICROROTATE(img, dx) rotates the image IMG by
    DX fine pixels at the extreme end. 
    (A "fine pixel" is one pixel in whichever direction of the image
    has the higher resolution. The image is assumed to be naturally 
    square; pixel aspect ratio is inferred from pixel count.)
    Edge pixels may become nan as a result.'''

    Y, X = img.shape
    R = max(Y,X)/2
    dx = dx / R # Convert to radians
    img = microadjust(img,
                      lambda xx, yy: xx*np.cos(dx) + yy*X/Y*np.sin(dx),
                      lamdba xx, yy: -yy*X/Y*np.sin(dx), xx*np.cos(dx))       
    return img


def microscale(img, dx):
    '''MICROSCALE - Microscaled image
    img = MICROSCALE(img, dx) scales the image IMG by DX 'pixels'
    in both dimensions. Pixels are measured in the finer dimension 
    (usually, X); the scale is isotropic assuming the image is square.
    Edge pixels may become nan.'''

    Y, X = img.shape
    R = max(X, Y) / 2
    img = microadjust(img,
                      lambda xx, yy: xx * (1 + dx/R),
                      lamdba xx, yy: yy * (1 + dy/R))
    return img

def microcorrect(vsd, opts='bxys', msk=None,
                 t0=None, sx=1, sy=.25, ss=.25, sr=1):
    '''MICROCORRECT - One step microcorrection
    out, info = MICROCORRECT(vsd) performs one iteration of microshifting,
    scaling, rotation, perspective, and/or brightness correction.
    VSD must be TxYxX.

    Returns:
      - OUT: Microcorrected image
      - INFO: Changes applied at each stage:
              T0: Inferred or explicit T0
              DX0: Calculated x-shift
              DX: Actually performed x-shift (which is zero if calculated 
                  x-shift somehow made the error larger)
              DY0, DY: Ditto, for y-shift
              DS0, DS: Ditto for scale
              DPX0, DPX, DPY0, DPY: Ditto for perspectives
              E0: Initial error
              EDX: Error after x-shift (if performed)
              EDY, ES, EDPX, EDPY: Ditto for y-shift, scaling, perspectives.

    Optional arguments (with defaults):
      - OPTS: Letters specify which of the steps are performed:
              - 'b': brightness
              - 'x': x-shift
              - 'y': y-shift
              - 's': scale
              - 'h': x-perspective
              - 'v': y-perspective
              - 'r': rotation
              Default: 'xys'.
      - MSK: a binary mask of which pixels to include in brightness 
            measurements. (Default: all pixels.)
      - T0: which frame to use for reference (T/2)
      - SX: max delta for X (1 pix)
      - SY: max delta for Y (.25 pix)
      - SS: max delta for scale (.25 pix/img)
      - SR: max delta for rotation (1 "fine" pix [see MICROROTATE])

    Example:
       cam = 'Bot'
       msk = np.logical_not(allroimask(x, cam, marg=2))
       out, info = microcorrect(x.ccd.data[cam], msk)'''
    
    T, Y, X = vsd.shape
    if T0 is None:
        T0 = T//2

    margx = 4 * max(1, round(X/128))
    margy = 4 * max(1, round(Y/128))
        
    ref = vsd[T0,:,:].astype(float)
    refbri0 = np.mean(ref[margy:Y-margy, margx:X-margx])

    vsd = vsd.copy()

    info = { 't0': T0, 'e0': np.zeros(T) }

    if 'b' in opts:
        if msk is None:
            refbri = refbri0
        else:
            refbri = np.mean(ref[np.nonzero(msk)])
    if 'x' in opts:
        rfx1 = microshift(ref, -sx, 0)
        rfx2 = microshift(ref, sx, 0)
        info['dx'] = np.zeros(T)
        info['dx0'] = np.zeros(T)
        info['ex'] = np.zeros(T)
    if 'y' in opts:
        rfy1 = microshift(ref, 0, -sy)
        rfy2 = microshift(ref, 0, sy)
        info['dy'] = np.zeros(T)
        info['dy0'] = np.zeros(T)
        info['ey'] = np.zeros(T)
    if 's' in opts:
        rfs1 = microscale(ref, -ss)
        rfs2 = microscale(ref, ss)
        info['ds'] = np.zeros(T)
        info['ds0'] = np.zeros(T)
        info['es'] = np.zeros(T)
    if 'h' in opts:
        rfpx1 = microperspective(ref, -sx, 0)
        rfpx2 = microperspective(ref, sx, 0)
        info['dpx'] = np.zeros(T)
        info['dpx0'] = np.zeros(T)
        info['epx'] = np.zeros(T)
    if 'v' in opts:
        rfpy1 = microperspective(ref, 0, -sy)
        rfpy2 = microperspective(ref, 0, sy)
        info['dpy'] = np.zeros(T)
        info['dpy0'] = np.zeros(T)
        info['epy'] = np.zeros(T)
    if 'r' in opts:
        rfr1 = microrotate(ref, 0, -sr)
        rfr2 = microrotate(ref, 0, sr)
        info['dr'] = np.zeros(T)
        info['dr0'] = np.zeros(T)
        info['er'] = np.zeros(T)
        

    fac0 = np.ones(T)
    for t in range(T):
        img = vsd[t,:,:].astype(float)
        imbri0 = np.mean(img[margy:Y-margy, margx:X-margx])
        fac0[t] = refbri0 / imbri0
        info['e0'][t]  = marginlessdiff(fac0[t]*img, ref)

    for t in range(T):
        print(f'Microcorrect {t}/{T}  ', end='\r')
        if t==T0:
            continue

        img = fac0[t] * vsd[t,:,:].astype(float)
        er0 = info['e0'][t]

        def onestep(name, func, rf1, rf2, sc):
            dx = bestshift(img, rf1, rf2) * sc
            out = func(img, ds)
            err = marginlessdiff(out, ref)
            info['e' + name][t] = err
            info['d' + name + '0'][t] = dx
            if err < er0:
                info['d' + name][t] = dx
                return out, err
            else:
                return img, er0

        if 'x' in opts:
            img, er0 = onestep('x',
                               lambda(img, dx): microshift(img, dx, 0),
                               rfx1, rfx2, sx/0.5)
        if 'y' in opts:
            img, er0 = onestep('y',
                               lambda(img, dy): microshift(img, 0, dx),
                               rfy1, rfy2, sy/0.5)
        if 'b' in opts:
            if msk is None:
                pass # Let original normalization stand
            else:
                fac = np.mean(img[msk]) / refbri
                img *= fac / fac0[t]
        else:
        if 's' in opts:
            img, er0 = onestep('s',
                               microscale,
                               rfs1, rfs2, ss/0.5)
        if 'h' in opts:
            img, er0 = onestep('px',
                               lambda(img, dx): microperspective(img, dx, 0),
                               rfpx1, rfpx2, sx/0.5)
        if 'v' in opts:
            img, er0 = onestep('py',
                               lambda(img, dy): microperspective(img, 0, dy),
                               rfpy1, rfpy2, sy/0.5)
        if 'r' in opts:
            img, er0 = onestep('r',
                               microrotate,
                               rfr1, rfr2, sr/0.5)
        if 'b' not in opts:
            img /= fac0[t]            

        vsd[t,:,:] = img
        
    return vsd, info
