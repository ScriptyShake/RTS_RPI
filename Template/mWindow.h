#pragma once
#include <EGL/egl.h>
#include <glm/vec2.hpp>
#include <X11/Xlib.h>

namespace Tmpl9
{
  class Game;
}

class mWindow
{
public:
  /**
   * \brief create a new window
   * \param position window initial position
   * \param resolution window resolution
   * \param borderWidth width of border (useless unless override redirect has been set)
   */
  mWindow(glm::ivec2 position, glm::ivec2 resolution, int borderWidth);
  ~mWindow();
  /**
   * \brief shows the current window and binds egl for drawing
   * \param name name of the window
   */
  void ShowWindow(const char* name);
  /**
   * \brief main event loop
   */
  void ProccessEvents(Tmpl9::Game* game);
  /**
   * \brief get the main display or initialize if none are active
   * \return Pointer to main display
   */
  static Display* GetDisplay();

  /**
   * \brief swap egl buffer
   */
  void SwapBuffer() const
  {
    eglSwapBuffers(egl.display, egl.surface);
  }
private:
  /**
   * \brief initializes X11 display
   * \return false if initialization failed
   */
  static bool LoadDisplay();
  /**
   * \brief initialize egl
   * \return false in initialization failed
   */
  static bool InnitEgl();

  // X11 Display data
  static Display* display_;
  static Screen* screen_;
  static int screenID_;
  static int nrWindows_;

  // X11 window
  Window window_;

  Atom DeleteWindow = 0;
  Atom protocol = 0;

  // window attributes
  XWindowAttributes attributes_{};

  glm::ivec2 initial_resoluation;

  // egl data
  struct EGLData {
    static const EGLint attribs[];
    static const EGLint surfaceAttribs[];
    static const EGLint contextAttribs[];
    static constexpr EGLint maxNumConfigs = 10;

    static EGLint GLmajor, GLminor;
    static EGLDisplay display;

    static EGLConfig configs[maxNumConfigs];
    static EGLint numConfigs;

    EGLSurface surface{};
    EGLContext context{};
  } egl;
};

