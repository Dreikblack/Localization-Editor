#pragma once
#include "UltraEngine.h"
#include "Managers/InterfaceManager.h"
#include "Managers/SettingsManager.h"
#include "Managers/ResourceManager.h"
#include "UI/CustomWidgets/ButtonPanel.h"
#include "UI/CustomWidgets/CustomButton.h"
#include "UI/CustomWidgets/CustomTextField.h"
#include "UI/CustomWidgets/CustomLabel.h"
#include "UI/CustomWidgets/CustomTable.h"
#include "UI/CustomWidgets/Container.h"
#include "UI/EditLocalStringDialog.h"
#include "UI/CustomWidgets/CustomComboBox.h"
#include "UI/CustomWidgets/CustomComboBox.cpp"

using namespace UltraEngine;

class Application : public Object {
protected:
	shared_ptr<ResourceManager> resourceManager;
	shared_ptr<SettingsManager> settingsManager;
	shared_ptr<ControlsManager> controlsManager;
	shared_ptr<Window> window;
	shared_ptr<Interface> ui;
	shared_ptr<Container> tableContainer;
	shared_ptr<CustomTable> table;
	shared_ptr<EditLocalStringDialog> editDialog;
	shared_ptr<CustomButton> saveFileButton;
	shared_ptr<CustomTextField> filterField;
	shared_ptr<CustomButton> addStringButton;
	shared_ptr<CustomButton> editStringButton;
	shared_ptr<CustomButton> removeStringButton;
	shared_ptr<CustomLabel> saveLabel;
	shared_ptr<Timer> saveLabelTimer;
	int keyWidth;
	//language - file path
	map<WString, WString> languages;
	WString currentLocalization;
	shared_ptr<CustomComboBox<WString>> localizationComboBox;
	//localization fileName -> localization map -> key:content
	std::map<WString, std::map<WString, WString>> localizationsMap;
	std::set<WString> keys;
	Application();
	void updateSizes();
	void loadLocalization(WString filePath);
	void setLocalizationToTable(WString language);
	void saveLocalizations();
public:
	static bool saveLabelCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra);
	Application(Application const&) = delete;
	Application& operator=(Application const&) = delete;
	static std::shared_ptr<Application> getInstance();
	void init();
	void loop();
};