#include "vector3.hpp"
#include <cmath>

  Vector3 Vector3::add(Vector3 other){
    return {x+other.x, y+other.y, z+other.z};
  }
  Vector3 Vector3::sub(Vector3 other){
    return {x-other.x, y-other.y, z-other.z};
  }
  Vector3 Vector3::times(float other){
    return {x*other, y*other, z*other};
  }
  float Vector3::dot(Vector3 other){
    return x*other.x + y*other.y + z*other.z;
  }
  Vector3 Vector3::cross(Vector3 other){
    return{
      y*other.z-z*other.y,
      z*other.x-x*other.z,
      x*other.y-y*other.x
    };
  }
  float Vector3::length(){
    return sqrt(x*x+y*y+z*z);
  }
  Vector3 Vector3::normalized(){
    const float l = length();
    return {x/l, y/l, z/l};
  }