#pragma once
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <App\ContextWindow.h>

namespace gl {

        using std::string;
        using std::vector;

        struct Index {
                Index(int i) {
                        index = i;
                }
                Index(int pIndex, unsigned int pID) {
                        index = pIndex;
                        ID = pID;
                }
                Index() {
                        index = -1;
                        ID = 0;
                }
                int index = -1;
                unsigned int ID = 0;

                Index operator++() {
                        ++index;
                        return *this;
                }
                Index operator++(int) {
                        Index ind(index);
                        ++index;
                        return ind;
                }
                Index operator--() {
                        --index;
                        return *this;
                }
                Index operator--(int) {
                        Index ind(index);
                        --index;
                        return ind;
                }
                void operator=(const int &i) {
                        index = i;
                }
                bool operator<(const int &i) {
                        return index < i ? true : false;
                }
                bool operator>(const int &i) {
                        return index > i ? true : false;
                }
                bool operator==(const int &i) {
                        return index == i ? true : false;
                }
                bool operator<=(const int &i) {
                        return index <= i ? true : false;
                }
                bool operator>=(const int &i) {
                        return index >= i ? true : false;
                }

        };

        struct Vertex {
                Vertex() {}
                Vertex(float pPosX, float pPosY, float pPosZ, float pNormX, float pNormY, float pNormZ, float pUVCoordS, float pUVCoordT)
                        :pos(glm::vec3(pPosX, pPosY, pPosZ)), normal(glm::vec3(pNormX, pNormY, pNormZ)), uv(glm::vec2(pUVCoordS, pUVCoordT)) {}
                Vertex(glm::vec3 pPos, glm::vec3 pNormal, glm::vec2 pUV)
                        :pos(pPos), normal(pNormal), uv(pUV) {}
                glm::vec3 pos;
                glm::vec3 normal;
                glm::vec2 uv;
        };

        struct LineVertex {
                LineVertex() {
                        position = glm::vec4();
                        color = glm::vec4();
                }
                LineVertex(float pX, float pY, float pZ, float pW, float pR, float pG, float pB, float pA) {
                        position = glm::vec4(pX, pY, pZ, pW);
                        color = glm::vec4(pR, pG, pB, pA);
                }
                LineVertex(glm::vec4 pPos, glm::vec4 pColor) {
                        position = pPos;
                        color = pColor;
                }
                glm::vec4 position;
                glm::vec4 color;
        };


        extern int MAX_WORK_GROUP_COUNT;
        extern int MAX_TEXTURE_UNIT_COUNT;
        extern glm::ivec3 MAX_WORK_GROUP_SIZE;
        extern unsigned int MAX_LIGHT_COUNT;
        extern vector<string> EXTENSION_LIST;
        extern int EXTENSIONS_SUPPORTED_NUM;
        extern int OPENGL_VERSION[2];
        extern string GLSL_VERSION;
        extern string SYSTEM_RENDERER;
        extern float resolution;
        extern unsigned int screenWidth;
        extern unsigned int screenHeight;
        extern unsigned int mergeComputeShader;
        extern unsigned int finalMergeShader;
        extern unsigned int mainFrame;
        extern unsigned int mainFBO;
        extern unsigned int quadVBO; 
        extern unsigned int quadEBO;
        extern unsigned int generalUniformBuffer;
        extern unsigned int screenShaderProgram;
        extern unsigned int screenQuadVAO;

        void init();
        void initFramebuffers();
        void initShaders();
        void initGeneralBuffers();
        void initGUI();
        void initModels();
        void initLighting();
        void bindUniformBufferLocations();
        void setViewport(App::ContextWindow::Window& pViewport);
        void getOpenGLInitValues();
        void initGLEW();
        void initGeneralQuadVBO();
        void updateGeneralUniformBuffer();
        void loadModels();
	void initScreenShader();
	void initScreenVAO();
	void renderFrame();
        void initGeneralUniformBuffer();
}
