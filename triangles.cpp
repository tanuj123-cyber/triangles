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
#include "Matrix4f.h"
#include "Vertex4.h"

#include <cassert>

#define DEBUG 1

#if DEBUG
#define _breakpoint() __asm__ volatile ("int $3\n")
#define _cond_break(cond) if(cond) __asm__ volatile ("int $3\n")
#else
#define _breakpoint() 
#define _cond_break(cond) 
#endif

using namespace std;

struct vertex {
  float x,y,z,w;
};

class ScanLineRasterization {
  int width;
  int height;
  int* scanBuffer;

  public: ScanLineRasterization(int width, int height): 
    height(height), width(width) {
      scanBuffer = new int[height * 2]();
    }

~ScanLineRasterization() {
    delete[] scanBuffer;
}
  void createTriangle(Vertex4 minVer, Vertex4 midVer, Vertex4 maxVer, int side){
    Matrix4f screenSpaceTransform;
    screenSpaceTransform.InitScreenSpaceTransform(width/2.0, height/2.0);
    Vertex4 minVert = screenSpaceTransform.Transform(minVer);
    minVert = minVert.PerspectiveDivide();
    Vertex4 midVert = screenSpaceTransform.Transform(midVer);
    midVert = midVert.PerspectiveDivide();
    Vertex4 maxVert = screenSpaceTransform.Transform(maxVer);
    maxVert = maxVert.PerspectiveDivide();
    //_breakpoint();

   if (maxVert.GetY() > midVert.GetY()) {
     Vertex4 temp = maxVert;
     maxVert = midVert;
     midVert = temp;
   }
   if (midVert.GetY() > minVert.GetY()){
     Vertex4 temp = midVert;
     midVert = minVert;
     minVert = temp;
   }
   if (maxVert.GetY() > midVert.GetY()){
     Vertex4 temp = maxVert;
     maxVert = midVert;
     midVert = temp;
   }



    std::fill(scanBuffer, scanBuffer + height * 2, 0);
    vertex minV = {minVert.GetX(), minVert.GetY(), minVert.GetZ(), minVert.GetW()};
    vertex midV = {midVert.GetX(), midVert.GetY(), midVert.GetZ(), midVert.GetW()};
    vertex maxV = {maxVert.GetX(), maxVert.GetY(), maxVert.GetZ(), maxVert.GetW()};
    
    convertToLine(minV, maxV, side);
    convertToLine(minV, midV, !side);
    convertToLine(midV, maxV, !side);

  }

  void convertToLine(vertex minY, vertex maxY, int side) {


    _breakpoint();

    //printf("-------------------\n");

    int yDist = minY.y - maxY.y;
    int xDist = minY.x - maxY.x;

    if (yDist < 0){
      return;
    }
    if (yDist == 0){
      int xMin = (int) xDist > 0 ? maxY.x : minY.x;
      int xMax = (int) xDist < 0 ? minY.x : maxY.x;
      scanBuffer[(int) minY.y * 2] = xMin;
      scanBuffer[(int) minY.y * 2 + 1] = xMax;
      return;
    }

    float xStep = (float) xDist/yDist;
    float currX = (float) maxY.x;

    for (int i = maxY.y; i < minY.y; i++){
      scanBuffer[i * 2 + side] = (int) currX;

      printf("%d ", (int) currX);


      currX += xStep;
    }

    printf("\n");
  }

  public: void FillTriangle(uint32_t *pixels, int w, int h){
    //clear cpu buffer
    std::fill(pixels, pixels + w * h, 0x00000000);
    
    //iterate through scanBuffer, fill out each row of pixels accordingly
    for (int i = 0; i < h; i ++){
      int xMin = scanBuffer[i*2];
      int xMax = scanBuffer[i*2 + 1];


      
      for (int j = xMin; j < xMax; j++) {
        pixels[i * w + j] = 0xFFFFFFFF;
      }
    
    }
  }
};



int main(int argc, char *argv[]){
    
    SDL_Renderer *renderer;
    SDL_Event event;
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    int width = 800;
    int height = 600;
    SDL_CreateWindowAndRenderer(
      "Triangles",
      width,
      height,
      0,
      &window,
      &renderer
    );


    SDL_Texture* texture = SDL_CreateTexture(
      renderer,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING,
      width,
      height
    );

    //create cpu buffer
   // std::vector<uint32_t> framebuffer(width * height);
    uint32_t* framebuffer = new uint32_t[width*height]; 
    // triangle vertices
    vertex minVert {-1, -1, 0};
    vertex midVert {0, 1, 0};
    vertex maxVert {1, -1, 0}; 
    Vertex4 minYVert(minVert.x, minVert.y, minVert.z, 1.0);
    Vertex4 midYVert(midVert.x, midVert.y, midVert.z, 1.0);
    Vertex4 maxYVert(maxVert.x, maxVert.y, maxVert.z, 1.0);

    float x_diff1 = maxVert.x - minVert.x;
    float y_diff1 = maxVert.y - minVert.y;

    float x_diff2 = midVert.x - minVert.x;
    float y_diff2 = midVert.y - minVert.y;

    float triangle_area = 0.5 * (x_diff1 * y_diff2 - x_diff2 * y_diff1);
    
    ScanLineRasterization triangle(width, height);

    auto prevTime = std::chrono::high_resolution_clock::now();
    Matrix4f projection;
    projection = projection.InitPerspective(70, width * 1.0/(height * 1.0), 0.1f, 1000.0);
    float rotCounter = 0.0f;

    while(1){
      SDL_PollEvent(&event);

      if (event.type == SDL_EVENT_QUIT){
        return 0;
      }
      
     //framebuffer.data() returns pointer to the first element of the vector, of type uint32_t*, address at &framebuffer[0]
      
      auto currTime = std::chrono::high_resolution_clock::now();

      float delta = std::chrono::duration<float>(currTime - prevTime).count();
      prevTime = currTime;

      rotCounter += delta;
 //insert matrix rotations translations etc. here 
 
      Matrix4f rotation;
      rotation = rotation.InitRotation(0.0f, rotCounter, 0.0f);
      Matrix4f translation;
      translation = translation.InitTranslation(0.0f, 0.0f, 3.0f);
      Matrix4f transformation = projection.matMultiply(translation.matMultiply(rotation));
      minYVert = transformation.Transform(minYVert);
      midYVert = transformation.Transform(midYVert);
      maxYVert = transformation.Transform(maxYVert);

      SDL_RenderClear(renderer); //clear gpu buffer
      triangle.createTriangle(minYVert, midYVert, maxYVert, triangle_area < 0);
      triangle.FillTriangle(framebuffer, width, height);
      //update cpu buffer here. aka function call

      SDL_UpdateTexture(
          texture,
          nullptr,
          framebuffer,
          width * sizeof(uint32_t)
      );
      //last parameter in UpdateTexture is pitch, i.e. the number of bytes in a row of pixel data. since width is 800 pixels, and each pixel is 4bytes=32bits, width * sizeof(uint32_t)

      SDL_RenderTexture(renderer, texture, nullptr, nullptr);
      SDL_RenderPresent(renderer);
    }
    delete[] framebuffer;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
