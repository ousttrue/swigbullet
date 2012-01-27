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

#ifndef DEMO_APPLICATION_H
#define DEMO_APPLICATION_H


#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
class BulletWorld;
class BulletShooter;
class BulletPicker;
class Camera;
class Profiler;
class Texture;
class btCollisionShape;


class DemoApplication
{
    BulletWorld *m_bulletworld;
    BulletShooter *m_shooter;
    BulletPicker *m_picker;
	//keep the collision shapes, for deletion/cleanup
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
    Camera *m_camera;
    Profiler *m_profiler;
    Texture *m_texture;
    int m_mouseOldX;
    int m_mouseOldY;
    int m_mouseButtons;
    float m_scaleBottom;
    float m_scaleFactor;
    bool m_stepping;
    bool m_singleStep;
    int m_modifierKeys;
    bool m_active_alt;
    bool m_enableshadows;
    int m_debugMode;
    btVector3 m_sundirection;
    bool m_textureenabled;
#ifdef USE_BT_CLOCK
    btClock m_clock;
#endif //USE_BT_CLOCK

public:
    DemoApplication();
    virtual ~DemoApplication();
    btScalar getDeltaTimeMicroseconds()
    {
#ifdef USE_BT_CLOCK
        btScalar dt = (btScalar)m_clock.getTimeMicroseconds();
        m_clock.reset();
        return dt;
#else
        return btScalar(16666.);
#endif
    }
    void createGround();
    void createCubes();
    void setDebugMode(int mode)
    {
        m_debugMode = mode;
    }
    int getDebugMode()
    {
        return m_debugMode ;
    }
    void toggleShadow(){ m_enableshadows=!m_enableshadows; }
    bool enableTexture(bool enable) 
    { 
        bool p=m_textureenabled;
        m_textureenabled=enable;
        return(p); 
    }
    bool hasTextureEnabled() const
    {
        return m_textureenabled;
    }
    Camera* getCamera(){ return m_camera; }
    BulletWorld* getBulletWorld(){ return m_bulletworld; }
    void setModifierKeys(int modifierKeys)
    {
        m_modifierKeys=modifierKeys;
    }
    void setActiveAlt(bool active_alt)
    {
        m_active_alt=active_alt;
    }

    // glut interface
    void keyboardCallback(unsigned char key, int x, int y);
    void keyboardUpCallback(unsigned char key, int x, int y) {}
    void specialKeyboard(int key, int x, int y);
    void specialKeyboardUp(int key, int x, int y){}
    void reshape(int w, int h);
    void update();
    void mouseFunc(int button, int state, int x, int y);
    void mouseMotionFunc(int x,int y);
    void displayCallback();

private:
    void myinit();
};

#endif //DEMO_APPLICATION_H
