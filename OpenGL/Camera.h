#pragma once
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
		glm::vec3& getPos();
		glm::mat4& getProjection();
		glm::mat4& getView();
	};
}