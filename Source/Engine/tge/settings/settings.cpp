#include <stdafx.h>
#include "settings.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include <io.h>
#define access _access_s

namespace Tga {
	namespace Settings {
		static std::string engine_assets_path;
		static std::string game_assets_path;
		static nlohmann::json game_settings;
	}
}

std::string Tga::Settings::Window::title;

std::string Tga::Settings::GetAsset(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return anAsset;
	}
	else if (access((game_assets_path + anAsset).c_str(), 0) == 0) {
		return game_assets_path + anAsset;
	}
	else if (access((engine_assets_path + anAsset).c_str(), 0) == 0) {
		return engine_assets_path + anAsset;
	}
	return engine_assets_path + "Sprites/error.dds";
}

//std::string Tga::Settings::GetAsset(const char* anAsset) {
//	return GetAsset(std::string(anAsset));
//}

std::wstring Tga::Settings::GetAssetW(const std::string& anAsset) {
	if (access(anAsset.c_str(), 0) == 0) {
		return ToWstring(anAsset);
	}
	else if (access((game_assets_path + anAsset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path + anAsset);
	}
	else if (access((engine_assets_path + anAsset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path + anAsset);
	}
	return ToWstring(engine_assets_path + "Sprites/error.dds");
}

//std::wstring Tga::Settings::GetAssetW(const char* anAsset) {
//	std::string asset(anAsset);
//	return GetAssetW(std::wstring(asset.begin(), asset.end()));
//}

std::wstring Tga::Settings::GetAssetW(const std::wstring& anAsset) {
	std::string asset = FromWstring(anAsset);
	
	if (access(asset.c_str(), 0) == 0) {
		return anAsset;
	}
	if (access((game_assets_path + asset).c_str(), 0) == 0) {
		return ToWstring(game_assets_path + asset);
	}
	else if (access((engine_assets_path + asset).c_str(), 0) == 0) {
		return ToWstring(engine_assets_path + asset);
	}
	return ToWstring(engine_assets_path + "Sprites/error.dds");
}

//std::wstring Tga::Settings::GetAssetW(const wchar_t* anAsset) {
//	return GetAssetW(std::wstring(anAsset));
//}

std::string Tga::Settings::EngineAsset(const std::string& anAsset) {
	return std::string(engine_assets_path + anAsset);
}

std::wstring Tga::Settings::EngineAssetW(const std::string& anAsset) {
	return std::wstring(ToWstring(engine_assets_path) + ToWstring(anAsset));
}
extern std::wstring Tga::Settings::EngineAssetW(const std::wstring& anAsset) {
	return std::wstring(ToWstring(engine_assets_path) + anAsset);
}

std::string Tga::Settings::GameAsset(const std::string& anAsset) {
	return std::string(game_assets_path + anAsset);
}

std::wstring Tga::Settings::GameAssetW(const std::string& anAsset) {
	return Tga::ToWstring(game_assets_path + anAsset);
}
std::wstring Tga::Settings::GameAssetW(const std::wstring& anAsset) {
	return std::wstring(Tga::ToWstring(game_assets_path) + anAsset);
}

std::string Tga::Settings::GameSettings(const std::string& aKey) {
	return game_settings[aKey];
}
std::wstring Tga::Settings::GameSettingsW(const std::string& aKey) {
	std::string s = game_settings[aKey];
	return Tga::ToWstring(game_settings[aKey]);
}
std::wstring Tga::Settings::GameSettingsW(const std::wstring& aKey) {
	std::string s = game_settings[Tga::FromWstring(aKey)];
	return Tga::ToWstring(s);
}

void Tga::LoadSettings(const std::string& aProjectName) {
	using namespace Settings;
	/////////////////////
	// Engine settings
	{
		std::string path = std::string("settings/EngineSettings.json");
		std::ifstream engine_ifs(path.c_str());
		using namespace nlohmann;
		json settings = { json::parse(engine_ifs) };
		engine_ifs.close();

		Settings::engine_assets_path = settings["engine_assets_path"];
		Settings::Window::title = settings["window_settings"]["title"];

		std::cout << "Read settings from: " << Settings::engine_assets_path << std::endl;
		std::cout << "found window title to be: " << Settings::Window::title << std::endl;
	}

	/////////////////////
	// Game settings
	{
		std::string filename = (aProjectName.find(".") == std::string::npos) ? (aProjectName + ".json").c_str() : aProjectName.c_str();
		std::string path("settings/");
		
		std::ifstream game_ifs(path + filename);
		assert(game_ifs);

		game_ifs >> Settings::game_settings;
		game_ifs.close();
		game_assets_path = game_settings["assets_path"];
	}
}