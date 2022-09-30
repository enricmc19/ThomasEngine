#pragma once
#include <iostream>
#include <vector>
#include <list>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Input.h"
#include "Camera3D.h"
#include "Physics3D.h"
#include "Editor.h"

class Application
{
public:
	Window* window;
	Camera3D* camera;
	Input* input;
	Renderer3D* renderer3D;
	Physics3D* physics;
	Editor* editor;

	std::vector<Module*> list_modules;
private:

	Timer	ms_timer;
	float	dt;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDt() const { return dt; }
	float GetFrameRate() const { return 1.f / dt; }

	int maxFPS;
	int screenRefresh;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

};
extern Application* app;