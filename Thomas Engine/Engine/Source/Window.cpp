#include "Globals.h"
#include "Application.h"
#include "Window.h"

Window::Window(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
Window::~Window()
{
}

// Called before render is available
bool Window::Init()
{
	LOG(LogType::L_NORMAL, "Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}
	else
	{
		// Get Screen Refresh of monitor
		for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i) {

			int should_be_zero = SDL_GetCurrentDisplayMode(i, &current);

			if (should_be_zero != 0)
				// In case of error...
				SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
			else
				// On success, print the current display mode.
				SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, current.w, current.h, current.refresh_rate);
		}
		App->screenRefresh = current.refresh_rate;

		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		//window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		// Setup window
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
		gl_context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, gl_context);

		if(window == NULL)
		{
			LOG(LogType::L_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}
	SDL_bool res = SDL_bool(false);
	SDL_SetWindowResizable(window, res);

	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	
	return ret;
}

// Called before quitting
bool Window::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Destroying SDL window and quitting all SDL systems");

	SDL_GL_DeleteContext(gl_context);

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}


	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void Window::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void Window::SetBrightness(float bright)
{
	brightness = bright;
	SDL_SetWindowBrightness(window, bright);
}

void Window::SetSize(uint w, uint h)
{
	height = h;
	width = w;
	SDL_SetWindowSize(window, w, h);
}

void Window::GetSize(int& w, int& h)const
{
	w = width;
	h = height;
}

void Window::SetFullscreen(bool _fullscreen)
{
	fullScreen = _fullscreen;
	if (fullScreen)fullScreenDesktop = !fullScreen;
	(fullScreen) ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) : SDL_SetWindowFullscreen(window, 0);
}

void Window::SetResizable(bool _resizable)
{
	resizable = _resizable;
	SDL_bool res = SDL_bool(_resizable);
	SDL_SetWindowResizable(window, res);
}
void Window::SetBorderless(bool _borderless)
{
	borderless = _borderless;
	SDL_bool bord = SDL_bool(!_borderless);
	SDL_SetWindowBordered(window, bord);
}

void Window::SetFullscreenDesktop(bool _fulldesktop)
{
	fullScreenDesktop = _fulldesktop;
	if (fullScreenDesktop) fullScreen = !fullScreenDesktop;
	(fullScreenDesktop) ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_SetWindowFullscreen(window, 0);
}

void Window::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	this->width = width;
	this->height = height;
}

void Window::OnGUI()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		// TODO: Cambiar el *default* por le nombre del archivo del icono del motor
		ImGui::TextWrapped("Icon: *default* ");

		if (ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.f))
		{
			SetBrightness(brightness);
		}
		if (ImGui::SliderInt("Width", &width, 640, current.w))
		{
			SetSize(width, height);
		}
		if (ImGui::SliderInt("Height", &height, 480, current.h))
		{
			SetSize(width, height);
		}

		IMGUI_PRINT("Refresh rate: ", "%d", current.refresh_rate);

		if (ImGui::Checkbox("Fullscreen", &fullScreen)) {
			SetFullscreen(fullScreen);

			SDL_GetWindowSize(window, &width, &height);
			OnResize(width, height);			
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change fullscreen mode");

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &resizable)){
			if (!fullScreen && !fullScreenDesktop) SetResizable(resizable);
			else resizable = !resizable;
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change resizable mode, only on Off FullScreen mode");

		if (ImGui::Checkbox("Borderless", &borderless)) {
			if (!fullScreen && !fullScreenDesktop) SetBorderless(borderless);
			else borderless = !borderless;
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change borderless mode, only on Off FullScreen mode");

		ImGui::SameLine();

		if (ImGui::Checkbox("Fullscreen Desktop", &fullScreenDesktop)) {
			SetFullscreenDesktop(fullScreenDesktop);

			SDL_GetWindowSize(window, &width, &height);
			OnResize(width, height);			
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change fullscreen desktop mode");

		ImGui::NewLine();
	}
}
