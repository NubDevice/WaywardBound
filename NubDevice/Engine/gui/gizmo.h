#ifndef _engine_gui_gizmo_h_
#define _engine_gui_gizmo_h_

#include "../loaders/scene.h"

namespace gui
{

   class Gizmo
   {
   public:
      Gizmo() = default;

      void initialize();
      
      void update(Display* display, Input* input, Loaders::Scene* scene, Shader* shaders, glm::mat4 mtx_VP);
      void draw(HWND hwnd, Shader* pShader, glm::mat4 mvp, bool select = false);
      void draw_translation(HWND hwnd, bool select=false);
      void draw_rotation(HWND hwnd, bool select=false);

      int mode = 0;

      glm::ivec3 is_axis_hover = glm::ivec3(0);
      int is_axis_active = 0;
      bool is_dragging = false;
      bool bObj_selected = false;
     
      GLuint color_id;
      GLuint transform_id;
      GLuint obj_color_id;

      int selected_item_id = 0;

   private:

      GLuint vao;
      GLuint vbo;
      GLuint ebo;
      GLuint ubo;
   };

}
#endif

