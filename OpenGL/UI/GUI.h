#pragma once
#include <OpenGL\Global\gl.h>
#include <string>
namespace gl {
	namespace GUI {
		typedef unsigned int Quad;
		typedef unsigned int Pos;
		typedef unsigned int Size;
		typedef unsigned int Color;

		struct RefQuad{
			RefQuad():pos(-1), size(-1), color(-1){}
			RefQuad(Pos pPos, Size pSize, Color pColor)
				:pos(pPos), size(pSize), color(pColor){}

			Pos pos;
			Size size;
			Color color;
		};
		
		inline bool operator==(const RefQuad& aq, const RefQuad& bq) { return std::memcmp(&aq, &bq, sizeof(RefQuad)) == 0; }
		
		Quad createQuad(Pos pPos, Size pSize, Color pColor);
		Quad createQuad(glm::vec2 pPos, glm::vec2 pSize, glm::vec4 pColor);
		Quad createQuad(float pPosX, float pPosY, float pWidth, float pHeight, float pR, float pG, float pB, float pA);
		void reserveQuadSpace(unsigned int pCount);
		void clearBuffers();
		void initGUIBuffers();
		void updateGUI();
		void initGUIShaders();
		void renderGUI();
		extern unsigned int guiVAO;
		extern unsigned int MAX_GUI_QUADS;
		extern unsigned int guiQuadShader;
		extern unsigned int guiTexQuadShader;

		Pos createPos(glm::vec2 pPos);
		Pos createPos(float pX, float pY);
		Size createSize(glm::vec2 pSize);
		Size createSize(float pW, float pH);
		Color createColor(glm::vec4 pColor);
		Color createColor(float pR, float pG, float pB, float pA);

		void changePos	(Quad pQuad, const glm::vec2& pPos);
		void changeSize	(Quad pQuad, const glm::vec2& pSize);
		void changeColor(Quad pQuad, const glm::vec4& pColor);
		void offsetPos	(Quad pQuad, const glm::vec2& pPos);
		void offsetSize	(Quad pQuad, const glm::vec2& pSize);
		void offsetColor(Quad pQuad, const glm::vec4& pColor);
		
		
		extern std::vector<RefQuad> allQuads;
		extern unsigned int guiQuadBuffer;
		
		extern std::vector<glm::vec2> allPositions;
		extern unsigned int guiPositionBuffer;

		extern std::vector<glm::vec2> allSizes;
		extern unsigned int guiSizeBuffer;

		extern std::vector<glm::vec4> allColors;
		extern unsigned int guiColorBuffer;

	}
}