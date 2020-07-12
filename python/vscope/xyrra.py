#!/usr/bin/python3

import numpy as np

class XYRRA:
    def __init__(self, x, y=None, R=None, r=None, phi=None):
        '''Build a struct for ELPLOT_XYRRA
        el = XYRRA(x,y,R,r,a) or el = ELBUILD_XYRRA([x,y,R,r,a])
        builds a xyrra structure.
        el = ELBUILD_XYRRA(el) where EL is already a xyrra structure just
        returns el.'''
        if type(x)==XYRRA:
            self.x = x.x
            self.y = x.y
            self.R = x.R
            self.r = x.r
            self.phi = x.phi
        elif type(x)==tuple or type(x)==list or type(x)==np.ndarray:
            self.x = x[0]
            self.y = x[1]
            self.R = x[2]
            self.r = x[3]
            self.phi = x[4]
        else:
            self.x = x
            self.y = y
            self.R = R
            self.r = r
            self.phi = phi

    def __repr__(self):
        return f'XYRRA({self.x},{self.y},{self.R},{self.r},{self.phi})'

    def stroke(self, npoints=64):
        '''STROKE - Stroke ellipse outline
        xx, yy = STROKE() returns coords of the ellipse outline.
        By default, 64 points are returned around the ellipse.
        Optional argument NPOINTS overrides this.'''
        omega = np.arange(npoints) * np.pi*2 / npoints
        xi = self.R*np.cos(omega)
        eta = self.r*np.sin(omega)
        cs = cos(self.phi)
        sn = sin(self.phi)
        xx = self.x + cs*xi - sn*eta
        yy = self.y + sn*xi + cs*eta
        return xx, yy
    
    def fill(self, Y, X, smo_r=None):
        '''FILL - Fill an ellipse
        img = FILL(Y,X,el) creates an image of size YxX, with pixels
        inside the ellipse set to logical one, others to zero.
        Optional argument SMO_R smoothes the edge by the given number
        of pixels. In that case, the result is float rather than bool.'''

        # This function is optimized by only performing computation on a 
        # subset of the image for which
        #
        #   |x-x0| <= max(el.R,el.r)   and   |y-y0| <= max(el.R,el.r).

        r = max(self.R, self.r)
        x0 = int(max(np.floor(self.x-r), 0))
        x1 = int(min(np.ceil(self.x+r), X))
        y0 = int(max(np.floor(self.y-r), 0))
        y1 = int(min(np.ceil(self.y+r), Y))

        X_ = x1 - x0
        Y_ = y1 - y0

        xx = np.reshape(np.arange(x0,x1) - self.x, [1, X_])
        yy = np.reshape(np.arange(y0,y1) - self.y, [Y_, 1])

        xi = np.cos(self.phi)*xx + np.sin(self.phi)*yy
        eta = -np.sin(self.phi)*xx + np.cos(self.phi)*yy

        efr = xi**2/self.R**2 + eta**2/self.r**2
        if smo_r is None:
            img = np.zeros((Y,X), bool)
            img[y0:y1,x0:x1] = efr <= 1
        else:
            r0 = (efr - 1) * sqrt(self.R**2+self.r**2)
            img = np.zeros((Y,X))
            img[y0:y1,x0:x1] = .5-.5*np.tanh(r0/smo_r)
        return img

if __name__=='__main__':
    el = XYRRA(30,40,20,10,0)
    img = el.fill(100,80)
    
