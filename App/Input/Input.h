#pragma once
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <utility>
#include <initializer_list>
namespace App {
	namespace Input {
		void init();
		void fetchGLFWEvents();
		void end();
	}
}
