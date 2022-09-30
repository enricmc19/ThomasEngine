#include "Globals.h"
#include "Application.h"
#include "Renderer3D.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#include "GPUDetected/DeviceId.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Renderer3D::Renderer3D(Application* app, bool start_enabled) : Module(app, start_enabled), vsync(false)
{
	GetCaps(hardware.caps);

	SDL_version version;
	SDL_GetVersion(&version);
	sprintf_s(hardware.SDLVersion, 25, "%i.%i.%i", version.major, version.minor, version.patch);
	hardware.CPUCount = SDL_GetCPUCount();
	hardware.CPUCache = SDL_GetCPUCacheLineSize();
	hardware.systemRAM = SDL_GetSystemRAM() / 1024.f;	

	uint vendor, deviceId;
	std::wstring brand;
	unsigned __int64 videoMemBudget;
	unsigned __int64 videoMemUsage;
	unsigned __int64 videoMemAvailable;
	unsigned __int64 videoMemReserved;

	if (getGraphicsDeviceInfo(&vendor, &deviceId, &brand, &videoMemBudget, &videoMemUsage, &videoMemAvailable, &videoMemReserved))
	{
		hardware.GPUVendor = vendor;
		hardware.GPUDevice = deviceId;
		sprintf_s(hardware.GPUBrand, 250, "%S", brand.c_str());
		hardware.VRAMBudget = float(videoMemBudget) / (1073741824.0f / 1024.f);
		hardware.VRAMUsage = float(videoMemUsage) / (1024.f * 1024.f * 1024.f);
		hardware.VRAMAvailable = float(videoMemAvailable) / (1024.f * 1024.f);
		hardware.VRAMReserved = float(videoMemReserved) / (1024.f * 1024.f * 1024.f);
	}
}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Init()
{
	LOG(LogType::L_NORMAL, "Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG(LogType::L_ERROR, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(static_cast<int>(vsync)) < 0)
			LOG(LogType::L_ERROR, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glewInit();
		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG(LogType::L_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// LADO FRONTAL: lado multicolor


	return ret;
}

// PreUpdate: clear buffer
update_status Renderer3D::PreUpdate(float dt)
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status Renderer3D::PostUpdate(float dt)
{
	update_status ret;
	//glClearColor(0.f, 0.f, 0.f, 1.f);
	//glClear(GL_COLOR_BUFFER_BIT);

	// Axis and grid
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	glBegin(GL_POLYGON);

	glVertex3f(1,0, 1);       // P1
	glVertex3f(1,1, 1);       // P2
	glVertex3f(0,1, 1);       // P3
	glVertex3f(0,0,1);       // P4

	glVertex3f(0,0,1);       // P
	glVertex3f(0, 1, 1);      // P
	glVertex3f(0, 1, 0);       // P2
	glVertex3f(0, 0, 0);       // P1


	glVertex3f(0, 0, 0);       // P4
	glVertex3f(0, 1, 0);       // P3
	glVertex3f(1, 1, 0);       // P2
	glVertex3f(1, 0, 0);       // P1


	glEnd();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG(LogType::L_NO_PRINTABLE, "Destroying 3D Renderer");
		
	SDL_GL_DeleteContext(context);

	return true;
}


void Renderer3D::GetCaps(std::string& caps)
{
	caps += (SDL_HasRDTSC()) ? "RDTSC," : "";
	caps += (SDL_HasMMX()) ? "MMX, " : "";
	caps += (SDL_HasSSE()) ? "SSE, " : "";
	caps += (SDL_HasSSE2()) ? "SSE2, " : "";
	caps += (SDL_HasSSE3()) ? "SSE3, " : "";
	caps += "\n";
	caps += (SDL_HasSSE41()) ? "SSE41, " : "";
	caps += (SDL_HasSSE42()) ? "SSE42, " : "";
	caps += (SDL_HasAVX()) ? "AVX, " : "";
	caps += (SDL_HasAltiVec()) ? "AltiVec, " : "";
	caps += (SDL_Has3DNow()) ? "3DNow, " : "";
}

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Renderer3D::OnGUI()
{
	if (ImGui::CollapsingHeader("Hardware"))
	{
		IMGUI_PRINT("SDL Version: ", hardware.SDLVersion);
		IMGUI_PRINT("OpenGL Version: ", "%s", glGetString(GL_VERSION));
		IMGUI_PRINT("Glew Version: ", "%s", glewGetString(GLEW_VERSION));

		ImGui::Separator();
		IMGUI_PRINT("GLSL: ", "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		IMGUI_PRINT("Vendor: ", "%s", glGetString(GL_VENDOR));
		IMGUI_PRINT("Renderer: ", "%s", glGetString(GL_RENDERER));

		//ImGui::TextWrapped("All external library versions can be found in the 'About' window with links to their pages.");

		ImGui::Separator();
		IMGUI_PRINT("CPUs: ", "%d (Cache: %dkb)", hardware.CPUCount, hardware.CPUCache);
		IMGUI_PRINT("System RAM: ", "%.1fGb", hardware.systemRAM);
		IMGUI_PRINT("Caps: ", hardware.caps.c_str());

		ImGui::Separator();
		IMGUI_PRINT("GPU:", "vendor %u device %u", hardware.GPUVendor, hardware.GPUDevice);
		IMGUI_PRINT("Brand:", hardware.GPUBrand);
		IMGUI_PRINT("VRAM Budget:", "%.1f Mb", hardware.VRAMBudget);
		IMGUI_PRINT("VRAM Usage:", "%.1f Mb", hardware.VRAMUsage);
		IMGUI_PRINT("VRAM Available:", "%.1f Mb", hardware.VRAMAvailable);
		IMGUI_PRINT("VRAM Reserved:", "%.1f Mb", hardware.VRAMReserved);
	}
}
