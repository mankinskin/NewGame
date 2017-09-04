#include "Global\stdafx.h"
#include "ContextWindow.h"
#include "Global\Debug.h"
#include <App\Global\App.h>
#include "edid.h"
int App::ContextWindow::monitorCount = 0;
GLFWmonitor** App::ContextWindow::allMonitors = nullptr;
App::ContextWindow::Monitor App::ContextWindow::primaryMonitor;


void App::ContextWindow::initMonitors()
{
	//find primary monitor for GLFW
	allMonitors = glfwGetMonitors(&monitorCount);
	if (monitorCount > 0) {
		primaryMonitor.monitor = allMonitors[0];
	}
	else {
		primaryMonitor.monitor = glfwGetPrimaryMonitor();
	}
	if (primaryMonitor.monitor == nullptr) {
	    App::Debug::pushError("GLFW could not find your (primary) monitor!", Debug::Error::Fatal);
	}
}

void App::ContextWindow::Monitor::init()
{
	//initialize monitor member variables
	vidModes = glfwGetVideoModes(monitor, &videoModeCount);
	currentVideoMode = glfwGetVideoMode(monitor);
	glfwGetMonitorPhysicalSize(monitor, &physical_width, &physical_height);
	pixels_x = currentVideoMode->width;
	pixels_y = currentVideoMode->height;
	dpi_x = (unsigned int)((float)pixels_x / (480.0f / 25.4f));
	dpi_y = (unsigned int)((float)pixels_y / (275.0f / 25.4f));
	glfwGetMonitorPos(monitor, &xpos, &ypos);
}

void App::ContextWindow::Window::setSize(unsigned int pWidth, unsigned int pHeight)
{
	//set the pixel width and height of the window
	width = pWidth;
	height = pHeight;
}

void App::ContextWindow::Window::init()
{
	//sets up GLFW Window with OpenGL context
	if (window != nullptr) {
#ifdef _DEBUG
		puts("\nDestroying current GLFW Window...");
#endif
		glfwDestroyWindow(window);
	}
#ifdef _DEBUG
	puts("\nCreating a GLFW Window...");
#endif


	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, 1);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_FOCUSED, 1);
	glfwWindowHint(GLFW_AUTO_ICONIFY, 1);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	//glfwWindowHint(GLFW_DEPTH_BITS, 24);
	//glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	glfwSetWindowPos(window, (primaryMonitor.currentVideoMode->width / 2) - width / 2, (primaryMonitor.currentVideoMode->height / 2) - height / 2);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, window);
}