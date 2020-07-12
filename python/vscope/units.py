# units.py - Unit conversions

import numpy as np
import copy

fundamental = [ 'A', 'g', 'm', 's' ]
prefixes = { 'f': -15, 'p': -12, 'n': -9, 'u': -6, 'μ': -6, 'm': -3,
             'c': -2, 'd': -1, 'k': 3, 'M': 6, 'G': 9, 'T': 12 }
derived_defs = [ 'N = kg m / s^2', 'J = N m', 'C = A s', 'V = J/C', 'W = J/s',
                 'VA = W', 'Hz = 1/s', '% = 0.01',
                 'Ω = V/A', 'ohm = V/A', 'Ohm = V/A',
                 'in = 25.4 mm', 'ft = 12 in']

unit_map = {}
preferred = {}

class Quantity:
    def __init__(self):
        self._units = np.zeros(len(fundamental), dtype='int')
        self._scale = 1
        self.preferred = None

    def __mul__(self, u1):
        res = copy.deepcopy(self)
        res._units += u1._units
        res._scale *= u1._scale
        res.preferred = None
        return res

    def __truediv__(self, u1):
        res = copy.deepcopy(self)
        res._units -= u1._units
        res._scale /= u1._scale
        res.preferred = None
        return res

    def __add__(self, u1):
        if np.all(self._units == u1._units):
            res = copy.deepcopy(self)
            res._scale += u1._scale
            return res

    def __sub__(self, u1):
        if np.all(self._units == u1._units):
            res = copy.deepcopy(self)
            res._scale -= u1._scale
            return res
    
    def _fromString(s):
        ss = s.split('*')
        if len(ss)>=2:
            a = Quantity._fromString(ss.pop(0))
            for k in ss:
                a = a * Quantity(k)
            return a
        ss = s.split('/')
        if len(ss)>=2:
            a = Quantity._fromString(ss.pop(0))
            for k in ss:
                a = a / Quantity._fromString(k)
            return a
        ss = s.split(' ')
        if len(ss)>=2:
            a = Quantity._fromString(ss.pop(0))
            for k in ss:
                a = a * Quantity._fromString(k)
            return a
        ss = s.split('^')
        if len(ss)==2:
            a = Quantity._fromString(ss[0])
            k = int(ss[1])
            a._units = a._units*k
            a._scale = a._scale**k
            return a
        elif len(ss)>1:
            raise ValueError(s)
        a = Quantity()
        if s=='':
            return a
        elif s[0]=='-' or s[0]=='.':
            a._scale = float(s)
            return a
        elif s[0]>='0' and s[0]<='9':
            a._scale = float(s)
            return a
        elif s in unit_map:
            return copy.deepcopy(unit_map[s])
        elif s[0] in prefixes:
            a = copy.deepcopy(unit_map[s[1:]])
            a._scale *= 10**prefixes[s[0]]
            return a
        else:
            raise ValueError(s)

    def fromString(s):
        s = s.replace('%', ' %')
        a = Quantity._fromString(s)
        if ' ' in s:
            a.preferred = s[s.index(' '):].strip()
        return a

    def value(self, s):
        '''VALUE - The numeric value of a quantity expressed in desired units.
        VALUE(u) returns the value of the quantity expressed in units U.
        Example:
          q = units.quantity('100 mV')
          print(q.value('V')) # prints "0.1 V"
        '''
        u1 = Quantity._fromString(s)
        if np.all(u1._units==self._units):
            return self._scale / u1._scale
        else:
            raise ValueError('Incompatible units: ' + s)

    def units(self):
        '''UNITS - Return the units associated with the quantity
        UNITS() returns the units associated with the quantity. If the
        quantity was defined with specific preferred units, those are
        the ones used, otherwise, basic SI units are used.
        Examples:
          q = units.quantity('100 mV')
          print(q.units()) # prints "mV"
          q /= units.quantity('100 mA')
          print(q.units()) # prints "XXX"'''
        if self.preferred is None:
            s = str(self._units)
            if s in preferred:
                return preferred[s]
            uu = []
            for k in range(len(fundamental)):
                if self._units[k]:
                    if self._units[k]==1:
                        uu.append(fundamental[k])
                    else:
                        uu.append(fundamental[k] + '^' + str(self._units[k]))
            return ' '.join(uu)
        else:
            return self.preferred

    def __call__(self, unit):
        return self.value(unit)

    def __repr__(self):
        u = self.units()
        return '%g %s' % (self.value(u), u)

unit_map['1'] = Quantity()
for k in range(len(fundamental)):
    _unit = Quantity()
    _unit._units[k] = 1
    unit_map[fundamental[k]] = _unit
    s = str(_unit._units[k])
    if s not in preferred:
        preferred[s] = fundamental[k]
        
for k in derived_defs:
    k,v = k.split(' = ')
    unit_map[k] = Quantity._fromString(v)
    s = str(unit_map[k]._units)
    if s not in preferred:
        preferred[s] = k
    
def quantity(s):
    '''QUANTITY - Define quantity in terms of physical units
    Examples:
      q1 = units.quantity('3.1 mV')
      print(q1.value('uV')) # prints "3100."
      q2 = units.quantity('5 nA')
      q3 = q1 / q2
      print(q3.value('kΩ')) # prints "620.0"
    QUANTITY understands the following international units:
      g, m, s, V, A, C, N, J, W, Ω, Hz
    As well as standard prefixed such as "k" and "M".
    Additionally, QUANTITY understands American units in and ft,
    and aliases "Ohm" and "ohm" for "Ω" and "VA" for "W".
    '''
    return Quantity.fromString(s)
