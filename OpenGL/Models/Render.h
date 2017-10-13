#pragma once
#include <glm\glm.hpp>
#include <vector>

namespace gl {
    namespace Models {
	void initMeshVAO();
	void initMeshShader();
	void fillMeshVAO();
	void storeMaterials();
	void render();
	void initNormalShader();
	void renderNormals();
	void updateBuffers();
    }
}
