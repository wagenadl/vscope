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
from . import units
import numbers

class PVal:
    def __init__(self, s=""):
        self.s = s
    def fromXML(xml):
        # xml must be a <pval>
        return PVal(xml.attrib['value'])
    def value(self, unit):
        # This slightly weird organization is done because not every pval
        # is a quantity. Some are simply strings.
        return units.quantity(self.s)(unit)
    def text(self):
        return self.s
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
        self.nscans = int(xml.attrib['scans'])
        self.rate = units.quantity(xml.attrib['rate'])
        self.nchannels = int(xml.attrib['channels'])
        self.info = {}
        self.data = {}
        for c in xml:
            cno = int(c.attrib['idx'])
            cid = c.attrib['id']
            self.info[cid] = c.attrib
            self.info[cid]['idx'] = cno
        self.channels = [ None for k in self.info.keys()]
        for cid, info in self.info.items():
            self.channels[info['idx']] = cid
    def keys(self):
        return info.keys()
    def __len__(self):
        return self.nchannels
    def __getitem__(self, k):
        if isinstance(k, numbers.Integral):
            k = self.channels[k]
        return self.data[k]
    def __repr__(self):
        ll = []
        ll.append('Analog data:')
        ll.append(f'Scans: {self.nscans}')
        ll.append(f'Rate:  {self.rate("kHz")} kHz')
        ll.append(f'Channels:')
        for cid in self.channels:
            ll.append(f'  {cid}')
        return '\n  '.join(ll) + '\n'

class VSDigital:
    def __init__(self, xml):
        self.nscans = int(xml.attrib['scans'])
        self.rate = units.quantity(xml.attrib['rate'])
        self.info = {}
        self.data = {}
        self.lines = {}
        for c in xml:
            cno = int(c.attrib['idx'])
            cid = c.attrib['id']
            self.info[cid] = c.attrib
            self.info[cid]['line'] = cno
            self.lines[cno] = cid
    def __len__(self):
        return len(self.info)
    def __getitem__(self, k):
        if isinstance(k, numbers.Integral):
            k = self.lines[k]
        return self.data[k]
    def __contains__(self, cid):
        return cid in self.info or cid in self.lines
    def keys(self):
        return self.info.keys()
    def __repr__(self):
        ll = []
        ll.append('Digital data:')
        ll.append('Scans: %i' % self.nscans)
        ll.append('Rate:  %g kHz' % (self.rate('kHz')))
        ll.append('Lines:')
        for cid in self.info.keys():
            ll.append(f'  {cid} ({self.info[cid]["line"]})')
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
        def inverse(self):
            t = VSCCD.Transform()
            t.ax = 1./self.ax
            t.ay = 1./self.ay
            t.bx = -self.bx/self.ax
            t.by = -self.by/self.ay
            return t
        def __repr__(self):
            return 'ax=%g bx=%g ay=%g by=%g' % (self.ax, self.bx,
                                                self.ay, self.by)
        def apply(self, pt):
            x, y = pt
            return (self.ax*x + self.bx, self.ay*y + self.by)
            
    class Info:
        def __init__(self, elt):
            self.vals = {}
            self.vals['typebytes'] = int(elt.attrib['typebytes'])
            self.vals['rate'] = units.quantity(elt.attrib['rate'])
            self.vals['frames'] = int(elt.attrib['frames'])
            self.vals['type'] = elt.attrib['type']
            if 'framedur' in elt.attrib:
                self.vals['framedur'] = units.quantity(elt.attrib['framedur'])
            else:
                self.vals['framedur'] = None
            self.vals['delay'] = units.quantity(elt.attrib['delay'])
            self.vals['name'] = elt.attrib['name']
            self.vals['serpix'] = int(elt.attrib['serpix'])
            self.vals['parpix'] = int(elt.attrib['parpix'])
        def __len__(self):
            return len(self.vals)
        def keys(self):
            return self.vals.keys()
        def __getattr__(self, id):
            return self[id]
        def __contains__(self, id):
            return id in self.vals
        def __getitem__(self, id):
            return self.vals[id]
        def __repr__(self):
            ss = []
            for k,v in self.vals.items():
                ss.append('  %s: %s' % (k,v))
            return 'CCD Info:\n  ' + '\n  '.join(ss) + '\n'
        
    def __init__(self, xml):
        self.caminfo = []
        self.data = {}
        for c in xml:
            cc = VSCCD.Info(c)
            for x in c:
                if x.tag=='transform':
                    cc.vals['transform'] = VSCCD.Transform.fromXML(x)
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
                                                    self.analog.rate('kHz'))
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
