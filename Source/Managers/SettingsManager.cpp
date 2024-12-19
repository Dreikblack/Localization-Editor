#include "UltraEngine.h"
#include "SettingsManager.h"
#ifdef _WIN32
#include <Windows.h>
#endif

SettingsManager::SettingsManager() {
	init();
}

struct StructSettingsManager : public SettingsManager {
};

void SettingsManager::init() {
	WString savesFolder = AppDir();
	WString configFolder = "Config";

	if (!ChangeDir(savesFolder)) {
		CreateDir(savesFolder);
	}
	ChangeDir(savesFolder);
	if (!ChangeDir(configFolder)) {
		CreateDir(configFolder);
	}
	ChangeDir(AppDir());

	configPath = savesFolder + "/Config/Config.json";

	try {
		configData = getJson(configPath);
	} catch (const std::invalid_argument& e) {
		configData = json();
	}

	try {
		windowWidth = configData[SETTINGS_WINDOW_WIDTH].get<int>();
		windowHeight = configData[SETTINGS_WINDOW_HEIGHT].get<int>();
		displayId = configData[SETTINGS_DISPLAY_ID].get<int>();
	} catch (const std::exception& e) {
		windowWidth = GetDisplays()[0]->GetSize().width * 0.5f;
		windowHeight = GetDisplays()[0]->GetSize().height * 0.5f;
		displayId = 0;
	}

	try {
		guiScale = configData[SETTINGS_GUI_SCALE].get<int>();
	} catch (const std::exception& e) {
		guiScale = GUI_SCALE;
	}

	try {
		language = configData[SETTINGS_LANGUAGE].get<string>();
	} catch (const std::exception& e) {
		language = "English";
#ifdef _WIN32
		int ret = GetSystemDefaultLangID();
		if (ret == 1049) {
			language = L"Русский";
		}
#endif
	}

	try {
		lastFilePath = configData[SETTINGS_LAST_PATH].get<string>();
	} catch (const std::exception& e) {
		lastFilePath = "";
	}

	try {
		effectVolume = configData[SETTINGS_SOUND_EFFECT_VOLUME].get<int>();
	} catch (const std::exception& e) {
		effectVolume = MAX_VOLUME;
	}
}

json SettingsManager::getJson(WString configPath) {
	auto stream = ReadFile(configPath);
	if (!stream) {
		throw std::invalid_argument("No config file");
	}
	WString configString = stream->ReadString();
	stream->Close();
	return json::parse(configString.ToUtf8String());
}

std::shared_ptr<SettingsManager> SettingsManager::getInstance() {
	static std::shared_ptr<SettingsManager> instance = std::make_shared<StructSettingsManager>();
	return instance;
}

void SettingsManager::saveConfig() {
	configData[SETTINGS_WINDOW_WIDTH] = windowWidth;
	configData[SETTINGS_WINDOW_HEIGHT] = windowHeight;
	configData[SETTINGS_GUI_SCALE] = guiScale;
	configData[SETTINGS_LANGUAGE] = language.ToUtf8String();
	configData[SETTINGS_DISPLAY_ID] = displayId;
	configData[SETTINGS_SOUND_EFFECT_VOLUME] = effectVolume;
	configData[SETTINGS_LAST_PATH] = lastFilePath.ToUtf8String();
	auto stream = WriteFile(configPath);
	stream->WriteString(configData.dump(), false);
	stream->Close();
}