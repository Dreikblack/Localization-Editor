#include "UltraEngine.h"
#include "EditLocalStringDialog.h"
#include "CustomWidgets/CustomLabel.h"
#include "../Managers/SettingsManager.h"
#include "CustomWidgets/ButtonPanel.h"

EditLocalStringDialog::EditLocalStringDialog() {
	localStringId = -1;
	minWidth = guiScale * 5;
	minHeight = guiScale * 5;
}

EditLocalStringDialog::~EditLocalStringDialog() = default;

std::shared_ptr<EditLocalStringDialog> EditLocalStringDialog::create(const int x, const int y, const int width, const int height,
	shared_ptr<Widget> parent)
{
	struct Struct : public EditLocalStringDialog {};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent);
	return instance;
}

bool EditLocalStringDialog::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent)
{
	bool isInit = Dialog::Initialize(x, y, width, height, parent, DIALOG_MODAL | DIALOG_ESC_CLOSE);
	minWidth = width;
	minHeight = height;

	setLocalTextTitle("EditLocalStringDialog.Title");

	setCancelListener([this]([[maybe_unused]] Event const& event) {
		SetHidden(true);
		return true;
		});

	auto rowHeight = guiScale / 2;
	int rowId = 0;

	auto keyLabel = CustomLabel::create(0, rowId * (indent + rowHeight), guiScale, rowHeight, As<EditLocalStringDialog>());
	keyLabel->setLocalText("EditLocalStringDialog.Key", true);
	addWidget(keyLabel);
	rowId++;

	keyField = CustomTextField::create(indent, rowId * (indent + rowHeight), width - indent * 2, rowHeight, As<EditLocalStringDialog>());
	addWidget(keyField);
	rowId++;

	auto contentLabel = CustomLabel::create(0, rowId * (indent + rowHeight), guiScale, rowHeight, As<EditLocalStringDialog>());
	contentLabel->setLocalText("EditLocalStringDialog.Content", true);
	addWidget(contentLabel);
	rowId++;

	contentField = CustomTextArea::create(indent, rowId * (indent + rowHeight), width - indent * 2, contentHeight - rowId *(rowHeight + indent) - indent, rowHeight, As<EditLocalStringDialog>());
	addWidget(contentField);
	contentField->setWarpMode(WARP_WORD);
	rowId++;

	return isInit;
}

void EditLocalStringDialog::setLocalString(WString key, WString content) {
	keyField->SetText(key);
	contentField->SetText(content);
}

vector<WString> EditLocalStringDialog::getLocalString() const {
	return { keyField->GetText(), contentField->GetText() };
}

void EditLocalStringDialog::SetHidden(const bool hide) {
	contentField->resetStates();
	Dialog::SetHidden(hide);
}

void EditLocalStringDialog::updateContentSize() {
	auto settingsManager = SettingsManager::getInstance();
	int stringHeight = settingsManager->stringHeight;
	int maxStringWidth = settingsManager->maxStringWidth;
	int stringsCount = settingsManager->stringsCount;
	int newHeight = stringsCount * stringHeight;
	contentField->setStringHeight(stringHeight);
	contentField->setSize(maxStringWidth, newHeight);

	int newDialogWidth = maxStringWidth + indent * 2;
	if (newDialogWidth > minWidth) {
		setWidth(newDialogWidth);
		container->setWidth(newDialogWidth);
	}
	int newDialogHeight = contentField->getPositionY() + newHeight + buttonHeight * 2 + indent * 3;
	if (newDialogHeight > minHeight) {
		setHeight(newDialogHeight);
		container->setHeight(newDialogHeight - buttonHeight * 2 - indent * 3);
	}
	container->updateInnerContainerSize();
	buttonPanel->setPositionY(getHeight() - buttonHeight - indent);
	buttonPanel->updateContentPosition();
	buttonPanel->draw();
	keyField->setWidth(getWidth() - 2 * indent);
}

 