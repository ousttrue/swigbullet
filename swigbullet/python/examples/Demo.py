# coding: utf-8
import sys
from OpenGL.GL import *
import numpy
import swigbullet as bullet
#from swigbullet import GL_ShapeDrawer
from bulletdemo.camera import Camera
from bulletdemo.bulletworld import BulletWorld
from bulletdemo import vector3
#from bulletdemo.profiler import Profiler
#from bulletdemo.texture import Texture
from swigbullet import Profiler
from swigbullet import Texture


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
        # bullet
        self.world=None
        self.world=BulletWorld()
        self.createGround()
        self.createCubes()
        # gldrawer
        self.profiler=Profiler()
        self.texture=Texture()
        # scene
        self.m_textureenabled=True
        self.m_enableshadows=True
        self.m_sundirection=vector3.mul((1,-2,1), 1000.0)
        self.m_debugMode=0
        #
        self.light_ambient = numpy.array([0.2, 0.2, 0.2, 1.0], 'f')
        self.light_diffuse = numpy.array([1.0, 1.0, 1.0, 1.0], 'f')
        self.light_specular = numpy.array([1.0, 1.0, 1.0, 1.0], 'f')
        # light_position is NOT default value
        self.light_position0 = numpy.array([1.0, 10.0, 1.0, 0.0], 'f')
        self.light_position1 = numpy.array([-1.0, -10.0, -1.0, 0.0], 'f')

        self.is_initialized=False

    def createGround(self):
        if not self.world:
            return
        groundShape = bullet.btBoxShape((50.0, 50.0, 50.0));
        groundTransform=bullet.btTransform();
        groundTransform.setIdentity();
        groundTransform.setOrigin((0,-50,0));
        self.world.localCreateRigidBody(0.0, groundTransform, groundShape)

    def createCubes(self):
        if not self.world:
            return
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
        self.profiler.resize(w, h)

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
        if not self.world:
            return
        self.world.update()

    def onInitialize(self):
        glLightfv(GL_LIGHT0, GL_AMBIENT, self.light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, self.light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, self.light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, self.light_position0);

        glLightfv(GL_LIGHT1, GL_AMBIENT, self.light_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, self.light_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, self.light_specular);
        glLightfv(GL_LIGHT1, GL_POSITION, self.light_position1);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);

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
        # render
        self.view.draw()
        if self.world:
            if self.m_enableshadows:
                bullet.renderwithshadow(self.world.m_dynamicsWorld, 
                        self.m_textureenabled, self.texture,
                        self.m_sundirection, self.m_debugMode)
            else:
                bullet.render(self.world.m_dynamicsWorld, self.m_debugMode)
            # profiler
            self.view.setOrthographicProjection();
            self.profiler.render(self.world.m_idle, self.m_debugMode)
            self.view.resetPerspectiveProjection();

        glFlush()

    def render(self):
        if(self.m_enableshadows):
            glClear(GL_STENCIL_BUFFER_BIT);
            # default draw
            glEnable(GL_CULL_FACE);
            self.renderscene(0);
            # setup stencil
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_STENCIL_TEST);
            glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
            glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
            glFrontFace(GL_CCW);
            # shadow pass 1(GL_CCW)
            glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
            self.renderscene(1);
            # shadow pass 2(GL_CW)
            glFrontFace(GL_CW);
            glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
            self.renderscene(1);
            # shadow pass 3
            glFrontFace(GL_CCW);
            glPolygonMode(GL_FRONT,GL_FILL);
            glPolygonMode(GL_BACK,GL_FILL);
            glShadeModel(GL_SMOOTH);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_LIGHTING);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_CULL_FACE);
            glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
            glDepthFunc(GL_LEQUAL);
            glStencilFunc(GL_NOTEQUAL, 0, 0xFFFFFFFF);
            glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
            glDisable(GL_LIGHTING);
            self.renderscene(2);
            # restore
            glEnable(GL_LIGHTING);
            glDepthFunc(GL_LESS);
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_CULL_FACE);
        else:
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

            if (not (self.m_debugMode & bullet.btIDebugDraw.DBG_DrawWireframe)):
                if path==0:
                    if(self.m_textureenabled):
                        self.texture.begin();
                    bullet.Draw(m, 
                            body.getCollisionShape(),
                            wireColor,
                            self.m_debugMode,
                            aabbMin, aabbMax);
                    if(self.m_textureenabled):
                        self.texture.end();
                elif path==1:
                    bullet.Shadow(m, rot.apply(self.m_sundirection),
                            body.getCollisionShape(),
                            aabbMin, aabbMax);
                elif path==2:
                    if(self.m_textureenabled):
                        self.texture.begin();
                    bullet.Draw(m,
                            body.getCollisionShape(),
                            vector3.mul(wireColor, 0.3),
                            0,
                            aabbMin,aabbMax);
                    if(self.m_textureenabled):
                        self.texture.end();


if __name__=="__main__":
    controller=Controller()
    #import glglue.glut
    #glglue.glut.mainloop(controller, width=1024, height=600, 
    #        title="Bullet Physics Demo. http:#bulletphysics.org")

    #import glglue.wgl
    #glglue.wgl.mainloop(controller, width=640, height=480, title="sample")

    import pygame
    from pygame.locals import *
    pygame.init()
    size=(1024, 600)
    pygame.display.gl_set_attribute(pygame.GL_STENCIL_SIZE, 2)
    screen = pygame.display.set_mode(size,
            HWSURFACE | OPENGL | DOUBLEBUF)

    controller.onResize(*size)

    clock = pygame.time.Clock()
    is_running=True
    while is_running:
        #pressed = pygame.key.get_pressed()

        # event handling
        for event in pygame.event.get():
            if event.type == QUIT:
                is_running=False
            if event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    is_running=False
                else:
                    controller.onKeyDown(event.key)
            
        # update
        d = clock.tick()
        if d>0:
            controller.onUpdate(d)
            controller.draw()
            pygame.display.flip()

    sys.exit(0)

