import swigbullet as bullet
from .vector3 import Vector3


class BulletShooter(object):
    def __init__(self):
        self.m_shootBoxShape=None
        self.m_ShootBoxInitialSpeed=40.0
        box = bullet.btBoxShape((0.5, 0.5, 0.5))
        box.initializePolyhedralFeatures()
        self.m_shootBoxShape=box

    def shootBox(self, world, camPos, destination):
        startTransform=bullet.btTransform()
        startTransform.setIdentity()

        body = world.localCreateRigidBody(1.0, startTransform, self.m_shootBoxShape)

        body.setLinearFactor((1.0,1.0,1.0))

        body.getWorldTransform().setOrigin(camPos);
        body.getWorldTransform().setRotation(bullet.btQuaternion(0,0,0,1));
        linVel=Vector3(
                destination[0]-camPos[0],
                destination[1]-camPos[1],
                destination[2]-camPos[2]).normalize()*self.m_ShootBoxInitialSpeed
        body.setLinearVelocity(linVel)
        body.setAngularVelocity((0.0, 0.0, 0.0))
        body.setCcdMotionThreshold(0.5)
        body.setCcdSweptSphereRadius(0.9)

