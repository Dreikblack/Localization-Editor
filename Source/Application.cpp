#include "UltraEngine.h"
#include "Application.h"


Application::Application() {
	keyWidth = 32;
}

struct StructApplication : public Application {
};

std::shared_ptr<Application> Application::getInstance() {
	static std::shared_ptr<Application> instance = std::make_shared<StructApplication>();
	return instance;
}


void Application::init() {
	resourceManager = ResourceManager::getInstance();
	settingsManager = SettingsManager::getInstance();
	controlsManager = ControlsManager::getInstance();
	controlsManager->init();

	auto displays = GetDisplays();
	int displayId = settingsManager->displayId < displays.size() ? settingsManager->displayId : 0;
	window = CreateWindow(APPLICATION_NAME, 0, 0, settingsManager->windowWidth, settingsManager->windowHeight, displays[displayId], WINDOW_TITLEBAR | WINDOW_CENTER | WINDOW_RESIZABLE);
	ui = CreateInterface(window);
	ui->root->SetColor(0.3f, 0.3f, 0.3f);

	int indent = settingsManager->guiScale / 8;
	int guiScale = settingsManager->guiScale;
	int halfGuiScale = guiScale / 2;
	int buttonHeight = settingsManager->guiScale / 2;
	int width = ui->root->ClientSize().width;
	int height = ui->root->ClientSize().height;

	int buttonPositionX = 1;
	auto newFileButton = CustomButton::create(buttonPositionX, 1, settingsManager->guiScale, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	newFileButton->setListener([this](Event event) {
		WString file = RequestFile(resourceManager->getLocalString("NewFile.Dialog.Title"), settingsManager->lastFilePath, resourceManager->getLocalString("OpenFile.Dialog.FileType"), 0, true);
		if (!file.empty()) {
			WriteFile(file);
			loadLocalization(file);
		}
		return true;
		});
	auto buttonName = resourceManager->getLocalString("NewFile");
	int buttonWidth = ui->GetTextWidth(ui->font, newFileButton->getFontScale(), buttonName, 1) + indent;
	newFileButton->setWidth(buttonWidth);
	newFileButton->SetText(buttonName);

	buttonName = resourceManager->getLocalString("OpenFile");
	float fontScale = newFileButton->getFontScale();
	buttonPositionX = buttonPositionX + buttonWidth;
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	auto openFileButton = CustomButton::create(buttonPositionX, 1, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	openFileButton->SetText(buttonName);
	openFileButton->setListener([this](Event event) {
		WString file = RequestFile(resourceManager->getLocalString("OpenFile.Dialog.Title"), settingsManager->lastFilePath, resourceManager->getLocalString("OpenFile.Dialog.FileType"));
		if (!file.empty()) {
			loadLocalization(file);
		}
		return true;
		});

	buttonName = resourceManager->getLocalString("SaveFile");
	buttonPositionX = buttonPositionX + buttonWidth;
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	saveFileButton = CustomButton::create(buttonPositionX, 1, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	saveFileButton->SetText(buttonName);
	saveFileButton->enable(false);
	saveFileButton->setListener([this](Event event) {
		saveLocalizations();
		return true;
		});

	buttonPositionX = buttonPositionX + buttonWidth;
	buttonName = resourceManager->getLocalString("CurrentLanguage");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;
	auto localizationLabel = CustomLabel::create(buttonPositionX, 1, buttonWidth, buttonHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	localizationLabel->SetText(buttonName);

	buttonPositionX = buttonPositionX + buttonWidth;

	int localizationComboBoxPositionX = buttonPositionX;
	int localizationComboBoxPositionY = 1;

	buttonPositionX = buttonPositionX + guiScale * 2;
	buttonName = resourceManager->getLocalString("LanguageSetting");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;
	auto languageLabel = CustomLabel::create(buttonPositionX, 1, buttonWidth, buttonHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	languageLabel->SetText(buttonName);

	buttonPositionX = buttonPositionX + buttonWidth;

	int languageComboBoxPositionX = buttonPositionX;
	int languageComboBoxPositionY = 1;	

	buttonPositionX = 1;
	buttonName = resourceManager->getLocalString("KeyFilter");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	int postitionY = buttonHeight + indent;
	auto filterLabel = CustomLabel::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	filterLabel->SetText(buttonName);
	
	buttonPositionX = buttonPositionX + buttonWidth + indent;
	buttonWidth = guiScale * 5;
	filterField = CustomTextField::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root);
	filterField->doTriggerValueChangeOnType = true;

	buttonPositionX = buttonPositionX + buttonWidth;
	buttonWidth = halfGuiScale;
	auto filterResetButton = CustomButton::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER);
	auto crossIcon = resourceManager->getGuiIcon("Cross");
	if (crossIcon) {
		auto color = filterResetButton->GetColor(WIDGETCOLOR_BACKGROUND);
		crossIcon->SetColor(color.x, color.y, color.z, color.w);
		filterResetButton->SetIcon(crossIcon, PIXMAP_CENTER, (float)halfGuiScale / (float)GUI_SCALE);
	}

	buttonName = resourceManager->getLocalString("AddString");
	buttonPositionX = buttonPositionX + buttonWidth + indent;
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	addStringButton = CustomButton::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	addStringButton->SetText(buttonName);
	addStringButton->enable(false);
	addStringButton->setListener([this](Event event) {
		editDialog->setLocalString("Key", "Content");
		editDialog->localStringId = -1;
		editDialog->setLocalTextTitle("EditLocalStringDialog.Title.Add");
		editDialog->SetHidden(false);
		return true;
		});

	buttonName = resourceManager->getLocalString("RemoveString");
	buttonPositionX = buttonPositionX + buttonWidth + indent;
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	removeStringButton = CustomButton::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	removeStringButton->SetText(buttonName);
	removeStringButton->enable(false);
	removeStringButton->setListener([this](Event event) {
		if (!table->getSelectedItem().empty()) {
			auto key = table->getSelectedItem()[0];
			keys.erase(key);
			for (auto& localMap : localizationsMap) {
				localMap.second.erase(key);
			}
			setLocalizationToTable(currentLocalization);
		}
		return true;
		});
	
	postitionY = postitionY + buttonHeight + indent;

	tableContainer = Container::create(indent, postitionY, width - indent, height - postitionY, ui->root, CONTAINER_SCROLL);
	keyWidth = guiScale * 5;
	table = CustomTable::create(0, 0, guiScale, guiScale, ui->root, { keyWidth, width - keyWidth - indent }, halfGuiScale, WIDGET_BACKGROUND);
	table->textAlignment = TEXT_LEFT;
	table->setHeaders({ resourceManager->getLocalString("Table.Key"), resourceManager->getLocalString("Table.Content") });
	table->setDoubleClickListener([this]([[maybe_unused]] Event const& event) {
		if (event.data != -1) {
			auto dataRow = table->getSelectedItem();
			editDialog->setLocalString(dataRow[0], dataRow[1]);
			editDialog->localStringId = event.data;
			editDialog->setLocalTextTitle("EditLocalStringDialog.Title");
			editDialog->SetHidden(false);
		}
		return true;
	});
	table->setFilter([this]([[maybe_unused]] vector<WString> dataRow) {
		if (filterField->GetText().empty()) {
			return true;
		} else {
			return dataRow[0].Lower().Find(filterField->GetText().Lower()) != -1;
		}
		});
	tableContainer->addWidget(table);
	std::weak_ptr<CustomTable> tableWeak = table;
	filterField->setValueChangeListener([tableWeak](Event event) {
		auto table = tableWeak.lock();
		if (table) {
			table->Redraw();
			table->deselect();
		}
		return true;
		});
	std::weak_ptr<CustomTextField> filterFieldWeak = filterField;
	filterResetButton->setListener
	([tableWeak, filterFieldWeak](Event event) {
		filterFieldWeak.lock()->SetText("");
		auto table = tableWeak.lock();
		if (table) {
			table->Redraw();
			table->deselect();
		}
		return true;
		}
	);
	int editDialogWidth = guiScale * 10;
	int editDialogHeight = guiScale * 10;

	//should be last for correct draw order
	localizationComboBox = CustomComboBox<WString>::create(localizationComboBoxPositionX, languageComboBoxPositionY, guiScale * 2, buttonHeight, ui->root);
	localizationComboBox->Disable();
	localizationComboBox->setListener([this]([[maybe_unused]] Event const& event) {
		auto language = localizationComboBox->getSelectedItem().first;
		if (language != currentLocalization) {
			setLocalizationToTable(language);
		}
		return true;
		});

	//should be last for correct draw order
	shared_ptr<CustomComboBox<WString>> languageComboBox = CustomComboBox<WString>::create(languageComboBoxPositionX, localizationComboBoxPositionY, guiScale * 2, buttonHeight, ui->root);
	std::weak_ptr<CustomComboBox<WString>> languageComboBoxWeak = languageComboBox;
	vector<std::pair<WString, WString>> localLanguages;
	std::pair<WString, WString> currentLanguage;
	for (auto& local : ResourceManager::getInstance()->getAvaiableLanguages()) {
		localLanguages.push_back(std::make_pair(local, local));
		if (local == settingsManager->language) {
			currentLanguage = std::make_pair(local, local);
		}
	}
	languageComboBox->addItems(localLanguages);
	languageComboBox->selectItem(currentLanguage);
	languageComboBox->setListener([languageComboBoxWeak]([[maybe_unused]] Event const& event) {
		auto language = languageComboBoxWeak.lock()->getSelectedItem().first;
		auto settingsManager = SettingsManager::getInstance();
		if (language != settingsManager->language) {
			settingsManager->language = language;
			settingsManager->saveConfig();
			RunFile(APPLICATION_NAME + ".exe");
			exit(0);
		}
		return true;
		});

	editDialog = EditLocalStringDialog::create(width / 2 - editDialogWidth / 2, height / 2 - editDialogHeight / 2, editDialogWidth, editDialogHeight, ui->root);
	editDialog->SetHidden(true);
	editDialog->setOkListener([this, tableWeak]([[maybe_unused]] Event const& event) {
		auto table = tableWeak.lock();
		if (!table) {
			return true;
		}
		WString newKey = editDialog->getLocalString()[0];
		WString newContent = editDialog->getLocalString()[1];

		if (editDialog->localStringId == -1) {
			keys.insert(newKey);
			for (auto& localMap : localizationsMap) {
				localMap.second[newKey] = newContent;
			}
			setLocalizationToTable(currentLocalization);
		} else {
			WString key = table->getSelectedItem()[0];
			if (key != newKey) {
				keys.erase(key);
				keys.insert(newKey);
				for (auto& localMap : localizationsMap) {
					auto nodeHandler = localMap.second.extract(key);
					nodeHandler.key() = newKey;
					localMap.second.insert(std::move(nodeHandler));
				}
			}
			WString content = table->getSelectedItem()[1];
			if (content != newContent) {
				localizationsMap[currentLocalization][newKey] = newContent;
			}
			table->updateDataRow(editDialog->localStringId, editDialog->getLocalString());
		}
		editDialog->SetHidden(true);
		return true;
		});

	if (!settingsManager->lastFilePath.empty()) {
		loadLocalization(settingsManager->lastFilePath);
	}
}

void Application::loop() {

	//Main loop
	while (window->Closed() == false) {
		controlsManager->clearActionUp();
		while (PeekEvent()) {
			const Event ev = WaitEvent();
			controlsManager->pollEvents(ev);
			switch (ev.id) {
			case EVENT_WINDOWCLOSE:
				if (ev.source == window) {
					exit(0);
					break;
				}
				break;		
			case EVENT_ERROR:
			{
				Notify(ev.text);
				break;
			}
			case EVENT_WINDOWDESELECT:
			{
				controlsManager->clearActionUp();
				controlsManager->clearActionDown();				
				break;
			}
			case EVENT_WINDOWSIZE:
			{
				updateSizes();
				break;
			}
			}
		}
	}
}

void Application::updateSizes() {
	int witdh = ui->root->ClientSize().width;
	int height = ui->root->ClientSize().height;
	int indent = settingsManager->guiScale / 8;
	table->setColumnWidthes({ keyWidth, witdh - keyWidth - indent });
	tableContainer->setSize(witdh - indent, height - tableContainer->getPositionY());
	tableContainer->updateInnerContainerSize();
	
}

void Application::loadLocalization(WString file) {
	settingsManager->lastFilePath = file;
	WString fileName = StripAll(file);
	settingsManager->saveConfig();
	WString path = ExtractDir(settingsManager->lastFilePath);
	auto dir = LoadDir(path);
	localizationsMap.clear();
	keys.clear();
	languages.clear();
	// To store the index of last character found
	size_t foundDot = fileName.find_last_of('.');
	WString langName;
	if (foundDot == string::npos) {
		langName = fileName;
	} else {
		langName = fileName.Left(foundDot);
	}
	window->SetText(APPLICATION_NAME + " - " + StripDir(file));
	saveFileButton->enable(true);
	addStringButton->enable(true);
	removeStringButton->enable(true);
	//keys
	for (WString localFile : dir) {
		if (ExtractExt(localFile) == WString::WString("local")) {
			WString localFileNoExt = StripExt(localFile);
			foundDot = localFileNoExt.find_last_of('.');
			WString fileLangName;
			WString language;
			if (foundDot == string::npos) {
				fileLangName = localFileNoExt;
				language = fileLangName;
			} else {
				fileLangName = localFileNoExt.Left(foundDot);
				language = localFileNoExt.Right(localFileNoExt.length() - foundDot - 1);
			}
			if (fileLangName != langName && language != fileLangName) {
				continue;
			}
			auto stream = ReadFile(path + "/" + localFile);
			if (!stream) {
				continue;
			}
			std::map<WString, WString> newLocalMap;
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
				keys.insert(entitiyProperties[0]);
			}
			stream->Close();
			localizationsMap[language] = newLocalMap;
			languages[language] = path + "/" + localFile;
		}
	}
	setLocalizationToTable(langName);
	localizationComboBox->Enable();
	vector<std::pair<WString, WString>> localLanguages;
	std::pair<WString, WString> currentLanguage;
	for (auto& [language, file] : languages) {
		localLanguages.emplace_back(language, language);
		if (language == langName) {
			currentLanguage = std::make_pair(language, language);
		}
	}
	localizationComboBox->clearItems();
	localizationComboBox->addItems(localLanguages);
	localizationComboBox->selectItem(currentLanguage);
}

void Application::setLocalizationToTable(WString language) {
	currentLocalization = language;
	vector<vector<WString>> data;
	int maxWidth = 32 * 10;
	float fontScale = table->getFontScale();
	for (auto const& key : keys) {
		maxWidth = Max(maxWidth, ui->GetTextWidth(ui->font, fontScale, key, 1));
		data.push_back({ key, localizationsMap[language][key]});
	}
	keyWidth = maxWidth;
	table->setData(data);
	updateSizes();
}

void Application::saveLocalizations() {
	for (auto& [language, file] : languages) {
		auto stream = WriteFile(file);
		for (auto& [key, content] : localizationsMap[language]) {
			WString newLine = content.Replace("\n", "\\n");
			stream->WriteWString(key + WString("=") + newLine + "\r\n", false);
		}		
		stream->Close();
	}
	
}

