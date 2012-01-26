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
        m_textureenabled(true)
{
    m_bulletworld=new BulletWorld();
    m_camera=new Camera();
    m_camera->setCameraDistance(btScalar(50.));
    m_profiler=new Profiler;
    m_texture=new Texture;
}


DemoApplication::~DemoApplication()
{
    if(m_texture)
        delete m_texture;
    if(m_profiler)
        delete m_profiler;
    if(m_camera)
        delete m_camera;
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
}


void DemoApplication::keyboardCallback(unsigned char key, int x, int y)
{
    m_profiler->keyboardCallback(key, x, y);

    switch (key)
    {
        case 'l' : m_camera->stepLeft(); break;
        case 'r' : m_camera->stepRight(); break;
        case 'f' : m_camera->stepFront(); break;
        case 'b' : m_camera->stepBack(); break;
        case 'z' : m_camera->zoomIn(); break;
        case 'x' : m_camera->zoomOut(); break;
        case 'i' : m_bulletworld->toggleIdle(); break;
        case 'g' : toggleShadow();;break;
        case 'o' :
                   {
                       m_camera->toggleOrtho();
                       break;
                   }
        case 's' :
                   m_bulletworld->update();
                   break;

        case ' ':
                   m_bulletworld->clientResetScene();
                   break;
        case '.':
                   {
                       m_bulletworld->shootBox(m_camera, x, y);
                       break;
                   }
        case 'u' : enableTexture(!enableTexture(false));break;
        case 'h':
                   if (m_debugMode & btIDebugDraw::DBG_NoHelpText)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_NoHelpText);
                   else
                       m_debugMode |= btIDebugDraw::DBG_NoHelpText;
                   break;

        case 'w':
                   if (m_debugMode & btIDebugDraw::DBG_DrawWireframe)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawWireframe);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawWireframe;
                   break;

        case 'p':
                   if (m_debugMode & btIDebugDraw::DBG_ProfileTimings)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_ProfileTimings);
                   else
                       m_debugMode |= btIDebugDraw::DBG_ProfileTimings;
                   break;

        case 'm':
                   if (m_debugMode & btIDebugDraw::DBG_EnableSatComparison)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_EnableSatComparison);
                   else
                       m_debugMode |= btIDebugDraw::DBG_EnableSatComparison;
                   break;

        case 'n':
                   if (m_debugMode & btIDebugDraw::DBG_DisableBulletLCP)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DisableBulletLCP);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DisableBulletLCP;
                   break;

        case 't' : 
                   if (m_debugMode & btIDebugDraw::DBG_DrawText)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawText);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawText;
                   break;
        case 'y':		
                   if (m_debugMode & btIDebugDraw::DBG_DrawFeaturesText)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawFeaturesText);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawFeaturesText;
                   break;
        case 'a':	
                   if (m_debugMode & btIDebugDraw::DBG_DrawAabb)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawAabb);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawAabb;
                   break;
        case 'c' : 
                   if (m_debugMode & btIDebugDraw::DBG_DrawContactPoints)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawContactPoints);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawContactPoints;
                   break;
        case 'C' : 
                   if (m_debugMode & btIDebugDraw::DBG_DrawConstraints)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawConstraints);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawConstraints;
                   break;
        case 'L' : 
                   if (m_debugMode & btIDebugDraw::DBG_DrawConstraintLimits)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_DrawConstraintLimits);
                   else
                       m_debugMode |= btIDebugDraw::DBG_DrawConstraintLimits;
                   break;

        case 'd' : 
                   if (m_debugMode & btIDebugDraw::DBG_NoDeactivation)
                       m_debugMode = m_debugMode & (~btIDebugDraw::DBG_NoDeactivation);
                   else
                       m_debugMode |= btIDebugDraw::DBG_NoDeactivation;
                   if (m_debugMode & btIDebugDraw::DBG_NoDeactivation)
                   {
                       gDisableDeactivation = true;
                   } else
                   {
                       gDisableDeactivation = false;
                   }
                   break;

        case '1':
                   {
                       if (m_debugMode & btIDebugDraw::DBG_EnableCCD)
                           m_debugMode = m_debugMode & (~btIDebugDraw::DBG_EnableCCD);
                       else
                           m_debugMode |= btIDebugDraw::DBG_EnableCCD;
                       break;
                   }

        default:
                   //        std::cout << "unused key : " << key << std::endl;
                   break;
    }
    m_bulletworld->keyboardCallback(key, x, y);

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
                    m_bulletworld->pickStart(m_camera, x, y);
                }
                else {
                    // Up
                    m_bulletworld->removePickingConstraint();
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
                    m_bulletworld->shootBox(m_camera, x, y);
                }
                break;
            }

        default:
            break;
    }
}


void DemoApplication::mouseMotionFunc(int x,int y)
{
    m_bulletworld->pick(m_camera, x, y);

    if (m_active_alt) {
        int dx = x - m_mouseOldX;
        int dy = y - m_mouseOldY;
        ///only if ALT key is pressed (Maya style)
        if(m_mouseButtons & 2) {
            m_camera->move(dx, dy);
        }
        if(m_mouseButtons & (2 << 2) && m_mouseButtons & 1) {
        }
        else if(m_mouseButtons & 1) {
            m_camera->rot(dx, dy);
        }
        else if(m_mouseButtons & 4) {
            m_camera->dolly(dy);
        }
    }

    m_mouseOldX = x;
    m_mouseOldY = y;
}


static inline btVector3 getColor(int state, int i)
{
    ///color differently for active, sleeping, wantsdeactivation states
    switch(state)
    {
        case 1:
            //active
            if (i & 1) {
                return btVector3(0.0f, 0.0f, 1.0f)+btVector3(1.0f, 0.0f, 0.0f);
            }
            else {
                return btVector3(1.0f, 1.0f, 0.5f)+btVector3(0.5f, 0.0f, 0.0f);
            }
            break;

        case 2:
            //ISLAND_SLEEPING 
            if(i&1) {
                return btVector3(0.0f, 0.0f, 1.0f)+btVector3(0.0f, 1.0f, 0.0f);
            }
            else {
                return btVector3(1.0f, 1.0f, 0.5f)+btVector3(0.0f, 0.5f, 0.0f);
            }
            break;

        default:
            //wants deactivation
            if(i&1){
                return btVector3(0.0f, 0.0f, 1.0f);
            }
            else{
                return btVector3(1.0f, 1.0f, 0.5f); 
            }
            break;
    }
}


btScalar m[16];
btMatrix3x3 rot;
btVector3 wireColor;
static void renderobj(int i, btCollisionObject *colObj,
        const btVector3 &aabbMin, const btVector3 &aabbMax, int debugMode)
{
    btRigidBody* body=btRigidBody::upcast(colObj);
    if(body&&body->getMotionState()) {
        btDefaultMotionState* myMotionState = 
            (btDefaultMotionState*)body->getMotionState();
        myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
    }
    else {
        colObj->getWorldTransform().getOpenGLMatrix(m);
    }
    wireColor=getColor(colObj->getActivationState(), i);
    GL_ShapeDrawer::Draw(m, 
            colObj->getCollisionShape(),
            wireColor,
            debugMode,
            aabbMin, aabbMax);
}
static void rendershadowvolume(btCollisionObject *colObj,
        const btVector3 &aabbMin, const btVector3 &aabbMax, 
        const btVector3 &sundirection)
{
    btRigidBody* body=btRigidBody::upcast(colObj);
    if(body&&body->getMotionState()) {
        btDefaultMotionState* myMotionState = 
            (btDefaultMotionState*)body->getMotionState();
        myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
        rot=myMotionState->m_graphicsWorldTrans.getBasis();
    }
    else {
        colObj->getWorldTransform().getOpenGLMatrix(m);
        rot=colObj->getWorldTransform().getBasis();
    }
    GL_ShapeDrawer::Shadow(m, sundirection*rot,
            colObj->getCollisionShape(),
            aabbMin, aabbMax);
}
static void rendershadowcolor(int i, btCollisionObject *colObj,
        const btVector3 &aabbMin, const btVector3 &aabbMax)
{
    btRigidBody* body=btRigidBody::upcast(colObj);
    if(body&&body->getMotionState()) {
        btDefaultMotionState* myMotionState = 
            (btDefaultMotionState*)body->getMotionState();
        myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
    }
    else {
        colObj->getWorldTransform().getOpenGLMatrix(m);
    }
    wireColor=getColor(colObj->getActivationState(), i);

    GL_ShapeDrawer::Draw(m,
            colObj->getCollisionShape(),
            wireColor*btScalar(0.3), 
            0,
            aabbMin,aabbMax);
}
static void renderscene(btDynamicsWorld *dynamicsWorld, int pass, int debugMode,
        const btVector3 &sundirection)
{
    btVector3 aabbMin,aabbMax;
    dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin,aabbMax);
    aabbMin-=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
    aabbMax+=btVector3(BT_LARGE_FLOAT,BT_LARGE_FLOAT,BT_LARGE_FLOAT);
    if (!(debugMode& btIDebugDraw::DBG_DrawWireframe)){
        const int numObjects=dynamicsWorld->getNumCollisionObjects();
        switch(pass)
        {
            case 0:
                {
                    for(int i=0;i<numObjects;i++)
                    {
                        renderobj(i, dynamicsWorld->getCollisionObjectArray()[i],
                                aabbMin, aabbMax, debugMode);
                    }
                    break;
                }
            case 1:
                {
                    for(int i=0;i<numObjects;i++)
                    {
                        rendershadowvolume(dynamicsWorld->getCollisionObjectArray()[i],
                                aabbMin, aabbMax, sundirection);
                    }
                    break;
                }
            case 2:
                {
                    for(int i=0;i<numObjects;i++)
                    {
                        rendershadowcolor(i, dynamicsWorld->getCollisionObjectArray()[i],
                                aabbMin, aabbMax);
                    }
                    break;
                }

        }
    }
}

static void renderwithshadow(btDynamicsWorld *dynamicsWorld, 
        bool textureenabled, Texture *texture, 
        const btVector3 &sundirection, int debugMode)
{
    glClear(GL_STENCIL_BUFFER_BIT);
    // default draw
    glEnable(GL_CULL_FACE);
    if(textureenabled){
        texture->begin();
    }
    renderscene(dynamicsWorld, 0, debugMode, sundirection);
    if(textureenabled){
        texture->end();
    }
    // shadow pass 1(GL_CCW)
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    glStencilFunc(GL_ALWAYS,1,0xFFFFFFFFL);
    glFrontFace(GL_CCW);
    glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
    renderscene(dynamicsWorld, 1, debugMode, sundirection);
    // shadow pass 2(GL_CW)
    glFrontFace(GL_CW);
    glStencilOp(GL_KEEP,GL_KEEP,GL_DECR);
    renderscene(dynamicsWorld, 1, debugMode, sundirection);
    // shadow pass 3
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
    glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFFL );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    glDisable(GL_LIGHTING);
    if(textureenabled){
        texture->begin();
    }
    renderscene(dynamicsWorld, 2, debugMode, sundirection);
    if(textureenabled){
        texture->end();
    }
    // restore
    glEnable(GL_LIGHTING);
    glDepthFunc(GL_LESS);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
}

static void render(btDynamicsWorld *dynamicsWorld, int debugMode)
{
    glDisable(GL_CULL_FACE);
    renderscene(dynamicsWorld, 0, debugMode, btVector3());
}

void DemoApplication::displayCallback(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myinit();
    m_camera->updateCamera();
    btDynamicsWorld *dynamicsWorld=m_bulletworld->getDynamicsWorld();
    if(dynamicsWorld){
        if(m_enableshadows) {
            renderwithshadow(dynamicsWorld, m_textureenabled, m_texture, 
                    m_sundirection, getDebugMode());
        }
        else{
            if(m_textureenabled){
                m_texture->begin();
            }
            render(dynamicsWorld, getDebugMode());
            if(m_textureenabled){
                m_texture->end();
            }
        }
    }
    //optional but useful: debug drawing to detect problems
    {
        m_camera->setOrthographicProjection();
        m_profiler->render(m_bulletworld->isIdle(), getDebugMode());
        m_camera->resetPerspectiveProjection();
    }
    glDisable(GL_LIGHTING);
    m_bulletworld->debugDraw();
    glFlush();
}

