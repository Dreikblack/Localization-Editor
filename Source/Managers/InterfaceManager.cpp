#include "UltraEngine.h"
#include "InterfaceManager.h"
#include "../UI/EditLocalStringDialog.h"

InterfaceManager::InterfaceManager() {
	doInit = true;
	doUpdateSaveList = false;
}

struct StructInterfaceManager : public InterfaceManager {
};

void InterfaceManager::init() {
	doInit = false;
	shared_ptr<ControlsManager> controlsManager = ControlsManager::getInstance();
	controlsManager->addActionListener(ACTION_CANCEL, Self(), [this](Event event) {
		cancelTopDialog();
		});
	controlsManager->addActionListener(ACTION_CONFIRM, Self(), [this](Event event) {
		okTopDialog();
		});
}

std::shared_ptr<InterfaceManager> InterfaceManager::getInstance() {
	static std::shared_ptr<InterfaceManager> instance = std::make_shared<StructInterfaceManager>();
	if (instance->doInit) {
		instance->init();
	}
	return instance;
}

void InterfaceManager::addDialog(shared_ptr<Dialog> dialog) {
	if (!dialogStack.empty() && dialogStack.top().lock() == dialog) {
		return;
	}
	dialogStack.push(dialog);
}

void InterfaceManager::removeTopDialog() {
	if (dialogStack.empty()) {
		return;
	}
	dialogStack.pop();
}

void InterfaceManager::removeDialog(shared_ptr<Dialog> dialog) {
	if (dialogStack.empty()) {
		return;
	}
	auto topDialog = dialogStack.top().lock();
	if (topDialog == dialog) {
		removeTopDialog();
	}
}

void InterfaceManager::clear() {
	while (!dialogStack.empty()) {
		dialogStack.pop();
	}
}

void InterfaceManager::cancelTopDialog() {
	while (!dialogStack.empty()) {
		auto dialog = dialogStack.top().lock();
		if (!dialog || dialog->GetHidden()) {
			dialogStack.pop();
		} else {
			dialog->useCancelListener();
			return;
		}
	}
}

void InterfaceManager::okTopDialog() {
	while (!dialogStack.empty()) {
		auto dialog = dialogStack.top().lock();
		if (dialog && dialog->As<EditLocalStringDialog>()) {
			return;
		} else if (!dialog || dialog->GetHidden()) {
			dialogStack.pop();
		} else {
			try {
				dialog->useOkListener();
			} catch (const std::invalid_argument& e) {
				// New window, stop processing in ui with old window
			}
			return;
		}
	}
}

bool InterfaceManager::isEmpty() {
	if (dialogStack.empty()) {
		return true;
	} else {
		while (!dialogStack.empty()) {
			auto dialog = dialogStack.top().lock();
			if (!dialog || dialog->GetHidden()) {
				dialogStack.pop();
			} else {
				return false;
			}
		}
	}
}
