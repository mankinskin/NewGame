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

gl::GUI::DynamicColor::DynamicColor(glm::vec4 pColor)
	:colorIndex(allDynamicColors.size())
{
	allDynamicColors.push_back(pColor);
}

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
void gl::GUI::renderColorQuadTemplates<gl::GUI::DynamicColor>() {
	
	glBindVertexArray(ColorQuadTemplate<DynamicColor>::VAO);
	Shader::use(ColorQuadTemplate<DynamicColor>::shader);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<DynamicColor>::quads.size());
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
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::DynamicColor>() {
	
	Shader::bindUniformBufferToShader(ColorQuadTemplate<DynamicColor>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<DynamicColor>::shader, dynamicColorBuffer, "DynamicColorBuffer");
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
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::DynamicColor>() {

	ColorQuadTemplate<DynamicColor>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<DynamicColor>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<DynamicColor>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<DynamicColor>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<DynamicColor>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<DynamicColor>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<DynamicColor>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<DynamicColor>::VAO, 1, ColorQuadTemplate<DynamicColor>::elementBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(ColorQuadTemplate<DynamicColor>::VAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<DynamicColor>::VAO, 1, 1);
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
void gl::GUI::initColorQuadTemplateShader<gl::GUI::DynamicColor>() {
	ColorQuadTemplate<DynamicColor>::shader = Shader::newProgram("dynamicColorQuadShader", Shader::createModule("dynamicColorQuadShader.vert"), Shader::createModule("dynamicColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<DynamicColor>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<DynamicColor>::shader, "colored_quad", 1);
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
void gl::GUI::updateColorQuadTemplates<gl::GUI::DynamicColor>()
{
	if (ColorQuadTemplate<DynamicColor>::quads.size()) {
		VAO::streamStorage(ColorQuadTemplate<DynamicColor>::elementBuffer, sizeof(ColorQuad<DynamicColor>)*ColorQuadTemplate<DynamicColor>::quads.size(), &ColorQuadTemplate<DynamicColor>::quads[0]);
		VAO::streamStorage(dynamicColorBuffer, sizeof(glm::vec4)*allDynamicColors.size(), &allDynamicColors[0]);
	}
}
//these functions initialize all templates of Colored_Quads
void gl::GUI::initColorQuadVAOs()
{
	initColorQuadTemplateVAO<ConstColor>();
	initColorQuadTemplateVAO<DynamicColor>();
}

void gl::GUI::initColorQuadShaders()
{
	initColorQuadTemplateShader<ConstColor>();
	initColorQuadTemplateShader<DynamicColor>();
}

void gl::GUI::setupColorQuadShaders()
{
	setupColorQuadTemplateShader<ConstColor>();
	setupColorQuadTemplateShader<DynamicColor>();
}

void gl::GUI::updateColorQuads()
{
	updateColorQuadTemplates<ConstColor>();
	updateColorQuadTemplates<DynamicColor>();
}

void gl::GUI::renderColorQuads()
{
	renderColorQuadTemplates<ConstColor>();
	renderColorQuadTemplates<DynamicColor>();
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

glm::vec4 & gl::GUI::getDynamicColor(DynamicColor pColor)
{
	return allDynamicColors[pColor.colorIndex];
}
