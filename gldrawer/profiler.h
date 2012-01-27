#ifndef PROFILER_H
#define PROFILER_H

class CProfileIterator;


class Profiler
{
    CProfileIterator* m_profileIterator;
    int m_w;
    int m_h;

public:
    Profiler();
    ~Profiler();
    void keyboardCallback(unsigned char key, int x, int y);
    void render(bool isIdle, int debugMode, int w, int h);
private:
    void showProfileInfo(bool isIdle, int& xOffset,int& yStart, int yIncr, int w, int h);
};


#endif
