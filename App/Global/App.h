#pragma once
#include "..\ContextWindow.h"


namespace App {

    enum State {
	Init,
	MainMenu,
	Running,
	Exit
    };

    extern State state;
    void init();
    void initGLFW();
    void frameLoop();
    void mainMenuLoop();
    void fetchInput();
    void run();
    void quit();
    void mainmenu();
    extern ContextWindow::Window mainWindow;

    extern double timeFactor;
    extern double lastFrameLimitedMS;
    extern double lastFrameMS;
    extern double totalMS;
    extern double targetFrameMS;
    void setTargetFPS(size_t pTargetFPS);
    void updateTime();
    void updateTimeFactor();
    void limitFPS();

    extern glm::vec3 light_pos;
    extern glm::vec3 light_mov;
}