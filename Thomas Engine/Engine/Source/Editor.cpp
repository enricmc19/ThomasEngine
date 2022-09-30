#include "Application.h"
#include "Globals.h"
#include "Editor.h"

#include "Parson/parson.h"

#include "Configuration.h"
#include "ConsoleTab.h"

Editor::Editor(Application* app, bool start_enabled): Module(app, start_enabled)
{	
	// Define size of the vector
	tabs = std::vector<Tab*>(static_cast<unsigned int>(TabType::MAX), nullptr);

	// Fill the array with the types of tabs 
	tabs[static_cast<unsigned int>(TabType::CONFIGURATION)] = new Configuration();
	tabs[static_cast<unsigned int>(TabType::CONSOLE)] = new ConsoleTab();

	// Assign a shortcut to each tab
	for (int i = 0; i < tabs.size(); i++)
	{
		tabs[i]->shortcut = i + 1;
	}
}

Editor::~Editor()
{
}

bool Editor::Init()
{
	//print_commits_info("torvalds", "linux");
	//persistence_example();
	parsonTest();
	//serialization_example();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->window->gl_context);
	ImGui_ImplOpenGL3_Init();

    return true;
}

bool Editor::Start()
{
    return true;
}

update_status Editor::PreUpdate(float dt)
{
    return UPDATE_CONTINUE;
}

update_status Editor::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Check if any key is pressed and disable/enable the tab
	for (int i = 0; i < tabs.size(); i++)
	{
		if (App->input->GetKey(29 + tabs[i]->shortcut) == KEY_UP)
		{
			tabs[i]->active = !tabs[i]->active;
		}
	}
	
	// Call Updates of tabs
	for (int i = 0; i < tabs.size(); i++)
	{
		if (tabs[i]->active)
		{
			tabs[i]->Update();
		}
	}

    return ret;
}

update_status Editor::PostUpdate(float dt)
{	
	update_status ret = UPDATE_CONTINUE;
	// Rendering the tabs
	ret = ImGuiMenu();
	if (showCase)
	{
		//ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::ShowDemoWindow();
		//ImGui::PopStyleColor();
	}
	for (unsigned int i = 0; i < tabs.size(); i++)
	{
		if (tabs[i]->active)
		{
			tabs[i]->Draw();
		}
	}

	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
	SDL_GL_SwapWindow(App->window->window);

    return ret;
}

update_status Editor::ImGuiMenu()
{
	// Create a MenuBar
	update_status ret = update_status::UPDATE_CONTINUE;
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC"))
				ret = UPDATE_STOP;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			for (int i = 0; i < tabs.size(); i++)
			{
				if (ImGui::MenuItem(tabs[i]->name.c_str(), std::to_string(i+1).c_str(), tabs[i]->active, &tabs[i]->active))
					tabs[i]->active =! tabs[i]->active;
			}			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("ImGui Demo", nullptr, showCase))
			{
				showCase = !showCase;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/wiki", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/releases", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine/issues", 0, 0, SW_SHOW);
			}
			if (ImGui::BeginMenu("About"))
			{
				ImGui::Text("Thomas Engine");
				ImGui::Text("This is a university project focused");
				ImGui::Text("on the development of a video game engine");

				ImGui::NewLine();
				ImGui::Text("Developed by: ");
				if (ImGui::MenuItem("Enric Morales"))
				{
					ShellExecute(0, 0, "https://github.com/enricmc19", 0, 0, SW_SHOW);
				}
				if (ImGui::MenuItem("Laia Pachon"))
				{
					ShellExecute(0, 0, "https://github.com/laiapachon/ThomasEngine", 0, 0, SW_SHOW);
				}

				ImGui::NewLine();
				ImGui::Separator();

				char SDLVersion[25];
				SDL_version version;
				SDL_GetVersion(&version);
				sprintf_s(SDLVersion, 25, "%i.%i.%i", version.major, version.minor, version.patch);

				ImGui::Text("3rd Party Libraries used:");
				IMGUI_PRINT("SDL Version: ", SDLVersion);
				IMGUI_PRINT("OpenGL Version: ", "%s", glGetString(GL_VERSION));
				IMGUI_PRINT("Glew Version: ", "%s", glewGetString(GLEW_VERSION));
				IMGUI_PRINT("MathGeoLib Version: ", "1.5");
				IMGUI_PRINT("ImGui Version: ", "1.84.2");

				ImGui::NewLine();
				ImGui::Separator();

				PrintLicense();

				ImGui::EndMenu();
			}			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return ret;
}

bool Editor::CleanUp()
{
	bool ret= true;

	// CleanUp all tabs
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (unsigned int i = 0; i < tabs.size(); ++i)
	{
		delete tabs[i];
		tabs[i] = nullptr;
	}
	tabs.clear();

	return ret;
}
void Editor::LogToConsole(const char* msg, LogType _type)
{
	ConsoleTab* consoleWindow = dynamic_cast<ConsoleTab*>(GetTab(TabType::CONSOLE));

	if (consoleWindow != nullptr)
		consoleWindow->AddLog(msg, _type);
}

Tab* Editor::GetTab(TabType type)
{
	unsigned int vecPosition = static_cast<unsigned int>(type);

	SDL_assert(vecPosition < tabs.size());
	return (vecPosition < tabs.size()) ? tabs[vecPosition] : nullptr;
}

void Editor::PrintLicense()
{
	ImGui::Text("License:");
	ImGui::Text("MIT License \nCopyright(c) 2022 laiapachon \n");
	ImGui::NewLine();
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this softwareand associated documentation files (the \"Software\"), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and /or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions:");
	ImGui::NewLine();
	ImGui::Text("The above copyright noticeand this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::NewLine();
	ImGui::Text("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE");
}

void Editor::print_commits_info(const char* username, const char* repo) {
	JSON_Value* root_value;
	JSON_Array* commits;
	JSON_Object* commit;
	size_t i;

	char curl_command[512];
	char cleanup_command[256];
	char output_filename[] = "commits.json";

	/* it ain't pretty, but it's not a libcurl tutorial */
	sprintf(curl_command, "curl -s \"https://api.github.com/repos/%s/%s/commits\" > %s", username, repo, output_filename);
	sprintf(cleanup_command, "rm -f %s", output_filename);
	system(curl_command);

	/* parsing json and validating output */
	root_value = json_parse_file(output_filename);
	if (json_value_get_type(root_value) != JSONArray) {
		system(cleanup_command);
		return;
	}

	/* getting array from root value and printing commit info */
	commits = json_value_get_array(root_value);
	printf("%-10.10s %-10.10s %s\n", "Date", "SHA", "Author");
	for (i = 0; i < json_array_get_count(commits); i++) {
		commit = json_array_get_object(commits, i);
		printf("%.10s %.10s %s\n",
			json_object_dotget_string(commit, "commit.author.date"),
			json_object_get_string(commit, "sha"),
			json_object_dotget_string(commit, "commit.author.name"));
	}

	/* cleanup code */
	json_value_free(root_value);
	system(cleanup_command);
}

void Editor::persistence_example(void) {
	JSON_Value* schema = json_parse_string("{\"name\":\"\"}");
	JSON_Value* user_data = json_parse_file("user_data.json");
	char buf[256];
	const char* name = NULL;
	if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess) {
		puts("Enter your name:");
		scanf("%s", buf);
		user_data = json_value_init_object();
		json_object_set_string(json_object(user_data), "name", buf);
		json_serialize_to_file(user_data, "user_data.json");
	}
	name = json_object_get_string(json_object(user_data), "name");
	printf("Hello, %s.", name);
	json_value_free(schema);
	json_value_free(user_data);
	return;
}

void  Editor::serialization_example(void) {
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);

	char* serialized_string = NULL;
	json_object_set_string(root_object, " nombre ", " John Smith ");
	json_object_set_number(root_object, " edad ", 25);
	json_object_dotset_string(root_object, " address.city ", " Cupertino ");
	json_object_dotset_value(root_object, " contacto.", json_parse_string(" [ \" email@example.com \" , \"email2@example.com \" ] "));
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);

}

void Editor::parsonTest(void) {
	JSON_Value* schema = json_parse_string("{\"name\":\"\"}");
	const char* filename= "userData.json";
	JSON_Value* user_data = json_parse_file(filename);

	JSON_Value* root_value = json_value_init_object();

	JSON_Object* root_object = json_value_get_object(root_value);

	char buf[256];
	const char* name = NULL;
	if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess) {
		puts("Enter your name:");
		
		user_data = json_value_init_object();
		json_object_set_string(json_object(user_data), "name", "Enric");
		json_object_dotset_string(json_object(user_data), "address.city", "Cupertino");
		json_serialize_to_file(user_data, filename);
	}
	name = json_object_get_string(json_object(user_data), "name");
	printf("Hello, %s.", name);
	json_value_free(schema);
	json_value_free(user_data);
	return;
}
