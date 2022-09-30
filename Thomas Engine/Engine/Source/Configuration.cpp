#include "Configuration.h"
#include "mmgr/mmgr.h"

Configuration::Configuration() : Tab()
{
	name = "Configuration";
	// Reserve 100 positions
	fpsLog.reserve(FPS_MS_LOG_MAXLENGHT);
	msLog.reserve(FPS_MS_LOG_MAXLENGHT);
	memoryLog.reserve(FPS_MS_LOG_MAXLENGHT);
}

Configuration::~Configuration()
{
	msLog.clear();
	fpsLog.clear();
	memoryLog.clear();
}

void Configuration::Update()
{
	update_status ret = UPDATE_CONTINUE;
	// Get framerate
	float currentFPS = floorf(app->GetFrameRate())/*ImGui::GetIO().Framerate*/;
	float currentMS = (1000.f * app->GetDt());

	PushBackLog(&fpsLog, currentFPS);
	PushBackLog(&msLog, currentMS);
}

void Configuration::Draw()
{
	if (ImGui::Begin("Configuration"))
	{
		// CollapsingHeader is to create new Header
		if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Input Text is to create a text with background and a const text
			ImGui::InputText("App Name", "Thomas Engine", 12);
			ImGui::InputText("Organization", "UPC CITM", 9);
			ImGui::SliderInt("Max FPS", &app->maxFPS, 0, 144);
			// TextWrapped is to create a simple text
			IMGUI_PRINT("Limit Framerate: ", (app->maxFPS == 0) ? "VSync" : "%d", app->maxFPS);

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
			ImGui::PlotHistogram("##frameRate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", msLog[msLog.size() - 1]);
			ImGui::PlotHistogram("##miliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
			ImGui::NewLine();

			// Memory --------------------

			sMStats stats = m_getMemoryStatistics();

			PushBackLog(&memoryLog, stats.totalReportedMemory);
			ImGui::PlotHistogram("##memory", &memoryLog[0], memoryLog.size(), 0, "Memory Consumption", 0.0f, (float)stats.peakReportedMemory * 1.2f, ImVec2(310, 100));

			IMGUI_PRINT("Total Reported Mem: ", "%u", stats.totalReportedMemory);
			IMGUI_PRINT("Total Actual Mem: ", "%u", stats.totalActualMemory);
			IMGUI_PRINT("Peak Reported Mem: ", "%u", stats.peakReportedMemory);
			IMGUI_PRINT("Peak Actual Mem: ", "%u", stats.peakActualMemory);
			IMGUI_PRINT("Accumulated Reported Mem: ", "%u", stats.accumulatedReportedMemory);
			IMGUI_PRINT("Accumulated Actual Mem: ", "%u", stats.accumulatedActualMemory);
			IMGUI_PRINT("Accumulated Alloc Unit Count: ", "%u", stats.accumulatedAllocUnitCount);
			IMGUI_PRINT("Total Alloc Unit Count: ", "%u", stats.totalAllocUnitCount);
			IMGUI_PRINT("Peak Alloc Unit Count: ", "%u", stats.peakAllocUnitCount);
		}
		for (unsigned int i = 0; i < app->list_modules.size(); ++i)
		{
			app->list_modules[i]->OnGUI();
		}
	}
	ImGui::End();
}

void Configuration::PushBackLog(std::vector<float> *log, float current)
{
	if (log->size() <= FPS_MS_LOG_MAXLENGHT) log->push_back(current);
	else
	{
		log->erase(log->begin());
		log->push_back(current);
	}
}
