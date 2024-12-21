#pragma once
#include "CustomWidgets/Dialog.h"
#include "CustomWidgets/CustomTextField.h"
#include "CustomWidgets/CustomTextArea.h"

using namespace UltraEngine;

class SaveDialog : public Dialog
{
protected:
	std::function<void(Event)> saveListener;
	//for app buttons functions
	std::function<void(Event)> functionListener;
	shared_ptr<CustomButton> notSaveButton;
	SaveDialog();
	~SaveDialog() override;
public:
	static shared_ptr<SaveDialog> create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	virtual bool Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent);
	void setSaveListener(std::function<void(Event)> const& listener);
	void setFunctionListener(std::function<void(Event)> const& listener);
};

