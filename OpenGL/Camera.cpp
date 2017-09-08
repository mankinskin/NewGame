#include "Global\stdafx.h"
#include "Camera.h"
#include "Global\gl.h"
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtx\rotate_vector.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <App\Global\App.h>
#include <math.h>
#include "Global\glDebug.h"
#include <algorithm>
glm::mat4 gl::Camera::viewMatrix = {};
glm::mat4 gl::Camera::projectionMatrix = {};
glm::mat4 gl::Camera::infiniteProjectionMatrix = {};
glm::vec3 gl::Camera::pos = {};
glm::vec3 gl::Camera::normal = {};
glm::vec3 gl::Camera::lookAt = {};
glm::vec3 gl::Camera::cross_right = {};
glm::vec3 gl::Camera::UP = {};
glm::vec2 gl::Camera::lookRotation = {};
glm::mat4 gl::Camera::translationMatrix = {};
glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 mov = glm::vec3(0.0f, 0.0f, 0.0f);
float gl::Camera::cam_speed = 0.5f;
float gl::Camera::FOV = 70.0f;
float gl::Camera::sensitivity = 0.5f;
float gl::Camera::yRestrictionAngle = 3.0f;
float gl::Camera::nearPlane = 0.0f;
float gl::Camera::farPlane = 10.0f;
const float gl::Camera::eulerian = 0.0001f;
glm::vec3 gl::Camera::LOOK_AT_CENTER = glm::vec3(0.0, -1.0, 0.0);
int gl::Camera::expect_x_stop = 0;
int gl::Camera::expect_y_stop = 0;
int gl::Camera::expect_z_stop = 0;


void gl::Camera::init()
{
	FOV = 70.0f;
	pos = glm::vec3(0.0, 2.0, 0.0);
	UP = glm::vec3(0.0, 0.0, -1.0);
	lookAt = LOOK_AT_CENTER;
	normal = glm::vec3(0.0f, 0.0f, -1.0f);
	cross_right = glm::cross(normal, UP );
	
	nearPlane = 0.01f;
	farPlane = 1000.0f;
	viewMatrix = glm::lookAt(pos, lookAt, normal);
	projectionMatrix = glm::perspective(FOV, (float)(App::mainWindow.width*resolution) / (float)(App::mainWindow.height*resolution), nearPlane, farPlane);
	infiniteProjectionMatrix = projectionMatrix;
	infiniteProjectionMatrix[2][2] = eulerian - 1.0f;
	infiniteProjectionMatrix[3][2] = nearPlane*(eulerian - 2.0f);

	Debug::getGLError("Camera::init()");
}

void gl::Camera::setPosition(glm::vec3 pPos)
{
	pos = pPos;
}

void gl::Camera::translateLocal(glm::vec3 pDir)
{
	glm::vec3 d = ((cross_right * pDir.x) + (-lookAt * pDir.y) + (-normal * pDir.z))*cam_speed * (float)App::timeFactor;
	translationMatrix = glm::translate(translationMatrix, d);
	pos += d;
}

void gl::Camera::translateGlobal(glm::vec3 pDir)
{
	pos += pDir * cam_speed * (float)App::timeFactor;
}

void gl::Camera::translateLocal(float pX, float pY, float pZ)
{
	translateLocal(glm::vec3(pX, pY, pZ));
}

void gl::Camera::look(glm::vec2 pDir)
{
	if (pDir != glm::vec2()) {
		pDir = pDir * sensitivity;

		
		//lookAt = glm::rotate(lookAt, pDir.y, cross_right);
		normal = glm::rotate(normal, pDir.x, lookAt);
		//lookAt.y = std::max(std::min(lookAt.y, 0.9f), -0.9f);
		//lookAt = glm::normalize(lookAt);
	}
}

void gl::Camera::lookAtCenter()
{
	lookAt = LOOK_AT_CENTER;
}

void gl::Camera::update()
{
	//update camera matrices
	cross_right = glm::normalize(glm::cross(lookAt, normal));
	normal = glm::normalize(glm::cross(cross_right, lookAt));
	vel = vel + mov;
	if (mov != glm::vec3()) {
		translateLocal(normalize(mov));
	}
	
	
	viewMatrix = glm::lookAt(pos, pos + lookAt, normal);
}

void gl::Camera::forward()
{
	expect_z_stop = !expect_z_stop;
	move(glm::vec3(0.0f, 0.0f, -1.0f));
}

void gl::Camera::back()
{
	expect_z_stop = !expect_z_stop;
	move(glm::vec3(0.0f, 0.0f, 1.0f));
}

void gl::Camera::stop_z() 
{
	expect_z_stop = 0;
	mov.z = 0.0f;
}

void gl::Camera::left()
{
	expect_x_stop = !expect_x_stop;
	move(glm::vec3(-1.0f, 0.0f, 0.0f));
}

void gl::Camera::right()
{
	expect_x_stop = !expect_x_stop;
	move(glm::vec3(1.0f, 0.0f, 0.0f));
}

void gl::Camera::stop_x() {
	expect_x_stop = 0;
	mov.x = 0.0f;
}

void gl::Camera::up()
{
	expect_y_stop = !expect_y_stop;
	move(glm::vec3(0.0f, 1.0f, 0.0f));
}

void gl::Camera::down()
{
	expect_y_stop = !expect_y_stop;
	move(glm::vec3(0.0f, -1.0f, 0.0f));
}

void gl::Camera::stop_y() {
	expect_y_stop = 0;
	mov.y = 0.0f;
}

void gl::Camera::move(glm::vec3 pDir) {
	mov = mov + pDir;
}
