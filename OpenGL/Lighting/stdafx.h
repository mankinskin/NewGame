#pragma once
#include <vector>
#include <glm/glm.hpp>
namespace gl {
    namespace Lighting {
	struct LightIndexRange {
	    LightIndexRange(size_t pOffset, size_t pCount)
		:offset(pOffset), count(pCount) {}
	    size_t offset;
	    size_t count;
	};

	extern std::vector<glm::vec4> allLightData;
	extern std::vector<LightIndexRange> allLightIndexRanges;
	extern size_t lightVAO;
	extern size_t lightIndexVBO;
	extern size_t lightDataUBO;
	extern size_t lightShaderProgram;
	extern size_t MAX_LIGHT_COUNT;
    }
}