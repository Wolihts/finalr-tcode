#ifndef __triangle__
#define __triangle__

#include "vector3.hpp"
#include <vector>
using namespace std;
struct Triangle{
    Vector3 p0, p1, p2;
    Vector3 n0, n1, n2;
    Vector3 uv0, uv1, uv2;
    Vector3 (*shader)(Vector3 position, Vector3 normal, Vector3 uv, Vector3 light);
    float reflectivity = 0;
};
#endif