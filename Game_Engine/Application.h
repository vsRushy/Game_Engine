#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <list>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleUserInterface.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleUserInterface* user_interface;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;

private:
	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

public:
	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

#endif /* __APPLICATION_H__ */