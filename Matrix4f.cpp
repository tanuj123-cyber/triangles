#include <SDL3/SDL.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <cstdint>
#include <fstream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace std;

class Matrix4f{
  private:
  float m[4][4];

  public: 
  Matrix4f(){
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++){
        m[i][j] = 0.0f;        
      }
    }
   
  }

  Matrix4f& InitIdentity() {
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++){
        m[i][j] = i == j ? 1.0f : 0.0f;
      }
    }
    return *this;
  }

  Matrix4f InitPerspective(float fov, float asp_ratio, float zLow, float zHigh){
    Matrix4f res;

    float halfFov = fov / 2.0;
    
    // tan(halffov) = x/z. this is the slope of the hypotenuse of the right triangle. multiplying by aspect ratio will scale this along the width. we dont include the asp ratio thing to y bc its wider than it is height

    res.m[0][0] = 1.0 / (tan(halfFov) * asp_ratio);
    res.m[1][1] = 1.0 / tan(halfFov);
    res.m[2][2] = (-zLow - zHigh) / (zHigh - zLow);
    res.m[2][3] = 2 * zLow * zHigh / (zHigh - zLow);
    return res;
  }

  Matrix4f InitRotation(float angX, float angY, float angZ) {
    Matrix4f rx;
    rx.InitIdentity();
    Matrix4f ry;
    ry.InitIdentity();
    Matrix4f rz;
    rz.InitIdentity();

    ry.m[0][0] = cos(angY);
    ry.m[0][2] = -sin(angY);
    ry.m[2][0] = sin(angY);
    ry.m[2][2] = cos(angY);
    rx.m[1][1] = cos(angX);
    rx.m[1][2] = -sin(angX);
    rx.m[2][1] = sin(angX);
    rx.m[2][2] = cos(angX);
    rz.m[0][0] = cos(angZ);
    rz.m[0][1] = -sin(angZ);
    rz.m[1][0] = sin(angZ);
    rz.m[1][1] = cos(angZ);

    return rz.matMultiply(ry.matMultiply(rx));
  }

  Matrix4f InitTranslation(float x, float y, float z){
    Matrix4f res;
    res.InitIdentity();

    res.m[0][3] = x;
    res.m[1][3] = y;
    res.m[2][3] = z;
    return res;
  }

  Matrix4f matMultiply(Matrix4f x){
    Matrix4f res;
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++){
        for (int k = 0; k < 4; k++){
          res.m[i][j] += m[i][k] * x.m[k][j];
        }
      }
    }
    return res;
  }
};
