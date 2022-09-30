#ifndef __Window_H__
#define __Window_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class Window : public Module
{
public:

	Window(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~Window();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);

	void SetBrightness(float bright);
	void SetSize(uint w, uint h);
	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetFullscreenDesktop(bool fullsdesktop);
	bool IsFullscreen() const { return fullScreen; }
	bool IsResizable() const { return resizable; }
	bool IsBorderless() const { return borderless; }
	bool IsFullscreenDesktop() const { return fullScreenDesktop; }

	float GetBrightness() const { return brightness; }
	void GetSize(int& h, int& w)const;

	void OnResize(int width, int height);

	void OnGUI() override;

public:
	//The window we'll be rendering to
	SDL_Window* window;
	SDL_GLContext gl_context;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	SDL_DisplayMode current;

	int width;
	int height;
	float brightness = 1;

	bool fullScreen = false;
	bool borderless = false;
	bool resizable = false;
	bool fullScreenDesktop = false;
};

#endif // __Window_H__