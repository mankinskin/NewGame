#pragma once

#include <glm\glm.hpp>
namespace gl {
	namespace Camera {
		void init();
		void setPosition(glm::vec3 pPos);
		void translateLocal(glm::vec3 pDir);
		void translateGlobal(glm::vec3 pDir);
		void translateLocal(float pX, float pY, float pZ);
		void look(glm::vec2 pDir);
		void lookAtCenter();
		void update();
		void forward();
		void back();
		void stop_z();
		void left();
		void right();
		void stop_x();
		void up();
		void down();
		void stop_y();
		void move(glm::vec3 pDir);
		extern float FOV;
		extern float sensitivity;
		extern float yRestrictionAngle;
		extern float cam_speed;
		extern float nearPlane;
		extern float farPlane;
		extern glm::vec3 pos;
		extern glm::vec3 normal;
		extern glm::vec3 lookAt;
		extern glm::vec3 LOOK_AT_CENTER;
		extern glm::vec3 cross_right;
		extern glm::vec3 UP;
		extern glm::vec2 lookRotation;
		extern glm::mat4 translationMatrix;
		extern glm::mat4 viewMatrix;
		extern glm::mat4 projectionMatrix;
		extern glm::mat4 infiniteProjectionMatrix;
		extern const float eulerian;
		extern int expect_x_stop;
		extern int expect_y_stop;
		extern int expect_z_stop;
	};
}