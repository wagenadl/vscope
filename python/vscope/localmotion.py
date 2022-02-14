#!/usr/bin/python3

import vscope.rois
import numpy as np
import swiftir
import scipy.signal

def upsampledImages(x, cam):
    '''Return Tx512x512 images, upsampling the Y direction to make square
    pixels and returns the scale factor.'''
    s = x.ccd[cam].astype(float) - 1000
    T,Y,X = s.shape
    SY = 1
    if Y<X:
        SY = X//Y
        s = s.reshape(T,Y,1,X).repeat(SY,2).reshape(T,Y*SY,X)
    return s.astype(np.float32), SY


def stationaryImage(frms, skipstart=30, skipend=10):
    '''Return an average image from a series of frames'''
    return frms[skipstart:-skipend].mean(0)


def blockcorners(ff, r):
    '''Remove the corners from a 2d fft to block low frequencies'''
    ff = 0 + ff
    ff[:r, :r, :] = 0
    ff[:r, -r:, :] = 0
    ff[-r:, :r, :] = 0
    ff[-r:, -r:, :] = 0
    return ff

def blockedges(ff, r):
    '''Remove the edges from a 2d fft to block low frequencies'''
    ff = 0 + ff
    ff[:r, :, :] = 0
    ff[-r:, :, :] = 0
    ff[:, :r, :] = 0
    ff[:, -r:, :] = 0
    return ff

def blockhighfreq(ff, SY):
    '''Drops high Y frequencies from the FFT image. This is needed,
    because those are an artifact from upsampling and should not be
    used for alignment.
    '''
    Y,X,C = ff.shape
    ff = 0 + ff
    ff[Y//SY//2:-Y//SY//2,:,:] = 0
    return ff

def globalAffines(frms, stat=None, lp=None, blockhigh=None):
    '''Calculate affine transforms to align a stack of frames with a
    stationary image'''
    if stat is None:
        stat = stationaryImage(frms)
    T, Y, X = frms.shape
    pa = np.array([[.4*X, .4*X, .6*X, .6*X], [.4*Y, .6*Y, .4*Y, .6*Y]])
    ppb = []
    siz = (320,320)
    for p in pa.T:
        apostat = swiftir.apodize(swiftir.extractStraightWindow(stat, p, siz))
        fftstat = swiftir.fft(apostat)
        fftstat = blockedges(fftstat, 4)
        if blockhigh is not None:
            fftstat = blockhighfreq(fftstat, blockhigh)
        #qp.figure('s5')
        #qp.imsc(swiftir.ifft(fftstat))
        swim = [ swiftir.swim(fftstat,
                              swiftir.extractStraightWindow(frms[t], p, siz),
                              rad=10)
                 for t in range(T)]
        swim = np.array(swim)
        dx1 = swim[:,0]
        dy1 = swim[:,1]
        sx1 = swim[:,2]
        sy1 = swim[:,3]
        snr1 = swim[:,4]
        ppb.append(p.reshape(2,1) + np.array([dx1,dy1]))
    ppb = np.array(ppb).transpose(2,1,0)
    if lp is not None:
        b,a=scipy.signal.butter(1, 1/lp)
        ppb = scipy.signal.filtfilt(b,a, ppb, axis=0)
    afms = [ swiftir.mirAffine(pa, pb)[0] for pb in ppb ]
    return afms


def correctGlobal(frms, afms):
    '''Interpolate a stack of frames so as to align them onto a stationary
    image. AFMS must be from globalAffines.'''
    T = len(afms)
    clnd = [swiftir.affineImage(afms[t], frms[t]) for t in range(T)]
    return np.array(clnd)


def localSwim(stat, frms, p0, Q, SY, lp=None):
    '''Calculate xy shift near point of interest. Q must be window size.
    SY is Y scale factor, lp>1 enables low-pass temporal filtering.'''
    T, Y, X = frms.shape
    statwin = swiftir.extractStraightWindow(stat, p0, Q)
    apostat = swiftir.apodize(statwin)
    fftstat = swiftir.fft(apostat)
    fftstat = blockedges(fftstat, 4)
    if SY:
        fftstat = blockhighfreq(fftstat, SY)
    swim = [ swiftir.swim(fftstat,
                          swiftir.extractStraightWindow(frms[t], p0, Q))
                  for t in range(T)]
    swim = np.array(swim)
    dxy1 = swim[:,:2]
    if lp is not None and lp>1:
        b,a=scipy.signal.butter(1, 1/lp)
        dxy1 = scipy.signal.filtfilt(b,a, dxy1, axis=0)
    sxy1 = swim[:,2:4]
    snr1 = swim[:,4]
    return dxy1, sxy1, snr1


def localShift(clndfrms, x, roiid, cam, stat, lp=None, blockhigh=None):
    '''Calculate xy shift near a cell of interest.
    returns (dx,dy) [Tx2], (sx,sy) [Tx2], snr [T], p0, Q,
    where p0 is the center of the image tile and Q its width.'''
    xx, yy = vscope.rois.pixelcoords(x, roiid, cam)
    #Q = int(max(np.max(np.abs(xx-x0)),
    #         np.max(np.abs(yy*4-y0)))) * 4 # Plenty of space for apo
    Q = 128 # Fix it to avoid surprises
    x0 = xx.mean()
    M = Q//2
    T,Y,X = clndfrms.shape
    if x0<M:
        x0 = M
    elif x0>X-M:
        x0 = X-M
    y0 = yy.mean()*4
    if y0<M:
        y0 = M
    elif y0>Y-M:
        y0 = Y-M
    p0 = np.round(np.array([x0,y0]))
    dxy, sxy, snr = localSwim(stat, clndfrms, p0, Q, blockhigh, lp)
    return dxy, sxy, snr, p0, Q


def correctLocal(clndfrms, dxy, p0, Q):
    '''To be used after localshift, returns the contents of the tile 
    mapped onto the stationary image.'''
    T = len(clndfrms)
    cl1 = [swiftir.extractStraightWindow(clndfrms[t], p0 + dxy[t], Q)
           for t in range(T)]
    try:
        qx = Q[0]
        qy = Q[1]
    except:
        qx = qy = Q
    return np.array(cl1).reshape(T,qy//4,4,qx).mean(2)



def patchwork(stat, frms, width=128, step=32, SY=4, sigma=64):
    T, Y, X = frms.shape
    xx0 = np.arange(width//2 + step//2, X - width//2, step)
    yy0 = np.arange(width//2 + step//2, Y - width//2, step)
    NX = len(xx0)
    NY = len(yy0)
    dxy = []
    snr = []
    for ny in range(NY):
        dxy.append([])
        snr.append([])
        print(f"LOCALMOTION Patchwork row {ny} of {NY}")
        for nx in range(NX):
            p0 = np.array([xx0[nx], yy0[ny]])
            dxy1, sxy1, snr1 = localSwim(stat, frms, p0, width, SY)
            dxy[-1].append(dxy1)
            snr[-1].append(snr1)
            
    dxy = np.array(dxy) # YxXxTx2
    snr = np.array(snr) # YxXxT

    snr0 = np.mean(snr, -1) # YxX

    weight = np.exp(-(15-snr0)/5)
    weight[weight>1] = 1
    dxymax = np.sqrt(np.max(dxy[:,:,:,0]**2 + dxy[:,:,:,1]**2, axis=-1))
    weight[dxymax>5] = 0
    
    details = {
        "xx0": xx0,
        "yy0": yy0,
        "dxy": dxy,
        "snr": snr,
        "weight": weight,
        "dxymax": dxymax,
        "width": width,
        "step": step,
        "SY": SY,
        "sigma": sigma }
    def dist(x, y):
        return np.array([ [ np.exp(-.5*((x-x0)**2 + (y-y0)**2)/sigma**2)
                   for x0 in xx0 ]
                 for y0 in yy0])
    def deltaxy(x, y):
        ww = dist(x, y) * weight
        norm = np.sum(ww)
        dxy1 = np.sum(ww.reshape(NY,NX,1,1) * dxy, (0,1)) / norm
        return dxy1

    return deltaxy, details


class LocalCorrector:
    def __init__(self, x, cam, global_lp=5, local_lp=5, twice=True):
        self.x = x
        self.cam = cam
        self.frms, self.SY = upsampledImages(x, cam)
        print(f"LOCALMOTION Calculating stationary image for {cam}")
        self.stat = stationaryImage(self.frms)
        self.global_lp = global_lp # low pass timescale for global affine
        self.local_lp = local_lp # low pass timescale for local shift
        ts, te, ok = vscope.ccdtime(x, cam)
        self.tt = (ts+te)/2
        print(f"LOCALMOTION Calculating global affines for {cam}")
        self.afms = globalAffines(self.frms, self.stat,
                                  lp=self.global_lp,
                                  blockhigh=self.SY)
        print(f"LOCALMOTION Aligning image stack for {cam}")
        self.clnd = correctGlobal(self.frms, self.afms)
        T, Y, X = self.clnd.shape
        self.c0 = self.clnd.reshape(T,Y//4,4,X).mean(2)
        print(f"LOCALMOTION Recalculating stationary image for {cam}")
        self.stat = stationaryImage(self.clnd)

        if twice:
            print(f"LOCALMOTION Final recalc of global affines for {cam}")
            self.afms2 = globalAffines(self.clnd, self.stat,
                                      lp=self.global_lp,
                                      blockhigh=self.SY)
            print(f"LOCALMOTION Final aligning image stack for {cam}")
            self.clnd = correctGlobal(self.clnd, self.afms2)
            T, Y, X = self.clnd.shape
            self.c0 = self.clnd.reshape(T,Y//4,4,X).mean(2)
            print(f"LOCALMOTION Final recalc of stationary image for {cam}")
            self.stat = stationaryImage(self.clnd)

        self.patchdxy, self.patchdetails = patchwork(self.stat, self.clnd)

    def timestamps(self):
        return self.tt

    def rawSignal(self, roiid, normalize=True):
        xx, yy = vscope.rois.pixelcoords(self.x, roiid, self.cam)
        sig = self.x.ccd[self.cam][:,yy,xx].mean(-1) - 1000
        if normalize:
            return 100 * (sig/sig.mean() - 1)
        else:
            return sig
        
    def globallyCorrectedSignal(self, roiid, normalize=True):
        xx, yy = vscope.rois.pixelcoords(self.x, roiid, self.cam)
        sig = self.c0[:,yy,xx].mean(-1)
        if normalize:
            return 100 * (sig/sig.mean() - 1)
        else:
            return sig

    def localShift(self, roiid):
        dxy, sxy, snr, p0, Q = localShift(self.clnd, self.x, roiid,
                                    self.cam, self.stat,
                                          lp=self.local_lp,
                                          blockhigh=self.SY)
        return dxy, p0, Q
    
    def locallyCorrectedSignal(self, roiid, normalize=True):
        dxy, p0, Q = self.localShift(roiid)
        img = correctLocal(self.clnd, dxy, p0, Q)
        xx, yy = vscope.rois.pixelcoords(self.x, roiid, self.cam)
        y0 = int(p0[1]/4)
        x0 = int(p0[0])
        sig = img[:,Q//8+yy-y0,Q//2+xx-x0].mean(-1)
        if normalize:
            return 100 * (sig/sig.mean() - 1)
        else:
            return sig
        
    def patchCorrectedSignal(self, roiid, normalize=True):
        T = len(self.afms)
        xx, yy = vscope.rois.pixelcoords(self.x, roiid, self.cam)
        x0 = np.mean(xx)
        y0 = np.mean(yy)*self.SY
        dxy1 = self.patchdxy(x0, y0)
        b,a = scipy.signal.butter(1, 1/self.local_lp)
        dxy1 = scipy.signal.filtfilt(b, a, dxy1, axis=0)
        xx0 = []
        yy0 = []
        for t in range(T):
            pmov = swiftir.stationaryToMoving(self.afms[t], [x0, y0])
            pmov = swiftir.stationaryToMoving(self.afms2[t], pmov)
            xx0.append(pmov[0] + dxy1[t,0])
            yy0.append(pmov[1] + dxy1[t,1])

        dxx = np.array(xx0) - x0
        dyy = (np.array(yy0) - y0)/4
        dxy = np.array((dxx, dyy)).T

        xmax = np.max(xx)
        xmin = np.min(xx)
        ymax = np.max(yy)
        ymin = np.min(yy)
        p0 = np.array([int((xmax+xmin)/2+.5), int((ymax+ymin)/2+.5)])
        qx = int((xmax - xmin) + 4)//2
        qy = int((ymax - ymin) + 4)//2
        imgs = [swiftir.extractStraightWindow(self.x.ccd[self.cam][t]
                                              .astype(np.float32)-1000,
                                              p0 + dxy[t], (qx*2,qy*2))
                for t in range(T)]
        imgs = np.array(imgs)
        psig = imgs[:, yy-p0[1]+qy, xx-p0[0]+qx].mean(-1)
        if normalize:
            return 100 * (psig/psig.mean() - 1)
        else:
            return psig

        
