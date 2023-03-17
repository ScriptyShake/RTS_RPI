#pragma once


#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"
#include "LinearMath/btIDebugDraw.h"
#pragma GCC diagnostic pop

class Surface;
class Shader2;
class Camera;

namespace Tmpl9
{
	class DebugDrawer : public btIDebugDraw
	{
	public:
		DebugDrawer();
		Shader2* debugShader;
		// debug mode functions
		void setDebugMode(int debugMode) override { m_debugMode = debugMode; }
		int getDebugMode() const override { return m_debugMode; }

		// drawing functions
		void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
		                      const btVector3& color) override;
		void drawLine(const btVector3& from, const btVector3& to,
		              const btVector3& color) override;

		// unused
		void reportErrorWarning(const char* warningString) override
		{
			std::cout<<warningString<<std::endl;
		}

		void draw3dText(const btVector3& location, const char* textString) override
		{
		}

		static void SetCamera(Camera* camera)
		{
			DebugDrawer::camera = camera;
		}

		void ToggleDebugFlag(int flag);

	private:
		unsigned int VBO, VAO;
		static Camera* camera;
	protected:
		int m_debugMode;
	};
}
