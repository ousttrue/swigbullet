#ifndef BULLET_SHOOTER_H
#define BULLET_SHOOTER_H


class btVector3;
class BulletWorld;
class btCollisionShape;


class BulletShooter
{
    btCollisionShape* m_shootBoxShape;
    float m_ShootBoxInitialSpeed;
public:
    BulletShooter();
    ~BulletShooter();
    void setShootInitialSpeed(float initialSpeed)
    {
        m_ShootBoxInitialSpeed=initialSpeed;
    }
    float getShootInitialSpeed(){
        return m_ShootBoxInitialSpeed;
    }
    void shootBox(BulletWorld *world, 
            const btVector3 &camPos, const btVector3 &destination);
};


#endif
