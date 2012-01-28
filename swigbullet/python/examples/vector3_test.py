from bulletdemo.vector3 import Vector3


def test_vector3():
    v1=Vector3(1, 0, 0)
    v2=Vector3(0, 1, 0)
    assert Vector3(1, 1, 0)==v1+v2
    assert Vector3(1, -1, 0)==v1-v2
    assert 0==v1.dot(v2)
    assert Vector3(0, 0, 1)==v1.cross(v2)
    assert 1==v1.length2()
    assert Vector3(1, 0, 0)==v1.normalize()

