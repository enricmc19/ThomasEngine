#pragma once
#include <vector>
#include "Tab.h"
#include "Application.h"

#define FPS_MS_LOG_MAXLENGHT 100


struct LogMsg
{
	LogMsg(std::string _msg, LogType _type) : msg(_msg), prints(1), logType(_type) {};

	std::string msg;
	LogType logType;
	unsigned int prints;

	bool operator==(const char* _msg)
	{
		return (strcmp(msg.c_str(), _msg) == 0) ? true : false;
	}
};

class ConsoleTab : public Tab
{
public:
	ConsoleTab();

	~ConsoleTab();

	void Update() override;
	void Draw() override;

	void AddLog(const char*, LogType);

	char GetMsgType(LogType, ImVec4&);


public:

	bool collapsed=false;
	bool bottom= false;
	ImVec2 winSize = {200,400};
	LogMsg* charLog;
	std::vector<LogMsg> logs;

private:

	float offset;

};

