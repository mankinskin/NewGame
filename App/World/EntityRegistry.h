#pragma once
#include <vector>
#include <glm\glm.hpp>

//an entity is a world object. 
namespace EntityRegistry {
	void initEntities();
	void createEntities(unsigned int pCount, unsigned int* pEntities);
	void setPos(unsigned int pEntityID, glm::vec3 pPos);
	void translate(unsigned int pEntityID, glm::vec3 pPos);
	const unsigned int MAX_ENTITIES = 10000;
	extern unsigned int num_entities;
	extern std::vector<glm::vec3> allPositions;
	extern std::vector<glm::mat4> allMatrices;
}