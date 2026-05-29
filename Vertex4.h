#ifndef VERTEX_H
#define VERTEX_H

class Vertex4 {
    
public:
  float v[4];

    // Constructor
    Vertex4(float x, float y, float z, float w);

    // Perspective divide
    Vertex4 PerspectiveDivide();

    // Getters (you’ll probably need these)
    float GetX();
    float GetY();
    float GetZ();
    float GetW();
};

#endif
