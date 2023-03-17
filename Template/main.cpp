#include "mWindow.h"
#include "../Game.h"
#include "Surface.h"
#include "Timer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_opengl3.h"

#pragma GCC diagnostic pop

#include "GLES2/gl2ext.h"
#include <iostream>

bool ShouldClose = false;
void DrawQuad();
GLuint CreateVBO(const GLfloat* data, uint size);
void BindVBO(uint idx, uint N, GLint id);

#define SCRWIDTH 400
#define SCRHEIGHT 400

Tmpl9::Game game{};

int main(int argc, char* argv[])
{
	mWindow m_window{{0, 0}, {SCRWIDTH, SCRHEIGHT}, 10};
	m_window.ShowWindow("RTS 3D");



	const auto pegl_debug_message_control_khr = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKKHRPROC>(eglGetProcAddress("glDebugMessageCallback"));
  if (pegl_debug_message_control_khr == nullptr) {
    printf("failed to eglGetProcAddress eglDebugMessageControlKHR\n");
  }
  else {
    const GLDEBUGPROCKHR debug_fn = [](GLenum source, GLenum type, GLuint id, const GLenum severity, GLsizei length, const GLchar* message, const void*)
    {
      switch (severity)
      {
      case GL_DEBUG_SEVERITY_HIGH_KHR:
      case GL_DEBUG_SEVERITY_MEDIUM_KHR:
        std::cout << message << std::endl;
      case GL_DEBUG_SEVERITY_LOW_KHR:
      case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
      default:
        break; //Ignore.
      }
    };
    pegl_debug_message_control_khr(debug_fn, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);
  }



	// create ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// set display size for ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = {SCRWIDTH, SCRHEIGHT};

	// set ImGui style
	ImGui_ImplOpenGL3_Init("#version 300 es");
	ImGui::StyleColorsClassic();

	Tmpl9::Shader shader{"Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl"};
	Tmpl9::GLTexture Graphics{SCRWIDTH, SCRHEIGHT, Tmpl9::GLTexture::INTTARGET};
	Tmpl9::Surface screen{SCRWIDTH, SCRHEIGHT};

	game.SetScreen(&screen);
	game.Init();

	Timer timer;

	while (game.should_close == false)
	{
		float deltaTime = std::min(500.0f, 1000.0f * timer.elapsed());
		timer.reset();

		// start new ImGui frame for use inside game.tick()
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		io.DeltaTime = deltaTime / 1000.0f;

		game.Tick(deltaTime / 1000.0f);


		Graphics.CopyFrom(&screen);
		shader.Bind();
		shader.SetInputTexture(0, "c", &Graphics);

		//DrawQuad();

		shader.unBind();

		// draw ImGui frame after drawing screen so it's always on top
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		m_window.SwapBuffer();
		m_window.ProccessEvents(&game);
	}

	game.ShutDown();

	// destroy ImGui context
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

void DrawQuad()
{
	static GLuint vao = 0;
	if (!vao)
	{
		// generate buffers
		static const GLfloat verts[] = {-1, 1, 0, 1, 1, 0, -1, -1, 0, 1, 1, 0, -1, -1, 0, 1, -1, 0};
		static const GLfloat uvdata[] = {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1};
		GLuint vertexBuffer = CreateVBO(verts, sizeof(verts));
		GLuint UVBuffer = CreateVBO(uvdata, sizeof(uvdata));
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		BindVBO(0, 3, vertexBuffer);
		BindVBO(1, 2, UVBuffer);
		glBindVertexArray(0);
	}
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

GLuint CreateVBO(const GLfloat* data, const uint size)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	return id;
}

void BindVBO(const uint idx, const uint N, const GLint id)
{
	glEnableVertexAttribArray(idx);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glVertexAttribPointer(idx, N, GL_FLOAT, GL_FALSE, 0, nullptr);
}
