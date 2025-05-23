#include "UltraEngine.h"
#include "Application.h"


Application::Application() {
	keyWidth = 32;
	isSaved = true;
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
	int windowPosX = settingsManager->windowPosX;
	int windowPosY = settingsManager->windowPosY;
	if (windowPosX == 0) {
		windowPosX = displays[displayId]->GetSize().x / 2 - settingsManager->windowWidth / 2;
	}
	if (windowPosY == 0) {
		windowPosY = displays[displayId]->GetSize().y / 2 - settingsManager->windowHeight / 2;
	}
	window = CreateWindow(APPLICATION_NAME, windowPosX, windowPosY, settingsManager->windowWidth, settingsManager->windowHeight, displays[displayId], WINDOW_TITLEBAR | WINDOW_RESIZABLE);
	if (settingsManager->isMaximized) {
		window->Maximize();
	}
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
		if (!isSaved) {
			notSavedDialog->setFunctionListener([this](Event event) {
				newFile();
				return true;
				});
			notSavedDialog->SetHidden(false);
		} else {
			newFile();
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
		if (!isSaved) {
			notSavedDialog->setFunctionListener([this](Event event) {
				openFile();
				return true;
				});
			notSavedDialog->SetHidden(false);
		} else {
			openFile();
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

	buttonName = resourceManager->getLocalString("Settings");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	shared_ptr<CustomButton> settingsButton = CustomButton::create(buttonPositionX, 1, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	settingsButton->SetText(buttonName);
	settingsButton->setListener([this](Event event) {
		settingsDialog->SetHidden(false);
		return true;
		});

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
		auto title = resourceManager->getLocalString("EditLocalStringDialog.Title.Add");
		editDialog->setTitle(title + " - " + currentLocalization);
		editDialog->SetHidden(false);
		return true;
		});

	buttonName = resourceManager->getLocalString("EditString");
	buttonPositionX = buttonPositionX + buttonWidth + indent;
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) + indent;
	editStringButton = CustomButton::create(buttonPositionX, postitionY, buttonWidth, buttonHeight, ui->root, WIDGET_BORDER | WIDGET_BACKGROUND | WIDGET_BORDER_ROUNDED);
	editStringButton->SetText(buttonName);
	editStringButton->enable(false);
	editStringButton->setListener([this](Event event) {
		if (!table->getSelectedItem().empty()) {
			auto dataRow = table->getSelectedItem();
			editDialog->setLocalString(dataRow[0], dataRow[1]);
			editDialog->localStringId = event.data;
			auto title = resourceManager->getLocalString("EditLocalStringDialog.Title");
			editDialog->setTitle(title + " - " + currentLocalization);
			editDialog->SetHidden(false);
		}
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
			isSaved = false;
			updateWindowName();
			saveFileButton->enable(true);
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
			auto title = resourceManager->getLocalString("EditLocalStringDialog.Title");
			editDialog->setTitle(title + " - " + currentLocalization);
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
	std::weak_ptr<Container> tableContainerWeak = tableContainer;
	filterField->setValueChangeListener([tableWeak, tableContainerWeak](Event event) {
		auto table = tableWeak.lock();
		auto tableContainer = tableContainerWeak.lock();
		if (table) {
			table->Redraw();
			table->deselect();
			tableContainer->resetScrollPosition();
		}
		return true;
		});
	std::weak_ptr<CustomTextField> filterFieldWeak = filterField;
	filterResetButton->setListener
	([tableWeak, filterFieldWeak, tableContainerWeak](Event event) {
		filterFieldWeak.lock()->SetText("");
		auto table = tableWeak.lock();
		auto tableContainer = tableContainerWeak.lock();
		if (table) {
			tableContainer->Redraw();
			table->deselect();
			tableContainer->resetScrollPosition();
			table->Redraw();
		}
		return true;
		}
	);
	int editDialogWidth = guiScale * 10;
	int editDialogHeight = guiScale * 5;

	//should be last for correct draw order
	localizationComboBox = CustomComboBox<WString>::create(localizationComboBoxPositionX, localizationComboBoxPositionY, guiScale * 2, buttonHeight, ui->root);
	localizationComboBox->Disable();
	localizationComboBox->setListener([this]([[maybe_unused]] Event const& event) {
		auto language = localizationComboBox->getSelectedItem().first;
		if (language != currentLocalization) {
			setLocalizationToTable(language);
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
			table->selectDataRow(editDialog->getLocalString());
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
		saveFileButton->enable(true);
		isSaved = false;
		updateWindowName();
		editDialog->SetHidden(true);
		return true;
		});

	settingsDialog = SettingsDialog::create(width / 2 - editDialogWidth / 2, height / 2 - editDialogHeight / 2, editDialogWidth, editDialogHeight, ui->root);
	settingsDialog->SetHidden(true);
	settingsDialog->setOkListener([this]([[maybe_unused]] Event const& event) {
		return true;
		});

	settingsDialog->languageComboBox->setListener([this]([[maybe_unused]] Event const& event) {
		if (!isSaved) {
			notSavedDialog->setFunctionListener([this](Event event) {
				switchLanguage();
				return true;
				});
			notSavedDialog->SetHidden(false);
		} else {
			switchLanguage();
		}
		return true;
		});

	settingsDialog->stringHeightField->setValueChangeListener([this](Event event) {
		int value = settingsDialog->stringHeightField->GetText().ToInt();
		if (value > 5 && settingsManager->stringHeight != value) {
			settingsManager->stringHeight = value;
			editDialog->updateContentSize();
			settingsManager->saveConfig();
		}
		return true;
		});

	settingsDialog->maxStringWidthField->setValueChangeListener([this](Event event) {
		int value = settingsDialog->maxStringWidthField->GetText().ToInt();
		if (value > 15 && settingsManager->maxStringWidth != value) {
			settingsManager->maxStringWidth = value;
			editDialog->updateContentSize();
			settingsManager->saveConfig();
		}
		return true;
		});

	settingsDialog->stringsCountField->setValueChangeListener([this](Event event) {
		int value = settingsDialog->stringsCountField->GetText().ToInt();
		if (value > 0 && settingsManager->stringsCount != value) {
			settingsManager->stringsCount = value;
			editDialog->updateContentSize();
			settingsManager->saveConfig();
		}
		return true;
		});

	settingsDialog->resolutiontWidthField->setValueChangeListener([this](Event event) {
		int value = settingsDialog->resolutiontWidthField->GetText().ToInt();
		if (value > 1280 && settingsManager->windowWidth != value) {
			settingsManager->windowWidth = value;
			window->SetShape(window->GetPosition().x, window->GetPosition().y, value, window->GetSize().height);
			settingsManager->saveConfig();
		}
		return true;
		});

	settingsDialog->resolutiontHeightField->setValueChangeListener([this](Event event) {
		int value = settingsDialog->resolutiontHeightField->GetText().ToInt();
		if (value > 720 && settingsManager->windowHeight != value) {
			settingsManager->windowHeight = value;
			window->SetShape(window->GetPosition().x, window->GetPosition().y, window->GetSize().width, value);
			settingsManager->saveConfig();
		}
		return true;
		});

	saveLabel = CustomLabel::create(guiScale, guiScale, buttonWidth, guiScale, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	saveLabel->setLocalText("FilesSaved", true);
	saveLabel->SetHidden(true);
	saveLabel->setBackground(true);
	saveLabel->SetColor(Vec4(0.1, 0.1, 0.1, 0.75));

	newFileButton->setLocalHintText("NewFile.Hint", true, true);
	openFileButton->setLocalHintText("OpenFile.Hint", true, true);
	saveFileButton->setLocalHintText("SaveFile.Hint", true, true);
	addStringButton->setLocalHintText("AddString.Hint", true, true);
	editStringButton->setLocalHintText("EditString.Hint", true, true);
	removeStringButton->setLocalHintText("RemoveString.Hint", true, true);

	int notSavedDialogWidth = guiScale * 10;
	int notSavedDialogHeight = guiScale * 3;
	notSavedDialog = SaveDialog::create(width / 2 - notSavedDialogWidth / 2, height / 2 - notSavedDialogHeight / 2, notSavedDialogWidth, notSavedDialogHeight, ui->root);
	notSavedDialog->SetHidden(true);
	notSavedDialog->setSaveListener([this](Event event) {
		saveLocalizations();
		return true;
		});

	if (!settingsManager->lastFilePath.empty()) {
		loadLocalization(settingsManager->lastFilePath);
	}

	editDialog->updateContentSize();

	std::weak_ptr<CustomButton> newFileButtonWeak = newFileButton;
	controlsManager->setActionListener(ACTION_NEW_FILE, Self(), [this, newFileButtonWeak](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			newFileButtonWeak.lock()->useListener();
		}
		});

	std::weak_ptr<CustomButton> openFileButtonWeak = openFileButton;
	controlsManager->setActionListener(ACTION_OPEN_FILE, Self(), [this, openFileButtonWeak](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			openFileButtonWeak.lock()->useListener();
		}
		});

	std::weak_ptr<CustomButton> saveFileButtonWeak = saveFileButton;
	controlsManager->setActionListener(ACTION_SAVE_FILES, Self(), [this, saveFileButtonWeak](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			saveFileButtonWeak.lock()->useListener();
		}
		});

	controlsManager->setActionListener(ACTION_ADD_STRING, Self(), [this](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			addStringButton->useListener();
		}
		});

	controlsManager->setActionListener(ACTION_EDIT_STRING, Self(), [this](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			editStringButton->useListener();
		}
		});

	controlsManager->setActionListener(ACTION_DELETE_STRING, Self(), [this](Event event) {
		if (editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			removeStringButton->useListener();
		}
		});

	controlsManager->setActionListener(ACTION_SAVE_EDIT, Self(), [this](Event event) {
		if (!editDialog->GetHidden() && controlsManager->isActionKeyDown(ACTION_CONTROL)) {
			editDialog->useOkListener();
		}
		});
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
					if (!isSaved) {
						notSavedDialog->setFunctionListener([this](Event event) {
							exit(0);
							return true;
							});
						notSavedDialog->SetHidden(false);
					} else {
						exit(0);
					}
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
				settingsManager->isMaximized = window->Maximized();
				updateSizes();
				break;
			}
			case EVENT_WINDOWMOVE:
			{
				settingsManager->windowPosX = window->GetPosition().x;
				settingsManager->windowPosY = window->GetPosition().y;
				settingsManager->saveConfig();
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

	editDialog->setPosition(witdh / 2 - editDialog->getWidth() / 2, height / 2 - editDialog->getHeight() / 2);
	editDialog->updateModelPanel();

	saveLabel->setPosition(witdh / 2 - saveLabel->getWidth() / 2, height / 2 - saveLabel->getHeight() / 2);
	notSavedDialog->setPosition(witdh / 2 - notSavedDialog->getWidth() / 2, height / 2 - notSavedDialog->getHeight() / 2);
	notSavedDialog->updateModelPanel();

	settingsDialog->setPosition(witdh / 2 - settingsDialog->getWidth() / 2, height / 2 - settingsDialog->getHeight() / 2);
	settingsDialog->updateModelPanel();

	settingsDialog->resolutiontWidthField->SetText(window->GetSize().width);
	settingsDialog->resolutiontHeightField->SetText(window->GetSize().height);
	settingsManager->windowWidth = settingsDialog->resolutiontWidthField->GetText().ToInt();
	settingsManager->windowHeight = settingsDialog->resolutiontHeightField->GetText().ToInt();

	settingsManager->saveConfig();
}

void Application::loadLocalization(WString file) {
	isSaved = true;
	saveFileButton->enable(false);
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
	updateWindowName();
	addStringButton->enable(true);
	removeStringButton->enable(true);
	editStringButton->enable(true);
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
				if (line.empty()) {
					continue;
				}
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
	for (auto& [language, languageFile] : languages) {
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
	saveLabel->SetHidden(false);
	if (saveLabelTimer) {
		saveLabelTimer->Stop();
	}
	saveLabelTimer = UltraEngine::CreateTimer(HINT_MAX_TIME);
	ListenEvent(EVENT_TIMERTICK, saveLabelTimer, saveLabelCallback);
	saveFileButton->enable(false);
	isSaved = true;
	updateWindowName();
}

void Application::newFile() {
	WString file = RequestFile(resourceManager->getLocalString("NewFile.Dialog.Title"), settingsManager->lastFilePath, resourceManager->getLocalString("OpenFile.Dialog.FileType"), 0, true);
	if (!file.empty()) {
		WriteFile(file);
		loadLocalization(file);
	}
}

void Application::openFile() {
	WString file = RequestFile(resourceManager->getLocalString("OpenFile.Dialog.Title"), settingsManager->lastFilePath, resourceManager->getLocalString("OpenFile.Dialog.FileType"));
	if (!file.empty()) {
		loadLocalization(file);
	}
}

void Application::switchLanguage() {
	auto language = settingsDialog->languageComboBox->getSelectedItem().first;
	if (language != settingsManager->language) {
		settingsManager->language = language;
		settingsManager->saveConfig();
		RunFile(APPLICATION_NAME + ".exe");
		exit(0);
	}
}

void Application::updateWindowName() {
	WString asterix = isSaved ? "" : "*";
	window->SetText(APPLICATION_NAME + " - " + settingsManager->lastFilePath + asterix);
}

bool Application::saveLabelCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra) {
	auto app = Application::getInstance();
	app->saveLabelTimer->Stop();
	app->saveLabelTimer = nullptr;
	app->saveLabel->SetHidden(true);
	return false;
}

