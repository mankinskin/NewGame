#include "../../Global/stdafx.h"
#include "stdafx.h"
#include "GUI.h"
#include "Colored_Quads.h"
#include "../../BaseGL/VAO.h"
#include "../../BaseGL/Shader/Shader.h"
#include "../../Global/gl.h"
#include "../../Global/glDebug.h"
#include "../../BaseGL/Texture.h"

size_t guiTextureAtlas;
std::vector<glm::vec4> allAtlasUVs;
size_t uvBuffer;
template<class ColorPolicy>
std::vector<gl::GUI::ColorQuad<ColorPolicy>> gl::GUI::ColorQuadTemplate<ColorPolicy>::quads;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::elementBuffer;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::VAO;
template<class ColorPolicy>
size_t gl::GUI::ColorQuadTemplate<ColorPolicy>::shader;



template<class ColorPolicy>
void gl::GUI::renderColorQuadTemplates() {
}
template<>
void gl::GUI::renderColorQuadTemplates<gl::GUI::PlainColor>() {
	glBindVertexArray(ColorQuadTemplate<PlainColor>::VAO);
	Shader::use(ColorQuadTemplate<PlainColor>::shader);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<PlainColor>::quads.size());

	Shader::unuse();
	glBindVertexArray(0);
}
template<>
void gl::GUI::renderColorQuadTemplates<gl::GUI::MultiColor>() {
	glBindVertexArray(ColorQuadTemplate<MultiColor>::VAO);
	Shader::use(ColorQuadTemplate<MultiColor>::shader);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<MultiColor>::quads.size());

	Shader::unuse();
	glBindVertexArray(0);
}
template<>
void gl::GUI::renderColorQuadTemplates<gl::GUI::TextureColor>() {
	auto aref = 0.8f;
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, aref);
	glBindVertexArray(ColorQuadTemplate<TextureColor>::VAO);
	Shader::use(ColorQuadTemplate<TextureColor>::shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, guiTextureAtlas);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, ColorQuadTemplate<TextureColor>::quads.size());
	glBindTexture(GL_TEXTURE_2D, 0);
	Shader::unuse();
	glBindVertexArray(0);
	glDisable(GL_ALPHA_TEST);
}


template<class ColorPolicy>
void gl::GUI::setupColorQuadTemplateShader() {}
template<>
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::PlainColor>() {
	Shader::bindUniformBufferToShader(ColorQuadTemplate<PlainColor>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<PlainColor>::shader, GUI::colorBuffer, "ColorBuffer");
	gl::Debug::getGLError("gl::setupColorQuadTemplateShader<PlainColor>()");
}
template<>
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::MultiColor>() {
	Shader::bindUniformBufferToShader(ColorQuadTemplate<MultiColor>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<MultiColor>::shader, GUI::colorBuffer, "ColorBuffer");
	gl::Debug::getGLError("gl::setupColorQuadTemplateShader<MultiColor>()");
}
template<>
void gl::GUI::setupColorQuadTemplateShader<gl::GUI::TextureColor>() {
	Shader::bindUniformBufferToShader(ColorQuadTemplate<TextureColor>::shader, GUI::quadBuffer, "QuadBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<TextureColor>::shader, uvBuffer, "UVBuffer");
	Shader::bindUniformBufferToShader(ColorQuadTemplate<TextureColor>::shader, GUI::colorBuffer, "ColorBuffer");
	gl::Debug::getGLError("gl::setupColorQuadTemplateShader<TextureColor>()");
}

template<class ColorPolicy>
void gl::GUI::initColorQuadTemplateVAO() {

}

template<>
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::PlainColor>() {

	ColorQuadTemplate<PlainColor>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<PlainColor>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<PlainColor>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<PlainColor>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<PlainColor>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<PlainColor>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<PlainColor>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<PlainColor>::VAO, 1, ColorQuadTemplate<PlainColor>::elementBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(ColorQuadTemplate<PlainColor>::VAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<PlainColor>::VAO, 1, 1);
}
template<>
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::MultiColor>() {

	ColorQuadTemplate<MultiColor>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<MultiColor>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<MultiColor>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<MultiColor>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<MultiColor>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<MultiColor>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<MultiColor>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<MultiColor>::VAO, 1, ColorQuadTemplate<MultiColor>::elementBuffer, sizeof(glm::uvec2));
	VAO::setVertexAttrib(ColorQuadTemplate<MultiColor>::VAO, 1, 1, 2, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<MultiColor>::VAO, 1, 1);
}
template<>
void gl::GUI::initColorQuadTemplateVAO<gl::GUI::TextureColor>() {

	ColorQuadTemplate<TextureColor>::elementBuffer = VAO::createStorage(sizeof(ColorQuad<TextureColor>)*MAX_QUAD_COUNT, 0, GL_MAP_WRITE_BIT | VAO::STREAM_FLAGS);
	VAO::createStream(ColorQuadTemplate<TextureColor>::elementBuffer, GL_MAP_WRITE_BIT);

	glCreateVertexArrays(1, &ColorQuadTemplate<TextureColor>::VAO);
	glVertexArrayElementBuffer(ColorQuadTemplate<TextureColor>::VAO, quadEBO + 1);
	glVertexArrayVertexBuffer(ColorQuadTemplate<TextureColor>::VAO, 0, quadVBO + 1, 0, sizeof(glm::vec2));
	VAO::setVertexAttrib(ColorQuadTemplate<TextureColor>::VAO, 0, 0, 2, GL_FLOAT, 0);

	VAO::setVertexArrayVertexStorage(ColorQuadTemplate<TextureColor>::VAO, 1, ColorQuadTemplate<TextureColor>::elementBuffer, sizeof(glm::uvec3));
	VAO::setVertexAttrib(ColorQuadTemplate<TextureColor>::VAO, 1, 1, 3, GL_UNSIGNED_INT, 0);
	glVertexArrayBindingDivisor(ColorQuadTemplate<TextureColor>::VAO, 1, 1);
	uvBuffer = VAO::createStorage();
	VAO::bindStorage(GL_UNIFORM_BUFFER, uvBuffer);
}
template<class ColorPolicy>
void gl::GUI::initColorQuadTemplateShader() {}
template<>
void gl::GUI::initColorQuadTemplateShader<gl::GUI::PlainColor>() {
	ColorQuadTemplate<PlainColor>::shader = Shader::newProgram("plainColorQuadShader", Shader::createModule("plainColorQuadShader.vert"), Shader::createModule("plainColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<PlainColor>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<PlainColor>::shader, "colored_quad", 1);
	gl::Debug::getGLError("gl::initColorQuadTemplateShader<PlainColor>()");
}
template<>
void gl::GUI::initColorQuadTemplateShader<gl::GUI::MultiColor>() {
	ColorQuadTemplate<MultiColor>::shader = Shader::newProgram("multiColorQuadShader", Shader::createModule("multiColorQuadShader.vert"), Shader::createModule("multiColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<MultiColor>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<MultiColor>::shader, "colored_quad", 1);
	gl::Debug::getGLError("gl::initColorQuadTemplateShader<MultiColor>()");
}
template<>
void gl::GUI::initColorQuadTemplateShader<gl::GUI::TextureColor>() {
	ColorQuadTemplate<TextureColor>::shader = Shader::newProgram("textureColorQuadShader", Shader::createModule("textureColorQuadShader.vert"), Shader::createModule("textureColorQuadShader.frag"));
	Shader::addVertexAttribute(ColorQuadTemplate<TextureColor>::shader, "corner_pos", 0);
	Shader::addVertexAttribute(ColorQuadTemplate<TextureColor>::shader, "colored_quad", 1);
	gl::Debug::getGLError("gl::initColorQuadTemplateShader<MultiColor>()");
	
}
template<class ColorPolicy>
void gl::GUI::updateColorQuadTemplates()
{
	if (ColorQuadTemplate<ColorPolicy>::quads.size()) {
		VAO::streamStorage(ColorQuadTemplate<ColorPolicy>::elementBuffer, sizeof(ColorQuad<ColorPolicy>)*ColorQuadTemplate<ColorPolicy>::quads.size(), &ColorQuadTemplate<ColorPolicy>::quads[0]);
	}
}

//these functions initialize all templates of Colored_Quads
void gl::GUI::initColorQuadVAOs()
{
	initColorQuadTemplateVAO<PlainColor>();
}

void gl::GUI::initColorQuadShaders()
{
	initColorQuadTemplateShader<PlainColor>();
}

void gl::GUI::setupColorQuadShaders()
{
	setupColorQuadTemplateShader<PlainColor>();
}

void gl::GUI::updateColorQuads()
{
	updateColorQuadTemplates<PlainColor>();
}

void gl::GUI::renderColorQuads()
{
	renderColorQuadTemplates<PlainColor>();;
}

void gl::GUI::uploadUVBuffer()
{
	//TODO: 
	// - include textures to use in gui
	// - pack them into an atlas
	// - 
	guiTextureAtlas = Texture::createTexture2D("gui_flat_edges.png");
	Texture::setTextureFilter(guiTextureAtlas, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	guiTextureAtlas = Texture::get2DTextureID(guiTextureAtlas);
	VAO::initStorageData(uvBuffer, sizeof(glm::vec4)*MAX_QUAD_COUNT, &allAtlasUVs[0], 0);
	
}

size_t gl::GUI::createAtlasUVRange(glm::vec4 pUVRange)
{
	allAtlasUVs.push_back(pUVRange);
	return allAtlasUVs.size() - 1;
}
