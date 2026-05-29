#include <cstdint>
#include <fstream>
#include <vector>
#include <chrono>
#include "Vertex4.h"

using namespace std;

//class Vertex4 {

  //public:

    float v[4];
    Vertex4::Vertex4(float x, float y, float z, float w){
      v[0] = x;
      v[1] = y;
      v[2] = z;
      v[3] = w;
    }

    Vertex4 Vertex4::PerspectiveDivide() {
      return Vertex4(v[0]/v[3], v[1]/v[3], v[2]/v[3], 1);
    }

    float Vertex4::GetX(){
      return v[0];
    }
    float Vertex4::GetY(){
      return v[1];
    }
    float Vertex4::GetZ(){
      return v[2];
    }   
    float Vertex4::GetW(){
      return v[3];
    }
//};
