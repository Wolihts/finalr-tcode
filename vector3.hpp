#ifndef __vector3__
#define __vector3__

struct Vector3{
  float x, y, z;
  Vector3 add(Vector3 other);
  Vector3 sub(Vector3 other);
  Vector3 times(float other);
  float dot(Vector3 other);
  Vector3 cross(Vector3 other);
  float length();
  Vector3 normalized();
};
#endif