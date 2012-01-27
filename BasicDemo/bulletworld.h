#ifndef BULLET_WORLD_H
#define BULLET_WORLD_H
#include <btBulletDynamicsCommon.h>

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
	btBroadphaseInterface*	m_broadphase;
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
    ///this is the most important class
    btDynamicsWorld* m_dynamicsWorld;
    btScalar m_defaultContactProcessingThreshold;
    bool m_idle;

public:
    BulletWorld();
    ~BulletWorld();
    void serialize();
    void setDebugMode(int mode);
    void debugDraw();
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
    btDynamicsWorld* getDynamicsWorld()
    {
        return m_dynamicsWorld;
    }
    void update(btScalar d);
    btRigidBody* localCreateRigidBody(float mass, 
            const btTransform& startTransform,btCollisionShape* shape);
    void removeLastObject();
};


#endif
