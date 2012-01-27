/*
   Bullet Continuous Collision Detection and Physics Library
   Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

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
//glut is C code, this global gDemoApplication links glut to the C++ demo
static DemoApplication* gDemoApplication = 0;

#include "camera.h"
#include "bulletworld.h"
#include "GlutStuff.h"
#include <iostream>


static	void glutKeyboardCallback(unsigned char key, int x, int y)
{
    if(key=='q'){
#ifdef BT_USE_FREEGLUT
            //return from glutMainLoop(), detect memory leaks etc.
            glutLeaveMainLoop();
            return;
#else
            exit(0);
#endif
    }
    gDemoApplication->keyboardCallback(key,x,y);
}

static	void glutKeyboardUpCallback(unsigned char key, int x, int y)
{
    gDemoApplication->keyboardUpCallback(key,x,y);
}

static void glutSpecialKeyboardCallback(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_F1:
            break;
        case GLUT_KEY_F2:
            break;
        case GLUT_KEY_END:
            gDemoApplication->getBulletWorld()->removeLastObject();
            break;
        case GLUT_KEY_LEFT : 
            gDemoApplication->getCamera()->stepLeft(); 
            break;
        case GLUT_KEY_RIGHT : 
            gDemoApplication->getCamera()->stepRight(); 
            break;
        case GLUT_KEY_UP : 
            gDemoApplication->getCamera()->stepFront(); 
            break;
        case GLUT_KEY_DOWN : 
            gDemoApplication->getCamera()->stepBack(); 
            break;
        case GLUT_KEY_PAGE_UP : 
            gDemoApplication->getCamera()->zoomIn(); 
            break;
        case GLUT_KEY_PAGE_DOWN : 
            gDemoApplication->getCamera()->zoomOut(); 
            break;
        case GLUT_KEY_HOME : 
            gDemoApplication->getBulletWorld()->toggleIdle(); 
            break;
        default:
            std::cout << "unused (special) key : " << key << std::endl;
            break;
    }
    glutPostRedisplay();
}

static void glutSpecialKeyboardUpCallback(int key, int x, int y)
{
    gDemoApplication->specialKeyboardUp(key,x,y);
}

static void glutReshapeCallback(int w, int h)
{
    gDemoApplication->reshape(w,h);
}

static void glutUpdate()
{
    gDemoApplication->getBulletWorld()->update(
            gDemoApplication->getDeltaTimeMicroseconds());
    glutPostRedisplay();
}

static void glutMouseFuncCallback(int button, int state, int x, int y)
{
    int modifierKeys = 0;
    if (glutGetModifiers() & GLUT_ACTIVE_ALT)
       modifierKeys |= BT_ACTIVE_ALT;
    if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
       modifierKeys |= BT_ACTIVE_CTRL;
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
       modifierKeys |= BT_ACTIVE_SHIFT;
    gDemoApplication->setModifierKeys(modifierKeys);
    gDemoApplication->setActiveAlt(modifierKeys& BT_ACTIVE_ALT);
    gDemoApplication->mouseFunc(button,state,x,y);
}

static void	glutMotionFuncCallback(int x,int y)
{
    gDemoApplication->mouseMotionFunc(x,y);
}

static void glutDisplayCallback(void)
{
    gDemoApplication->displayCallback();
    glutSwapBuffers();
}


int glutmain(int argc, char **argv,int width,int height,const char* title,DemoApplication* demoApp) {

    gDemoApplication = demoApp;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);
#ifdef BT_USE_FREEGLUT
    glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif

    glutKeyboardFunc(glutKeyboardCallback);
    glutKeyboardUpFunc(glutKeyboardUpCallback);
    glutSpecialFunc(glutSpecialKeyboardCallback);
    glutSpecialUpFunc(glutSpecialKeyboardUpCallback);
    glutReshapeFunc(glutReshapeCallback);
    glutIdleFunc(glutUpdate);
    glutMouseFunc(glutMouseFuncCallback);
    glutPassiveMotionFunc(glutMotionFuncCallback);
    glutMotionFunc(glutMotionFuncCallback);
    glutDisplayFunc( glutDisplayCallback );

#if defined(__APPLE__) && !defined (VMDMESA)
    int swap_interval = 1;
    CGLContextObj cgl_context = CGLGetCurrentContext();
    CGLSetParameter(cgl_context, kCGLCPSwapInterval, &swap_interval);
#endif

    glutMainLoop();
    return 0;
}

