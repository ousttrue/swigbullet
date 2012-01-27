==========
swigbullet
==========
bullet wrapper by swig.
python binding is supported. 

directories
===========

bullet-2.79
-----------
* http://freeglut.sourceforge.net/
ZLib license.
Reconstruction of taking out the class in a class outside is carried out. 

For example, btRigidBody::btRigidBodyConstructionInfo to btRigidBodyConstructionInfo.

freeglut
--------
* http://freeglut.sourceforge.net/
MIT license.

BasicDemo
---------
based bullet-2.79/Demos/BasicDemo.
depend on freeglut.

gldrawer
--------
The utility which draws the bullet world  by OpenGL.
based bullet-2.79/Demos/OpenGL.

swigbullet
----------
bullet wrapper by swig.

build on Windows
================

0. requirement
--------------
on msys shell

::

    $ /c/Python27/python --version
    Python 2.7.2
    $ cat /c/Python27/Lib/distutils/distutils.cfg
    [build]
    compiler=mingw32
    
    $ gcc --version
    gcc.exe (tdm-1) 4.6.1
    Copyright (C) 2011 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    
    $ premake4 --version
    premake4 (Premake Build Script Generator) 4.3
    
    $ swig -version
    SWIG Version 2.0.4
    Compiled with i586-mingw32msvc-g++ [i686-pc-linux-gnu]
    Configured options: +pcre
    Please see http://www.swig.org for reporting bugs and further information

1. bullet-2.79
--------------
::

    $ cd bullet-2.79
    $ premake4 gmake
    $ make
    $ ls ../release
    libBulletDynamics.a libLinearMath.a libBulletCollision.a libBulletSoftBody.a

2. gldrawer
-----------
The utility which draws the bullet world by OpenGL.

::

    $ cd gldrawer
    $ premake4 gmake
    $ make
    $ ls ../release
    libgldrawer.a

3. swigbullet/python binding
----------------------------
::

    $ cd swigbullet/python
    $ /c/Python27/python setup.py install
    
    # examples
    $ /c/Python27/examples/HelloWorld.py
    $ /c/Python27/examples/Demo.py

