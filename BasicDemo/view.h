#ifndef VIEW_H
#define VIEW_H

#include "LinearMath/btVector3.h"


struct View
{
    btVector3 position;
    btVector3 target;
    btVector3 up;
    int w;
    int h;
    View();
    btVector3 getRayTo(int x,int y);
    void draw();
};


#endif
