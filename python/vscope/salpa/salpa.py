#!/usr/bin/python3

import ctypes as ct
import numpy as np
import os

_salpa = np.ctypeslib.load_library('libsalpa', os.path.dirname(__file__))

class Salpa:
    def __init__(self, data, tau, rail1=-np.inf, rail2=np.inf, thresh=3,
                 t_blankdepeg=5, t_ahead=5, t_chi2=15):
        self.data = data.astype('double')
        self.out = 0*data
        N = len(data)
        self.ptr = _salpa.salpa_start(
            self.data.ctypes.data_as(ct.POINTER(ct.c_double)),
            self.out.ctypes.data_as(ct.POINTER(ct.c_double)),
            ct.c_uint64(N),
            ct.c_double(rail1), ct.c_double(rail2),
            ct.c_double(thresh),
            ct.c_int(tau),
            ct.c_int(t_blankdepeg),
            ct.c_int(t_ahead),
            ct.c_int(t_chi2))
        if self.ptr==0:
            raise Exception('Could not construct salpa object')
    def __del__(self):
        self.close()
    def close(self):
        if self.ptr is not None:
            _salpa.salpa_end(self.ptr)
        self.ptr = None
    def complete(self):
        if self.ptr is None:
            return
        self.partial(len(self.data))
        self.close()
        return self.out
    def partial(self, t_to):
        if self.ptr is None:
            raise Exception('Salpa object not active')            
        _salpa.salpa_partial(self.ptr, ct.c_uint64(t_to))
    def forcepeg(self, t_from, t_to):
        if self.ptr is None:
            raise Exception('Salpa object not active')            
        _salpa.salpa_forcepeg(self.ptr, ct.c_uint64(t_from), ct.c_uint64(t_to))
    def result(self):
        return self.out

def salpa(data, tau, rail1=-np.inf, rail2=np.inf, thresh=3,
          t_blankdepeg=5, t_ahead=5, t_chi2=15, tt_stimuli=None, t_forcepeg=10):
    '''SALPA - Suppresion of Artifacts by Local Polynomial Approximation
    y = SALPA(x, tau) performs SALPA on the data X, which must be a 1D
    numpy array. Note that THRESH is absolute. You need to estimate the noise
    level with an external tool.
    See Wagenaar and Potter (2001).'''
    slp = Salpa(data, tau, rail1, rail2, thresh, t_blankdepeg, t_ahead, t_chi2)
    if tt_stimuli is not None:
        for t in tt_stimuli:
            slp.forcepeg(t, t+t_forcepeg)
    return slp.complete()

if __name__ == '__main__':
    import pyqplot as qp
    ## Prepare some fake data
    tt = 4*np.pi*np.arange(1e4)/1e4
    xx = np.sin(tt) + np.random.randn(len(tt))#sin(30*tt)
    A=100
    xx[np.logical_and(tt>4, tt<4.5)] = A
    iart = tt>=4.5
    tart = tt[iart] - 4.5
    xx[iart] += A*np.cos(np.sqrt(tart/.004))*np.exp(-tart/1)

    def show(tt, xx, yy, name):
        qp.figure(f'/tmp/{name}',7,2)
        qp.pen('469', 1, join='round')
        qp.plot(tt, xx)
        qp.pen('r')
        qp.plot(tt,yy)
        qp.shrink()
    
    ## Demonstrate full processing
    yy = salpa(xx, tau=75, thresh=3, rail1=-A*.99, rail2=A*.99)
    show(tt, xx, yy, 'salpa')
    
    ## Demonstrate without rails
    yy = salpa(xx, tau=75, thresh=3)
    show(tt, xx, yy, 'salpa0')
    
    ## Demonstrate partial processing
    slp = Salpa(xx, tau=75, thresh=3)
    istart = np.argmax(xx>.99*A)
    iend = istart + np.argmax(xx[istart:]<.99*A)
    slp.forcepeg(istart-10, iend)
    yy = slp.complete()
    show(tt, xx, yy, 'salpa1')
    
