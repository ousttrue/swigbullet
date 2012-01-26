#include "shapecache.h"
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS
#include "BulletCollision/CollisionShapes/btPolyhedralConvexShape.h"
#include "BulletCollision/CollisionShapes/btTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btConeShape.h"
#include "BulletCollision/CollisionShapes/btCylinderShape.h"
#include "BulletCollision/CollisionShapes/btTetrahedronShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h"
#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "BulletCollision/CollisionShapes/btMultiSphereShape.h"
#include "BulletCollision/CollisionShapes/btConvexPolyhedron.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"


struct ShapeCache
{
    struct Edge { btVector3 n[2];int v[2]; };
    btShapeHull m_shapehull;
    btAlignedObjectArray<Edge> m_edges;

    ShapeCache(const btConvexShape* s) : m_shapehull(s) {}
    void draw();
    void drawShadow(const btVector3 &extrusion);
};
    

void ShapeCache::draw()
{
    const btShapeHull* hull = &m_shapehull;
    if (hull->numTriangles () > 0) {
        int index = 0;
        const unsigned int* idx = hull->getIndexPointer();
        const btVector3* vtx = hull->getVertexPointer();

        glBegin (GL_TRIANGLES);

        for (int i = 0; i < hull->numTriangles (); i++)
        {
            int i1 = index++;
            int i2 = index++;
            int i3 = index++;
            btAssert(i1 < hull->numIndices () &&
                    i2 < hull->numIndices () &&
                    i3 < hull->numIndices ());

            int index1 = idx[i1];
            int index2 = idx[i2];
            int index3 = idx[i3];
            btAssert(index1 < hull->numVertices () &&
                    index2 < hull->numVertices () &&
                    index3 < hull->numVertices ());

            btVector3 v1 = vtx[index1];
            btVector3 v2 = vtx[index2];
            btVector3 v3 = vtx[index3];
            btVector3 normal = (v3-v1).cross(v2-v1);
            normal.normalize ();
            glNormal3f(normal.getX(),normal.getY(),normal.getZ());
            glVertex3f (v1.x(), v1.y(), v1.z());
            glVertex3f (v2.x(), v2.y(), v2.z());
            glVertex3f (v3.x(), v3.y(), v3.z());

        }
        glEnd ();
    }
}


void ShapeCache::drawShadow(const btVector3 &extrusion)
{
    btShapeHull* hull =&m_shapehull;
    glBegin(GL_QUADS);
    for(int i=0;i<m_edges.size();++i)
    { 
        const btScalar d=btDot(m_edges[i].n[0],extrusion);
        if((d*btDot(m_edges[i].n[1],extrusion))<0)
        {
            const int q= d<0?1:0;
            const btVector3& a= hull->getVertexPointer()[m_edges[i].v[q]];
            const btVector3& b= hull->getVertexPointer()[m_edges[i].v[1-q]];
            glVertex3f(a[0],a[1],a[2]);
            glVertex3f(b[0],b[1],b[2]);
            glVertex3f(b[0]+extrusion[0],b[1]+extrusion[1],b[2]+extrusion[2]);
            glVertex3f(a[0]+extrusion[0],a[1]+extrusion[1],a[2]+extrusion[2]);
        }
    }
    glEnd();
}


//////////////////////////////////////////////////////////////////////////////
ShapeCacheManager::ShapeCacheManager()
{
}


ShapeCacheManager::~ShapeCacheManager()
{
    // clear cache
    for (int i=0;i<m_shapecaches.size();i++) {
        m_shapecaches[i]->~ShapeCache();
        btAlignedFree(m_shapecaches[i]);
    }
    m_shapecaches.clear();
}


static ShapeCacheManager g_manager;


// static
void ShapeCacheManager::draw(btConvexShape* shape)
{
    g_manager.cache(shape)->draw();
}
void ShapeCacheManager::drawShadow(btConvexShape* shape, const btVector3 &extrusion)
{
    g_manager.cache(shape)->drawShadow(extrusion);
}


ShapeCache* ShapeCacheManager::cache(btConvexShape* shape)
{
    ShapeCache* sc=(ShapeCache*)shape->getUserPointer();
    if(!sc)
    {
        sc=new(btAlignedAlloc(sizeof(ShapeCache),16)) ShapeCache(shape);
        sc->m_shapehull.buildHull(shape->getMargin());
        m_shapecaches.push_back(sc);
        shape->setUserPointer(sc);
        /* Build edges */ 
        const int ni=sc->m_shapehull.numIndices();
        const int nv=sc->m_shapehull.numVertices();
        const unsigned int* pi=sc->m_shapehull.getIndexPointer();
        const btVector3* pv=sc->m_shapehull.getVertexPointer();
        btAlignedObjectArray<ShapeCache::Edge*> edges;
        sc->m_edges.reserve(ni);
        edges.resize(nv*nv,0);
        for(int i=0;i<ni;i+=3)
        {
            const unsigned int* ti=pi+i;
            const btVector3 nrm=btCross(pv[ti[1]]-pv[ti[0]],pv[ti[2]]-pv[ti[0]]).normalized();
            for(int j=2,k=0;k<3;j=k++)
            {
                const unsigned int a=ti[j];
                const unsigned int b=ti[k];
                ShapeCache::Edge*& e=edges[btMin(a,b)*nv+btMax(a,b)];
                if(!e)
                {
                    sc->m_edges.push_back(ShapeCache::Edge());
                    e=&sc->m_edges[sc->m_edges.size()-1];
                    e->n[0]=nrm;e->n[1]=-nrm;
                    e->v[0]=a;e->v[1]=b;
                }
                else
                {
                    e->n[1]=nrm;
                }
            }
        }
    }
    return(sc);
}

