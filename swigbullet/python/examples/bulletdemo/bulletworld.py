import swigbullet as bullet
from bulletdemo import vector3


class BulletWorld(object):

    def __init__(self):
        self.m_defaultContactProcessingThreshold=bullet.BT_LARGE_FLOAT
        self.m_idle=False
        self.keep=[]
        # setup
        #collision configuration contains default setup for memory, collision setup
        self.m_collisionConfiguration = bullet.btDefaultCollisionConfiguration();

        #use the default collision dispatcher. 
        #For parallel processing you can use a diffent dispatcher 
        #(see Extras/BulletMultiThreaded)
        self.m_dispatcher = bullet.btCollisionDispatcher(self.m_collisionConfiguration);
        self.m_broadphase = bullet.btDbvtBroadphase();

        #the default constraint solver. 
        #For parallel processing you can use a different solver 
        #(see Extras/BulletMultiThreaded)
        self.m_solver = bullet.btSequentialImpulseConstraintSolver();

        self.m_dynamicsWorld = bullet.btDiscreteDynamicsWorld(
                    self.m_dispatcher,
                    self.m_broadphase,
                    self.m_solver,
                    self.m_collisionConfiguration);

        self.m_dynamicsWorld.setGravity((0.0, -10.0, 0.0));

    def __del__(self):
        self.clear();
        del self.m_dynamicsWorld
        del self.m_solver
        del self.m_broadphase
        del self.m_dispatcher
        del self.m_collisionConfiguration

    def	clear(self):
        #cleanup in the reverse order of creation/initialization
        #remove the rigidbodies from the dynamics world and delete them
        self.m_dynamicsWorld.getCollisionObjectArray().clear()
        self.keep=[]

    def localCreateRigidBody(self, mass, startTransform, shape):
        assert((shape==None or shape.getShapeType() != bullet.INVALID_SHAPE_PROXYTYPE));

        #rigidbody is dynamic if and only if mass is non zero, otherwise static
        localInertia=(0,0,0);
        if (mass != 0.0):
            localInertia=shape.calculateLocalInertia(mass, localInertia);

        #using motionstate is recommended, it provides interpolation capabilities, 
        # and only synchronizes 'active' objects

        myMotionState = bullet.btDefaultMotionState(startTransform);
        body = bullet.btRigidBody(
                bullet.btRigidBodyConstructionInfo(mass,myMotionState,shape,localInertia));
        body.setContactProcessingThreshold(self.m_defaultContactProcessingThreshold);

        self.keep.append((body, myMotionState, shape))

        self.m_dynamicsWorld.addRigidBody(body);

        return body;

    def removeLastObject(self):
        numObj = self.m_dynamicsWorld.getNumCollisionObjects();
        if numObj==0:
            return
        obj = m_dynamicsWorld.getCollisionObjectArray()[numObj-1];
        m_dynamicsWorld.removeCollisionObject(obj);

    def update(self, ms):
        if self.m_idle:
            return
        self.m_dynamicsWorld.stepSimulation(ms / 1000000.0);

    def serialize(self):
        maxSerializeBufferSize = 1024*1024*5;
        serializer = bullet.btDefaultSerializer(maxSerializeBufferSize);
        self.m_dynamicsWorld.serialize(serializer);
        with open("testFile.bullet","wb") as f2:
            f2.write(serializer.getBufferPointer(), serializer.getCurrentBufferSize());

