#ifndef BULLET_WORLD_H
#define BULLET_WORLD_H

#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btAlignedObjectArray.h"


class	btCollisionShape;
class	btDynamicsWorld;
class	btRigidBody;
class	btTypedConstraint;
class btBroadphaseInterface;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;
class Camera;


class BulletWorld
{
	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
    ///this is the most important class
    btDynamicsWorld* m_dynamicsWorld;
    ///constraint for mouse picking
    btTypedConstraint* m_pickConstraint;
    btCollisionShape* m_shootBoxShape;
    btRigidBody* pickedBody;
#ifdef USE_BT_CLOCK
    btClock m_clock;
#endif //USE_BT_CLOCK

    float m_ShootBoxInitialSpeed;
    btScalar m_defaultContactProcessingThreshold;
    bool m_idle;
    bool use6Dof;
    btScalar mousePickClamping;
    int gPickingConstraintId;
    btVector3 gOldPickingPos;
    btVector3 gHitPos;
    float gOldPickingDist;

public:
    BulletWorld();
    ~BulletWorld();
    void initPhysics();
    void createWorld();
    void createGround();
    void createCubes();
    void exitPhysics();
    void serialize();
    void debugDrawWorld();
    btDynamicsWorld* getDynamicsWorld()
    {
        return m_dynamicsWorld;
    }
    btScalar getDeltaTimeMicroseconds()
    {
#ifdef USE_BT_CLOCK
        btScalar dt = (btScalar)m_clock.getTimeMicroseconds();
        m_clock.reset();
        return dt;
#else
        return btScalar(16666.);
#endif
    }
    void setShootBoxShape ();
    void shootBox(Camera *camera, int x, int y);
    void pickStart(Camera *camera, int x, int y);
    void pick(Camera *camera, int x, int y);
    void removePickingConstraint();
    void update();
    void debugDraw();
    void clientResetScene();
    void setDebugMode(int mode);
    void setShootInitialSpeed(btScalar initialSpeed)
    {
        m_ShootBoxInitialSpeed=initialSpeed;
    }
    void toggleIdle() {
        m_idle=!m_idle;
    }
    void setIdle(bool idle)
    {
        m_idle = idle;
    }
    bool isIdle() const
    {
        return m_idle;
    }
    btScalar getShootInitialSpeed(){
        return m_ShootBoxInitialSpeed;
    }
    btRigidBody* localCreateRigidBody(float mass, 
            const btTransform& startTransform,btCollisionShape* shape);
    void removeLastObject();
    void showProfileInfo(int& xOffset,int& yStart, int yIncr);
    void keyboardCallback(unsigned char key, int x, int y);
};


#endif
