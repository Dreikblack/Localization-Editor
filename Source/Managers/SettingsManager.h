#pragma once

#include "UltraEngine.h"
#include "../Interface/AllConstants.h"

using namespace UltraEngine;

using json = nlohmann::json;

class SettingsManager : public Object
{
protected:
	SettingsManager();
	void init();
	WString configPath;
	json configData;
public:
	SettingsManager(SettingsManager const&) = delete;
	SettingsManager& operator=(SettingsManager const&) = delete;
	void saveConfig();
	static std::shared_ptr<SettingsManager> getInstance();
	static json getJson(WString configPath);
	bool isMaximized = false;
	int windowPosX = 0;
	int windowPosY = 0;
	int windowWidth = 1280;
	int windowHeight = 720;
	short displayId = 0;
	int guiScale = GUI_SCALE;
	int fontHeight = FONT_SIZE;
	int effectVolume = 10;
	bool isSystemCursor = false;
	WString language;
	WString lastFilePath;
	int stringHeight = 32;
	int maxStringWidth = 640;
	int stringsCount = 8;
};
