#pragma once
#include "ParticleSystem.h"
#include <tge/settings/settings.h>
#include <tge/text/text.h>
#include <tge/math/Vector.h>
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

// Resize InputText taken from: https://github.com/ocornut/imgui/blob/master/misc/cpp/imgui_stdlib.cpp

class GameWorld
{
public:
	GameWorld(); 
	~GameWorld();

	void Init();
	void Update(float aTimeDelta); 
	void Render();

	void LoadParticleSystem();
	void SaveParticleSystem();

private:
	Tga::Sprite2DInstanceData myTGELogoInstance = {};
	Tga::SpriteSharedData sharedData = {};
	ParticleSystem ps;
	ParticleInstance p;
	float t = 0;
	float spawnRate = 0.01f;
	int maxParticles = 410;
	std::string particleSprite = "Sprites/tga_w.dds";
	std::string particleName = "My new particle";
	std::string loadName = "particle";
	bool openWindow = true;
	bool openSave = false;
	bool openLoad = false;
	bool simulate = true;
	Tga::Text dbgText;
};

namespace ImGuiExtra
{
	struct InputTextCallback_UserData
	{
		std::string* Str;
		ImGuiInputTextCallback  ChainCallback;
		void* ChainCallbackUserData;
	};

	static int ResizeCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			ImVector<char>* my_str = (ImVector<char>*)data->UserData;
			IM_ASSERT(my_str->begin() == data->Buf);
			my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
			data->Buf = my_str->begin();
		}
		return 0;
	}

	void InputVec2(const char* label, Tga::Vector2f& vector);
	void InputColor(const char* label, Tga::Color& color);
	int InputTextCallback(ImGuiInputTextCallbackData* data);
	bool _InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
	bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_::ImGuiInputTextFlags_None);
}