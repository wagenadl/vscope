# vscope.py - attempt at loading vscope data into python

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

def loadxml(fn):
    '''LOADXML - Load a vscope xml file

    dict = LOADXML(fn) loads the named XML file into a nested dictionary.
    Current version does not do much parsing at all.'''

    def settings(elt):
        res = {}
        for c in elt:
            atr = c.attrib
            if 'value' in atr:
                res[atr['id']] = atr['value']
            else:
                res[atr['id']] = settings(c)
        return res

    def info(elt):
        return elt.attrib

    def analog(elt):
        res = elt.attrib
        cc = []
        for c in elt:
            cc.append(c.attrib)
        res['channel'] = cc
        return res

    def digital(elt):
        res = elt.attrib
        cc = {}
        for c in elt:
            cc[int(c.attrib['idx'])] = c.attrib['id']
        res['line'] = cc
        return res

    def ccdcams(elt):
        res = elt.attrib
        res['camera'] = []
        for c in elt:
            cc = c.attrib
            for x in c:
                if x.tag=='transform':
                    cc['transform'] = x.attrib
            res['camera'].append(cc)
        return res
    
    tree = ET.parse(fn)
    root = tree.getroot()
    ss = None
    ii = None
    ana = None
    dig = None
    ccd = None
    for c in root:
        if c.tag == 'settings':
            ss = settings(c)
        elif c.tag == 'info':
            ii = info(c)
        elif c.tag == 'analog':
            ana = analog(c)
        elif c.tag == 'digital':
            dig = digital(c)
        elif c.tag == 'ccd':
            ccd = ccdcams(c)
    return { 'settings': ss, 'info': ii,
             'analog': ana, 'digital': dig, 'ccd': ccd }

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
    T = int(ana['scans'])
    C = int(ana['channels'])
    data = np.reshape(data, (T, C))
    data = data.astype(dtype='float32')
    for c in range(C):
        off = ana['channel'][c]['offset']
        scl = ana['channel'][c]['scale']
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
    for line in dig['line'].keys():
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
    ncam = len(ccd['camera'])
    frmw = np.zeros(ncam, dtype='int')
    frmh = np.zeros(ncam, dtype='int')
    nfrm = np.zeros(ncam, dtype='int')
    for k in range(ncam):
        frmw[k] = int(ccd['camera'][k]['serpix'])
        frmh[k] = int(ccd['camera'][k]['parpix'])
        nfrm[k] = int(ccd['camera'][k]['frames'])
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
        res[ccd['camera'][k]['name']] = dat
    return res
    
def load(fn):
    '''LOAD - Load all data for a VScope trial
    x = LOAD(fn) where FN is 'EXPT/TRIAL.xml' loads the xml file and all
    associated data files.
    See also LOADXML, LOADROIS, LOADANALOG, LOADDIGITAL, LOADCCD to load
    individual files.'''
    res = loadxml(fn)
    try:
        res['rois'] = loadrois(fn)
    except:
        pass
    if res['analog'] is not None:
        res['analog']['data'] = loadanalog(fn, res['analog'])
    if res['digital'] is not None:
        res['digital']['data'] = loaddigital(fn, res['digital'])
    if res['ccd'] is not None:
        res['ccd']['data'] = loadccd(fn, res['ccd'])
    return res
    
if __name__== '__main__':
    fn = '/home/wagenaar/tmp/170428/006.xml'
    xml = loadxml(fn)
    print(xml)

    rois = loadrois(fn)
    
    
