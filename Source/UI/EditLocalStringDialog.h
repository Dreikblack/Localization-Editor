#pragma once
#include "CustomWidgets/Dialog.h"
#include "CustomWidgets/CustomTextField.h"
#include "CustomWidgets/CustomTextArea.h"

using namespace UltraEngine;

class EditLocalStringDialog : public Dialog
{
protected:
	shared_ptr<CustomTextField> keyField;
	shared_ptr<CustomTextArea> contentField;
	EditLocalStringDialog();
	~EditLocalStringDialog() override;
public:
	int localStringId;
	static shared_ptr<EditLocalStringDialog> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	void setLocalString(WString _key, WString _content);
	vector<WString> getLocalString() const;
	void SetHidden(const bool hide) override;
};

