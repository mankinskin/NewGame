#pragma once 
#include <vector> 
#include <glm\glm.hpp>
namespace gl{ 
	namespace Lighting{ 

		struct LightIndexRange{ 
			LightIndexRange(unsigned int pOffset, unsigned int pCount)
				:offset(pOffset), count(pCount) {}
			unsigned int offset;
			unsigned int count;
		};

		extern unsigned int lightDataUBO;
		extern unsigned int lightShaderProgram;
		extern unsigned int MAX_LIGHT_COUNT;

		void initLightVAO();
		void initLightDataBuffer();
		void updateLightDataBuffer();
		void updateLightIndexRangeBuffer();
		void initLightShader();
		void renderLights();
		unsigned int createLight(glm::vec4& pPos, glm::vec4& pColor);
		unsigned int createLight(glm::vec4& pPos, glm::vec4& pColor, glm::vec4& pFrustum);
		void setLightPos(unsigned int pLightIndex, glm::vec3& pPos);
		void setLightPos(unsigned int pLightIndex, glm::vec4& pPos);
		void setLightColor(unsigned int pLightIndex, glm::vec3& pColor);
		void setLightColor(unsigned int pLightIndex, glm::vec4& pColor);

		void reserveLightSpace(unsigned int pCount);//reserves 3 vec4s and one index range for count
		void reservePointLightSpace(unsigned int pCount);//reserves only 2 vec4 (and one index range) per light
    }
}
