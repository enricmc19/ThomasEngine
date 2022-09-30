#include "Application.h"


using namespace std;

extern Application* app = nullptr;
Application::Application() : maxFPS(60)
{

	app = this;
	window = new Window(this);
	input = new Input(this);
	renderer3D = new Renderer3D(this);
	camera = new Camera3D(this);
	physics = new Physics3D(this);
	editor = new Editor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(physics);
	
	
	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);
}

Application::~Application()
{
	
	for (int i = list_modules.size() - 1; i >= 0; --i)
	{
		delete list_modules[i];
		list_modules[i] = nullptr;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (unsigned int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Init();
	}
	// After all Init calls we call Start() in all modules
	LOG(LogType::L_NORMAL, "-------------- Application Start --------------");

	// Call Init() in all modules
	for (unsigned int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Start();
	}

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (maxFPS == 0 || renderer3D->vsync || maxFPS > screenRefresh)
	{
		Uint32 last_frame_ms = ms_timer.Read();
		float wait_time = (1000.f / (float)screenRefresh) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
	else if (maxFPS > 0)
	{
		Uint32 last_frame_ms = ms_timer.Read();
		float wait_time = (1000.f / (float)maxFPS) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();	

	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}
	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}
	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	// Cleanup

	for (int i = list_modules.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	//list_modules.add(mod);
	list_modules.push_back(mod);
}