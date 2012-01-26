#ifndef TEXTURE_H
#define TEXTURE_H


class Texture
{
    unsigned int m_texturehandle;
    bool m_textureinitialized;

public:
    Texture();
    ~Texture();
    void setup();
    void begin();
    void end();
};


#endif
