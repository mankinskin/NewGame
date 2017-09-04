#pragma once
#include <string>
#include <GLFW\glfw3.h>
namespace App {
	namespace ContextWindow {

		struct Monitor {
			//structure to wrap glfw monitor functionality
			void init();
			GLFWmonitor* monitor;
			int videoModeCount = 0;
			const GLFWvidmode* vidModes = nullptr;
			const GLFWvidmode* currentVideoMode = nullptr;
			int xpos, ypos = 0;
			unsigned int dpi_x;
			unsigned int dpi_y;
			int physical_width;
			int physical_height;
			int pixels_x;
			int pixels_y;
		};

		void initMonitors();
		extern Monitor primaryMonitor;
		extern int monitorCount;
		extern GLFWmonitor** allMonitors;

		struct Window {

			void setSize(unsigned int pWidth, unsigned int pHeight);
			void init();

			GLFWwindow* operator=(const Window& obj) {
				return window;
			}

			GLFWwindow* window = nullptr;
			unsigned int width = 1000;
			unsigned int height = 7000;
			std::string name = "GLFW/OpenGL Window";
			int fullscreen = 0;;
		};
	}
}