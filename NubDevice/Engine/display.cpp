#include <pch.h>
#include "res/resource.h"

Display::Display(int width, int height)
{  dims.width = width;
   dims.height = height;
}

Display::~Display()
{  wglDeleteContext(hrc);
}


bool Display::Create(std::string title)
{  HINSTANCE hInstance = GetModuleHandle(nullptr);
   WNDCLASS wc = {
      CS_HREDRAW | CS_VREDRAW | CS_OWNDC,                    // style
      WndProc,                                               // lpfnWndProc
      0,                                                     // cbClsExtra
      0,                                                     // cbWndExtra
      hInstance,                                             // hInstance
      LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_ENGINE)), // hIcon
      LoadCursor(nullptr, IDC_ARROW),                        // hCursor
      (HBRUSH)(COLOR_BACKGROUND),                            // hbrBackground
      0,                                                     // lpszMenuName
      "NubDeviceResource_class"                              // lpszClassName
   };

   if (!RegisterClass(&wc))
      return false;

   DWORD style = WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
   //DWORD style = WS_POPUP | WS_VISIBLE;
   
   RECT wndRect = { 0, 0, dims.width, dims.height };
   ::AdjustWindowRect(&wndRect, style, false);
   
   hwnd = CreateWindow(wc.lpszClassName, title.c_str(), style, 0, 30, wndRect.right-wndRect.left, wndRect.bottom-wndRect.top, 0, 0, wc.hInstance, 0);

   // todo : each display has it's own render context.
   //        review the static handle share during display creation
   //
   // move the hwnd to class extra???
   //
   //
   
   _log.begin("window creation (", _log._success); std::cout << dims.width << ", " << dims.height; _log.end(")");
   return Initialize();
}

bool Display::Initialize()
{  PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
            32,                   // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24,                   // Number of bits for the depthbuffer
            8,                    // Number of bits for the stencilbuffer
            0,                    // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0
   };

   hdc = GetDC(hwnd);
   int pixelFormat = ChoosePixelFormat(hdc, &pfd);
   if (pixelFormat == 0)                                { _log.entry("ChoosePixelFormat failed", _log._error, ""); return false; }
   if (SetPixelFormat(hdc, pixelFormat, &pfd) == false) { _log.entry("SetPixelFormat failed", _log._error, " ");   return false; }

   hrc = wglCreateContext(hdc);
   wglMakeCurrent(hdc, hrc);

   _log.entry((char*)glGetString(GL_RENDERER), _log._info, "Graphics: ");
   _log.entry((char*)glGetString(GL_VERSION), _log._info, "OpenGL version: ");
   _log.entry((char*)glGetString(GL_SHADING_LANGUAGE_VERSION), _log._info, "GLSL version: ");

   
      // ogl state -----------------------------
      glClearColor(0.01f, 0.002f, 0.0f, 1.f);
      // depth buffer
      glClearDepth(1.f);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
      // blend state
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

      // face culling
      //glPolygonMode(GL_FRONT, GL_LINE); // GL_FILL
      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);

      glActiveTexture(0);
      glEnable(GL_TEXTURE_2D);
      glActiveTexture(1);
      glEnable(GL_TEXTURE_2D);
      glActiveTexture(2);
      glEnable(GL_TEXTURE_2D);

      //glEnable(GL_FRAMEBUFFER_SRGB); // gamma correction // mwk: no, shader decision

   return true;
}

