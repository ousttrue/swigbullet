#include "bulletworld.h"
#include "GLDebugFont.h"
#include <btBulletDynamicsCommon.h>


BulletWorld::BulletWorld()
    :
        m_dynamicsWorld(0),

        m_defaultContactProcessingThreshold(BT_LARGE_FLOAT),
        m_idle(false)
{
    initPhysics();
}


BulletWorld::~BulletWorld()
{
    exitPhysics();
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
	if (getDynamicsWorld() && getDynamicsWorld()->getDebugDrawer())
		getDynamicsWorld()->getDebugDrawer()->setDebugMode(mode);
}


btRigidBody*	BulletWorld::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);	
	body->setWorldTransform(startTransform);
#endif//

	m_dynamicsWorld->addRigidBody(body);

	return body;
}


void BulletWorld::update()
{
    if (!m_idle){
        //simple dynamics world doesn't handle fixed-time-stepping
        float ms = getDeltaTimeMicroseconds();

        ///step the simulation
        if (m_dynamicsWorld)
        {
            m_dynamicsWorld->stepSimulation(ms / 1000000.f);
            //optional but useful: debug drawing
            m_dynamicsWorld->debugDrawWorld();
        }
    }
}


void BulletWorld::debugDraw()
{
	if (m_dynamicsWorld)
		m_dynamicsWorld->debugDrawWorld();
}


void BulletWorld::createWorld()
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


void BulletWorld::createGround()
{
	///create a few basic rigid bodies
	btCollisionShape* groundShape = 
        new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
	
	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

	//We can also use DemoApplication::localCreateRigidBody, 
    //but for clarity it is provided here:
	{
		btScalar mass(0.);
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, 
        //it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		m_dynamicsWorld->addRigidBody(body);
	}
}


///create 125 (5x5x5) dynamic object
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

///scaling of the objects (0.1 = 20 centimeter boxes )
#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3


void BulletWorld::createCubes()
{
    //create a few dynamic rigidbodies
    // Re-using the same collision is better for memory usage and performance
    btCollisionShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    m_collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar	mass(1.f);
    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic){
        colShape->calculateLocalInertia(mass,localInertia);
    }
    float start_x = START_POS_X - ARRAY_SIZE_X/2;
    float start_y = START_POS_Y;
    float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

    for (int k=0;k<ARRAY_SIZE_Y;k++) {
        for (int i=0;i<ARRAY_SIZE_X;i++) {
            for(int j = 0;j<ARRAY_SIZE_Z;j++) {
                startTransform.setOrigin(SCALING*btVector3(
                            btScalar(2.0*i + start_x),
                            btScalar(20+2.0*k + start_y),
                            btScalar(2.0*j + start_z)));
                //using motionstate is recommended, 
                //it provides interpolation capabilities, 
                //and only synchronizes 'active' objects
                btDefaultMotionState* myMotionState = 
                    new btDefaultMotionState(startTransform);
                btRigidBodyConstructionInfo 
                    rbInfo(mass,myMotionState,colShape,localInertia);
                btRigidBody* body = new btRigidBody(rbInfo);
                m_dynamicsWorld->addRigidBody(body);
            }
        }
    }
}


void BulletWorld::initPhysics()
{
    createWorld();
    createGround();
    createCubes();
}


void	BulletWorld::exitPhysics()
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
	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	m_collisionShapes.clear();
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}


void BulletWorld::removeLastObject()
{
    int numObj = getDynamicsWorld()->getNumCollisionObjects();
    if (numObj)
    {
        btCollisionObject* obj =
            getDynamicsWorld()->getCollisionObjectArray()[numObj-1];

        getDynamicsWorld()->removeCollisionObject(obj);
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        delete obj;
    }
}


void BulletWorld::clientResetScene()
{
    int numObjects = 0;
    btDynamicsWorld *dynamicsWorld=getDynamicsWorld();
    if (dynamicsWorld)
    {
        int numConstraints = dynamicsWorld->getNumConstraints();
        for (int i=0;i<numConstraints;i++)
        {
            dynamicsWorld->getConstraint(0)->setEnabled(true);
        }
        numObjects = dynamicsWorld->getNumCollisionObjects();

        ///create a copy of the array, not a reference!
        btCollisionObjectArray copyArray = dynamicsWorld->getCollisionObjectArray();




        for (int i=0;i<numObjects;i++)
        {
            btCollisionObject* colObj = copyArray[i];
            btRigidBody* body = btRigidBody::upcast(colObj);
            if (body)
            {
                if (body->getMotionState())
                {
                    btDefaultMotionState* myMotionState = 
                        (btDefaultMotionState*)body->getMotionState();
                    myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
                    body->setCenterOfMassTransform( myMotionState->m_graphicsWorldTrans );
                    colObj->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
                    colObj->forceActivationState(ACTIVE_TAG);
                    colObj->activate();
                    colObj->setDeactivationTime(0);
                    //colObj->setActivationState(WANTS_DEACTIVATION);
                }
                //removed cached contact points (this is not necessary if all objects have been removed from the dynamics world)
                if (dynamicsWorld->getBroadphase()->getOverlappingPairCache())
                    dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(colObj->getBroadphaseHandle(),dynamicsWorld->getDispatcher());

                btRigidBody* body = btRigidBody::upcast(colObj);
                if (body && !body->isStaticObject())
                {
                    btRigidBody::upcast(colObj)->setLinearVelocity(btVector3(0,0,0));
                    btRigidBody::upcast(colObj)->setAngularVelocity(btVector3(0,0,0));
                }
            }
        }
        ///reset some internal cached data in the broadphase
        dynamicsWorld->getBroadphase()->resetPool(dynamicsWorld->getDispatcher());
        dynamicsWorld->getConstraintSolver()->reset();
    }
    exitPhysics();
    initPhysics();
}


