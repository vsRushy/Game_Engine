#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "ImGui/imgui.h"

#include "Panel.h"

class PanelScene : public Panel
{
public:
	PanelScene(std::string name);
	~PanelScene();

	bool Update();

	ImVec2 GetSize() const;

private:
	ImVec2 size { 0.0f, 0.0f };
};

#endif /* __PANEL_SCENE_H__ */