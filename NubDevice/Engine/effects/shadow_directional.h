// 
// file : shadow_directional.h
//

#ifndef _engine_effects_shadow_directional_h_
#define _engine_effects_shadow_directional_h_

namespace Effects 
{
   class cShadow_directional
	{
	public:
		cShadow_directional() {}
      ~cShadow_directional();
		
      void initialize(int bufferWidth, int bufferHeight);
		GLuint enableBuffer(GLuint shadowProgramID, glm::vec3 viewPos);
		void   disableBuffer();

		glm::vec3 lightPosition;
		glm::mat4 lightSpaceMatrix;

		unsigned int tex_depthMap;

	private:
		int _bufferWidth;
		int _bufferHeight;

		unsigned int _fbo_depthMap;
	};

} 

#endif
