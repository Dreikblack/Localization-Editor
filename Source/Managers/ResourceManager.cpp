#include "UltraEngine.h"
#include "ResourceManager.h"
#include "SettingsManager.h"
#include "../UI/CustomWidgets/Paint.h"
#include "../resource.h"

using namespace UltraEngine;

ResourceManager::ResourceManager() {
	initLocalization();
}

ResourceManager::~ResourceManager() = default;

struct StructResourceBundle : public ResourceManager {
};

std::shared_ptr<ResourceManager> ResourceManager::getInstance() {
	static std::shared_ptr<ResourceManager> instance = std::make_shared<StructResourceBundle>();
	return instance;
}

void ResourceManager::initLocalization() {
	localizationPath = AppDir() + "/Localization/";
	auto dir = LoadDir(localizationPath);
	for (WString localFile : dir) {
		if (ExtractExt(localFile) == WString::WString("local")) {
			localNames.push_back(StripExt(localFile));
		}
	}
	setLocalizationLanguage(SettingsManager::getInstance()->language);
}

std::map<WString, WString> ResourceManager::getNewLocalization(WString path) {
	std::map<WString, WString> newLocalMap;
	auto stream = ReadFile(path);
	if (!stream) {
		return newLocalMap;
	}
	WString text = stream->ReadWString();
	vector<WString> textStrings = text.Split("\r\n");
	for (WString line : textStrings) {
		vector<WString> entitiyProperties = line.Split("=");
		if (entitiyProperties.size() > 1) {
			WString newLine = entitiyProperties[1].Replace("\\n", "\n");
			newLocalMap[entitiyProperties[0]] = newLine;
		} else {
			newLocalMap[entitiyProperties[0]] = "";
		}
	}
	stream->Close();
	return newLocalMap;
}


void ResourceManager::setLocalizationLanguage(WString local) {
	bool isFound = false;
	for (auto& localization : localNames) {
		if (localization == local) {
			localizationMap = getNewLocalization(localizationPath + local + "." + LOCALIZATION_FORMAT);
			isFound = true;

		}
	}
	if (!isFound && !localNames.empty()) {
		localizationMap = getNewLocalization(localizationPath + localNames[0] + "." + LOCALIZATION_FORMAT);
	}
}

WString ResourceManager::getLocalString(WString line) {
	std::map<WString, WString>::iterator it;
	it = localizationMap.find(line);
	if (it == localizationMap.end()) {
		return line;
	}
	return it->second;
}

WString ResourceManager::getLocalString(WString line, vector<WString> params) {
	auto localString = getLocalString(line);
	for (int i = 0; i < params.size(); i++) {
		WString replaceString = WString("{") + WString(i) + WString("}");
		localString = localString.Replace(replaceString, params[i]);
	}
	return localString;
}

std::vector<WString> ResourceManager::getAvaiableLanguages() {
	return localNames;
}


shared_ptr<Icon> ResourceManager::getGuiIcon(WString iconName) {
	return LoadIcon(guiIconsPath + iconName + ".svg");
}


shared_ptr<Sound> ResourceManager::getSound(WString soundName) {
	if (soundMap.find(soundName) == soundMap.end()) {
		soundMap[soundName] = LoadSound("/Sound/" + soundName + ".wav");
	}
	return soundMap[soundName];
}

void ResourceManager::playSound(WString soundName, float soundVolume) {
	if (soundName.empty()) return;
	auto sound = getSound(soundName);
	if (!sound) return;
	if (!effectSpeaker) {
		effectSpeaker = CreateSpeaker(sound);
	} else {
		effectSpeaker->Stop();
		effectSpeaker->SetSound(sound);
	}
	effectSpeaker->SetVolume(soundVolume * (float)SettingsManager::getInstance()->effectVolume / (float)MAX_VOLUME);
	effectSpeaker->Play();
}

