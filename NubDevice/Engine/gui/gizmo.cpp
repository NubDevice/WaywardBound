#include <pch.h>
#include "gizmo.h"

// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/

unsigned int indices[] = {  0,  1,  0,  2,  0,  3,         // x, y, z translation pairs
                            4,  5,  6,  7,  8,  9, 10, 11, // x rotation (right)
                           12, 13, 14, 15, 16, 17, 18,
                           19, 20, 21, 22, 23, 24, 25, 26, // y rotation (up)
                           27, 28, 29, 30, 31, 32, 33,
                           34, 35, 36, 37, 38, 39, 40, 41, // z rotation (front)
                           42, 43, 44, 45, 46, 47, 48
};

void gui::Gizmo::initialize()
{
   float vertices[] = { 0.0f, 0.0f, 0.0f,
                        0.5f, 0.0f, 0.0f,
                        0.0f, 0.5f, 0.0f,
                        0.0f, 0.0f,-0.5f,
                        0.0f, 0.0f, 0.5f,    0.0f, 0.22f, 0.45f,  0.0f, 0.39f, 0.31f,  0.0f, 0.49f, 0.11f,  0.0f, 0.49f,-0.11f, 0.0f, 0.39f,-0.31f,  0.0f, 0.22f,-0.45f,  0.0f, 0.0f,-0.5f,                   
                        0.0f,-0.22f,-0.45f,  0.0f,-0.39f,-0.31f,  0.0f,-0.49f,-0.11f,  0.0f,-0.49f, 0.11f,  0.0f,-0.39f, 0.31f, 0.0f,-0.22f, 0.45f,  0.0f, 0.0f, 0.5f,

                       -0.5f, 0.0f, 0.0f,   -0.45f, 0.0f,-0.22f, -0.31f, 0.0f,-0.39f, -0.11f, 0.0f,-0.49f,  0.11f, 0.0f,-0.49f, 0.31f, 0.0f,-0.39f,  0.45f, 0.0f,-0.22f,  0.5f, 0.0f, 0.0f,  
                        0.45f, 0.0f, 0.22f,  0.31f, 0.0f, 0.39f,  0.11f, 0.0f, 0.49f, -0.11f, 0.0f, 0.49f, -0.31f, 0.0f,0.39f, -0.45f, 0.0f,0.22f,  -0.5f, 0.0f, 0.0f,
                        
                        0.5f, 0.0f, 0.0f,   0.45f, 0.22f, 0.0f,   0.31f, 0.39f, 0.0f,   0.11f, 0.49f, 0.0f,-0.11f, 0.49f, 0.0f, -0.31f, 0.39f, 0.0f, -0.45f, 0.22f, 0.0f, -0.5f, 0.0f, 0.0f,
                       -0.45f,-0.22f, 0.0f, -0.31f,-0.39f, 0.0f, -0.11f, -0.49f, 0.0f, 0.11f, -0.49f, 0.0f, 0.31f, -0.39f, 0.0f,0.45f,-0.22f, 0.0f, 0.5f, 0.0f, 0.0f,      
                      };
   
   glGenVertexArrays(1, &vao);
   glGenBuffers(1, &vbo);
   glGenBuffers(1, &ebo);

   glBindVertexArray(vao);
   
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
   
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float)*147, &indices[0], GL_STATIC_DRAW);

   glBindVertexArray(0);
}


#define BUFFER_OFFSET(i) ((float*)0 + (i))

float px = 0.f; // yeah whatever
float py = 0.f; // lots of todo
void gui::Gizmo::update(Display* display, Input* input, Loaders::Scene* scene, Shader* shaders, glm::mat4 mtx_VP)
{
   if (input->pAction_map->GetBoolIsNew(gizmo_translation))
   {
      if (mode == 0)
         mode = 1;
      else
         mode = 0;
   }
   is_dragging = false;
   if (!input->pAction_map->GetBool(confirm))
   {
      is_axis_active = 0; // reset for next run???
   }
   else if (is_axis_active > 0) {
      px = input->pAction_map->GetFloatDelta(mouseX);
      py = input->pAction_map->GetFloatDelta(mouseY);
      //printf_s("drag: %.2f, %.2f\n", px, py);
      
      // do that gizmo thing.......
      is_dragging = true;
      glm::mat4 model = scene->renderables[0].items[selected_item_id].transforms[0];

      if (mode == 1)
      {
         switch (is_axis_active)
         {
         case 1: model = glm::rotate(model, px * 10.f, glm::vec3(1.f, 0.f, 0.f)); break;
         case 2: model = glm::rotate(model, px * 10.f, glm::vec3(0.f, 1.f, 0.f)); break;
         case 3: model = glm::rotate(model, px * 10.f, glm::vec3(0.f, 0.f, 1.f)); break;
         }
      }
      else
      {
         switch (is_axis_active)
         {
         case 1: model = glm::translate(model, glm::vec3(-px * 10.f, 0.f, 0.f)); break;
         case 2: model = glm::translate(model, glm::vec3(0.f, -py * 10.f, 0.f)); break;
         case 3: model = glm::translate(model, glm::vec3(0.f, 0.f, -px * 10.f)); break;
         }
      }

      scene->renderables[0].items[selected_item_id].transforms[0] = model; // temp selected
   }


   // pick target draw
   if (bObj_selected && (is_axis_active == 0))
   {
      glLineWidth(8.0f);
      GLuint currentShader_id = shaders->use("gizmo");
      transform_id = glGetUniformLocation(currentShader_id, "transform");
      obj_color_id = glGetUniformLocation(currentShader_id, "obj_color");

      shaders->setMatrix("transform", mtx_VP * scene->renderables[0].items[selected_item_id].transforms[0]);
      shaders->setColor("obj_color_id", glm::vec4(1, 0, 0, 0));

      glClearColor(0, 0, 0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      if (mode == 0)
         draw_translation(display->hwnd, true);
      else
         draw_rotation(display->hwnd, true);

      glFlush();
      glFinish();
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      unsigned char data[4];
      POINT pt; ::GetCursorPos(&pt); ::ScreenToClient(display->hwnd, &pt);
      glReadPixels(pt.x, display->dims.height - pt.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
      int picked_id = data[0] +
         data[1] * 256 +
         data[2] * 256 * 256; // probably have enough here...

      //_log.begin("pick id: ", _log._warning); printf_s("%d", picked_id); _log.end();
      is_axis_hover = glm::ivec3(0);
      if (picked_id == 255) is_axis_hover = glm::ivec3(1, 0, 0);
      if (picked_id == 65280) is_axis_hover = glm::ivec3(0, 1, 0);
      if (picked_id == 16711680) is_axis_hover = glm::ivec3(0, 0, 1);

      glLineWidth(1.0f);
   }

}

void gui::Gizmo::draw(HWND hwnd, Shader* pShaders, glm::mat4 mvp, bool select)
{
   glDisable(GL_DEPTH_TEST);
   //glm::mat4 mvp = projection * camera.getViewMatrix() * scene.renderables[0].items[gizmo.selected_item_id].transforms[0];
   GLuint gizmo_id = pShaders->use("gizmo");
   pShaders->setMatrix("transform", mvp);
   obj_color_id = glGetUniformLocation(gizmo_id, "obj_color");

   if (mode == 0)
   draw_translation(hwnd);
   else
   draw_rotation(hwnd);

   glEnable(GL_DEPTH_TEST);
}

void gui::Gizmo::draw_translation(HWND hwnd, bool select)
{
   glBindVertexArray(vao);
   is_axis_hover.x == 0 || select ? glUniform4f(color_id, 1.0f, 0.0f, 0.0f, 1.0f) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINES, 0, 1, 2, GL_UNSIGNED_INT, BUFFER_OFFSET(0)); // x axis indices
   is_axis_hover.y == 0 || select ? glUniform4f(color_id, 0.0f, 1.0f, 0.0f, 1.0f) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINES, 2, 3, 2, GL_UNSIGNED_INT, BUFFER_OFFSET(2)); // y axis indices
   is_axis_hover.z == 0 || select ? glUniform4f(color_id, 0.0f, 0.0f, 1.0f, 1.0f) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINES, 4, 5, 2, GL_UNSIGNED_INT, BUFFER_OFFSET(4)); // z axis indices
   glBindVertexArray(0);
}

void gui::Gizmo::draw_rotation(HWND hwnd, bool select)
{
   glBindVertexArray(vao);
   is_axis_hover.x == 0 || select ? glUniform4f(color_id, 1, 0, 0, 0) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINE_STRIP, 6, 20, 15, GL_UNSIGNED_INT, BUFFER_OFFSET(6)); // x axis loop
   is_axis_hover.y == 0 || select ? glUniform4f(color_id, 0, 1, 0, 0) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINE_STRIP, 21, 35, 15, GL_UNSIGNED_INT, BUFFER_OFFSET(21)); // y axis loop
   is_axis_hover.z == 0 || select ? glUniform4f(color_id, 0, 0, 1, 0) : glUniform4f(color_id, 1, 1, 0, 0);
   glDrawRangeElements(GL_LINE_STRIP, 36, 50, 15, GL_UNSIGNED_INT, BUFFER_OFFSET(36)); // z axis loop
   glBindVertexArray(0);
}
