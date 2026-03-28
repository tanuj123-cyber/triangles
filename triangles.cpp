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

using namespace std;

struct vertex {
  int x,y;
};

class ScanLineRasterization {
  
  int height;
  int* scanBuffer;

  public: ScanLineRasterization(int width, int height): 
    height(height) {
      scanBuffer = new int[height * 2]();
    }

~ScanLineRasterization() {
    delete[] scanBuffer;
}
  void createTriangle(vertex minVert, vertex midVert, vertex maxVert, int side){
    std::fill(scanBuffer, scanBuffer + height * 2, 0);
    convertToLine(minVert, maxVert, side);
    convertToLine(minVert, midVert, 1 - side);
    convertToLine(midVert, maxVert, 1 - side);
  }

  void convertToLine(vertex minY, vertex maxY, int side) {
    int yDist = maxY.y - minY.y;
    int xDist = maxY.x - minY.x;

    if (yDist <= 0){
      return;
    }

    float xStep = (float) xDist/yDist;
    float currX = (float) minY.x;

    for (int i = minY.y; i < maxY.y; i++){
      scanBuffer[i * 2 + side] = (int) currX;
      currX += xStep;
    }
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
    vertex minVert {100, 100};
    vertex midVert {150, 200};
    vertex maxVert {80, 300}; 

    float x_diff1 = maxVert.x - minVert.x;
    float y_diff1 = maxVert.y - minVert.y;

    float x_diff2 = midVert.x - minVert.x;
    float y_diff2 = midVert.y - minVert.y;

    float triangle_area = 0.5 * (x_diff1 * y_diff2 - x_diff2 * y_diff1);
    
    ScanLineRasterization triangle(width, height);

    while(1){
      SDL_PollEvent(&event);
      if (event.type == SDL_EVENT_QUIT){
        return 0;
      }
      
     //framebuffer.data() returns pointer to the first element of the vector, of type uint32_t*, address at &framebuffer[0]
      
      SDL_RenderClear(renderer); //clear gpu buffer
      triangle.createTriangle(minVert, midVert, maxVert, triangle_area >= 0);
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
