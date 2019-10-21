#include "PanelHierarchy.h"

#include "Application.h"
#include "ModuleSceneIntro.h"
#include "GameObject.h"

PanelHierarchy::PanelHierarchy(std::string name, bool active) : Panel(name, active)
{

}

PanelHierarchy::~PanelHierarchy()
{

}

bool PanelHierarchy::Update()
{
	ImGui::Begin(name.c_str());

	GameObject* root = App->scene_intro->root_game_object;
	if (root != nullptr)
	{
		if (ImGui::TreeNode(root->name.c_str()))
		{
			for (std::vector<GameObject*>::iterator item = root->children.begin(); item != root->children.end(); item++)
				DrawTextGameObject(*item);

			ImGui::TreePop();
		}
	}

	ImGui::End();

	return true;
}

void PanelHierarchy::DrawTextGameObject(GameObject* game_object)
{
	bool is_open = ImGui::TreeNode(game_object->name.c_str());
	
	if (is_open)
	{
		for (std::vector<GameObject*>::iterator item = game_object->children.begin();
			item != game_object->children.end(); item++)
			DrawTextGameObject(*item);

		ImGui::TreePop();
	}
}
