#pragma once
#include <vector>
#include <glm\glm.hpp>

namespace gl{
        namespace GUI{
                
		void moveGroup(unsigned int pGroupIndex, glm::vec2 pDir);
		unsigned int createGroup(unsigned int pGroupQuad);
		void addGroupQuad(unsigned int pGroupIndex, unsigned int pQuadIndex);
		void addGroupLine(unsigned int pGroupIndex, unsigned int pLineIndex);
		void updateGroups();
		
		struct QuadElement {
			QuadElement(unsigned int pQuadIndex, unsigned int pParentGroupIndex)
				:quad_index(pQuadIndex), parent_group(pParentGroupIndex) {}
			unsigned int quad_index;
			unsigned int parent_group;
		};
		struct LineElement {
			LineElement(unsigned int pLineIndex, unsigned int pParentGroupIndex)
				:line_index(pLineIndex), parent_group(pParentGroupIndex) {}
			unsigned int line_index;
			unsigned int parent_group;
		};
		extern std::vector<unsigned int> allGroupBounds;
		extern std::vector<glm::vec2> allGroupDeltas;
		extern std::vector<QuadElement> allQuadElements;
		extern std::vector<LineElement> allLineElements;
		
        }
}
