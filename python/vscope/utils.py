# vscope/utils.py - utility functions

# This module contains:

# - rising
# - falling
# - ephystime
# - ccdtime
# - semiflatten, semiunflatten

import numpy as np
from .types import *

def semiflatten(x, d=0):
    '''SEMIFLATTEN - Permute and reshape an array to convenient matrix form
    y, s = SEMIFLATTEN(x, d) permutes and reshapes the arbitrary array X so 
    that input dimension D (default: 0) becomes the second dimension of the 
    output, and all other dimensions (if any) are combined into the first 
    dimension of the output. The output is always 2-D, even if the input is
    only 1-D.
    If D<0, dimensions are counted from the end.
    Return value S can be used to invert the operation using SEMIUNFLATTEN.
    This is useful to facilitate looping over arrays with unknown shape.'''
    x = np.array(x)
    shp = x.shape
    ndims = x.ndim
    if d<0:
        d = ndims + d
    perm = list(range(ndims))
    perm.pop(d)
    perm.append(d)
    y = np.transpose(x, perm)
    # Y has the original D-th axis first, followed by the other axes, in order
    rest = np.array(shp, int)[perm[:-1]]
    y = np.reshape(y, [np.prod(rest), y.shape[-1]])
    return y, (d, rest)

def semiunflatten(y, s):
    '''SEMIUNFLATTEN - Reverse the operation of SEMIFLATTEN
    x = SEMIUNFLATTEN(y, s), where Y, S are as returned from SEMIFLATTEN,
    reverses the reshaping and permutation.'''
    d, rest = s
    x = np.reshape(y, np.append(rest, y.shape[-1]))
    perm = list(range(x.ndim))
    perm.pop()
    perm.insert(d, x.ndim-1)
    x = np.transpose(x, perm)
    return x

def rising(x):
    '''RISING - Find indexes where digital data rises
    iup = RISING(x) returns a list of elements of X that are True but that
    have a preceding element that is False.
    If X[0] is True, the first value in IUP is 0.'''
    iup = np.nonzero(np.logical_and(x[1:], np.logical_not(x[:-1])))[0] + 1
    if len(x)>0:
        if x[0]:
            iup = np.hstack((np.zeros(1), iup))
    return iup

def falling(x):
    '''FALLING - Find indexes where digital data falls
    idn = FALLING(x) returns a list of elements of X that are False but that
    have a preceding element that is True.
    If the last element of X True, the last value in IUP is len(X).'''
    idn = np.nonzero(np.logical_and(np.logical_not(x[1:]), x[:-1]))[0] + 1
    if len(x)>0:
        if x[-1]:
            idn = np.hstack((idn, len(x)+np.zeros(1)))
    return idn

def ephystime(x):
    '''EPHYSTIME - Construct a vector of time points for ephys data
    tt = EPHYSTIME(x) where X is the result of LOADER.LOAD returns
    a vector of times (in seconds) corresponding to the samples in the
    analog data contained in X.analog.dat
    For convenience, X may also be the analog substructure itself.'''
    if type(x)==VScopeFile:
        x = x.analog
    if type(x)==VSAnalog:
        return np.arange(x.scans)/x.rate('Hz')
    else:
        raise ValueError
    
def ccdtime(x, id=None):
    '''CCDTIME - Construct a vector of time points for CCD data
    (ts, te, ok) = CCDTIME(x, id), where X is a VScopeFile from LOAD,
    and ID is the name of a camera contained therein, returns
    a vector of start times (in seconds) corresponding to the frames in 
    the ccd data for that camera as well as the end times.
    If ID is not given, the first camera is used.
    OK is True if the results are pulled from actual digital data, False if
    constructed based on trial settings (which is less accurate).'''

    if id is None:
        ids = x.ccd.keys()
        for id in ids:
            if len(x.ccd.framestart_s[id])>0:
                return ccdtime(x, id)
        if len(ids)>0:
            return ccdtime(x, ids[0])
        else:
            return (None, None, False)

    ts = x.ccd.framestart_s[id]
    te = x.ccd.frameend_s[id]
    ok = True
    if len(ts)==0:
        t0 = x.settings.acqCCD.camera[id].delay('s')
        dt = 1.0 / x.settings.acqCCD.camera[id].rate('Hz')
        ds = x.settings.acqCCD.camera[id].dutyCycle('%')
        n = x.ccd[id].shape[0]
        ts = t0 + dt * np.arange(n)
        te = ts + ds*dt/100.0
        ok = False
    return (ts, te, ok)
    
def ephysatccd(x, cam, yy, func=np.mean, dim=0):
    '''EPHYSATCCD - Interpolate ephys data at camera frame times
    zz = EPHYSATCCD(x, cam, yy) takes the values of the signal YY,
    which must be measured at the ephys times of the vscope trial X,
    and calculates the average over each of the frames of the camera CAM.
    Optional arguments:
      - func: alternative function to apply to the data within a frame
      - dim: dimension along which to apply the averaging (not relevant
             if YY is simply a vector)'''
    yy, S = semiflatten(yy, dim)
    t_on, t_off = ccdtime(x, cam)
    t_e = ephystime(x)
    T = len(t_on)
    N = yy.shape[0]
    res = np.zeros((N, T))
    for t in range(T):
        idx = np.nonzero(np.logical_and(t_e>=t_on[t], t_e<=t_off[t]))
        if len(idx)>0:
            for n in range(N):
                res[n,t] = func(yy[n,idx])
    return semiunflatten(res, S)
