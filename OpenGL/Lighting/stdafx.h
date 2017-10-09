#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace gl {
	namespace Lighting {
		struct LightIndexRange {
			LightIndexRange(unsigned int pOffset, unsigned int pCount)
				:offset(pOffset), count(pCount) {}
			unsigned int offset;
			unsigned int count;
		};

		extern std::vector<glm::vec4> allLightData;
		extern std::vector<LightIndexRange> allLightIndexRanges;
		extern unsigned int lightVAO;
		extern unsigned int lightIndexVBO;
		extern unsigned int lightDataUBO;
		extern unsigned int lightShaderProgram;
		extern unsigned int MAX_LIGHT_COUNT;
	}
}