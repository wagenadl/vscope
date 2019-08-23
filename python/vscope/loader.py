# vscope/loader.py - attempt at loading vscope data into python

import xml.etree.ElementTree as ET
import numpy as np

def units(s, unit):
    '''UNITS - Decode units
    v = UNITS(s, unit), where S is a string like "1.87 uV" and UNIT is
    a compatible unit like "mV", returns the value in the requested units.
    (In this example, the result would be 0.00187.)
    A ValueError is thrown if the unit is is not compatible.'''
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
    def __init__(self, xml):
        # xml must be a <pval> 
        self.s = xml.attrib['value']
    def value(self, unit):
        return units(self.s, unit)
    def __repr__(self):
        return self.s

class PArray:
    def __init__(self, xml):
        # xml must be an <array>
        self.ids = []
        self.elts = {}
        if len(xml)==0:
            return
        for p in xml[0]:
            self.ids.append(p.attrib['id'])
        for elt in xml:
            id = elt.attrib['id']
            val = {}
            for p in elt:
                val[p.attrib['id']] = PVal(p)
            self.elts[id] = val
    def __len__(self):
        return len(self.elts)
    def __getitem__(self, id):
        return self.elts[id]
    def keys(self):
        return self.elts.keys()
    def __contains__(self, id):
        return id in self.elts
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
        for elt in xml:
            if elt.tag=='pval':
                self.pvals[elt.attrib['id']] = PVal(elt)
            elif elt.tag=='category':
                self.subgrp[elt.attrib['id']] = PGroup(elt)
            elif elt.tag=='array':
                self.parrs[elt.attrib['id']] = PArray(elt)
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
        self.cc = []
        for c in xml:
            self.cc.append(c.attrib)
    def __len__(self):
        return len(self.cc)
    def __getitem__(self, k):
        return self.cc[k]
    def __repr__(self):
        ll = []
        ll.append('Analog data:')
        ll.append('Scans: %i' % self.scans)
        ll.append('Rate:  %g kHz' % (self.rate_Hz/1e3))
        ll.append('Channels:')
        for c in range(self.channels):
            ll.append('  %i: %s' % (c, self[c]['id']))
        return '\n  '.join(ll) + '\n'

class VSDigital:
    def __init__(self, xml):
        self.scans = int(xml.attrib['scans'])
        self.rate_Hz = units(xml.attrib['rate'], 'Hz')
        self.cc = {}
        for c in xml:
            self.cc[int(c.attrib['idx'])] = c.attrib
    def __len__(self):
        return len(self.cc)
    def __getitem__(self, k):
        return self.cc[k]
    def keys(self):
        return self.cc.keys()
    def __repr__(self):
        ll = []
        ll.append('Digital data:')
        ll.append('Scans: %i' % self.scans)
        ll.append('Rate:  %g kHz' % (self.rate_Hz/1e3))
        ll.append('Lines:')
        for k in self.cc.keys():
            ll.append('  %i: %s' % (k, self[k]['id']))
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
        self.ccd = []
        for c in xml:
            cc = c.attrib
            for x in c:
                if x.tag=='transform':
                    cc['transform'] = x.attrib
            self.ccd.append(cc)
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
            rr = []
            for x in self.ccd:
                rr.append('%s: %sx%s x %s @ %s' % (x['name'],
                                                   x['serpix'], x['parpix'],
                                                   x['frames'], x['rate']))
            s += '  ccd:\n    ' + '\n    '.join(rr) + '\n'
        return s
    
def loadxml(fn):
    '''LOADXML - Load a vscope xml file

    x = LOADXML(fn) loads the named XML file.'''

    x = VScopeFile()
    tree = ET.parse(fn)
    root = tree.getroot()
    x.parsexml(root)
    return x

def loadrois(fn):
    '''LOADROIS - Load ROI info from XML
    rois = LOADROIS(fn), where FN is 'EXPT/TRIAL.xml' or 'EXPT/TRIAL-rois.xml'
    loads the ROIs from the file.
    Result is a dict where keys are ROI numbers and values are dicts 
    containing CAM, X, and Y. (The latter being numpy arrays.) '''
    if not fn.endswith('-rois.xml'):
        if fn.endswith('.xml'):
            fn = fn[:-4]
        fn += '-rois.xml'
    tree = ET.parse(fn)
    root = tree.getroot()
    top = root[0]
    if top.tag!='rois':
        raise KeyError('Expected rois tag')
    rois = {}
    for roi in top:
        id = int(roi.attrib['id'])
        rois[id] = {'cam': roi.attrib['cam']}
        n = int(roi.attrib['n'])
        xx = np.zeros(n) + np.nan
        yy = np.zeros(n) + np.nan
        k = 0
        for line in roi.text.split('\n'):
            bits = line.split(' ')
            if len(bits)==2:
                xx[k] = float(bits[0])
                yy[k] = float(bits[1])
                k += 1
        rois[id]['x'] = xx
        rois[id]['y'] = yy
    return rois

def loadanalog(fn, ana):
    '''LOADANALOG - Load analog data from VScope files
    data = LOADANALOG(fn, ana), where FN is 'EXPT/TRIAL.xml' or 
    'EXPT/TRIAL-analog.dat' loads the analog data for the given trial.
    ANA must be the 'analog' section from LOADXML().
    Result is a numpy array of TxC floats. Voltages are converted to mV, 
    currents to nA.'''
    if not fn.endswith('-analog.dat'):
        if fn.endswith('.xml'):
            fn = fn[:-4]
        fn += '-analog.dat'
    if ana is None:
        return np.zeros((0,0), dtype='float32')

    with open(fn, 'rb') as f:
        data = f.read()
    data = np.frombuffer(data, dtype='int16')
    T = int(ana.scans)
    C = int(ana.channels)
    data = np.reshape(data, (T, C))
    data = data.astype(dtype='float32')
    for c in range(C):
        off = ana[c]['offset']
        scl = ana[c]['scale']
        uni = scl[-1]
        print(off, scl, uni)
        if uni=='V':
            off = units(off, 'mV')
            scl = units(scl, 'mV')
        elif uni=='A':
            off = units(off, 'nA')
            scl = units(scl, 'nA')
        else:
            raise ValueError('Bad unit for channel %i' % c)
        data[:,c] *= scl
        data[:,c] += off
    return data

def loaddigital(fn, dig):
    '''LOADDIGITAL - Load digital data from VScope files
    data = LOADDIGITAL(fn, dig), where FN is 'EXPT/TRIAL.xml' or 
    'EXPT/TRIAL-digital.dat' loads the digital data for the given trial.
    DIG must be the 'digital' section from LOADXML().
    Result is a dict if line numbers to numpy array of bools.'''
    if not fn.endswith('-digital.dat'):
        if fn.endswith('.xml'):
            fn = fn[:-4]
        fn += '-digital.dat'
    if dig is None:
        return {}

    with open(fn, 'rb') as f:
        data = f.read()
    data = np.frombuffer(data, dtype='uint32')
    res = {}
    for line in dig.keys():
        res[line] = np.bitwise_and(data, 2**line) != 0
    return res

def loadccd(fn, ccd):
    '''LOADCCD - Load CCD imagery from VScope files
    data = LOADCCD(fn, ccd), where FN is 'EXPT/TRIAL.xml' or 
    'EXPT/TRIAL-ccd.dat' loads the CCD data for the given trial.
    CCD must be the 'ccd' section from LOADXML().
    Result is a dict mapping camera names to TxYxX data (as uint16).'''
    if not fn.endswith('-ccd.dat'):
        if fn.endswith('.xml'):
            fn = fn[:-4]
        fn += '-ccd.dat'
    if ccd is None:
        return {}

    with open(fn, 'rb') as f:
        data = f.read()
    data = np.frombuffer(data, dtype='uint16')
    ncam = len(ccd)
    frmw = np.zeros(ncam, dtype='int')
    frmh = np.zeros(ncam, dtype='int')
    nfrm = np.zeros(ncam, dtype='int')
    for k in range(ncam):
        frmw[k] = int(ccd[k]['serpix'])
        frmh[k] = int(ccd[k]['parpix'])
        nfrm[k] = int(ccd[k]['frames'])
    res = {}
    bytesperframe = np.sum(frmw * frmh)
    offset = np.cumsum(frmw*frmh)
    offset = np.concatenate((np.zeros(1, dtype='int'), offset), 0)
    for k in range(ncam):
        dat = np.zeros((nfrm[k], frmh[k], frmw[k]))
        for f in range(nfrm[k]):
            o0 = f*bytesperframe + offset[k]
            o1 = f*bytesperframe + offset[k+1]
            frm = data[o0:o1]
            dat[f,:,:] = np.reshape(frm, (1, frmh[k], frmw[k]))
        res[ccd[k]['name']] = dat
    return res
    
def load(fn):
    '''LOAD - Load all data for a VScope trial
    x = LOAD(fn) where FN is 'EXPT/TRIAL.xml' loads the xml file and all
    associated data files.
    See also LOADXML, LOADROIS, LOADANALOG, LOADDIGITAL, LOADCCD to load
    individual files.'''
    res = loadxml(fn)
    try:
        res.rois = loadrois(fn)
    except:
        pass
    if res.analog is not None:
        res.analogdata = loadanalog(fn, res.analog)
    if res.digital is not None:
        res.digitaldata = loaddigital(fn, res.digital)
    if res.ccd is not None:
        res.ccddata = loadccd(fn, res.ccd)
    return res
    
if __name__== '__main__':
    fn = '/home/wagenaar/tmp/170428/006.xml'
    xml = loadxml(fn)
    print(xml)

    rois = loadrois(fn)
    
    
