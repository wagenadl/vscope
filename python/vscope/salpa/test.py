#!/usr/bin/python3

import ctypes as ct
import numpy as np

print('Python!')

_salpa = np.ctypeslib.load_library('libsalpa', '.')
