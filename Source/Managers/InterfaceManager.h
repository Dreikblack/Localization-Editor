#pragma once

#include "UltraEngine.h"
#include "../Interface/AllConstants.h"
#include "../UI/CustomWidgets/Dialog.h"

using namespace UltraEngine;

class InterfaceManager : public Object
{
protected:
	bool doInit;
	std::stack<std::weak_ptr<Dialog>> dialogStack;
	InterfaceManager();
	void init();
public:
	bool doUpdateSaveList;
	InterfaceManager(InterfaceManager const&) = delete;
	InterfaceManager& operator=(InterfaceManager const&) = delete;
	static std::shared_ptr<InterfaceManager> getInstance();
	void addDialog(shared_ptr<Dialog> dialog);
	void removeTopDialog();
	void removeDialog(shared_ptr<Dialog> dialog);
	void clear();
	void cancelTopDialog();
	void okTopDialog();
	bool isEmpty();
};
