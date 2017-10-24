#include "../Global/stdafx.h"
#include "EntityRegistry.h"
#include <glm\gtc\matrix_transform.hpp>

std::vector<glm::mat4> EntityRegistry::allMatrices;
std::vector<glm::vec3> EntityRegistry::allPositions;
std::vector<glm::vec3> EntityRegistry::allScales;
size_t EntityRegistry::num_entities;

void EntityRegistry::initEntities()
{
    size_t entities[2];
    createEntities(2, &entities[0]);
    setPos(entities[0], glm::vec3(3.0f, -0.0f, 0.0f));
    setScale(entities[0], glm::vec3(0.2f, 0.2f, 0.2f));

    setPos(entities[1], glm::vec3(-3.0f, 0.0f, 0.0f));
    setScale(entities[1], glm::vec3(1.0f, 0.5f, 1.0f));
    updateMatrices();
}

void EntityRegistry::updateMatrices()
{
    for (size_t e = 0; e < num_entities; ++e) {
	allMatrices[e] = glm::translate(glm::mat4(), allPositions[e]);
	allMatrices[e] = glm::scale(allMatrices[e], allScales[e]);


    }
}

void EntityRegistry::createEntities(size_t pCount, size_t * pEntities)
{
    size_t beg = num_entities;
    for (size_t* pEnt = pEntities; num_entities < beg + pCount; ++pEnt) {
	newEntityID(*pEnt);
    }
    allMatrices.resize(num_entities);
    allPositions.resize(num_entities);
    allScales.resize(num_entities);
}

void EntityRegistry::newEntityID(size_t& pEntity)
{
    pEntity = num_entities++;;
}

void EntityRegistry::setScale(size_t pEntityID, glm::vec3 pScale)
{
    allScales[pEntityID] = pScale;
}

void EntityRegistry::setPos(size_t pEntityID, glm::vec3 pPos)
{
    allPositions[pEntityID] = pPos;
}
void EntityRegistry::translate(size_t pEntityID, glm::vec3 pPos)
{
    allPositions[pEntityID] += pPos;
}
