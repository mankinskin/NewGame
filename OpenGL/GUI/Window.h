#pragma once
#include <vector>

#include <glm\glm.hpp>
namespace gl{
        namespace GUI{
                
		void moveGroup(unsigned int pGroupIndex, glm::vec2 pDir);
		unsigned int createGroup(unsigned int pGroupQuad);
		void addQuadsToGroup(unsigned int pGroupIndex, std::initializer_list<unsigned int> pQuadIndices);
		void addLinesToGroup(unsigned int pGroupIndex, std::initializer_list<unsigned int> pLineIndices);
		void updateQuadGroupQuads();
		struct Group {
			Group(unsigned pGroupQuad)
				:group_quad(pGroupQuad), quadOffset(0), quadCount(0), lineOffset(0), lineCount(0) {}
			unsigned int group_quad;
			unsigned int quadOffset;
			unsigned int quadCount;
			unsigned int lineOffset;
			unsigned int lineCount;
		};
		extern std::vector<Group> allGroups;
		extern std::vector<glm::vec2> allGroupDeltas;
		extern std::vector<unsigned int> allGroupQuadIndices;
		extern std::vector<unsigned int> allGroupLineIndices;
		
        }
}
