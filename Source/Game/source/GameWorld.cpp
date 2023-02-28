#include "stdafx.h"

#include "GameWorld.h"
#include <tge/graphics/GraphicsEngine.h>
#include <tge/drawers/SpriteDrawer.h>
#include <tge/texture/TextureManager.h>
#include <tge/drawers/DebugDrawer.h>

void ImGuiExtra::InputVec2(const char* label, Tga::Vector2f& vector)
{
	float* tempVec[2] = {};
	tempVec[0] = &vector.x;
	tempVec[1] = &vector.y;
	ImGui::InputFloat2(label, *tempVec);
}

void ImGuiExtra::InputColor(const char* label, Tga::Color& color)
{
	float* tempVec[4] = {};
	tempVec[0] = &color.myR;
	tempVec[1] = &color.myG;
	tempVec[2] = &color.myB;
	tempVec[3] = &color.myA;
	ImGui::ColorEdit4(label, *tempVec);
}

int ImGuiExtra::InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGuiExtra::_InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	ImGuiExtra::InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, ImGuiExtra::InputTextCallback, &cb_user_data);
}

bool ImGuiExtra::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
	return _InputText(label, str, flags, ResizeCallback, str->data());
}

GameWorld::GameWorld()
{
	p.lifetime = 2.f;
	p.initialVelocity = Tga::Vector2f(0.f, 5.f);
	p.deltaVelocity = Tga::Vector2f(0.f, -25.f);
	p.randomVelocity = Tga::Vector2f(10.f, 7.5f);
	p.randomPosition = Tga::Vector2f(50, 50);
	p.startSize = Tga::Vector2f(1.f, 1.f);
	p.endSize = Tga::Vector2f(0.25f, .25f);
	p.startColor = Tga::Color(1.f, 0.93333333333f, 0.67450980392f, 0.5f);
	p.endColor = Tga::Color(0.05490196078f, 0.59215686274f, 0.90980392156f, 0.f);

	auto& engine = *Tga::Engine::GetInstance();

	engine.SetClearColor(Tga::Color(0.f, 0.f, 0.f, 1.f));

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };

	ImGui::GetStyle().WindowRounding = 5.f;
	ImGui::GetStyle().PopupRounding = 5.f;
	ImGui::GetStyle().ChildRounding = 2.5f;
	ImGui::GetStyle().FrameRounding = 2.5f;
	ImGui::GetStyle().ScrollbarRounding = 2.5f;
	ImGui::GetStyle().TabRounding = 2.5f;
	//ImGui::GetIO().DisplaySize = ImVec2(resolution.x * .2f, resolution.y * .8f);
}

GameWorld::~GameWorld() 
{}

void GameWorld::Init()  
{
	auto& engine = *Tga::Engine::GetInstance();

	Tga::Vector2ui intResolution = engine.GetRenderSize();
	Tga::Vector2f resolution = { (float)intResolution.x, (float)intResolution.y };
	{
		sharedData = Tga::SpriteSharedData();
		sharedData.myTexture = engine.GetTextureManager().GetTexture(Tga::Settings::GetAssetW(particleSprite).c_str());

		myTGELogoInstance.myPivot = { 0.5f, 0.5f };
		myTGELogoInstance.myPosition = Tga::Vector2f{ 0.5f, 0.5f }*resolution;
		myTGELogoInstance.mySize = Tga::Vector2f{ 0.75f, 0.75f }*resolution.y;
		myTGELogoInstance.myColor = Tga::Color(1, 1, 1, 1);
	}

	ps = ParticleSystem(sharedData, maxParticles);
	ps.GetPosition() = resolution / 2;

	dbgText.SetColor({ 1, 1, 1, 1 });
	dbgText.SetPosition({ 4, 25 });

	simulate = true;
}
void GameWorld::Update(float aTimeDelta)
{
	ps.Update(aTimeDelta);

	while (t >= spawnRate)
	{
		ps.Emit(p);
		t -= spawnRate;
	}

	t += aTimeDelta;

	dbgText.SetText("Active Particles: " + std::to_string(ps.myActiveParticles) + "\nTS: " + std::to_string(aTimeDelta));
	
	ImGui::GetIO().DeltaTime = aTimeDelta;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (openSave && !openLoad)
		SaveParticleSystem();

	if (openLoad && !openSave)
		LoadParticleSystem();

	ImGui::Begin("Particle Editor", &openWindow, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Reload..")) Init();
			if (ImGui::MenuItem("Open..")) openLoad = true;
			if (ImGui::MenuItem("Save..")) openSave = true;

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	//ImGui::Checkbox("Simulate", &simulate);

	ImGuiExtra::InputText("Particle Sprite", &particleSprite);
	ImGui::InputInt("Max Particles", &maxParticles);

	ImGui::Text("^^^ These fields require a reset to take effect ^^^");

	ImGui::Spacing();
	ImGui::Spacing();

	float spawnTemp = spawnRate;
	ImGui::InputFloat("Spawn Rate", &spawnTemp);
	if (spawnTemp > 0)
		spawnRate = spawnTemp;

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::InputFloat("Lifetime", &p.lifetime);
	
	ImGuiExtra::InputVec2("Initial Velocity", p.initialVelocity);
	ImGuiExtra::InputVec2("Delta Velocity", p.deltaVelocity);
	ImGuiExtra::InputVec2("Random Velocity", p.randomVelocity);
	ImGuiExtra::InputVec2("Random Position", p.randomPosition);

	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Size");
	{
		ImGuiExtra::InputVec2("Start", p.startSize);
		ImGuiExtra::InputVec2("End", p.endSize);
	}
	
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("Color");
	{
		ImGuiExtra::InputColor("Start", p.startColor);
		ImGuiExtra::InputColor("End", p.endColor);
	}

	ImGui::End();
}

void GameWorld::Render()
{
	auto &engine = *Tga::Engine::GetInstance();
	Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
	// Game update
	{
		ps.Render(spriteDrawer);
		dbgText.Render();

		if (!simulate)
			return;

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

void GameWorld::SaveParticleSystem()
{
	ImGui::Begin("Save Particle", &openSave, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::BeginChild("saveWindow", ImVec2(200, 180));
	
	ImGuiExtra::InputText("File Name", &particleName);

	std::string path = ".\\Particles\\" + particleName + ".part";
	if (std::filesystem::exists(path))
		ImGui::TextColored(ImVec4(1, .9f, 0, 1), "WARNING: That file already exists");

	if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth(), 30)))
	{
		json j = json::object(
			{
				{"mp", maxParticles}, // Max particles
				{"sr", spawnRate}, // Spawn rate
				{"ps", particleSprite}, // Particle sprite
				{"l", p.lifetime}, // Lifetime
				{"iv", { // Initial velocity
						{"x", p.initialVelocity.x},
						{"y", p.initialVelocity.y}
					}
				},
				{"dv", { // Delta velocity
						{"x", p.deltaVelocity.x},
						{"y", p.deltaVelocity.y}
					}
				},
				{"rv", { // Random velocity
						{"x", p.randomVelocity.x},
						{"y", p.randomVelocity.y}
					}
				},
				{"rp", { // Random position
						{"x", p.randomPosition.x},
						{"y", p.randomPosition.y}
					}
				},
				{"ss", { // Start size
						{"x", p.startSize.x},
						{"y", p.startSize.y}
					}
				},
				{"es", { // End size
						{"x", p.endSize.x},
						{"y", p.endSize.y}
					}
				},
				{"sc", { // Start color
						{"r", p.startColor.myR},
						{"g", p.startColor.myG},
						{"b", p.startColor.myB},
						{"a", p.startColor.myA}
					}
				},
				{"ec", { // End color
						{"r", p.endColor.myR},
						{"g", p.endColor.myG},
						{"b", p.endColor.myB},
						{"a", p.endColor.myA}
					}
				}
			}
		);

		std::ofstream o = std::ofstream(path);
		o << j.dump();
		o.close();

		openSave = false;
	}

	ImGui::EndChild();
	ImGui::End();
}

void GameWorld::LoadParticleSystem()
{
	ImGui::Begin("Load Particle", &openLoad, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::BeginChild("loadWindow", ImVec2(200, 180));

	ImGuiExtra::InputText("File Name", &loadName);

	std::string path = ".\\Particles\\" + loadName + ".part";
	if (std::filesystem::exists(std::filesystem::path(path)))
	if (ImGui::Button("Load", ImVec2(ImGui::GetWindowWidth(), 30)))
	{
		std::ifstream i = std::ifstream(path);
		json j = json::parse(i);
		i.close();

		spawnRate = j["sr"];
		maxParticles = j["mp"];
		particleSprite = j["ps"];

		p.lifetime = j["l"];
		p.initialVelocity = Tga::Vector2f(j["iv"]["x"], j["iv"]["y"]);
		p.deltaVelocity = Tga::Vector2f(j["dv"]["x"], j["dv"]["y"]);
		p.randomVelocity = Tga::Vector2f(j["rv"]["x"], j["rv"]["y"]);
		p.randomPosition = Tga::Vector2f(j["rp"]["x"], j["rp"]["y"]);
		p.startSize = Tga::Vector2f(j["ss"]["x"], j["ss"]["y"]);
		p.endSize = Tga::Vector2f(j["es"]["x"], j["es"]["y"]);
		p.startColor = Tga::Color(j["sc"]["r"], j["sc"]["g"], j["sc"]["b"], j["sc"]["a"]);
		p.endColor = Tga::Color(j["ec"]["r"], j["ec"]["g"], j["ec"]["b"], j["ec"]["a"]);

		openLoad = false;
		Init();
	}

	ImGui::EndChild();
	ImGui::End();
}