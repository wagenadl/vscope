# vscope/types.py - class definitions

# This module contains:

# - units
# - PVal
# - PArray
# - PGroup
# - VSAnalog
# - VSDigital
# - VSCCD
# - VScopeFile

import numpy as np

def units(s, unit):
    '''UNITS - Decode units
    v = UNITS(s, unit), where S is a string like "1.87 uV" and UNIT is
    a compatible unit like "mV", returns the value in the requested units.
    (In this example, the result would be 0.00187.)
    A ValueError is thrown if the unit is is not compatible.'''
    if s.endswith('%'):
        s = s[:-1] + ' %'
    bits = s.split(' ')
    if len(bits)==1:
        if unit=='':
            return float(s)
        else:
            raise ValueError('Unit mismatch')
    else:
        if unit=='':
            raise ValueError('Unit mismatch')
        v = float(bits[0])
        u0 = bits[1]
        if u0.endswith('Hz'):
            u0 = u0[:-1]
        if unit.endswith('Hz'):
            unit = unit[:-1]
        if u0[-1]==unit[-1]:
            # Compatible
            try:
                pw0 = units.dct[u0[:-1]]
                pw1 = units.dct[unit[:-1]]
            except:
                raise ValueError('Illegal prefix')
            return v * 10**(pw0-pw1)
        else:
            raise ValueError('Unit mismatch')
units.dct = { '': 0, 'k': 3, 'M': 6, 'G': 9,
              'd': -1, 'c': -2,
              'm': -3, 'u': -6, 'n': -9, 'p': -12, 'f': -15 }


class PVal:
    def __init__(self, s=""):
        # xml must be a <pval>
        self.s = s
    def fromXML(xml):
        return PVal(xml.attrib['value'])
    def value(self, unit):
        return units(self.s, unit)
    def __call__(self, unit):
        return self.value(unit)
    def __repr__(self):
        return self.s

class PArray:
    def __init__(self, xml, gcreator=None):
        # xml must be an <array>
        self.ids = []
        self.elts = {}
        if len(xml)==0:
            return
        for p in xml[0]:
            self.ids.append(p.attrib['id'])
        for elt in xml:
            id = elt.attrib['id']
            if gcreator is None:
                val = {}
                for p in elt:
                    val[p.attrib['id']] = PVal.fromXML(p)
                    self.elts[id] = val
            else:
                self.elts[id] = gcreator(elt)
    def __len__(self):
        return len(self.elts)
    def __getitem__(self, id):
        return self.elts[id]
    def __contains__(self, id):
        return id in self.elts
    def keys(self):
        return self.elts.keys()
    def __repr__(self):
        s = 'Array with members:\n  ' + '\n  '.join(self.keys())
        s += '\ncontaining parameters:\n  ' + '\n  '.join(self.ids)
        return s

class PGroup:
    def __init__(self, xml):
        # xml must be <category> or <settings>
        self.pvals = {}
        self.subgrp = {}
        self.parrs = {}
        def gcreator(xml):
            return PGroup(xml)
        for elt in xml:
            if elt.tag=='pval':
                self.pvals[elt.attrib['id']] = PVal.fromXML(elt)
            elif elt.tag=='category':
                self.subgrp[elt.attrib['id']] = PGroup(elt)
            elif elt.tag=='array':
                self.parrs[elt.attrib['id']] = PArray(elt, gcreator)
            else:
                raise ValueError('Bad tag in PGroup: %s' % elt.tag)
    def __len__(self):
        return len(self.pvals) + len(self.subgrp) + len(self.parrs)
    def keys(self):
        kk = []
        for k in self.subgrp.keys():
            kk.append(k)
        for k in self.parrs.keys():
            kk.append(k)
        for k in self.pvals.keys():
            kk.append(k)
        return kk
    def __getattr__(self, id):
        return self[id]
    def __contains__(self, id):
        return id in self.pvals or id in self.subgrp or id in self.parrs
    def __getitem__(self, id):
        if id in self.pvals:
            return self.pvals[id]
        elif id in self.subgrp:
            return self.subgrp[id]
        elif id in self.parrs:
            return self.parrs[id]
        else:
            return KeyError('ID %s not in group' % id)
    def __repr__(self):
        ss = []
        if len(self.subgrp):
            ss.append('Subgroups:\n    ' + '\n    '.join(self.subgrp.keys()))
        if len(self.parrs):
            ss.append('Arrays:\n    ' + '\n    '.join(self.parrs.keys()))
        if len(self.pvals):
            ss.append('Params: ')
            for k,v in self.pvals.items():
                ss.append('  %s: %s' % (k,v))
        return 'Group containing:\n  ' + '\n  '.join(ss) + '\n'

class VSAnalog:
    def __init__(self, xml):
        self.scans = int(xml.attrib['scans'])
        self.rate_Hz = units(xml.attrib['rate'], 'Hz')
        self.channels = int(xml.attrib['channels'])
        self.revmap = {}
        self.cids = []
        self.cinfo = []
        self.data = []
        for c in xml:
            self.cinfo.append(c.attrib)
            self.revmap[c.attrib['id']] = int(c.attrib['idx'])
            self.cids.append(c.attrib['id'])
            self.data.append(None)
    def __len__(self):
        return len(self.cc)
    def __getitem__(self, k):
        if type(k)==str:
            k = self.revmap[k]
        return self.data[:,k]
    def __repr__(self):
        ll = []
        ll.append('Analog data:')
        ll.append('Scans: %i' % self.scans)
        ll.append('Rate:  %g kHz' % (self.rate_Hz/1e3))
        ll.append('Channels:')
        for c in range(self.channels):
            ll.append('  %i: %s' % (c, self.cids[c]))
        return '\n  '.join(ll) + '\n'

class VSDigital:
    def __init__(self, xml):
        self.scans = int(xml.attrib['scans'])
        self.rate_Hz = units(xml.attrib['rate'], 'Hz')
        self.cc = {}
        self.revmap = {}
        self.data = {}
        for c in xml:
            self.cc[int(c.attrib['idx'])] = c.attrib
            self.revmap[c.attrib['id']] = int(c.attrib['idx'])
    def __len__(self):
        return len(self.cc)
    def __getitem__(self, k):
        if type(k)==str:
            k = self.revmap[k]
        return self.data[k]
    def __contains__(self, id):
        return id in self.revmap or id in self.data
    def keys(self):
        return self.cc.keys()
    def __repr__(self):
        ll = []
        ll.append('Digital data:')
        ll.append('Scans: %i' % self.scans)
        ll.append('Rate:  %g kHz' % (self.rate_Hz/1e3))
        ll.append('Lines:')
        for k in self.cc.keys():
            ll.append('  %i: %s' % (k, self.cc[k]['id']))
        return '\n  '.join(ll) + '\n'

class VSCCD:
    class Transform:
        def __init__(self):
            self.ax = 1
            self.bx = 0
            self.ay = 1
            self.by = 0
        def fromXML(elt):
            res = VSCCD.Transform()
            res.ax = int(elt.attrib['ax'])
            res.bx = int(elt.attrib['bx'])
            res.ay = int(elt.attrib['ay'])
            res.by = int(elt.attrib['by'])
            return res
        def asAffine(self):
            return np.array(((self.ax, 0, self.bx),
                            (0, self.ay, self.by),
                            (0, 0, 1)))
        def __repr__(self):
            return 'ax=%g bx=%g ay=%g by=%g' % (self.ax, self.bx,
                                                self.ay, self.by)
    def __init__(self, xml):
        self.caminfo = []
        self.data = {}
        for c in xml:
            cc = c.attrib
            for x in c:
                if x.tag=='transform':
                    cc['transform'] = VSCCD.Transform.fromXML(x)
            self.caminfo.append(cc)
            self.data[cc['name']] = None
    def __len__(self):
        return len(self.caminfo)
    def info(self, k):
        for ii in self.caminfo:
            if ii['name']==k:
                return ii
        raise KeyError(k)
    def __getitem__(self, k):
        # key is a camera name
        return self.data[k]
    def __contains__(self, k):
        return k in self.data
    def keys(self):
        return self.data.keys()
    def __repr__(self):
        ll = []
        ll.append('CCD data:')
        ll.append('Cameras:')
        for x in self.caminfo:
            ll.append('  %s: %sx%s x %s @ %s' % (x['name'],
                                                 x['serpix'], x['parpix'],
                                                 x['frames'], x['rate']))
        return '\n  '.join(ll) + '\n'
    
class VScopeFile:
    def __init__(self):
        self.settings = None
        self.info = None
        self.analog = None
        self.digital = None
        self.ccd = None
        self.rois = None
    def parsesettings(self, xml):
        self.settings = PGroup(xml)
    def parseinfo(self, xml):
        self.info = xml.attrib
    def parseanalog(self, xml):
        self.analog = VSAnalog(xml)
    def parsedigital(self, xml):
        self.digital = VSDigital(xml)
    def parseccd(self, xml):
        self.ccd = VSCCD(xml)
    def parsexml(self, xml):
        for c in xml:
            if c.tag == 'settings':
                self.parsesettings(c)
            elif c.tag == 'info':
                self.parseinfo(c)
            elif c.tag == 'analog':
                self.parseanalog(c)
            elif c.tag == 'digital':
                self.parsedigital(c)
            elif c.tag == 'ccd':
                self.parseccd(c)
    def __repr__(self):
        s = 'VScopeFile:\n'
        if self.settings is not None:
            s += '  settings\n'
        if self.info is not None:
            s += '  info\n'
        if self.rois is not None:
            s += '  rois (%i)\n' % (len(self.rois))
        if self.analog is not None:
            s += '  analog (%i x %i @ %g kHz)\n' % (self.analog.channels,
                                                    self.analog.scans,
                                                    self.analog.rate_Hz/1e3)
        if self.digital is not None:
            s += '  digital (%i x %i)\n' % (len(self.digital),
                                            self.digital.scans)
        if self.ccd is not None:
            nn = set()
            ww = set()
            hh = set()
            rr = set()
            for x in self.ccd.caminfo:
                ww.add(x['serpix'])
                hh.add(x['parpix'])
                nn.add(x['frames'])
                rr.add(x['rate'])
            nrep = '%s' % nn.pop()
            if len(nn)>0:
                nrep += '+'
            wrep = '%s' % ww.pop()
            if len(ww)>0:
                wrep += '+'
            hrep = '%s' % hh.pop()
            if len(hh)>0:
                hrep += '+'
            rrep = '%s' % rr.pop()
            if len(rr)>0:
                rrep += '+'
            s += '  ccd (%i: %sx%s x %s @ %s)\n' % (len(self.ccd.caminfo),
                                                    wrep, hrep, nrep, rrep)
        return s
