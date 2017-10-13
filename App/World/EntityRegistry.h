#pragma once
#include <vector>
#include <glm\glm.hpp>

//an entity is a world object. 
namespace EntityRegistry {

    void initEntities();
    void updateMatrices();
    void createEntities(size_t pCount, size_t* pEntities);
    void newEntityID(size_t & pEntity);

    void setScale(size_t pEntityID, glm::vec3 pScale);
    void setPos(size_t pEntityID, glm::vec3 pPos);
    void translate(size_t pEntityID, glm::vec3 pPos);

    const size_t MAX_ENTITIES = 10000;
    extern size_t num_entities;
    extern std::vector<glm::vec3> allPositions;
    extern std::vector<glm::vec3> allScales;
    extern std::vector<glm::mat4> allMatrices;
}