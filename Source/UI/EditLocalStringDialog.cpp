#include "UltraEngine.h"
#include "EditLocalStringDialog.h"
#include "CustomWidgets/CustomLabel.h"

EditLocalStringDialog::EditLocalStringDialog() {
	localStringId = -1;
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
	setLocalTextTitle("EditLocalStringDialog.Title");

	setCancelListener([this]([[maybe_unused]] Event const& event) {
		SetHidden(true);
		return true;
		});

	auto rowHeight = guiScale / 2;
	auto keyLabel = CustomLabel::create(0, 0, guiScale, rowHeight, As<EditLocalStringDialog>());
	keyLabel->setLocalText("EditLocalStringDialog.Key", true);
	addWidget(keyLabel);
	keyField = CustomTextField::create(keyLabel->getWidth() + indent, 0, width - keyLabel->getWidth() - indent * 2, rowHeight, As<EditLocalStringDialog>());
	addWidget(keyField);
	auto contentLabel = CustomLabel::create(0, rowHeight + indent, guiScale, rowHeight, As<EditLocalStringDialog>());
	contentLabel->setLocalText("EditLocalStringDialog.Content", true);
	addWidget(contentLabel);
	contentField = CustomTextArea::create(indent, rowHeight * 2 + indent, width - indent * 2, contentHeight - (rowHeight * 2 + indent * 3), rowHeight, As<EditLocalStringDialog>());
	addWidget(contentField);	
	return isInit;
}

void EditLocalStringDialog::setLocalString(WString key, WString content) {
	keyField->SetText(key);
	contentField->SetText(content);
}

vector<WString> EditLocalStringDialog::getLocalString() const {
	return { keyField->GetText(), contentField->GetText() };
}

 