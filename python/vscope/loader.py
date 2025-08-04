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
    containing CAM, X, and Y. (The latter being numpy arrays.)
    Note that ROI numbers count from 1, as in Octave.'''
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
        rois[id] = {'cams': roi.attrib['cam'].split(':')}
        if 'n' in roi.attrib:
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
        else:
            rois[id]['x0'] = roi.attrib['x0']
            rois[id]['y0'] = roi.attrib['y0']
            rois[id]['R'] = roi.attrib['R']
            rois[id]['r'] = roi.attrib['r']
            rois[id]['a'] = roi.attrib['a']
    return rois

def loadanalog(fn, ana):
    '''LOADANALOG - Load analog data from VScope files
    data = LOADANALOG(fn, ana), where FN is 'EXPT/TRIAL.xml' or 
    'EXPT/TRIAL-analog.dat' loads the analog data for the given trial.
    ANA must be the 'analog' section from LOADXML().
    Result is dict of channel names to vectors of T floats.
    Voltages are converted to mV, currents to nA.'''
    if not fn.endswith('-analog.dat'):
        if fn.endswith('.xml'):
            fn = fn[:-4]
        fn += '-analog.dat'
    if ana is None:
        return np.zeros((0,0), dtype='float32')

    with open(fn, 'rb') as f:
        data = f.read()
    data = np.frombuffer(data, dtype='int16')
    T = int(ana.nscans)
    C = int(ana.nchannels)
    data = np.reshape(data, (T, C))
    res = {}
    for cid in ana.channels:
        if 'scale' in ana.info[cid]:
            scl = ana.info[cid]['scale']
        else:
            scl = '1 mV'
            print(f'Caution: assuming scale for channel {c} is {scl}')
        uni = scl[-1]
        if 'offset' in ana.info[cid]:
            off = ana.info[cid]['offset']
        else:
            off = '0 ' + uni
        if uni=='V':
            off = units.quantity(off)('mV')
            scl = units.quantity(scl)('mV')
        elif uni=='A':
            off = units.quantity(off)('nA')
            scl = units.quantity(scl)('nA')
        else:
            raise ValueError('Bad unit for channel %i' % c)
        res[cid] = data[:,ana.info[cid]['idx']].astype(np.float32)*scl + off
    return res

def loaddigital(fn, dig):
    '''LOADDIGITAL - Load digital data from VScope files
    data = LOADDIGITAL(fn, dig), where FN is 'EXPT/TRIAL.xml' or 
    'EXPT/TRIAL-digital.dat' loads the digital data for the given trial.
    DIG must be the 'digital' section from LOADXML().
    Result is a dict of line IDs to numpy array of bools.'''
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
    for cid in dig.keys():
        line = dig.info[cid]['line']
        res[cid] = np.bitwise_and(data, 2**line) != 0
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
    offset = 0
    for k in range(ncam):
        dat = np.zeros((nfrm[k], frmh[k], frmw[k]), dtype=np.uint16)
        for f in range(nfrm[k]):
            n = frmw[k]*frmh[k]
            frm = data[offset:offset+n]
            offset += n
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
        if x.digital and frmid in x.digital:
            start_s[cam] = utils.rising(x.digital[frmid]) / x.digital.rate('Hz')
            end_s[cam] = utils.falling(x.digital[frmid]) / x.digital.rate('Hz')
    
    return (start_s, end_s)

    
def load(fn, loaddata=True, loadvsddata=True):
    '''LOAD - Load all data for a VScope trial
    x = LOAD(fn) where FN is 'EXPT/TRIAL.xml' loads the xml file and all
    associated data files.
    x = LOAD(fn, loaddata=False) loads only the information (incl. rois).
    x = LOAD(fn, loadvsddata=False) loads only info and ephys
    See also LOADXML, LOADROIS, LOADANALOG, LOADDIGITAL, LOADCCD to load
    individual files.'''
    res = loadxml(fn)
    try:
        res.rois = loadrois(fn)
    except:
        res.rois = None
    if loaddata and res.analog is not None:
        res.analog.data = loadanalog(fn, res.analog)
    if loaddata and res.digital is not None:
        res.digital.data = loaddigital(fn, res.digital)
    if loaddata and res.ccd is not None:
        if loadvsddata:
            res.ccd.data = loadccd(fn, res.ccd)
        (res.ccd.framestart_s, res.ccd.frameend_s) = _ccdframetimes(res)
    for camno in range(len(res.ccd.caminfo)):
        info = res.ccd.caminfo[camno]
        camid = info.name
        xform = info.transform
        if xform.ax<0:
            if loadvsddata:
                res.ccd.data[camid] = np.flip(res.ccd.data[camid], 2)
            res.ccd.caminfo[camno].transform.ax = -xform.ax
            res.ccd.caminfo[camno].transform.bx -= xform.ax*info.serpix
        if xform.ay<0:
            if loadvsddata:
                res.ccd.data[camid] = np.flip(res.ccd.data[camid], 1)
            res.ccd.caminfo[camno].transform.ay = -xform.ay
            res.ccd.caminfo[camno].transform.by -= xform.ay*info.parpix
    return res
