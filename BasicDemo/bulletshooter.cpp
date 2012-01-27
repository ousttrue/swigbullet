#include "bulletshooter.h"
#include "bulletworld.h"
#include <btBulletDynamicsCommon.h>


BulletShooter::BulletShooter()
    :
        m_shootBoxShape(0),
        m_ShootBoxInitialSpeed(40.f)
{
    btBoxShape* box = new btBoxShape(btVector3(.5f,.5f,.5f));
    box->initializePolyhedralFeatures();
    m_shootBoxShape = box;
}


BulletShooter::~BulletShooter()
{
	if (m_shootBoxShape)
		delete m_shootBoxShape;
}


void BulletShooter::shootBox(BulletWorld *world, 
        const btVector3 &camPos, const btVector3 &destination)
{
    btTransform startTransform;
    startTransform.setIdentity();
    //startTransform.setOrigin(camPos);

    btRigidBody* body = world->localCreateRigidBody(1.0f, startTransform, m_shootBoxShape);

    body->setLinearFactor(btVector3(1,1,1));

    body->getWorldTransform().setOrigin(camPos);
    body->getWorldTransform().setRotation(btQuaternion(0,0,0,1));
    btVector3 linVel(
            destination[0]-camPos[0],
            destination[1]-camPos[1],
            destination[2]-camPos[2]);
    linVel.normalize();
    linVel*=m_ShootBoxInitialSpeed;
    body->setLinearVelocity(linVel);
    body->setAngularVelocity(btVector3(0,0,0));
    body->setCcdMotionThreshold(0.5);
    body->setCcdSweptSphereRadius(0.9f);
}

