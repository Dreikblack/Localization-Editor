#include "UltraEngine.h"
#include "SaveDialog.h"
#include "CustomWidgets/CustomLabel.h"
#include "../Managers/ResourceManager.h"
#include "CustomWidgets/ButtonPanel.h"
#include "../Managers/SettingsManager.h"

SaveDialog::SaveDialog() {
}

SaveDialog::~SaveDialog() = default;

std::shared_ptr<SaveDialog> SaveDialog::create(const int x, const int y, const int width, const int height,
	shared_ptr<Widget> parent)
{
	struct Struct : public SaveDialog {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent);
	return instance;
}

bool SaveDialog::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
	bool isInit = Dialog::Initialize(x, y, width, height, parent, DIALOG_MODAL);
	setLocalTextTitle("SaveDialog.Title");

	setCancelListener([this]([[maybe_unused]] Event const& event) {
		SetHidden(true);
		return true;
		});

	auto label = CustomLabel::create(0, 0, guiScale, buttonHeight * 2, As<SaveDialog>());
	label->setLocalText("SaveDialog.Warning");
	label->setWidth(container->getWidth());
	auto settingsManager = SettingsManager::getInstance();
	float newFonstScale = (float)buttonHeight / (float)settingsManager->fontHeight;
	label->SetFontScale(newFonstScale);
	addWidget(label);
	okButton->setLocalText("SaveDialog.Save", true);

	auto resourceManager = ResourceManager::getInstance();
	auto saveString = resourceManager->getLocalString("SaveDialog.NotSave");
	int notSaveButtonWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, saveString, 1) + indent * 2;
	notSaveButton = CustomButton::create(0, 0, notSaveButtonWidth, buttonHeight, Self()->As<SaveDialog>(), WIDGET_BORDER | WIDGET_BACKGROUND);
	notSaveButton->SetText(saveString);
	buttonPanel->insertButton(notSaveButton, 1);
	notSaveButton->setListener([this](Event event) {
		SetHidden(true);
		if (functionListener) {
			functionListener(Event());
		}
		return true;
		});
	okButton->setListener([this](Event event) {
		SetHidden(true);
		if (saveListener) {
			saveListener(Event());
		}
		if (functionListener) {
			functionListener(Event());
		}
		return true;
		});
	cancelButton->setListener([this](Event event) {
		SetHidden(true);
		return true;
		});
	return isInit;
}

void SaveDialog::setSaveListener(std::function<void(Event)> const& listener) {
	saveListener = listener;
}

void SaveDialog::setFunctionListener(std::function<void(Event)> const& listener) {
	functionListener = listener;
}
