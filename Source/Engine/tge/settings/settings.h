#pragma once

namespace Tga {
	extern void LoadSettings(const std::string &aProjectName);
	
	namespace Settings {
		namespace Window {
			extern std::string title;
		}
		//extern std::string game_assets_path;

		extern std::string GetAsset(const std::string& anAsset);
		//extern std::string GetAsset(const char* anAsset);

		extern std::wstring GetAssetW(const std::string& anAsset);
		//extern std::wstring GetAssetW(const char* anAsset);

		extern std::wstring GetAssetW(const std::wstring& anAsset);
		//extern std::wstring GetAssetW(const wchar_t* anAsset);

		extern std::string EngineAsset(const std::string& anAsset);
		extern std::wstring EngineAssetW(const std::string& anAsset);
		extern std::wstring EngineAssetW(const std::wstring& anAsset);

		extern std::string GameAsset(const std::string& anAsset);
		extern std::wstring GameAssetW(const std::string& anAsset);
		extern std::wstring GameAssetW(const std::wstring& anAsset);

		extern std::string GameSettings(const std::string& aKey);
		extern std::wstring GameSettingsW(const std::string& aKey);
		extern std::wstring GameSettingsW(const std::wstring& aKey);
	}
}