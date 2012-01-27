#include "bulletworld.h"


BulletWorld::BulletWorld()
    :
        m_dynamicsWorld(0),
        m_defaultContactProcessingThreshold(BT_LARGE_FLOAT),
        m_idle(false)
{
    ///collision configuration contains default setup for memory, collision setup
    m_collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. 
    //For parallel processing you can use a diffent dispatcher 
    //(see Extras/BulletMultiThreaded)
    m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
    m_broadphase = new btDbvtBroadphase();

    ///the default constraint solver. 
    //For parallel processing you can use a different solver 
    //(see Extras/BulletMultiThreaded)
    m_solver = new btSequentialImpulseConstraintSolver;

    m_dynamicsWorld = 
        new btDiscreteDynamicsWorld(
                m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0,-10,0));
}


BulletWorld::~BulletWorld()
{
	//cleanup in the reverse order of creation/initialization
	//remove the rigidbodies from the dynamics world and delete them
	for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}


void BulletWorld::serialize()
{ 
    int maxSerializeBufferSize = 1024*1024*5;
    btDefaultSerializer*	serializer = new btDefaultSerializer(maxSerializeBufferSize);
    //serializer->setSerializationFlags(BT_SERIALIZE_NO_DUPLICATE_ASSERT);
    m_dynamicsWorld->serialize(serializer);
    FILE* f2 = fopen("testFile.bullet","wb");
    fwrite(serializer->getBufferPointer(),serializer->getCurrentBufferSize(),1,f2);
    fclose(f2);
    delete serializer;
}


void BulletWorld::setDebugMode(int mode)
{
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(mode);
}


void BulletWorld::debugDraw()
{
    m_dynamicsWorld->debugDrawWorld();
}


btRigidBody* BulletWorld::localCreateRigidBody(float mass, 
        const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic){
		shape->calculateLocalInertia(mass,localInertia);
    }

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

	m_dynamicsWorld->addRigidBody(body);

	return body;
}


void BulletWorld::onUpdate(btScalar ms)
{
    if(m_idle){
        return;
    }
    ///step the simulation
    m_dynamicsWorld->stepSimulation(ms / 1000000.f);
}


void BulletWorld::removeLastObject()
{
    int numObj = m_dynamicsWorld->getNumCollisionObjects();
    if(numObj==0){
        return;
    }
    btCollisionObject* obj =
        m_dynamicsWorld->getCollisionObjectArray()[numObj-1];
    m_dynamicsWorld->removeCollisionObject(obj);
    btRigidBody* body = btRigidBody::upcast(obj);
    if (body && body->getMotionState()) {
        delete body->getMotionState();
    }
    delete obj;
}

