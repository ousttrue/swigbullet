/*
   Bullet Continuous Collision Detection and Physics Library
   Copyright (c) 2003-2006 Erwin Coumans http://continuousphysics.com/Bullet/

   This software is provided 'as-is', without any express or implied warranty.
   In no event will the authors be held liable for any damages arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it freely,
   subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
   3. This notice may not be removed or altered from any source distribution.
   */
#include "DemoApplication.h"
#include "camera.h"
#include "bulletworld.h"
#include "bulletshooter.h"
#include "bulletpicker.h"
#include "profiler.h"
#include "GL_ShapeDrawer.h"
#include "texture.h"
#include <btBulletDynamicsCommon.h>
#ifdef _WINDOWS
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#endif //_WINDOWS


DemoApplication::DemoApplication()
    //see btIDebugDraw.h for modes
    :
        m_mouseOldX(0),
        m_mouseOldY(0),
        m_mouseButtons(0),
        m_scaleBottom(0.5f),
        m_scaleFactor(2.f),
        m_stepping(true),
        m_singleStep(false),
        m_modifierKeys(0),
        m_active_alt(false),
        m_enableshadows(true),
        m_debugMode(0),
        m_sundirection(btVector3(1,-2,1)*1000),
        m_glutScreenWidth(1),
        m_glutScreenHeight(1),
        m_textureenabled(true)
{
    // bullet
    m_bulletworld=new BulletWorld();
    m_shooter=new BulletShooter();
    m_picker=new BulletPicker();
    createGround();
    createCubes();
    // opengl
    m_camera=new PerspectiveCamera();
    m_texture=new Texture;

    m_profiler=new Profiler;
}


DemoApplication::~DemoApplication()
{
	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
	}
	m_collisionShapes.clear();
    if(m_profiler)
        delete m_profiler;
    if(m_texture)
        delete m_texture;
    if(m_camera)
        delete m_camera;
    if(m_picker)
        delete m_picker;
    if(m_shooter)
        delete m_shooter;
    if(m_bulletworld)
        delete m_bulletworld;
}


void DemoApplication::myinit(void)
{
    static GLfloat light_ambient[] = { btScalar(0.2), btScalar(0.2), btScalar(0.2), btScalar(1.0) };
    static GLfloat light_diffuse[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0) };
    static GLfloat light_specular[] = { btScalar(1.0), btScalar(1.0), btScalar(1.0), btScalar(1.0 )};
    /* light_position is NOT default value */
    static GLfloat light_position0[] = { btScalar(1.0), btScalar(10.0), btScalar(1.0), btScalar(0.0 )};
    static GLfloat light_position1[] = { btScalar(-1.0), btScalar(-10.0), btScalar(-1.0), btScalar(0.0) };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);


    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(btScalar(0.7),btScalar(0.7),btScalar(0.7),btScalar(0));
}


void DemoApplication::reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    m_profiler->resize(w, h);
    m_camera->reshape(w, h);
    m_glutScreenWidth = w;
    m_glutScreenHeight = h;
}


void DemoApplication::keyboardCallback(unsigned char key, int x, int y)
{
    m_profiler->keyboardCallback(key, x, y);

    switch (key)
    {
        case 'l' : 
            m_camera->stepLeft(); 
            break;
        case 'r' : 
            m_camera->stepRight(); 
            break;
        case 'f' : 
            m_camera->stepFront(); 
            break;
        case 'b' : 
            m_camera->stepBack(); 
            break;
        case 'z' : 
            m_camera->zoomIn(); 
            break;
        case 'x' : 
            m_camera->zoomOut(); 
            break;
        case 'i' : 
            m_bulletworld->toggleIdle(); 
            break;
        case 'g' : 
            toggleShadow();
            break;
        case 'o' :
            if(m_camera->getOrtho()){
                delete m_camera;
                m_camera=new PerspectiveCamera();
            }
            else{
                delete m_camera;
                m_camera=new OrthogonalCamera();
            }
            m_camera->reshape(m_glutScreenWidth, m_glutScreenHeight);
            break;
        case 's' :
            m_bulletworld->update(getDeltaTimeMicroseconds());
            break;
        case ' ':
            {
                m_picker->removePickingConstraint(m_bulletworld->getDynamicsWorld());
                if(m_bulletworld)
                    delete m_bulletworld;
                m_bulletworld=new BulletWorld;
                createGround();
                createCubes();
                break;
            }
        case '.':
            {
                m_shooter->shootBox(m_bulletworld, 
                        m_camera->getCameraPosition(), 
                        m_camera->getRayTo(x,y, m_glutScreenWidth, m_glutScreenHeight));
                break;
            }
        case 'u' : 
            enableTexture(!enableTexture(false));
            break;
        case 'h':
            if (m_debugMode & btIDebugDraw::DBG_NoHelpText){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_NoHelpText);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_NoHelpText;
            }
            break;
        case 'w':
            if (m_debugMode & btIDebugDraw::DBG_DrawWireframe){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawWireframe);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawWireframe;
            }
            break;
        case 'p':
            if (m_debugMode & btIDebugDraw::DBG_ProfileTimings){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_ProfileTimings);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_ProfileTimings;
            }
            break;
        case 'm':
            if (m_debugMode & btIDebugDraw::DBG_EnableSatComparison){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_EnableSatComparison);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_EnableSatComparison;
            }
            break;
        case 'n':
            if (m_debugMode & btIDebugDraw::DBG_DisableBulletLCP){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DisableBulletLCP);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DisableBulletLCP;
            }
            break;
        case 't' : 
            if (m_debugMode & btIDebugDraw::DBG_DrawText){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawText);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawText;
            }
            break;
        case 'y':		
            if (m_debugMode & btIDebugDraw::DBG_DrawFeaturesText){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawFeaturesText);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawFeaturesText;
            }
            break;
        case 'a':	
            if (m_debugMode & btIDebugDraw::DBG_DrawAabb){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawAabb);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawAabb;
            }
            break;
        case 'c' : 
            if (m_debugMode & btIDebugDraw::DBG_DrawContactPoints){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawContactPoints);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawContactPoints;
            }
            break;
        case 'C' : 
            if (m_debugMode & btIDebugDraw::DBG_DrawConstraints){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawConstraints);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawConstraints;
            }
            break;
        case 'L' : 
            if (m_debugMode & btIDebugDraw::DBG_DrawConstraintLimits){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawConstraintLimits);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_DrawConstraintLimits;
            }
            break;
        case 'd' : 
            if (m_debugMode & btIDebugDraw::DBG_NoDeactivation){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_NoDeactivation);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_NoDeactivation;
            }
            if (m_debugMode & btIDebugDraw::DBG_NoDeactivation){
                gDisableDeactivation = true;
            } 
            else {
                gDisableDeactivation = false;
            }
            break;
        case '1':
            if (m_debugMode & btIDebugDraw::DBG_EnableCCD){
                m_debugMode = m_debugMode & (~btIDebugDraw::DBG_EnableCCD);
            }
            else{
                m_debugMode |= btIDebugDraw::DBG_EnableCCD;
            }
            break;
        case '+':
            m_shooter->setShootInitialSpeed(
                    m_shooter->getShootInitialSpeed() + 10.f);
            break;
        case '-':
            m_shooter->setShootInitialSpeed(
                    m_shooter->getShootInitialSpeed() - 10.f);
            break;
        case '=':
            m_bulletworld->serialize();
            break;
        default:
            //        std::cout << "unused key : " << key << std::endl;
            break;
    }

    if (m_bulletworld->getDynamicsWorld() && 
            m_bulletworld->getDynamicsWorld()->getDebugDrawer())
    {
        m_bulletworld->getDynamicsWorld()->getDebugDrawer()->setDebugMode(
                getDebugMode());
    }
}


void DemoApplication::mouseFunc(int button, int state, int x, int y)
{
    m_mouseOldX = x;
    m_mouseOldY = y;

    if (state == 0) {
        m_mouseButtons |= 1<<button;
    }
    else {
        m_mouseButtons = 0;
    }

    if (m_active_alt && (state==0)) {
        // do nothing
        return;
    }

    switch (button)
    {
        case 0:
            // left
            {
                if (state==0) {
                    // Down
                    btVector3 rayTo=m_camera->getRayTo(
                            x, y, m_glutScreenWidth, m_glutScreenHeight);
                    btVector3 rayFrom=m_camera->getOrtho()
                        ? btVector3(rayTo[0], rayTo[1], 1000.0f)
                        : m_camera->getCameraPosition();
                    m_picker->pickStart(m_bulletworld->getDynamicsWorld(), 
                            rayFrom, rayTo);
                }
                else {
                    // Up
                    m_picker->removePickingConstraint(m_bulletworld->getDynamicsWorld());
                }
                break;
            }

        case 1:
            // middle
            break;

        case 2:
            // right
            {
                if (state==0) {
                    // Down
                    m_shooter->shootBox(m_bulletworld, 
                            m_camera->getCameraPosition(), 
                            m_camera->getRayTo(x,y, m_glutScreenWidth, m_glutScreenHeight));
                }
                break;
            }

        default:
            break;
    }
}


void DemoApplication::mouseMotionFunc(int x,int y)
{
    m_picker->pick(m_bulletworld->getDynamicsWorld(), 
            m_camera->getCameraPosition(),
            m_camera->getRayTo(x, y, m_glutScreenWidth, m_glutScreenHeight),
            m_camera->getOrtho()
            );

    if (m_active_alt) {
        int dx = x - m_mouseOldX;
        int dy = y - m_mouseOldY;
        ///only if ALT key is pressed (Maya style)
        if(m_mouseButtons & 2) {
            // middle
            m_camera->move(dx, dy, m_glutScreenWidth, m_glutScreenHeight);
        }
        if(m_mouseButtons & (2 << 2) && m_mouseButtons & 1) {
            // ?
        }
        if(m_mouseButtons & 1) {
            // left
            m_camera->rot(dx, dy);
        }
        if(m_mouseButtons & 4) {
            // right
            m_camera->dolly(dy);
        }
    }

    m_mouseOldX = x;
    m_mouseOldY = y;
}


void DemoApplication::displayCallback(void)
{
    myinit();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // reset matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_camera->draw();
    btDynamicsWorld *dynamicsWorld=m_bulletworld->getDynamicsWorld();
    if(dynamicsWorld){
        if(m_enableshadows) {
            GL_ShapeDrawer::renderwithshadow(dynamicsWorld, m_textureenabled, m_texture, 
                    m_sundirection, getDebugMode());
        }
        else{
            if(m_textureenabled){
                m_texture->begin();
            }
            GL_ShapeDrawer::render(dynamicsWorld, getDebugMode());
            if(m_textureenabled){
                m_texture->end();
            }
        }
    }
    //optional but useful: debug drawing to detect problems
    {
        // push
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        setOrthographicProjection(m_glutScreenWidth, m_glutScreenHeight);
        m_profiler->render(m_bulletworld->isIdle(), getDebugMode());

        // restore
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
    glDisable(GL_LIGHTING);
    m_bulletworld->debugDraw();
    glFlush();
}


void DemoApplication::createGround()
{
	///create a few basic rigid bodies
	btCollisionShape* groundShape = 
        new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));
	
	m_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

    m_bulletworld->localCreateRigidBody(0.0, groundTransform, groundShape);
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


void DemoApplication::createCubes()
{
    //create a few dynamic rigidbodies
    // Re-using the same collision is better for memory usage and performance
    btCollisionShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
    //btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    m_collisionShapes.push_back(colShape);

    /// Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

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
                m_bulletworld->localCreateRigidBody(1.0f, startTransform, colShape);
            }
        }
    }
}



