#ifndef _engine_display_h_
#define _engine_display_h_

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Display 
{

public:
   Display(int _width, int _height);
   ~Display();

   bool Create(std::string title);
   bool Initialize();
   
   struct sDims {
      int width;
      int height;
   } dims;

   HWND  hwnd;
   HDC   hdc;
   HGLRC hrc;

private:
   Display();

};

#endif 
