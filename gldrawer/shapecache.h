#ifndef SHAPE_CACHE_H
#define SHAPE_CACHE_H

#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"

class btConvexShape;
struct ShapeCache;


class ShapeCacheManager
{
    //clean-up memory of dynamically created shape hulls
    btAlignedObjectArray<ShapeCache*> m_shapecaches;
public:
    ShapeCacheManager();
    ~ShapeCacheManager();
    ShapeCache *cache(btConvexShape*);
    static void draw(btConvexShape*);
    static void drawShadow(btConvexShape*, const btVector3 &extrusion);
};


#endif
