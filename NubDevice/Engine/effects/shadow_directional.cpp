//
// file : shadow_directional.cpp
//

#include <pch.h>
#include "shadow_directional.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace Effects;

void cShadow_directional::initialize(int bufferWidth, int bufferHeight)
{
	_bufferWidth = bufferWidth;
	_bufferHeight = bufferHeight;

	glGenFramebuffers(1, &_fbo_depthMap);
	glGenTextures(1, &tex_depthMap);
	glBindTexture(GL_TEXTURE_2D, tex_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _bufferWidth, _bufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo_depthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	lightPosition = glm::vec3(-2.0f, 4.0f, 1.0f);
}


cShadow_directional::~cShadow_directional()
{

}


GLuint cShadow_directional::enableBuffer(GLuint shadowProgramID, glm::vec3 viewPos)
{
	float nearPlane = 1.0f;
	float farPlane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, 10.0f, -10.0f, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;

	GLuint lightSpaceMatrixLoc = glGetUniformLocation(shadowProgramID, "lightSpace");
	GLuint modelMatrixLoc = glGetUniformLocation(shadowProgramID, "model");

	glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	//glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model));

   glUniform3f(glGetUniformLocation(shadowProgramID, "lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
   glUniform3f(glGetUniformLocation(shadowProgramID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	// render scene
	glViewport(0, 0, _bufferWidth, _bufferHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo_depthMap);
	glClear(GL_DEPTH_BUFFER_BIT);

	return modelMatrixLoc;
}


void cShadow_directional::disableBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
