#include "UltraEngine.h"
#include "SettingsDialog.h"
#include "CustomWidgets/CustomLabel.h"
#include "../Managers/ResourceManager.h"
#include "../Managers/SettingsManager.h"

SettingsDialog::SettingsDialog() {

}

SettingsDialog::~SettingsDialog() = default;

std::shared_ptr<SettingsDialog> SettingsDialog::create(const int x, const int y, const int width, const int height,
	shared_ptr<Widget> parent)
{
	struct Struct : public SettingsDialog {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent);
	return instance;
}

bool SettingsDialog::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
	bool isInit = Dialog::Initialize(x, y, width, height, parent, DIALOG_MODAL | DIALOG_ESC_CLOSE | DIALOG_CROSS_ONLY);
	setLocalTextTitle("SettingsDialog.Title");

	setCancelListener([this]([[maybe_unused]] Event const& event) {
		SetHidden(true);
		return true;
		});

	auto resourceManager = ResourceManager::getInstance();
	auto settingsManager = SettingsManager::getInstance();

	auto ui = GetInterface();
	auto rowHeight = guiScale / 2;
	int rowId = 0;
	float fontScale = (float)rowHeight / (float)settingsManager->fontHeight;
	int buttonPositionX = indent;

	auto buttonName = resourceManager->getLocalString("Settings.StringHeight");
	auto buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;

	auto stringHeightLabel = CustomLabel::create(buttonPositionX, rowId * (indent + rowHeight) + indent, buttonWidth, rowHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	stringHeightLabel->SetText(buttonName);
	addWidget(stringHeightLabel);
	buttonPositionX = buttonPositionX + buttonWidth - indent;

	stringHeightField = CustomTextField::create(buttonPositionX, rowId * (indent + rowHeight) + indent, guiScale, buttonHeight, ui->root, CUSTOM_TEXT_FIELD_INTEGER);
	stringHeightField->SetText(settingsManager->stringHeight);
	addWidget(stringHeightField);
	rowId++;

	buttonPositionX = indent;
	buttonName = resourceManager->getLocalString("Settings.MaxStringWidth");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;

	auto maxStringWidthLabel = CustomLabel::create(buttonPositionX, rowId * (indent + rowHeight) + indent, buttonWidth, rowHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	maxStringWidthLabel->SetText(buttonName);
	addWidget(maxStringWidthLabel);
	buttonPositionX = buttonPositionX + buttonWidth - indent;

	maxStringWidthField = CustomTextField::create(buttonPositionX, rowId * (indent + rowHeight) + indent, guiScale, buttonHeight, ui->root, CUSTOM_TEXT_FIELD_INTEGER);
	maxStringWidthField->SetText(settingsManager->maxStringWidth);
	addWidget(maxStringWidthField);
	rowId++;

	buttonName = resourceManager->getLocalString("Settings.StringsCount");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;
	buttonPositionX = indent;
	auto stringsCounttLabel = CustomLabel::create(buttonPositionX, rowId * (indent + rowHeight) + indent, buttonWidth, rowHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	stringsCounttLabel->SetText(buttonName);
	addWidget(stringsCounttLabel);
	buttonPositionX = buttonPositionX + buttonWidth - indent;

	stringsCountField = CustomTextField::create(buttonPositionX, rowId * (indent + rowHeight) + indent, guiScale, buttonHeight, ui->root, CUSTOM_TEXT_FIELD_INTEGER);
	stringsCountField->SetText(settingsManager->stringsCount);
	addWidget(stringsCountField);
	rowId++;

	buttonName = resourceManager->getLocalString("Settings.LanguageSetting");
	buttonWidth = ui->GetTextWidth(ui->font, fontScale, buttonName, 1) - indent;

	buttonPositionX = indent;
	auto languageLabel = CustomLabel::create(buttonPositionX, rowId * (indent + rowHeight) + indent, buttonWidth, rowHeight, ui->root, TEXT_LEFT | TEXT_MIDDLE);
	languageLabel->SetText(buttonName);
	addWidget(languageLabel);
	buttonPositionX = buttonPositionX + buttonWidth;

	languageComboBox = CustomComboBox<WString>::create(buttonPositionX, rowId * (indent + rowHeight) + indent, guiScale * 2, rowHeight, ui->root);
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
	addWidget(languageComboBox);

	rowId++;

	return isInit;
}

 