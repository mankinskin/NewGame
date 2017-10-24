#include "..\Global\stdafx.h"
#include "stdafx.h"
#include "Input.h"
#include "..\Global\App.h"
#include "..\Global\Debug.h"
#include <OpenGL\Camera.h>
#include <OpenGL\Global\glDebug.h>
#include <functional>
#include "Keys.h"
#include "Mouse.h"
#include <OpenGL\GUI\/UI/Buttons.h>
#include <algorithm>
#include "Event.h"

/*
button-pipeline
to set up
- bind quads as buttons
- calculateQuadRanges

every frame
- fetch Events
	- iterate through buttons
	- get button state and compare it to previous state
	- if different -> generate event
- check for relevant events/wheater callbacks should be called
	- iterate through all events
	- check buttonID-specific range in functor array
- callButtons
*/

void App::Input::init()
{
    puts("Initilizing GLFW Input...\n");
    //initializes GLFW input and defines the keys to track
    glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetKeyCallback(App::mainWindow.window, key_Callback);
    glfwSetCharCallback(App::mainWindow.window, char_Callback);
    //glfwSetCursorPosCallback(App::mainWindow.window, cursorPosition_Callback);
    glfwSetCursorEnterCallback(App::mainWindow.window, cursorEnter_Callback);
    glfwSetMouseButtonCallback(App::mainWindow.window, mouseKey_Callback);
    glfwSetScrollCallback(App::mainWindow.window, scroll_Callback);
}

void App::Input::fetchGLFWEvents()
{
    //updates the states of the mouse buttons, mouse wheel and all tracked keys
    glfwPollEvents();
}


void App::Input::end()
{
    resetMouse();
    if (glfwWindowShouldClose(App::mainWindow.window)) {
		App::state = App::State::Exit;
    }
}