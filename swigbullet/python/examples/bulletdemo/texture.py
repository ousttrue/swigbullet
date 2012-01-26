from OpenGL.GL import *
from OpenGL.GLU import *
import numpy


class Texture(object):
    def __init__(self):
        self.m_texturehandle=0
        self.m_textureinitialized=False
        self.planex=numpy.array([1,0,0,0], 'f')
        self.planez=numpy.array([0,0,1,0], 'f')

    def __del__(self):
        if(self.m_textureinitialized):
            glDeleteTextures(1, self.m_texturehandle)

    def _create(self):
        if self.m_textureinitialized:
            glBindTexture(GL_TEXTURE_2D, self.m_texturehandle);
            return

        image=numpy.zeros((256*256*3,), numpy.uint8)
        pi=0
        for y in range(256):
            t=y>>4;
            for x in range(256):
                s=x>>4;
                b=180; 
                c=b+(((s+t)&1)&1)*(255-b);
                image[pi+0]=c
                image[pi+1]=c
                image[pi+2]=c
                pi+=3;

        self.m_texturehandle=glGenTextures(1);
        glBindTexture(GL_TEXTURE_2D, self.m_texturehandle);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, image);
        self.m_textureinitialized=True

    def begin(self):
        self._create();

        glEnable(GL_TEXTURE_2D);
        # uv generation
        glTexGenfv(GL_S, GL_OBJECT_PLANE, self.planex);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, self.planez);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);
        # texture scaling
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glScalef(0.025 ,0.025 ,0.025);
        glMatrixMode(GL_MODELVIEW);

    def end(self):
        glDisable(GL_TEXTURE_2D);

