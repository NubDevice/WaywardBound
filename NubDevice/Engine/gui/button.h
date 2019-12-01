#ifndef _engine_gui_button_h_
#define _engine_gui_button_h_


namespace gui
{
   class Button
   {

   public:
      Button() {}
      ~Button();

      void initialize(std::string _name, glm::mat4 _world);
      
      void begin(); // push state
      void end();   // pop  state
      
      bool draw();

      std::string name;
      glm::mat4 world;

      bool isClick;
      bool isHover;

   private:
      Mesh mesh_skin;
      Mesh mesh_hover;

      sBounds bounds;
      
   };

}// end namespace gui

#endif

