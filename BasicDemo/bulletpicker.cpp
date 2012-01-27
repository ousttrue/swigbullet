#include "bulletpicker.h"
#include <btBulletDynamicsCommon.h>


BulletPicker::BulletPicker()
    :
        m_pickConstraint(0),
        pickedBody(0), //for deactivation state
        mousePickClamping(30.f),
        gPickingConstraintId(0),
        gHitPos(-1,-1,-1),
        use6Dof(false),
        gOldPickingDist(0.f)
{
}


BulletPicker::~BulletPicker()
{
}


void BulletPicker::pickStart(btDynamicsWorld *dynamicsWorld, 
        const btVector3 &rayFrom, const btVector3 &rayTo)
{
    //add a point to point constraint for picking
    if (!dynamicsWorld) {
        return;
    }

    ClosestRayResultCallback rayCallback(rayFrom, rayTo);
    dynamicsWorld->rayTest(rayFrom,rayTo,rayCallback);
    if (rayCallback.hasHit())
    {


        btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
        if (body)
        {
            //other exclusions?
            if (!(body->isStaticObject() || body->isKinematicObject()))
            {
                pickedBody = body;
                pickedBody->setActivationState(DISABLE_DEACTIVATION);


                btVector3 pickPos = rayCallback.m_hitPointWorld;
                //printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());


                btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;






                if (use6Dof)
                {
                    btTransform tr;
                    tr.setIdentity();
                    tr.setOrigin(localPivot);
                    btGeneric6DofConstraint* dof6 = new btGeneric6DofConstraint(*body, tr,false);
                    dof6->setLinearLowerLimit(btVector3(0,0,0));
                    dof6->setLinearUpperLimit(btVector3(0,0,0));
                    dof6->setAngularLowerLimit(btVector3(0,0,0));
                    dof6->setAngularUpperLimit(btVector3(0,0,0));

                    dynamicsWorld->addConstraint(dof6);
                    m_pickConstraint = dof6;

                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,0);
                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,1);
                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,2);
                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,3);
                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,4);
                    dof6->setParam(BT_CONSTRAINT_STOP_CFM,0.8,5);

                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,0);
                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,1);
                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,2);
                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,3);
                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,4);
                    dof6->setParam(BT_CONSTRAINT_STOP_ERP,0.1,5);
                } else
                {
                    btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body,localPivot);
                    dynamicsWorld->addConstraint(p2p);
                    m_pickConstraint = p2p;
                    p2p->m_setting.m_impulseClamp = mousePickClamping;
                    //very weak constraint for picking
                    p2p->m_setting.m_tau = 0.001f;
                    /*
                       p2p->setParam(BT_CONSTRAINT_CFM,0.8,0);
                       p2p->setParam(BT_CONSTRAINT_CFM,0.8,1);
                       p2p->setParam(BT_CONSTRAINT_CFM,0.8,2);
                       p2p->setParam(BT_CONSTRAINT_ERP,0.1,0);
                       p2p->setParam(BT_CONSTRAINT_ERP,0.1,1);
                       p2p->setParam(BT_CONSTRAINT_ERP,0.1,2);
                       */


                }
                use6Dof = !use6Dof;

                //save mouse position for dragging
                gOldPickingPos = rayTo;
                gHitPos = pickPos;

                gOldPickingDist  = (pickPos-rayFrom).length();
            }
        }
    }
}


void BulletPicker::pick(btDynamicsWorld *dynamicsWorld, 
        const btVector3 &camPos, const btVector3 &rayTo)
{
	if (!m_pickConstraint){
        return;
    }

    //move the constraint pivot
    if (m_pickConstraint->getConstraintType() == D6_CONSTRAINT_TYPE)
    {
        btGeneric6DofConstraint* pickCon = 
            static_cast<btGeneric6DofConstraint*>(m_pickConstraint);
        if(!pickCon){
            return;
        }
        //keep it at the same picking distance
        btVector3 newRayTo = rayTo;
        btVector3 rayFrom;
        btVector3 oldPivotInB = pickCon->getFrameOffsetA().getOrigin();

        btVector3 newPivotB;
        {
            rayFrom = camPos;
            btVector3 dir = newRayTo-rayFrom;
            dir.normalize();
            dir *= gOldPickingDist;

            newPivotB = rayFrom + dir;
        }
        pickCon->getFrameOffsetA().setOrigin(newPivotB);
    } 
    else {
        btPoint2PointConstraint* pickCon = 
            static_cast<btPoint2PointConstraint*>(m_pickConstraint);
        if (!pickCon){
            return;
        }
        //keep it at the same picking distance
        btVector3 newRayTo = rayTo;
        btVector3 rayFrom;
        btVector3 oldPivotInB = pickCon->getPivotInB();
        btVector3 newPivotB;
        {
            rayFrom = camPos;
            btVector3 dir = newRayTo-rayFrom;
            dir.normalize();
            dir *= gOldPickingDist;

            newPivotB = rayFrom + dir;
        }
        pickCon->setPivotB(newPivotB);
    }
}


void BulletPicker::removePickingConstraint(btDynamicsWorld *dynamicsWorld)
{
    if(!dynamicsWorld){
        return;
    }
    if(!m_pickConstraint){
        return;
    }
    dynamicsWorld->removeConstraint(m_pickConstraint);
    delete m_pickConstraint;
    //printf("removed constraint %i",gPickingConstraintId);
    m_pickConstraint = 0;
    pickedBody->forceActivationState(ACTIVE_TAG);
    pickedBody->setDeactivationTime( 0.f );
    pickedBody = 0;
}

