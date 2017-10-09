#pragma once
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <App\ContextWindow.h>

namespace gl {

        using std::string;
        using std::vector;

        
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
        void initGeneralUniformBuffer();
}
