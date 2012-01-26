#include "texture.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


Texture::Texture()
    :
        m_texturehandle(0),   
        m_textureinitialized(false)
{
}


Texture::~Texture()
{
    // clear texture
    if(m_textureinitialized) {
        glDeleteTextures(1,(const GLuint*) &m_texturehandle);
    }
}


void Texture::setup()
{
    if(!m_textureinitialized) {
        GLubyte* image=new GLubyte[256*256*3];
        for(int y=0;y<256;++y)
        {
            const int t=y>>4;
            GLubyte* pi=image+y*256*3;
            for(int x=0;x<256;++x)
            {
                const int s=x>>4;
                const GLubyte b=180; 
                GLubyte c=b+(((s+t)&1)&1)*(255-b);
                pi[0]=pi[1]=pi[2]=c;pi+=3;
            }
        }

        glGenTextures(1,(GLuint*)&m_texturehandle);
        glBindTexture(GL_TEXTURE_2D,m_texturehandle);
        glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D,3,256,256,GL_RGB,GL_UNSIGNED_BYTE,image);
        delete[] image;
    }
}


void Texture::begin()
{
    setup();

    static const GLfloat planex[]={1,0,0,0};
    static const GLfloat planez[]={0,0,1,0};
    glTexGenfv(GL_S,GL_OBJECT_PLANE,planex);
    glTexGenfv(GL_T,GL_OBJECT_PLANE,planez);
    glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    m_textureinitialized=true;

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(0.025f,0.025f,0.025f);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texturehandle);
}


void Texture::end()
{
    glDisable(GL_TEXTURE_2D);
}

