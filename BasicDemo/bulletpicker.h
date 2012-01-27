#ifndef BULLET_PICKER_H
#define BULLET_PICKER_H


#include "LinearMath/btVector3.h"
class btTypedConstraint;
class btRigidBody;
class btVector3;
class btDynamicsWorld;


class BulletPicker
{
    ///constraint for mouse picking
    btTypedConstraint* m_pickConstraint;
    btRigidBody* pickedBody;
    float mousePickClamping;
    int gPickingConstraintId;
    btVector3 gOldPickingPos;
    btVector3 gHitPos;
    float gOldPickingDist;
    bool use6Dof;

public:
    BulletPicker();
    ~BulletPicker();
    void pickStart(btDynamicsWorld *dynamicsWorld,
            const btVector3 &rayFrom, const btVector3 &rayTo);
    void pick(btDynamicsWorld *dynamicsWorld,
            const btVector3 &camPos, const btVector3 &rayTo, bool ortho);
    void removePickingConstraint(btDynamicsWorld *dynamicsWorld);
};


#endif
