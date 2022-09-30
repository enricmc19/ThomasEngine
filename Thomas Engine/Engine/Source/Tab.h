#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <string>

class Tab
{

public:
	Tab();
	virtual ~Tab();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	std::string name;
	bool active;
	int shortcut;
};

#endif //__WINDOW_H__