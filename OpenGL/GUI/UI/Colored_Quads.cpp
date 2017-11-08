#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "GUI.h"
#include "Colored_Quads.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/Shader/Shader.h"
#include "../../Global/gl.h"
#include "../../Global/glDebug.h"
#include "../../BaseGL/Texture.h"

const size_t MAX_DYNAMIC_COLOR_COUNT = 10;
size_t guiTextureAtlas;
std::vector<glm::vec4> allAtlasUVs;
std::vector<glm::vec4> allDynamicColors;
size_t uvBuffer = 0;
size_t dynamicColorBuffer = 0;
template<class ColorPolicy>
std::vector<gl::GUI::ColorQuad<ColorPolicy>> gl::GUI::ColorQuadTemplate<ColorPolicy>::quads;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::elementBuffer;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::VAO;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::shader;

gl::GUI::Color::Color(glm::vec4 pColor)
	:color(pColor)
{}

gl::GUI::Color::Color(size_t pColorIndex)
	: color(allDynamicColors[pColorIndex])
{}

template<class ColorPolicy>
void gl::GUI::renderColorQuadTemplates() 
{}
template<>
void gl::GUI::renderColorQuadTemplates<gl::GUI::ConstColor>() {
	glBindVertexArray(ColorQuadTemplate<ConstColor>::VAO);
	Shader::use(ColorQuadTemplate<ConstColor>::shader);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<ConstColor>::quads.size());

	Shader::unuse();
	glBindVertexArray(0);
}
template<>
void gl::GUI::renderColorQuadTemplates<gl::GUI::Color>() {
	
	glBindVertexArray(ColorQuadTemplate<Color>::VAO);
	Shader::use(ColorQuadTemplate<Color>::shader);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<Color>::quads.size());
	glBindTexture(GL_TEXTURE_2D, 0);
	Shader::unuse();
	glBindVertexArray(0);
}

template<class ColorPolicy>
void gl::GUI::setupColorQuadTemplateShader() {}
template<>
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::ConstColor>() {
	Shader::bindUniformBufferToShader(ColorQuadTemplate<ConstColor>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<ConstColor>::shader, GUI::colorBuffer, "ColorBuffer");
	gl::Debug::getGLError("gl::setupColorQuadTemplateShader<PlainColor>()");
}
template<>
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::Color>() {
	
	Shader::bindUniformBufferToShader(ColorQuadTemplate<Color>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<Color>::shader, dynamicColorBuffer, "DynamicColorBuffer");
	gl::Debug::getGLError("gl::setupColorQuadTemplateShader<DynamicColor>()");
}
template<class ColorPolicy>
void gl::GUI::initColorQuadTemplateVAO() {}

template<>
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::ConstColor>() {

	ColorQuadTemplate<ConstColor>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<ConstColor>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<ConstColor>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<ConstColor>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<ConstColor>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<ConstColor>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<ConstColor>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<ConstColor>::VAO, 1, ColorQuadTemplate<ConstColor>::elementBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(ColorQuadTemplate<ConstColor>::VAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<ConstColor>::VAO, 1, 1);
}

template<>
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::Color>() {

	ColorQuadTemplate<Color>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<Color>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<Color>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<Color>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<Color>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<Color>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<Color>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<Color>::VAO, 1, ColorQuadTemplate<Color>::elementBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(ColorQuadTemplate<Color>::VAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<Color>::VAO, 1, 1);
	dynamicColorBuffer = VAO::createStorage(sizeof(glm::vec4)*MAX_DYNAMIC_COLOR_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(dynamicColorBuffer, GL_MAP_WRITE_BIT);
	VAO::bindStorage(GL_UNIFORM_BUFFER, dynamicColorBuffer);
	
}
template<class ColorPolicy>
void gl::GUI::initColorQuadTemplateShader() {}
template<>
void gl::GUI::initColorQuadTemplateShader<gl::GUI::ConstColor>() {
	ColorQuadTemplate<ConstColor>::shader = Shader::newProgram("plainColorQuadShader", Shader::createModule("plainColorQuadShader.vert"), Shader::createModule("plainColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<ConstColor>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<ConstColor>::shader, "colored_quad", 1);
	gl::Debug::getGLError("gl::initColorQuadTemplateShader<PlainColor>()");
}
template<>
void gl::GUI::initColorQuadTemplateShader<gl::GUI::Color>() {
	ColorQuadTemplate<Color>::shader = Shader::newProgram("dynamicColorQuadShader", Shader::createModule("dynamicColorQuadShader.vert"), Shader::createModule("dynamicColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<Color>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<Color>::shader, "colored_quad", 1);
	gl::Debug::getGLError("gl::initColorQuadTemplateShader<DynamicColor>()");
}
template<class ColorPolicy>
void gl::GUI::updateColorQuadTemplates()
{
	if (ColorQuadTemplate<ColorPolicy>::quads.size()) {
		VAO::streamStorage(ColorQuadTemplate<ColorPolicy>::elementBuffer, sizeof(ColorQuad<ColorPolicy>)*ColorQuadTemplate<ColorPolicy>::quads.size(), &ColorQuadTemplate<ColorPolicy>::quads[0]);
	}
}
template<>
void gl::GUI::updateColorQuadTemplates<gl::GUI::Color>()
{
	if (ColorQuadTemplate<Color>::quads.size()) {
		VAO::streamStorage(ColorQuadTemplate<Color>::elementBuffer, sizeof(ColorQuad<Color>)*ColorQuadTemplate<Color>::quads.size(), &ColorQuadTemplate<Color>::quads[0]);
		VAO::streamStorage(dynamicColorBuffer, sizeof(glm::vec4)*allDynamicColors.size(), &allDynamicColors[0]);
	}
}
//these functions initialize all templates of Colored_Quads
void gl::GUI::initColorQuadVAOs()
{
	initColorQuadTemplateVAO<ConstColor>();
	initColorQuadTemplateVAO<Color>();
}

void gl::GUI::initColorQuadShaders()
{
	initColorQuadTemplateShader<ConstColor>();
	initColorQuadTemplateShader<Color>();
}

void gl::GUI::setupColorQuadShaders()
{
	setupColorQuadTemplateShader<ConstColor>();
	setupColorQuadTemplateShader<Color>();
}

void gl::GUI::updateColorQuads()
{
	updateColorQuadTemplates<ConstColor>();
	updateColorQuadTemplates<Color>();
}

void gl::GUI::renderColorQuads()
{
	renderColorQuadTemplates<ConstColor>();
	renderColorQuadTemplates<Color>();
}

void gl::GUI::uploadUVRanges()
{
	VAO::initStorageData(uvBuffer, sizeof(glm::vec4)*MAX_QUAD_COUNT, &allAtlasUVs[0], 0);
}


size_t gl::GUI::createAtlasUVRange(glm::vec4 pUVRange)
{
	allAtlasUVs.push_back(pUVRange);
	return allAtlasUVs.size() - 1;
}

glm::vec4 & gl::GUI::getDynamicColor(size_t pIndex)
{
	return allDynamicColors[pIndex];
}

