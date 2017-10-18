#include "../Global/stdafx.h"
#include "stdafx.h"
#include "Mouse.h"
#include "..\Global\App.h"
#include "..\ContextWindow.h"
#include <OpenGL\GUI\UI\Buttons.h>
#include <algorithm>
#include "Event.h"
glm::vec2 App::Input::relativeCursorPosition;
glm::uvec2 App::Input::absoluteCursorPosition;
glm::vec2 App::Input::cursorFrameDelta;

App::Input::KeyCondition mouseKeys[3];
int scroll = 0;
int disableCursor = 0;
size_t hovered_button = 0;
size_t last_hovered_button = 0;

void App::Input::updateCursor() {
	//update cursor pos
	double ax = 0.0;//absolute positions
	double ay = 0.0;
	
	//GLFW window coordinates are from top to bottom all others are bottom to top
	glfwGetCursorPos(App::mainWindow.window, &ax, &ay);
	float rx = (float)ax;
	float ry = (float)ay;
	ax = std::clamp(ax, 0.0, (double)mainWindow.width - 1.0);
	ay = std::clamp(ay, 0.0, (double)mainWindow.height - 1.0);
	if (!disableCursor) {
		rx = (float)ax;//clamp relative positions too if cursor is not disabled
		ry = (float)ay;
	}
	absoluteCursorPosition = glm::uvec2((unsigned int)ax, mainWindow.height - (unsigned int)ay - 1);

	rx = ((rx / (float)App::mainWindow.width)*2.0f) - 1.0f;
	ry = 1.0f - (ry / (float)App::mainWindow.height)*2.0f;
	glm::vec2 newRelativeCursorPosition = glm::vec2(rx, ry);
	cursorFrameDelta = glm::vec2((newRelativeCursorPosition.x - relativeCursorPosition.x),
		(newRelativeCursorPosition.y - relativeCursorPosition.y));
	relativeCursorPosition = newRelativeCursorPosition;

	//printf("%f\n%f\n\n\n", pX, pY);
	//printf("%i\n%i\n\n\n", absoluteCursorPosition.x, absoluteCursorPosition.y);
	
}

void App::Input::resetMouse()
{
	scroll = 0;
}

void App::Input::toggleCursor()
{
	disableCursor = !disableCursor;
	if (disableCursor) {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(App::mainWindow.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void App::Input::getCursorEvents()
{
	last_hovered_button = hovered_button;
	hovered_button = gl::GUI::readButtonIndexMap(absoluteCursorPosition.x, absoluteCursorPosition.y);
	if(last_hovered_button != hovered_button){
		if (last_hovered_button) {
			//button leave event
			pushEvent(CursorEvent(last_hovered_button - 1, 0));
		}
		if (hovered_button) {
			//button enter event
			pushEvent(CursorEvent(hovered_button - 1, 1));
		}
	}
}

void App::Input::mouseKey_Callback(GLFWwindow * window, int pKey, int pAction, int pMods)
{
	MouseKeyEvent mkev(pKey, pAction, pMods);
	pushEvent(mkev);
	if (hovered_button) {
		pushEvent(MouseEvent(hovered_button - 1, mkev));
	}
}

void App::Input::cursorPosition_Callback(GLFWwindow * window, double pX, double pY)
{

}
void App::Input::scroll_Callback(GLFWwindow * window, double pX, double pY)
{

}
void App::Input::cursorEnter_Callback(GLFWwindow* window, int pEntered)
{
	//Entered = 1 if entered, on exit = 0
}
