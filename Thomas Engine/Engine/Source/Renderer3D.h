#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "Glew/include/glew.h"
#include "SDL\include\SDL_opengl.h"


#define MAX_LIGHTS 8

class Hardware {
public:
	std::string caps;
	char SDLVersion[25] = "";

	uint CPUCount;
	uint CPUCache;
	float systemRAM;

	uint GPUVendor = 0;
	uint GPUDevice = 0;
	char GPUBrand[250] = "";
	float VRAMBudget = 0.f;
	float VRAMUsage = 0.f;
	float VRAMAvailable = 0.f;
	float VRAMReserved = 0.f;
};

class Renderer3D : public Module
{
public:
	Renderer3D(Application* app, bool start_enabled = true);
	~Renderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void GetCaps(std::string& caps);
	void OnResize(int width, int height);
	void OnGUI() override;

	Hardware GetHardware() { return hardware; };

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	bool vsync;

private:
	Hardware hardware;
};