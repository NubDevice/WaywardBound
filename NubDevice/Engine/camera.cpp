#include <pch.h>
#include "camera.h"

using namespace glm;


Camera::Camera(vec3 pos = vec3(0.0f, 0.0f, 0.0f), 
               vec3 up  = vec3(0.0f, 1.0f, 0.0f),
               float yaw = YAW, float pitch = PITCH) 
             :
               movementSpeed(SPEED), 
               mouseSensitivity(SENSITIVITY), 
               zoom(ZOOM)
{
   position = pos;
   worldUp = up;
   this->yaw = yaw;
   this->pitch = pitch;
   updateCameraVectors();
   // defaultState for reset???
}


Camera::Camera(float posX, float posY, float posZ, 
               float upX, float upY, float upZ, 
               float yaw, float pitch) 
             : 
               movementSpeed(SPEED), 
               mouseSensitivity(SENSITIVITY), 
               zoom(ZOOM)
{
   position = glm::vec3(posX, posY, posZ);
   worldUp = glm::vec3(upX, upY, upZ);
   this->yaw = yaw;
   this->pitch = pitch;
   updateCameraVectors();
}


void Camera::reset()
{
   _log.entry("reset not implemented", _log._error, "Error: ");
   //updateCameraVectors();
}

void Camera::move(eMovement direction, float deltaTime)
{
   float velocity = movementSpeed * deltaTime;
   if (direction == FORWARD)
      position += front * velocity;
   if (direction == BACKWARD)
      position -= front * velocity;
   if (direction == LEFT)
      position -= right * velocity;
   if (direction == RIGHT)
      position += right * velocity;
   if (direction == UP)
      position += up * velocity;
   if (direction == DOWN)
      position -= up * velocity;

   //_log.entry("position", _log._warning, "");
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
  
   yoffset *= mouseSensitivity;

   yaw = -xoffset+130; // some random twist off grid (input wrap)
   pitch -= yoffset;

   if (constrainPitch)
   {
      if (pitch > 89.0f)
         pitch = 89.0f;
      if (pitch < -89.0f)
         pitch = -89.0f;
   }

   updateCameraVectors();
}


void Camera::ProcessMouseScroll(float yoffset)
{
   if (zoom >= 1.0f && zoom <= 45.0f)
      zoom -= yoffset;
   if (zoom <= 1.0f)
      zoom = 1.0f;
   if (zoom >= 45.0f)
      zoom = 45.0f;
}


void Camera::updateCameraVectors()
{
   front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
   front.y = sin(glm::radians(pitch));
   front.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
   front = glm::normalize(front);
 
   right = glm::normalize(glm::cross(front, worldUp));  
   up = glm::normalize(glm::cross(front, right));
}

