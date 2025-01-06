#pragma once
#include "CustomWidgets/Dialog.h"
#include "CustomWidgets/CustomTextField.h"
#include "CustomWidgets/CustomTextArea.h"
#include "CustomWidgets/CustomComboBox.h"

using namespace UltraEngine;

class SettingsDialog : public Dialog
{
protected:
	SettingsDialog();
	~SettingsDialog() override;
public:
	shared_ptr<CustomTextField> resolutiontWidthField;
	shared_ptr<CustomTextField> resolutiontHeightField;
	shared_ptr<CustomTextField> stringHeightField;
	shared_ptr<CustomTextField> maxStringWidthField;
	shared_ptr<CustomTextField> stringsCountField;
	shared_ptr<CustomComboBox<WString>> languageComboBox;
	static shared_ptr<SettingsDialog> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
};

