#!/usr/bin/env python

import os
import shutil
from setuptools import setup, Extension
from distutils.dir_util import mkpath

BULLET_DIR='../../bullet-2.79'
PY_MODULE_DIR='build/lib.win32-2.7/swigbullet'

# copy to current
def remove_if_eixsts(f):
    if os.path.exists(f):
        shutil.rmtree(f, True)
def copy_if_not_exists(f):
    if not os.path.exists(f):
        shutil.copy('../'+f, f)
copy_if_not_exists('swigbullet.__init__.i')
mkpath(PY_MODULE_DIR)

include_dirs=[
        BULLET_DIR,
        ]

def sub_module(name):
    return Extension('swigbullet._'+name,
            sources=['swigbullet.'+name+'.i'],
            swig_opts=['-I'+d for d in include_dirs]+[
                '-module', name,
                '-outdir', PY_MODULE_DIR,
                '-c++',
                '-fastdispatch',
                '-DSWIG_TYPE_TABLE=swigbullet',
                ],
            include_dirs=include_dirs,
            extra_compile_args=[
                '-Wno-unused-parameter',
                '-Wno-unused-but-set-variable',
                ],
            define_macros=[
                ],
            library_dirs=[os.path.join(BULLET_DIR, '../release')],
            libraries=[
                'BulletSoftBody',
                'BulletDynamics',
                'BulletCollision',
                'LinearMath',
                ]
            )

setup (name = 'swigbullet',
        version = '2.79-1',
        author      = "ousttrue",
        description = """python binding of bullet by swig""",
        ext_modules = [
            sub_module('__init__'),
            ],
        packages=['swigbullet.opengl']
        )

