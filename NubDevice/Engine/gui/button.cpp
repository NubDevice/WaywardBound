#include <pch.h>
#include "mesh.h"
#include "button.h"
#include "input.h"

using namespace gui;

extern Input input;     // todo:
extern Display display; // todo:

// 147 x 76 texels () into 512^2 [ ]

float btn_vertices[] = {
      0.15f, 0.15f, 0.0f, 0.1525f, 0.9804f, // top right
      0.15f, 0.0f,  0.0f, 0.1552f, 0.8339f, // bottom right
      0.01f, 0.0f,  0.0f, 0.0136f, 0.8339f, // bottom left
      0.01f, 0.15f, 0.0f, 0.0136f, 0.9804f, // top left
      0.30f, 0.15f, 0.0f, 0.2968f, 0.9804f,
      0.30f, 0.0f,  0.0f, 0.2968f, 0.8339f
};

float btn_vertices_hover[] = {
      0.15f, 0.15f, 0.0f, 0.4411f, 0.9804f, // top right
      0.15f, 0.0f,  0.0f, 0.4411f, 0.8339f, // bottom right
      0.01f, 0.0f,  0.0f, 0.2968f, 0.8339f, // bottom left
      0.01f, 0.15f, 0.0f, 0.2968f, 0.9804f, // top left
      0.30f, 0.15f, 0.0f, 0.58f,   0.9804f,
      0.30f, 0.0f,  0.0f, 0.58f,   0.8339f
};    

unsigned int btn_indices[] = { // shared quad configuration
   0, 3, 1,
   1, 3, 2, // button left half
   4, 0, 5,
   5, 0, 1  // button right
};


void Button::initialize(std::string _name, glm::mat4 _world)
{
   isClick=false;
   isHover=false;
   name = _name;
   world = _world;
   mesh_skin.initialize(btn_vertices, btn_indices, 6, 4);
   mesh_hover.initialize(btn_vertices_hover, btn_indices, 6, 4);

   // bounds (VERTICAL:600px == 1:1)
   float minx = 1.0f, miny = 1.0f; 
   float maxx = 0.0f, maxy = 0.0f;
   int ind;
   for (int i = 0; i < 6; i++)
   {  ind = i*5;
      if (btn_vertices[ind] < minx)   minx = btn_vertices[ind];
      if (btn_vertices[ind] > maxx)   maxx = btn_vertices[ind];
      if (btn_vertices[ind+1] < miny) miny = btn_vertices[ind+1];
      if (btn_vertices[ind+1] > maxy) maxy = btn_vertices[ind+1];
   }
   bounds = sBounds(minx, miny, maxx, maxy);
}

Button::~Button()
{
   mesh_skin.destroy();
   mesh_hover.destroy();
}

void Button::begin() {} // push state
void Button::end() {}   // pop  state


bool Button::draw()
{  
   bool result = false;
   bool isHover = false;
   bool isClick = false;
   
   POINT point;
   ::GetCursorPos(&point); //point.y+=::GetSystemMetrics(SM_CYCAPTION)-::GetSystemMetrics(SM_CYBORDER);
   point.x+=::GetSystemMetrics(SM_CXBORDER)*2; 
   ::ScreenToClient(display.hwnd, &point);     // todo: viewport must be wrong (make pixel perfect)
   glm::vec2 nds(float(point.x)/display.dims.width, float(point.y)/display.dims.height);
   nds*=2.0f; nds.x-=1.0f; nds.y-=1.0f; nds.y*=-1.0f;// transform to world screen space (center zero)(flip y)
      
      //char msg[64]; sprintf_s(msg, sizeof(msg), "(%.4f, %.4f)", nds.x, nds.y);
      //_log.entry(msg, _log._error, "mouse: "); 

   nds.x-=world[3][0]; 
   nds.y-=world[3][1]; // inverse transform the pick point to vertex space

   
   if (nds.x > bounds.left) {
      if (nds.x < bounds.right) {
         if (nds.y > bounds.bottom+0.008f) {
            if (nds.y < bounds.top)
            {
               isHover = true;
               if (input.pAction_map->GetBoolWasDown(confirm))
               {
                  // take one time action
                  _log.entry("button clicked", _log._default, name.c_str());
                  isClick = true;
                  result = true; // activated
               }
            }
         }
      }
   }  

   if (isHover == true)
   {
      mesh_hover.draw();
   }
   mesh_skin.draw();
   return result;
   
}
