#ifndef _engine_camera_h_
#define _engine_camera_h_


const float YAW =   0.0f;
const float PITCH = 0.0f;
const float SPEED = 6.5f;
const float SENSITIVITY = 151.f;
const float ZOOM = 45.0f;

class Camera
{
public:
   enum eMovement {
      FORWARD,
      BACKWARD,
      LEFT,
      RIGHT,
      UP,
      DOWN
   };

   glm::mat4 currentViewMatrix;
   glm::mat4 previousViewMatrix;

   glm::vec3 position;
   glm::vec3 front;
   glm::vec3 up;
   glm::vec3 right;
   glm::vec3 worldUp;
  
   float yaw;
   float pitch;
  
   float movementSpeed;
   float mouseSensitivity;
   float zoom;


   Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch); 
   Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
   
   glm::mat4 getViewMatrix() { return glm::lookAt(position, position + front, up); } // todo: wht up?? umm, working on it :)
  
   void move(eMovement direction, float deltaTime);
   void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
   void ProcessMouseScroll(float yoffset);

   void updateCameraVectors();

private:
   void reset();
   
};

#endif
