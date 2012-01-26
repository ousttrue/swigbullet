import swigbullet as bullet
from bulletdemo import vector3


class BulletWorld(object):

    def __init__(self):
        self.m_shootBoxShape=None
        self.m_ShootBoxInitialSpeed=40.0
        self.m_defaultContactProcessingThreshold=bullet.BT_LARGE_FLOAT
        self.m_idle=False
        self.use6Dof=False
        self.mousePickClamping=30.0
        self.gPickingConstraintId=0
        self.gHitPos=(-1,-1,-1)
        self.gOldPickingDist=0.0
        self.m_clock=bullet.btClock()
        self.keep=[]
        # setup
        self.createWorld();

    def __del__(self):
        self.clear();
        if self.m_shootBoxShape:
            del self.m_shootBoxShape
        del self.m_dynamicsWorld
        del self.m_solver
        del self.m_broadphase
        del self.m_dispatcher
        del self.m_collisionConfiguration
        print self

    def	clear(self):
        #cleanup in the reverse order of creation/initialization
        #remove the rigidbodies from the dynamics world and delete them
        self.m_dynamicsWorld.getCollisionObjectArray().clear()
        self.keep=[]

    def createWorld(self):
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

    def localCreateRigidBody(self, mass, startTransform, shape):
        assert((shape==None or shape.getShapeType() != bullet.INVALID_SHAPE_PROXYTYPE));

        #rigidbody is dynamic if and only if mass is non zero, otherwise static
        localInertia=(0,0,0);
        if (mass != 0.0):
            shape.calculateLocalInertia(mass, localInertia);

        #using motionstate is recommended, it provides interpolation capabilities, 
        # and only synchronizes 'active' objects

        myMotionState = bullet.btDefaultMotionState(startTransform);
        body = bullet.btRigidBody(
                bullet.btRigidBodyConstructionInfo(mass,myMotionState,shape,localInertia));
        #body.setContactProcessingThreshold(self.m_defaultContactProcessingThreshold);

        self.keep.append((body, myMotionState, shape))

        self.m_dynamicsWorld.addRigidBody(body);

        return body;

    def getDeltaTimeMicroseconds(self):
        dt = self.m_clock.getTimeMicroseconds();
        self.m_clock.reset();
        return dt;

    def serialize(self):
        maxSerializeBufferSize = 1024*1024*5;
        serializer = bullet.btDefaultSerializer(maxSerializeBufferSize);
        self.m_dynamicsWorld.serialize(serializer);
        with open("testFile.bullet","wb") as f2:
            f2.write(serializer.getBufferPointer(), serializer.getCurrentBufferSize());

    def setShootBoxShape(self):
        if (not self.m_shootBoxShape):
            box = bullet.btBoxShape((0.5, 0.5, 0.5));
            box.initializePolyhedralFeatures();
            self.m_shootBoxShape = box;

    def shootBox(self, camera, x, y):
        if not self.m_dynamicsWorld:
            return

        self.setShootBoxShape();

        camPos=camera.getCameraPosition();
        destination=camera.getRayTo(x,y);

        startTransform=bullet.btTransform();
        startTransform.setIdentity();
        startTransform.setOrigin(camPos);

        body = self.localCreateRigidBody(1.0, startTransform, self.m_shootBoxShape);
        body.setLinearFactor((1,1,1));

        linVel=[d-c for d, c in zip(destination, camPos)]
        linVel=vector3.normalize(linVel);
        linVel=vector3.mul(linVel, self.m_ShootBoxInitialSpeed)

        body.getWorldTransform().setOrigin(camPos);
        body.getWorldTransform().setRotation(bullet.btQuaternion(0,0,0,1));
        body.setLinearVelocity(linVel);
        body.setAngularVelocity((0,0,0));
        body.setCcdMotionThreshold(0.5);
        body.setCcdSweptSphereRadius(0.9);

    def update(self, ms=None):
        if (not self.m_idle):
            # simple dynamics world doesn't handle fixed-time-stepping
            if not ms:
                ms = self.getDeltaTimeMicroseconds();

            # step the simulation
            if (self.m_dynamicsWorld):
                self.m_dynamicsWorld.stepSimulation(ms / 1000000.0);
                # optional but useful: debug drawing
                #self.m_dynamicsWorld.debugDrawWorld();

    def debugDraw(self):
        if (self.m_dynamicsWorld):
            self.m_dynamicsWorld.debugDrawWorld();

    def removeLastObject(self):
        numObj = self.m_dynamicsWorld.getNumCollisionObjects();
        if (numObj>0):
            obj = m_dynamicsWorld.getCollisionObjectArray()[numObj-1];
            m_dynamicsWorld.removeCollisionObject(obj);
            # todo

    def keyboardCallback(self, key, x, y):
        if key=='=':
            self.serialize();
        elif key=='+':
            self.m_ShootBoxInitialSpeed+=10.0
        elif key=='-':
            self.m_ShootBoxInitialSpeed-=10.0
        else:
            print 'unknown key:', key

    def pickStart(self, camera, x, y):
        # add a point to point constraint for picking
        if (self.m_dynamicsWorld):
            rayTo=camera.getRayTo(x,y);
            rayFrom=None
            if (camera.m_ortho):
                rayFrom=(rayTo[0], To[1], -100.0);
            else:
                rayFrom = camera.getCameraPosition();
            rayCallback=bullet.ClosestRayResultCallback(rayFrom, rayTo);
            self.m_dynamicsWorld.rayTest(rayFrom, rayTo, rayCallback);
            if (rayCallback.hasHit()):
                body = bullet.btRigidBody.upcast(rayCallback.m_collisionObject);
                if (body):
                    print 'hit'
                    # other exclusions?
                    if (not (body.isStaticObject() or body.isKinematicObject())):
                        print body
                        self.pickedBody = body;
                        self.pickedBody.setActivationState(bullet.DISABLE_DEACTIVATION);
                        #pickPos = rayCallback.m_hitPointWorld;
                        pickPos = rayCallback.m_hitPointWorld
                        # printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
                        localPivot = body.getCenterOfMassTransform().inverse().apply(pickPos);

                        self.m_keep.append(self.m_pickConstraint)
                        if (self.m_use6Dof):
                            print 'm_use6Dof'
                            tr=bullet.btTransform();
                            tr.setIdentity();
                            tr.setOrigin(localPivot);
                            dof6 = bullet.btGeneric6DofConstraint(body, tr, False);
                            dof6.setLinearLowerLimit((0,0,0));
                            dof6.setLinearUpperLimit((0,0,0));
                            dof6.setAngularLowerLimit((0,0,0));
                            dof6.setAngularUpperLimit((0,0,0));

                            self.m_dynamicsWorld.addConstraint(dof6);
                            self.m_pickConstraint = dof6;

                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,0);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,1);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,2);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,3);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,4);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_CFM,0.8,5);

                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,0);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,1);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,2);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,3);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,4);
                            dof6.setParam(bullet.BT_CONSTRAINT_STOP_ERP,0.1,5);
                        else:
                            print 'not m_use6Dof'
                            p2p = bullet.btPoint2PointConstraint(body,localPivot);
                            self.m_dynamicsWorld.addConstraint(p2p);
                            self.m_pickConstraint = p2p;
                            p2p.m_setting.m_impulseClamp = 30.0;
                            # very weak constraint for picking
                            p2p.m_setting.m_tau = 0.001;

                        self.m_use6Dof = not self.m_use6Dof;

                        # save mouse position for dragging
                        self.gOldPickingPos = rayTo;
                        self.gHitPos = pickPos;

                        self.m_oldPickingDist  = vector3.length(
                                vector3.sub(pickPos, rayFrom));
                        print 'end'


    def removePickingConstraint(self):
        if (self.m_pickConstraint and self.m_dynamicsWorld):
            self.m_dynamicsWorld.removeConstraint(self.m_pickConstraint);
            self.m_pickConstraint=None
            self.pickedBody.forceActivationState(bullet.ACTIVE_TAG);
            self.pickedBody.setDeactivationTime(0.0);
            self.pickedBody = 0;

    def pick(self, camera, x, y):
        if (self.m_pickConstraint):
            #move the constraint pivot
            if (self.m_pickConstraint.getConstraintType() == self.D6_CONSTRAINT_TYPE):
                pickCon = bullet.btGeneric6DofConstraint.downcast(self.m_pickConstraint);
                if (pickCon):
                    #keep it at the same picking distance
                    newRayTo = camera.getRayTo(x,y);
                    oldPivotInB = pickCon.getFrameOffsetA().getOrigin();
                    rayFrom=None;
                    newPivotB=None;
                    if (camera.m_ortho):
                        newPivotB = (newRayTo[0], newRayTo[1], oldPivotInB[2]);
                    else:
                        rayFrom = camera.getCameraPosition();
                        dir = vector3.mul(
                                vector3.normalize(vector3.sub(newRayTo, rayFrom)),
                                self.gOldPickingDist);

                        newPivotB = vector3.add(rayFrom, dir);
                    pickCon.getFrameOffsetA().setOrigin(newPivotB);
            else:
                pickCon = btPoint2PointConstraint.downcast(self.m_pickConstraint);
                if (pickCon):
                    #keep it at the same picking distance
                    newRayTo = camera.getRayTo(x,y);
                    oldPivotInB = pickCon.getPivotInB();
                    rayFrom=None;
                    newPivotB=None;
                    if (camera.m_ortho):
                        newPivotB = (newRayTo[0], newRayTo[1], oldPivotInB[2]);
                    else:
                        rayFrom = camera.getCameraPosition();
                        dir = vector3.mul(
                                vector3.normalize(vector3.sub(newRayTo, rayFrom)),
                                self.gOldPickingDist);

                        newPivotB = vector3.add(rayFrom, dir);
                    pickCon.setPivotB(newPivotB);


