#pragma once
#include "UltraEngine.h"

using namespace UltraEngine;

using json = nlohmann::json;

class ResourceManager : public Object {
protected:
	shared_ptr<Plugin> fITextureLoaderPlugin;
	shared_ptr<Speaker> effectSpeaker;
	WString guiIconsPath = "Icons//Menu//";

	WString saveFilePath;

	std::map<WString, shared_ptr<Sound>> soundMap;

	void initLocalization();
	WString localizationPath;
	std::map<WString, WString> getNewLocalization(WString path);
	std::vector<WString> localNames;
	std::map<WString, WString> localizationMap;

	ResourceManager();
	~ResourceManager() override;
public:
	ResourceManager(ResourceManager const&) = delete;
	ResourceManager& operator=(ResourceManager const&) = delete;
	static std::shared_ptr<ResourceManager> getInstance();
	void setLocalizationLanguage(WString local);
	WString getLocalString(WString line);
	WString getLocalString(WString line, vector<WString> params);
	std::vector<WString> getAvaiableLanguages();
	shared_ptr<Icon> getGuiIcon(WString iconName);
	shared_ptr<Sound> getSound(WString soundName);
	void playSound(WString soundName, float soundVolume = 1.0f);
};
