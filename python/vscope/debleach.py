#!/usr/bin/python3

from . import salpa
from . import physfit
from . import utils
import numpy as np

class Debleach:
    def __init__(self):
        self.typ = None
    def apply(self, data, skipstart=0, skipend=0):
        if type(data)==dict:
            res = {}
            for k, dat in data.items():
                res[k] = self._apply(data, skipstart, skipend)
            return res
        elif type(data)==np.ndarray:
            data, S = utils.semiflatten(data, -1)
            res = data.copy()
            for n in range(data.shape[0]):
                res[n,:] = self._apply(data[n,:], skipstart, skipend)
            return utils.semiunflatten(res, S)
        else:
            raise ValueError(f'Unsupported type for debleach: {type(data)}')
        
    def _apply(self, data, skipstart, skipend):
        return data

class SalpaDebleach(Debleach):
    def __init__(self, tau):
        '''Construct a debleacher that uses SALPA.
        TAU must be the TAU parameter for SALPA.'''
        self.typ = 'salpa'
        self.tau = tau
    def _apply(self, data, skipstart, skipend):
        m0 = np.mean(data)
        return salpa.salpa(data - m0, tau=self.tau) + m0

class PolyDebleach(Debleach):
    def __init__(self, degree=2):
        '''Construct a debleacher that uses polynomial subtraction.
        By default, a second degree polynomial is removed.
        Optional argument DEGREE changes that.'''
        self.typ = 'poly'
        self.degree = degree
    def _apply(self, data, skipstart, skipend):
        T = len(data)
        tt = np.arange(T)
        tidx = np.arange(skipstart, T-skipend, dtype=int)
        tt = tt - np.mean(tt[tidx])
        p = np.polyfit(tt[tidx], data[tidx], self.degree)
        res = data.copy()
        for k in np.arange(self.degree):
            res -= p[k] * tt**(self.degree-k)
        return res

class ExpDebleach(Debleach):
    def __init__(self):
        '''Construct a debleacher that removes an exponential trend
        from the data.'''
        self.typ = 'exp'
    def _apply(self, data, skipstart, skipend):
        T = len(data)
        tt = np.arange(T, int)
        tidx = np.arange(skipstart, T-skipend, int)
        tt -= np.mean(tt[idx])
        p = physfit.physfit('expc', tt[idx], xx[tidx])
        return data - p.apply(tt) + np.mean(data)
        
