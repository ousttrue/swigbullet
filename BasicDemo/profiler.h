#ifndef PROFILER_H
#define PROFILER_H

class CProfileIterator;


class Profiler
{
    CProfileIterator* m_profileIterator;

public:
    Profiler();
    ~Profiler();
    void keyboardCallback(unsigned char key, int x, int y);
    void render(bool isIdle, int debugMode);
    void showProfileInfo(bool isIdle, int& xOffset,int& yStart, int yIncr);
    void resize(int w, int h);
};


#endif
