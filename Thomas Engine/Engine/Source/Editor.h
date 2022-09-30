#pragma once
#include "Module.h"
#include "Globals.h"

#include "Tab.h"

#include "imgui/imgui.h"
#include "imgui/imgui_user.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#define FPS_MS_LOG_MAXLENGHT 100

enum class TabType {

	CONFIGURATION,
	CONSOLE,

	MAX
};

class Editor:public Module
{
public:
	Editor(Application* app, bool start_enabled = true);

	~Editor();
	bool Init();

	bool Start();
	
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	update_status ImGuiMenu();

	bool CleanUp();

	void LogToConsole(const char* msg, LogType _type);

	Tab* GetTab(TabType type);
	void PrintLicense();

    void print_commits_info(const char* username, const char* repo);

	void persistence_example(void);

	void serialization_example(void);

	void parsonTest(void);

private:
	std::vector<Tab*> tabs;

	bool show_demo_window = false;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	bool showCase = false;
};