from distutils.core import setup, Extension
import numpy.distutils.misc_util

c_ext = Extension("pinfo", ["pinfo.c"])

setup(
    ext_modules=[c_ext],
    include_dirs=[ "./" ],
)
