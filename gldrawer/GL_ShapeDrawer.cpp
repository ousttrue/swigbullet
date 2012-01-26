/*
   Bullet Continuous Collision Detection and Physics Library
   Copyright (c) 2003-2006 Erwin Coumans http://continuousphysics.com/Bullet/

   This software is provided 'as-is', without any express or implied warranty.
   In no event will the authors be held liable for any damages arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose, 
   including commercial applications, and to alter it and redistribute it freely, 
   subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.
   */

#include "GL_ShapeDrawer.h"


#include "shapecache.h"
#include "GlutStuff.h"
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
#include "LinearMath/btIDebugDraw.h"

//for debugmodes
//#include <stdio.h> //printf debugging


namespace GL_ShapeDrawer {

void vector(const btVector3& v) 
{ 
    glVertex3d(v[0], v[1], v[2]); 
}


void CustomConvexShape(const btScalar *m, const btCollisionShape *shape)
{
    btVector3 org(m[12], m[13], m[14]);
    btVector3 dx(m[0], m[1], m[2]);
    btVector3 dy(m[4], m[5], m[6]);
    // btVector3 dz(m[8], m[9], m[10]);
    const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
    btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
    dx *= halfExtent[0];
    dy *= halfExtent[1];
    // dz *= halfExtent[2];
    glColor3f(1,1,1);
    glDisable(GL_LIGHTING);
    glLineWidth(2);

    glBegin(GL_LINE_LOOP);
    vector(org - dx - dy);
    vector(org - dx + dy);
    vector(org + dx + dy);
    vector(org + dx - dy);
    glEnd();
}


void BoxShapeWireFrame(const btScalar *m, const btBoxShape *boxShape)
{
    btVector3 org(m[12], m[13], m[14]);
    btVector3 dx(m[0], m[1], m[2]);
    btVector3 dy(m[4], m[5], m[6]);
    btVector3 dz(m[8], m[9], m[10]);
    btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
    dx *= halfExtent[0];
    dy *= halfExtent[1];
    dz *= halfExtent[2];
    glBegin(GL_LINE_LOOP);
    vector(org - dx - dy - dz);
    vector(org + dx - dy - dz);
    vector(org + dx + dy - dz);
    vector(org - dx + dy - dz);
    vector(org - dx + dy + dz);
    vector(org + dx + dy + dz);
    vector(org + dx - dy + dz);
    vector(org - dx - dy + dz);
    glEnd();
    glBegin(GL_LINES);
    vector(org + dx - dy - dz);
    vector(org + dx - dy + dz);
    vector(org + dx + dy - dz);
    vector(org + dx + dy + dz);
    vector(org - dx - dy - dz);
    vector(org - dx + dy - dz);
    vector(org - dx - dy + dz);
    vector(org - dx + dy + dz);
    glEnd();
}


class GlDrawcallback : public btTriangleCallback
{
public:
    virtual void processTriangle(btVector3* triangle,int partId, int triangleIndex)
    {
        glBegin(GL_TRIANGLES);
        glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
        glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
        glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
        glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
        glEnd();
    }
};
void Concave(const btConcaveShape *concaveMesh,
        const btVector3 &worldBoundsMin, const btVector3 &worldBoundsMax)
{
    GlDrawcallback drawCallback;
    concaveMesh->processAllTriangles(&drawCallback,
            worldBoundsMin, worldBoundsMax);
}


class GlDrawcallbackWireframe : public btTriangleCallback
{
public:
    virtual void processTriangle(btVector3* triangle,int partId, int triangleIndex)
    {
        glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
        glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
        glColor3f(0, 1, 0);
        glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        glVertex3d(triangle[1].getX(), triangle[1].getY(), triangle[1].getZ());
        glColor3f(0, 0, 1);
        glVertex3d(triangle[2].getX(), triangle[2].getY(), triangle[2].getZ());
        glVertex3d(triangle[0].getX(), triangle[0].getY(), triangle[0].getZ());
        glEnd();
    }
};
void ConcaveWireframe(const btConcaveShape *concaveMesh,
        const btVector3 &worldBoundsMin, const btVector3 &worldBoundsMax)
{
    GlDrawcallbackWireframe drawCallback;
    concaveMesh->processAllTriangles(&drawCallback,
            worldBoundsMin, worldBoundsMax);
}


void Sphere(btScalar radius, int lats, int longs) 
{
    int i, j;
    for(i = 0; i <= lats; i++) {
        btScalar lat0 = SIMD_PI * (-btScalar(0.5) + (btScalar) (i - 1) / lats);
        btScalar z0 = radius*sin(lat0);
        btScalar zr0 = radius*cos(lat0);

        btScalar lat1 = SIMD_PI * (-btScalar(0.5) + (btScalar) i / lats);
        btScalar z1 = radius*sin(lat1);
        btScalar zr1 = radius*cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= longs; j++) {
            btScalar lng = 2 * SIMD_PI * (btScalar) (j - 1) / longs;
            btScalar x = cos(lng);
            btScalar y = sin(lng);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
        }
        glEnd();
    }
}


void Box(const btVector3 &halfExtent)
{
    static int indices[36] = {
        0,1,2,
        3,2,1,
        4,0,6,
        6,0,2,
        5,1,4,
        4,1,0,
        7,3,1,
        7,1,5,
        5,4,7,
        7,4,6,
        7,2,3,
        7,6,2};

    btVector3 vertices[8]={ 
        btVector3(halfExtent[0],halfExtent[1],halfExtent[2]),
        btVector3(-halfExtent[0],halfExtent[1],halfExtent[2]),
        btVector3(halfExtent[0],-halfExtent[1],halfExtent[2]), 
        btVector3(-halfExtent[0],-halfExtent[1],halfExtent[2]), 
        btVector3(halfExtent[0],halfExtent[1],-halfExtent[2]),
        btVector3(-halfExtent[0],halfExtent[1],-halfExtent[2]), 
        btVector3(halfExtent[0],-halfExtent[1],-halfExtent[2]), 
        btVector3(-halfExtent[0],-halfExtent[1],-halfExtent[2])};
#if 1
    glBegin (GL_TRIANGLES);
    int si=36;
    for (int i=0;i<si;i+=3)
    {
        const btVector3& v1 = vertices[indices[i]];;
        const btVector3& v2 = vertices[indices[i+1]];
        const btVector3& v3 = vertices[indices[i+2]];
        btVector3 normal = (v3-v1).cross(v2-v1);
        normal.normalize ();
        glNormal3f(normal.getX(),normal.getY(),normal.getZ());
        glVertex3f (v1.x(), v1.y(), v1.z());
        glVertex3f (v2.x(), v2.y(), v2.z());
        glVertex3f (v3.x(), v3.y(), v3.z());

    }
    glEnd();
#endif
}


void Plane(const btVector3& planeNormal, const btVector3& planeOrigin)
{
    btVector3 vec0,vec1;
    btPlaneSpace1(planeNormal,vec0,vec1);
    btScalar vecLen = 100.f;
    btVector3 pt0 = planeOrigin + vec0*vecLen;
    btVector3 pt1 = planeOrigin - vec0*vecLen;
    btVector3 pt2 = planeOrigin + vec1*vecLen;
    btVector3 pt3 = planeOrigin - vec1*vecLen;
    glBegin(GL_LINES);
    glVertex3f(pt0.getX(),pt0.getY(),pt0.getZ());
    glVertex3f(pt1.getX(),pt1.getY(),pt1.getZ());
    glVertex3f(pt2.getX(),pt2.getY(),pt2.getZ());
    glVertex3f(pt3.getX(),pt3.getY(),pt3.getZ());
    glEnd();
}


void ConvexPolyhedron(const btConvexPolyhedron *poly)
{
    glBegin (GL_TRIANGLES);
    for (int i=0;i<poly->m_faces.size();i++)
    {
        btVector3 centroid(0,0,0);
        int numVerts = poly->m_faces[i].m_indices.size();
        if (numVerts>2) {
            btVector3 v1 = poly->m_vertices[poly->m_faces[i].m_indices[0]];
            for (int v=0;v<poly->m_faces[i].m_indices.size()-2;v++)
            {

                btVector3 v2 = poly->m_vertices[poly->m_faces[i].m_indices[v+1]];
                btVector3 v3 = poly->m_vertices[poly->m_faces[i].m_indices[v+2]];
                btVector3 normal = (v3-v1).cross(v2-v1);
                normal.normalize ();
                glNormal3f(normal.getX(),normal.getY(),normal.getZ());
                glVertex3f (v1.x(), v1.y(), v1.z());
                glVertex3f (v2.x(), v2.y(), v2.z());
                glVertex3f (v3.x(), v3.y(), v3.z());
            }
        }
    }
    glEnd ();
} 


void CoordSystem() {
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(1, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 1, 0);
    glColor3f(0, 0, 1);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 1);
    glEnd();
}


/*
void Cylinder(float radius,float halfHeight, int upAxis)
{
    glPushMatrix();
    switch (upAxis)
    {
        case 0:
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glTranslatef(0.0, 0.0, -halfHeight);
            break;
        case 1:
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            glTranslatef(0.0, 0.0, -halfHeight);
            break;
        case 2:

            glTranslatef(0.0, 0.0, -halfHeight);
            break;
        default:
            {
                btAssert(0);
            }
    }
    GLUquadricObj *quadObj = gluNewQuadric();

    //The gluCylinder subroutine draws a cylinder that is oriented along the z axis. 
    //The base of the cylinder is placed at z = 0; 
    //the top of the cylinder is placed at z=height. 
    //Like a sphere, the cylinder is subdivided around the z axis 
    //into slices and along the z axis into stacks.

    gluQuadricDrawStyle(quadObj, (GLenum)GLU_FILL);
    gluQuadricNormals(quadObj, (GLenum)GLU_SMOOTH);

    gluDisk(quadObj,0,radius,15, 10);

    gluCylinder(quadObj, radius, radius, 2.f*halfHeight, 15, 10);
    glTranslatef(0.0f, 0.0f, 2.f*halfHeight);
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f);
    gluDisk(quadObj,0.f,radius,15, 10);

    glPopMatrix();
    gluDeleteQuadric(quadObj);
}
*/


void Shape(const btScalar* m, const btCollisionShape *shape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin, const btVector3& worldBoundsMax)
{
    glColor3f(color.x(), color.y(), color.z()); 

    if (!(debugMode & btIDebugDraw::DBG_DrawWireframe)) {

        ///you can comment out any of the specific cases, and use the default
        ///the benefit of 'default' is that it approximates 
        //the actual collision shape including collision margin
        switch (shape->getShapeType())
        {
            case SPHERE_SHAPE_PROXYTYPE:
                {
                    const btSphereShape* sphereShape = 
                        static_cast<const btSphereShape*>(shape);
                    float radius = sphereShape->getMargin();
                    //radius doesn't include the margin, so draw with margin
                    Sphere(radius, 10, 10);
                    break;
                }

            case BOX_SHAPE_PROXYTYPE:
                {
                    const btBoxShape* boxShape = 
                        static_cast<const btBoxShape*>(shape);
                    btVector3 halfExtent = boxShape->getHalfExtentsWithMargin();
                    Box(halfExtent);
                    break;
                }

            case STATIC_PLANE_PROXYTYPE:
                {
                    const btStaticPlaneShape* staticPlaneShape = 
                        static_cast<const btStaticPlaneShape*>(shape);
                    btScalar planeConst = staticPlaneShape->getPlaneConstant();
                    const btVector3& planeNormal = staticPlaneShape->getPlaneNormal();
                    btVector3 planeOrigin = planeNormal * planeConst;
                    Plane(planeNormal, planeOrigin);
                    break;
                }

            default:
                {
                    if (shape->isConvex()) {
                        const btConvexPolyhedron* poly = shape->isPolyhedral() 
                            ? ((btPolyhedralConvexShape*) shape)->getConvexPolyhedron() 
                            : 0;
                        if (poly) {
                            ConvexPolyhedron(poly);
                        }
                        else {
                            ShapeCacheManager::draw((btConvexShape*)shape);
                        }
                    }
                    break;
                }
        }

    }
    glNormal3f(0,1,0);

    if (shape->isConcave() && !shape->isInfinite())
    {
        if((debugMode & btIDebugDraw::DBG_DrawWireframe)!=0){
            ConcaveWireframe((btConcaveShape*)shape, worldBoundsMin, worldBoundsMax);
        }
        else{
            Concave((btConcaveShape*)shape, worldBoundsMin, worldBoundsMax);
        }
    }
}


void UniformScalingShape(const btScalar* m, 
        const btUniformScalingShape *scalingShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
    glPushMatrix(); 
    btglMultMatrix(m);
    const btConvexShape* convexShape = scalingShape->getChildShape();
    float scalingFactor = (float)scalingShape->getUniformScalingFactor();
    {
        btScalar tmpScaling[4][4]={{scalingFactor,0,0,0},
            {0,scalingFactor,0,0},
            {0,0,scalingFactor,0},
            {0,0,0,1}};

        Draw((btScalar*)tmpScaling, convexShape,
                color,debugMode,worldBoundsMin,worldBoundsMax);
    }
    glPopMatrix();
}


void CompoundShape(const btScalar *m, 
        const btCompoundShape* compoundShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
    glPushMatrix(); 
    btglMultMatrix(m);
    for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
    {
        btTransform childTrans = compoundShape->getChildTransform(i);
        const btCollisionShape* colShape = compoundShape->getChildShape(i);
        btScalar childMat[16];
        childTrans.getOpenGLMatrix(childMat);
        Draw(childMat,colShape,color,debugMode,worldBoundsMin,worldBoundsMax);
    }
    glPopMatrix();
}


void MultiSphereShape(const btScalar* m, 
        const btMultiSphereShape* multiSphereShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
    glPushMatrix(); 
    btglMultMatrix(m);
    btTransform childTransform;
    childTransform.setIdentity();
    for (int i = multiSphereShape->getSphereCount()-1; i>=0;i--)
    {
        btSphereShape sc(multiSphereShape->getSphereRadius(i));
        childTransform.setOrigin(multiSphereShape->getSpherePosition(i));
        btScalar childMat[16];
        childTransform.getOpenGLMatrix(childMat);
        Draw(childMat,&sc,color,debugMode,worldBoundsMin,worldBoundsMax);
    }
    glPopMatrix();
}


void Draw(const btScalar* m, const btCollisionShape* shape, 
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
    if (shape->getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE) {
        CustomConvexShape(m, shape);
    } 
    else if((shape->getShapeType() == BOX_SHAPE_PROXYTYPE) 
            && (debugMode & btIDebugDraw::DBG_FastWireframe)) {
        BoxShapeWireFrame(m, static_cast<const btBoxShape*>(shape));
    }
    else if (shape->getShapeType() == UNIFORM_SCALING_SHAPE_PROXYTYPE) {
        UniformScalingShape(m, static_cast<const btUniformScalingShape*>(shape),
                color, debugMode, worldBoundsMin, worldBoundsMax);
    }
    else if (shape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE) {
        CompoundShape(m, static_cast<const btCompoundShape*>(shape),
                color, debugMode, worldBoundsMin, worldBoundsMax);
    } 
    else if (shape->getShapeType() == MULTI_SPHERE_SHAPE_PROXYTYPE) {
        MultiSphereShape(m, static_cast<const btMultiSphereShape*>(shape),
                color, debugMode, worldBoundsMin, worldBoundsMax);
    }
    else {
        // single shape
        glPushMatrix(); 
        btglMultMatrix(m);
        glEnable(GL_COLOR_MATERIAL);
        Shape(m, shape, color, debugMode, worldBoundsMin, worldBoundsMax);
        glPopMatrix();
    }
}


void Shadow(const btScalar* m,const btVector3& extrusion,
        const btCollisionShape* shape,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax)
{
    glPushMatrix(); 
    btglMultMatrix(m);
    if(shape->getShapeType() == UNIFORM_SCALING_SHAPE_PROXYTYPE)
    {
        const btUniformScalingShape* scalingShape = 
            static_cast<const btUniformScalingShape*>(shape);
        const btConvexShape* convexShape = scalingShape->getChildShape();
        float scalingFactor = (float)scalingShape->getUniformScalingFactor();
        btScalar tmpScaling[4][4]={ {scalingFactor,0,0,0},
            {0,scalingFactor,0,0},
            {0,0,scalingFactor,0},
            {0,0,0,1}};
        Shadow((btScalar*)tmpScaling,extrusion,convexShape,worldBoundsMin,worldBoundsMax);
        glPopMatrix();
        return;
    }
    else if(shape->getShapeType()==COMPOUND_SHAPE_PROXYTYPE)
    {
        const btCompoundShape* compoundShape = static_cast<const btCompoundShape*>(shape);
        for (int i=compoundShape->getNumChildShapes()-1;i>=0;i--)
        {
            btTransform childTrans = compoundShape->getChildTransform(i);
            const btCollisionShape* colShape = compoundShape->getChildShape(i);
            btScalar childMat[16];
            childTrans.getOpenGLMatrix(childMat);
            Shadow(childMat, 
                    extrusion*childTrans.getBasis(),colShape,worldBoundsMin,worldBoundsMax);
        }
    }
    else
    {
        if (shape->isConvex())
        {
            ShapeCacheManager::drawShadow((btConvexShape*)shape, extrusion);
        }
    }

    if (shape->isConcave())
    {
        Concave((btConcaveShape*) shape, worldBoundsMin, worldBoundsMax);
    }
    glPopMatrix();
}


} // namespace

