#include "mWindow.h"

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <iostream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wswitch"

#include "../Game.h"
#include "../ImGUI/imgui.h"

#pragma GCC diagnostic pop

#include "Helper.h"

// initialize statics
//-------------------

// display and root window/screen
Display* mWindow::display_ = nullptr;
Screen* mWindow::screen_ = nullptr;
int mWindow::screenID_ = 0;
int mWindow::nrWindows_ = 0;

// egl data
//---------
EGLDisplay mWindow::EGLData::display = nullptr;
// version
EGLint mWindow::EGLData::GLmajor;
EGLint mWindow::EGLData::GLminor;
// config
EGLConfig mWindow::EGLData::configs[maxNumConfigs];
EGLint mWindow::EGLData::numConfigs;
// attributes
const EGLint mWindow::EGLData::attribs[] = {
	EGL_CONFORMANT, EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_DEPTH_SIZE, 8,
	EGL_SAMPLE_BUFFERS, 0,
	EGL_SAMPLES, 0,
	EGL_NONE
};
const EGLint mWindow::EGLData::surfaceAttribs[] = {
	EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
	EGL_NONE
};
const EGLint mWindow::EGLData::contextAttribs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};
// set to true if program should exit
// extern bool ShouldClose;

mWindow::mWindow(glm::ivec2 position, glm::ivec2 resolution, int borderWidth)
{
	initial_resoluation = resolution;

	// initialize X display server
	if (display_ == nullptr)
	{
		const bool success = LoadDisplay();
		assert(success);
	}

	//setup window attributes
	XSetWindowAttributes attributes{};
	attributes.border_pixel = BlackPixel(display_, screenID_);
	attributes.background_pixel = WhitePixel(display_, screenID_);
	attributes.override_redirect = True;
	attributes.event_mask =
		//key events
		KeyPressMask | KeyReleaseMask | KeymapStateMask |
		//mouse events
		PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
		// resize event
		StructureNotifyMask;

	// create window
	window_ = XCreateWindow(display_, RootWindow(display_, screenID_), position.x, position.y, resolution.x,
	                        resolution.y, borderWidth, DefaultDepth(display_, screenID_),
	                        InputOutput, DefaultVisual(display_, screenID_), CWBackPixel | CWBorderPixel | CWEventMask,
	                        &attributes);

	XClearWindow(display_, window_);

	// get protocal (idk what this protocal does but the window wont show without it so hey)
	protocol = XInternAtom(display_, "WM_PROTOCOLS", False);

	// keep track of active windows
	nrWindows_++;

	// initialize egl
	if (egl.display == nullptr)
	{
		const bool success = InnitEgl();
		assert(success);
	}

	// create egl surface for window
	egl.surface = eglCreateWindowSurface(egl.display, egl.configs[0], window_, nullptr);
	egl.context = eglCreateContext(egl.display, egl.configs[0], EGL_NO_CONTEXT, egl.contextAttribs);
	if (egl.context == EGL_NO_CONTEXT)
	{
		std::cout << "could not create context" << std::endl;
	}
}

mWindow::~mWindow()
{
	// destroy window, if no other windows exist close X11 server connection
	XDestroyWindow(display_, window_);
	if (--nrWindows_ <= 0)
	{
		XCloseDisplay(display_);
	}
}

void mWindow::ShowWindow(const char* name)
{
	//set gl viewport
	XGetWindowAttributes(display_, window_, &attributes_);
	glViewport(0, 0, attributes_.width, attributes_.height);
	//set window name
	XStoreName(display_, window_, name);
	//show window
	XMapRaised(display_, window_);

	// make sure event get sent when window gets closed by pressing the x
	DeleteWindow = XInternAtom(display_, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display_, window_, &DeleteWindow, 1);

	// make window current
	if (!eglMakeCurrent(egl.display, egl.surface, egl.surface, egl.context))
	{
		std::cout << "could not set context as current" << std::endl;
	}
}

void mWindow::ProccessEvents(Tmpl9::Game* game)
{
	//event to store event data in
	static XEvent event;

	// key event data
	char str[25] = {0};
	KeySym key_sym = 0;
	int len;

	// mouse event data
	int x, y;
	int button;

	//see if queue is empty and if not handle events
	while (XEventsQueued(display_, QueuedAfterReading) > 0)
	{
		//get next event
		XNextEvent(display_, &event);
		switch (event.type)
		{
		// keys have been remapped
		case KeymapNotify:
			XRefreshKeyboardMapping(&event.xmapping);
			break;
		//key has been pressed
		case KeyPress:
			// get pressed key
			len = XLookupString(&event.xkey, str, 25, &key_sym, nullptr);
			if (len > 0)
			{
				//std::cout << "Key pressed: " << str << " - " << len << " - " << key_sym << std::endl;
			}
		// stop program if escape is pressed
			if (key_sym == XK_Escape)
			{
				game->should_close = true;
				return;
			}
			game->KeyDown(key_sym);

			ImGui::GetIO().AddInputCharacter(str[0]);
			ImGui::GetIO().AddKeyEvent(X11symToImGuiKeycode(key_sym), true);

			break;
		// key is released
		case KeyRelease:
			// get pressed key
			len = XLookupString(&event.xkey, str, 25, &key_sym, nullptr);
			if (len > 0)
			{
				//std::cout << "Key released: " << str << " - " << len << " - " << key_sym << std::endl;
			}
			game->KeyUp(key_sym);

			ImGui::GetIO().AddKeyEvent(X11symToImGuiKeycode(key_sym), false);

			break;
		// mouse button pressed
		case ButtonPress:

			// calculate which button is pressed as X11 switches middle and right click + is 1 indexed
			button = event.xbutton.button - 1;

			if (button == 1)
			{
				button = 2;
			}
			else if (button == 2)
			{
				button = 1;
			}

			ImGui::GetIO().MouseDown[button] = true;

			game->MouseDown(button);

			break;
		// mouse button released
		case ButtonRelease:
			// calculate which button is pressed as X11 switches middle and right click + is 1 indexed
			button = event.xbutton.button - 1;

			if (button == 1)
			{
				button = 2;
			}
			else if (button == 2)
			{
				button = 1;
			}

			ImGui::GetIO().MouseDown[button] = false;

			game->MouseUp(button);
			break;
		// mouse moved
		case MotionNotify:
			x = event.xmotion.x;
			y = event.xmotion.y;

			ImGui::GetIO().MousePos = {static_cast<float>(x), static_cast<float>(y)};

			game->MouseMove(
				static_cast<int>(static_cast<float>(x) / static_cast<float>(attributes_.width) * static_cast<float>(
					initial_resoluation.x)),
				static_cast<int>(static_cast<float>(y) / static_cast<float>(attributes_.height) * static_cast<float>(
					initial_resoluation.y)));
			break;
		// mouse entered window
		case EnterNotify:
			break;
		// mouse left window
		case LeaveNotify:
			break;
		// screen got resized
		case ConfigureNotify:
			XGetWindowAttributes(display_, event.xexpose.window, &attributes_);
			glViewport(0, 0, attributes_.width, attributes_.height);

			ImGui::GetIO().DisplaySize = ImVec2{
				static_cast<float>(attributes_.width), static_cast<float>(attributes_.height)
			};
			break;
		case ClientMessage:
			// window closed
			if (static_cast<Atom>(event.xclient.data.l[0]) == DeleteWindow)
			{
				game->should_close = true;
				return;
			}
		//window got destroyed
		case DestroyNotify:
			game->should_close = true;
			return;
		default:
			break;
		}
	}
}

// return main display or initialize if none are active
Display* mWindow::GetDisplay()
{
	if (display_ == nullptr)
	{
		const bool success = LoadDisplay();
		assert(success);
	}

	return display_;
}

// initialize display
bool mWindow::LoadDisplay()
{
	display_ = XOpenDisplay(nullptr);
	if (display_ == nullptr)
	{
		std::cout << "Could not open display" << std::endl;
		return false;
	}
	screen_ = DefaultScreenOfDisplay(display_);
	if (screen_ == nullptr)
	{
		std::cout << "could not find default screen" << std::endl;
		return false;
	}
	screenID_ = DefaultScreen(display_);
	return true;
}

// initialize egl
bool mWindow::InnitEgl()
{
	EGLData::display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (EGLData::display == EGL_NO_DISPLAY)
	{
		std::cout << "Could not open EGL display" << std::endl;
		return false;
	}
	if (!eglInitialize(EGLData::display, &(EGLData::GLmajor), &(EGLData::GLminor)))
	{
		std::cout << "could not initialize egl" << std::endl;
		return false;
	}
	if (!eglChooseConfig(EGLData::display, EGLData::attribs, EGLData::configs, EGLData::maxNumConfigs,
	                     &EGLData::numConfigs))
	{
		std::cout << "could not choose configs" << std::endl;
		return false;
	}

	return true;
}
