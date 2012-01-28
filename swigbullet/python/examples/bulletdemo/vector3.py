import math


class Vector3(tuple):
    def __new__(cls, x, y, z):
        return tuple.__new__(cls, (x, y, z))

    def __add__(self, rhs):
        return Vector3(*[l+r for l, r in zip(self, rhs)])

    def __sub__(self, rhs):
        return Vector3(*[l-r for l, r in zip(self, rhs)])

    def __mul__(self, s):
        return Vector3(*(self[0]*s, self[1]*s, self[2]*s))

    def cross(l, r):
        return Vector3(*(l[1]*r[2]-l[2]*r[1], l[2]*r[0]-l[0]*r[2], l[0]*r[1]-l[1]*r[0]))

    def dot(l, r):
        return l[0]*r[0]+l[1]*r[1]+l[2]*r[2]

    def length2(self):
        return self.dot(self)

    def length(self):
        return math.sqrt(self.length2())

    def normalize(self):
        return self*(1.0/self.length())

