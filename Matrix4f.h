
#ifndef MATRIX4F_H
#define MATRIX4F_H

#include "Vertex4.h"
#include <cmath>

class Matrix4f {
private:
    float m[4][4];

public:
    Matrix4f();

    Matrix4f& InitIdentity();

    Matrix4f InitScreenSpaceTransform(float halfWidth, float halfHeight);
    
    Matrix4f InitPerspective(float fov, float asp_ratio, float zLow, float zHigh);

    Matrix4f InitRotation(float angX, float angY, float angZ);

    Matrix4f InitTranslation(float x, float y, float z);

    Vertex4 Transform(Vertex4);

    Matrix4f matMultiply(Matrix4f x);
};

#endif
