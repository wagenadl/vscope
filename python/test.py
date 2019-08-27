#!/usr/bin/python3

import vscope
fn = '/home/wagenaar/tmp/170428/006.xml'
x = vscope.load(fn)
print(x.digital.keys())
print(x.ccd.keys())
print(4 in x.digital)
print('Top' in x.ccd)
print('Bottom' in x.ccd)

