# vscope/utils.py - utility functions

# This module contains:

# - rising
# - falling
# - ephystime
# - ccdtime

import numpy as np
from .types import *

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
    '''VSCOPE_CCDTIME - Construct a vector of time points for CCD data
    (ts, te, ok) = VSCOPE_CCDTIME(x, id), where X is a VScopeFile from LOAD,
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
    
    

    
