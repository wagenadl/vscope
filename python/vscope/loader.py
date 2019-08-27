# vscope/loader.py - loading vscope data into python

# This module contains:

# - load
# - loadxml
# - loadanalog
# - loaddigital
# - loadccd
# - loadrois

import xml.etree.ElementTree as ET
import numpy as np
from .types import *
from . import utils
    
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
        off = ana.cinfo[c]['offset']
        scl = ana.cinfo[c]['scale']
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
    Result is a dict of line numbers to numpy array of bools.'''
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
        frmw[k] = int(ccd.caminfo[k]['serpix'])
        frmh[k] = int(ccd.caminfo[k]['parpix'])
        nfrm[k] = int(ccd.caminfo[k]['frames'])
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
        res[ccd.caminfo[k]['name']] = dat
    return res

def _ccdframetimes(x):
    '''_CCDFRAMETIMES - Find start and end times of CCD frames
    (start_s, end_s) = _ccdframetimes(x), where X is a VScopeFile containing
    both digital data and CCD information, returns a dict of camera names to
    frame start times and frame end times.'''
    start_s = {}
    end_s = {}

    for cam in x.ccd.keys():
        frmid = 'Frame:' + cam
        if frmid in x.digital:
            start_s[cam] = utils.rising(x.digital[frmid]) / x.digital.rate_Hz
            end_s[cam] = utils.falling(x.digital[frmid]) / x.digital.rate_Hz
    
    return (start_s, end_s)

    
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
        res.analog.data = loadanalog(fn, res.analog)
    if res.digital is not None:
        res.digital.data = loaddigital(fn, res.digital)
    if res.ccd is not None:
        res.ccd.data = loadccd(fn, res.ccd)
        (res.ccd.framestart_s, res.ccd.frameend_s) = _ccdframetimes(res)
    return res
