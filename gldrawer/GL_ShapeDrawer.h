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
#ifndef GL_SHAPE_DRAWER_H
#define GL_SHAPE_DRAWER_H

#include "LinearMath/btVector3.h"
class btCollisionShape;
class btBoxShape;
class btConcaveShape;
class btConvexPolyhedron;
class btUniformScalingShape;
class btCompoundShape;
class btMultiSphereShape;


namespace GL_ShapeDrawer {

void vector(const btVector3& v) ;
void CustomConvexShape(const btScalar *m, const btCollisionShape *shape);
void BoxShapeWireFrame(const btScalar *m, const btBoxShape *boxShape);
void Concave(const btConcaveShape *concaveMesh,
        const btVector3 &worldBoundsMin, const btVector3 &worldBoundsMax);
void ConcaveWireframe(const btConcaveShape *concaveMesh,
        const btVector3 &worldBoundsMin, const btVector3 &worldBoundsMax);
void Sphere(btScalar radius, int lats, int longs);
void Box(const btVector3 &halfExtent);
void Plane(const btVector3& planeNormal, const btVector3& planeOrigin);
void ConvexPolyhedron(const btConvexPolyhedron *poly);

//void Cylinder(float radius, float halfHeight, int upAxis);
void CoordSystem();

void Shape(const btScalar* m, const btCollisionShape *shape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin, const btVector3& worldBoundsMax);

void MultiSphereShape(const btScalar* m, 
        const btMultiSphereShape* multiSphereShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);
void UniformScalingShape(const btScalar* m, const btUniformScalingShape *scalingShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);
void CompoundShape(const btScalar *m, 
        const btCompoundShape* compoundShape,
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);

void Draw(const btScalar* m, const btCollisionShape* shape, 
        const btVector3& color,
        int debugMode,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);

void Shadow(const btScalar* m, 
        const btVector3& extrusion,
        const btCollisionShape* shape,
        const btVector3& worldBoundsMin,const btVector3& worldBoundsMax);

}


#endif //GL_SHAPE_DRAWER_H
