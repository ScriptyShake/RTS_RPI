#include "DebugDrawer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"

#include <GLES3/gl3.h>
#include <glm/gtc/type_ptr.hpp>

#pragma GCC diagnostic pop

#include "camera.h"
#include "shaders.h"


Camera* Tmpl9::DebugDrawer::camera = nullptr;

Tmpl9::DebugDrawer::DebugDrawer()
{
	debugShader = new Shader2("Shaders/DebugVertex.glsl", "Shaders/DebugFragment.glsl");

	// Generate buffers / Vertex Arrays
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	setDebugMode(DBG_DrawWireframe);
}

void Tmpl9::DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	// draws a simple line of pixels between points.

	const GLfloat vertices[] = {
		from.x(), from.y(), from.z(),
		to.x(), to.y(), to.z()
	};

	// Bind buffer and fill it with data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr);
	glEnableVertexAttribArray(0);


	// Set color uniform in fragment shader
	int vertexColorLocation = glGetUniformLocation(debugShader->ID, "color");
	debugShader->use();
	glUniform4f(vertexColorLocation, color[0], color[1], color[2], 1.0f);

	// set view uniform in vertex shader
	glm::mat4 view = camera->GetViewMatrix();
	debugShader->setMat4("view", view);

	// set projection uniform in vertex shader
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), 1.0f / 1.0f,
	                                        0.1f, 100.0f);
	debugShader->setMat4("projection", projection);

	// Bind vertex array and draw line
	glDrawArrays(GL_LINES, 0, 2);


	// Bind buffer & bind vertex array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Tmpl9::DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance,
                                          int lifeTime, const btVector3& color)
{
	// draws a line between two contact points
	const btVector3 startPoint = pointOnB;
	const btVector3 endPoint = pointOnB + normalOnB * distance;
	drawLine(startPoint, endPoint, color);
}

void Tmpl9::DebugDrawer::ToggleDebugFlag(int flag)
{
	// checks if a flag is set and enables/
	// disables it
	if (m_debugMode & flag)
		// flag is enabled, so disable it
		m_debugMode = m_debugMode & (~flag);
	else
		// flag is disabled, so enable it
		m_debugMode |= flag;
}
