#include "ConsoleTab.h"

ConsoleTab::ConsoleTab() : Tab()
{
	name = "Console";
	// Reserve 100 positions
}

ConsoleTab::~ConsoleTab()
{
}

void ConsoleTab::Update()
{
	update_status ret = UPDATE_CONTINUE;

}

void ConsoleTab::Draw()
{
	
	winSize = ImGui::GetWindowSize();

	if (ImGui::Begin("Console"))
	{
		
		if (ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar))
		{
			offset = ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize("Clear").x - 7;

			ImGui::SetCursorPosX(offset);

			offset -= ImGui::CalcTextSize("Collapse").x + (ImGui::CalcTextSize("Clear").x / 2) - 7;
			ImGui::SetCursorPosX(offset);
			//if (ImGui::Button("Collapse"/*, ImGuiDir_Right*/)) collapsed = !collapsed;
		
			ImGui::Separator();
			//ImGui::SetWindowSize("Console", { winSize.x/2, winSize.y/2 });

		
			if (ImGui::BeginChild("##outPutConsole", winSize))
			{
				charLog = nullptr;
				ImVec4 labelColor(0.f, 0.f, 0.f, 0.f);

				for (unsigned int i = 0; i < logs.size(); ++i)
				{
					charLog = &logs[i];

					//ImGui::TextWrapped("[%c]", GetMsgType(charLog->logType));
					char labelLevel = GetMsgType(charLog->logType, labelColor);

					ImGui::TextColored(labelColor, "[%c]", labelLevel);

					ImGui::SameLine();
					ImGui::TextWrapped(charLog->msg.c_str());

					if (logs[i].prints > 1)
					{
						ImGui::SameLine();
						ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - ImGui::CalcTextSize(std::to_string(charLog->prints).c_str()).x);
						ImGui::Text("%i", charLog->prints);
					}
				}

				if (bottom && !ImGui::IsWindowFocused())
				{
					ImGui::SetScrollHereY(1.0f);
					bottom = false;
				}
			}
			ImGui::EndChild();

		}ImGui::End();
		
	}
	ImGui::End();
}

char ConsoleTab::GetMsgType(LogType type, ImVec4& lColor)
{
	char ret = 'I';

	switch (type)
	{
	case LogType::L_NORMAL:
		ret = 'I';
		lColor = ImVec4(1.f, 1.f, 1.f, 1.f);
		break;

	case LogType::L_WARNING:
		ret = 'W';
		lColor = ImVec4(1.f, 1.f, 0.f, 1.f);
		break;

	case LogType::L_ERROR:
		ret = 'E';
		lColor = ImVec4(1.f, 0.f, 0.f, 1.f);
		break;
	}

	return ret;
}

void ConsoleTab::AddLog(const char* s_msg, LogType _type)
{
	if (collapsed && logs.size() >= 1)
	{
		if (logs[logs.size() - 1].msg == s_msg)
		{
			++logs[logs.size() - 1].prints;
			return;
		}
	}

	logs.push_back(LogMsg(s_msg, _type));
	bottom = true;
}
