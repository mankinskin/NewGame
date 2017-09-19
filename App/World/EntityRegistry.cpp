#include "../Global/stdafx.h"
#include "EntityRegistry.h"
#include <glm\gtc\matrix_transform.hpp>

std::vector<glm::mat4> EntityRegistry::allMatrices;
unsigned EntityRegistry::num_entities;

void EntityRegistry::initEntities()
{
	unsigned int first_entity = 0;
	createEntities(1, &first_entity);
	setPos(first_entity, glm::vec3( 1.0f, 1.0f, 1.0f));
}

void EntityRegistry::createEntities(unsigned int pCount, unsigned int * pEntities)
{
	unsigned int beg = num_entities;
	for (unsigned int* pEnt = pEntities; num_entities < beg + pCount; pEnt += sizeof(unsigned int)) {
		*pEnt = num_entities++;
	}
	allMatrices.resize(num_entities);
}

void EntityRegistry::setPos(unsigned int pEntityID, glm::vec3 pPos)
{
	std::memcpy(&allMatrices[pEntityID][3].x, &pPos, sizeof(glm::vec3));
}

void EntityRegistry::translate(unsigned int pEntityID, glm::vec3 pPos)
{
	allMatrices[pEntityID] = glm::translate(allMatrices[pEntityID], pPos);
}
