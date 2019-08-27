#!/usr/bin/python3

import ctypes as ct
import numpy as np
import os

_salpa = np.ctypeslib.load_library('libsalpa', os.path.dirname(__file__))

def salpa(data, tau, rail1=-np.inf, rail2=np.inf, thresh=np.inf,
          t_blankdepeg=5, t_ahead=5, t_chi2=15):
    '''SALPA - Suppresion of Artifacts by Local Polynomial Approximation
    y = SALPA(x, tau) performs SALPA on the data X, which must be a 1D
    numpy array.
    See Wagenaar and Potter (2001).'''
    data = data.astype('double')
    out = 0*data
    N = len(data)
    _salpa.salpapy(data.ctypes.data_as(ct.POINTER(ct.c_double)),
                   out.ctypes.data_as(ct.POINTER(ct.c_double)),
                   ct.c_int(N),
                   ct.c_double(rail1), ct.c_double(rail2),
                   ct.c_double(thresh),
                   ct.c_int(tau),
                   ct.c_int(t_blankdepeg),
                   ct.c_int(t_ahead),
                   ct.c_int(t_chi2))
    return out

if __name__ == '__main__':
    import matplotlib.pyplot as plt
    tt = 4*np.pi*np.arange(1e3)/1e3
    xx = np.sin(tt) + np.sin(30*tt)
    yy = salpa(xx, 100)
    plt.plot(xx)
    plt.plot(yy)
    plt.show()
    
