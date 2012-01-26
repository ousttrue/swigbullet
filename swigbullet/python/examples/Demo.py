# coding: utf-8
import sys
from OpenGL.GL import *
import swigbullet as bullet
#from swigbullet import GL_ShapeDrawer
from bulletdemo.camera import Camera
from bulletdemo.bulletworld import BulletWorld
from bulletdemo import vector3


#create 125 (5x5x5) dynamic object
ARRAY_SIZE_X=5
ARRAY_SIZE_Y=5
ARRAY_SIZE_Z=5

#scaling of the objects (0.1 = 20 centimeter boxes )
SCALING=1.0

START_POS_X=-5
START_POS_Y=-5
START_POS_Z=-3


class Controller(object):
    def __init__(self):
        self.view=Camera(50)
        self.world=BulletWorld()
        self.createGround()
        self.createCubes()
        self.is_initialized=False

    def createGround(self):
        groundShape = bullet.btBoxShape((50.0, 50.0, 50.0));
        groundTransform=bullet.btTransform();
        groundTransform.setIdentity();
        groundTransform.setOrigin((0,-50,0));
        self.world.localCreateRigidBody(0.0, groundTransform, groundShape)

    def createCubes(self):
        colShape = bullet.btBoxShape((SCALING*1,SCALING*1,SCALING*1));
        startTransform=bullet.btTransform();
        startTransform.setIdentity();

        start_x = START_POS_X - ARRAY_SIZE_X/2;
        start_y = START_POS_Y;
        start_z = START_POS_Z - ARRAY_SIZE_Z/2;
        for k in range(ARRAY_SIZE_Y):
            for i in range(ARRAY_SIZE_X):
                for j in range(ARRAY_SIZE_Z):
                    startTransform.setOrigin(vector3.mul(
                        (
                            (2.0*i + start_x),
                            (20+2.0*k + start_y),
                            (2.0*j + start_z)
                            ), 
                        SCALING));
                    self.world.localCreateRigidBody(1.0, startTransform, colShape)

    def onResize(self, w, h):
        glViewport(0, 0, w, h)
        self.view.resize(w, h)

    def onLeftDown(self, x, y):
        print 'onLeftDown', x, y

    def onLeftUp(self, x, y):
        print 'onLeftUp', x, y

    def onMiddleDown(self, x, y):
        print 'onMiddleDown', x, y

    def onMiddleUp(self, x, y):
        print 'onMiddleUp', x, y

    def onRightDown(self, x, y):
        print 'onRightDown', x, y

    def onRightUp(self, x, y):
        print 'onRightUp', x, y

    def onMotion(self, x, y):
        print 'onMotion', x, y

    def onWheel(self, d):
        print 'onWheel', d

    def onKeyDown(self, keycode):
        print 'onKeyDown', keycode

    def onUpdate(self, d):
        self.world.update()

    def onInitialize(self):
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.7, 0.7 ,0.7, 0.0);
        #glClearColor(0.9, 0.5, 0.0, 0.0)
        self.is_initialized=True

    def draw(self):
        if not self.is_initialized:
            self.onInitialize()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        self.view.draw()
        self.render()
        glFlush()

    def render(self):
        glDisable(GL_CULL_FACE);
        self.renderscene(0);

    def getColor(self, state, odd):
        # color differently for active, sleeping, wantsdeactivation states
        if state==1:
            # active
            if odd:
                return (1.0, 0.0, 1.0)
            else:
                return (1.5, 1.0, 0.5)
        elif state==2:
            # ISLAND_SLEEPING 
            if odd:
                return (0.0, 1.0, 1.0)
            else:
                return (1.0, 1.5, 0.5)
        else:
            # wants deactivation
            if odd:
                return (0.0, 0.0, 1.0);
            else:
                return (1.0, 1.0, 0.5); 

    def renderscene(self, path):
        dynamicsWorld=self.world.m_dynamicsWorld
        if not dynamicsWorld:
            return;

        for i, colObj in enumerate(dynamicsWorld.getCollisionObjectArray()):
            body=bullet.btRigidBody.upcast(colObj);
            if body and body.getMotionState():
                myMotionState = bullet.btDefaultMotionState.downcast(body.getMotionState());
                m=myMotionState.m_graphicsWorldTrans.getOpenGLMatrix();
                rot=myMotionState.m_graphicsWorldTrans.getBasis();
            else:
                m=body.getWorldTransform().getOpenGLMatrix();
                rot=body.getWorldTransform().getBasis();
            wireColor=self.getColor(body.getActivationState(), i%2==1);

            aabbMin, aabbMax=dynamicsWorld.getBroadphase().getBroadphaseAabb();

            aabbMin=vector3.sub(aabbMin, 
                    (bullet.BT_LARGE_FLOAT,bullet.BT_LARGE_FLOAT,bullet.BT_LARGE_FLOAT));
            aabbMax=vector3.add(aabbMax, 
                    (bullet.BT_LARGE_FLOAT,bullet.BT_LARGE_FLOAT,bullet.BT_LARGE_FLOAT));
            debugMode=0

            if (not (debugMode & bullet.btIDebugDraw.DBG_DrawWireframe)):
                if path==0:
                    #if(m_textureenabled):
                    #    m_texture.begin();
                    #}
                    bullet.Draw(m, 
                            body.getCollisionShape(),
                            wireColor,
                            debugMode,
                            aabbMin, aabbMax);
                    #if(m_textureenabled){
                    #    m_texture.end();
                    #}
                elif path==1:
                    bullet.Shadow(m, m_sundirection*rot,
                            body.getCollisionShape(),
                            aabbMin, aabbMax);
                elif path==2:
                    #if(m_textureenabled){
                    #    m_texture.begin();
                    #}
                    bullet.Draw(m,
                            body.getCollisionShape(),
                            wireColor*btScalar(0.3), 
                            0,
                            aabbMin,aabbMax);
                    #if(m_textureenabled){
                    #    m_texture.end();
                    #}


if __name__=="__main__":
    m=(
            0, 1, 2, 3,
            0, 1, 2, 3,
            0, 1, 2, 3,
            0, 1, 2, 3
            )
           
    controller=Controller()
    import glglue.glut
    glglue.glut.mainloop(controller, width=1024, height=600, 
            title="Bullet Physics Demo. http:#bulletphysics.org")

