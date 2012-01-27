#ifndef BULLET_WORLD_H
#define BULLET_WORLD_H

#include "LinearMath/btAlignedObjectArray.h"
class btCollisionShape;
class btDynamicsWorld;
class btRigidBody;
class btBroadphaseInterface;
//class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
//struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;
class btTransform;


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
#ifdef USE_BT_CLOCK
    btClock m_clock;
#endif //USE_BT_CLOCK
    btScalar m_defaultContactProcessingThreshold;
    bool m_idle;

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
    void update();
    void debugDraw();
    void clientResetScene();
    void setDebugMode(int mode);
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
    btRigidBody* localCreateRigidBody(float mass, 
            const btTransform& startTransform,btCollisionShape* shape);
    void removeLastObject();
    void showProfileInfo(int& xOffset,int& yStart, int yIncr);
};


#endif
