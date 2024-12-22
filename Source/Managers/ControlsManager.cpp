#include "UltraEngine.h"
#include "ControlsManager.h"
#include "SettingsManager.h"
#include "InterfaceManager.h"

ControlsManager::ControlsManager() {
	actionsNamesMap[ACTION_CONFIRM] = "Controls.Action.Confirm";
}

struct StructControlsManager : public ControlsManager {
};


std::shared_ptr<ControlsManager> ControlsManager::getInstance() {
	static std::shared_ptr<ControlsManager> instance = std::make_shared<StructControlsManager>();
	return instance;
}

void ControlsManager::init() {
	//actionListeners.clear();
	actionDownListeners.clear();
	actionsMouseMoveWidgetsMap.clear();

	configPath = AppDir() + "/Config/Controls.json";
	
	setDefaultKeys();
	try {
		configData = SettingsManager::getJson(configPath);
		setSavedKeys();
	} catch (const std::invalid_argument& e) {
		configData = json();
	}
}

void ControlsManager::setAction(ACTION_TYPE action, vector<KeyCode> keys) {
	actionsMap[action] = keys;
}

void ControlsManager::setAction(ACTION_TYPE action, MouseWheel mouseWheel) {
	actionsWheelMap[action] = mouseWheel;
}

void ControlsManager::setActionListener(ACTION_TYPE action, std::weak_ptr<Object> id, std::function<void(Event)> listener) {
	clearActionListener(action);
	addActionListener(action, id, listener);
}

void ControlsManager::addActionListener(ACTION_TYPE action, std::weak_ptr<Object> id, std::function<void(Event)> listener) {
	actionListeners[action][id] = listener;
}

void ControlsManager::removeActionListener(ACTION_TYPE action, std::weak_ptr<Object> id) {
	actionListeners[action].erase(id);
}

void ControlsManager::removeActionListener(ACTION_TYPE action) {
	actionListeners[action].clear();
}

void ControlsManager::addMouseMoveActionListener(ACTION_TYPE action, std::weak_ptr<CustomWidget> widget, std::function<void(Event)> listener) {
	actionsMouseMoveWidgetsMap[action].push_back(MouseMoveListener(widget, listener));
}

void ControlsManager::clearActionListener(ACTION_TYPE action) {
	actionDownListeners[action].clear();
}

void ControlsManager::setDefaultKeys() {
	setAction(ACTION_SCROLL_UP, MOUSE_WHEEL_UP);
	setAction(ACTION_SCROLL_DOWN, MOUSE_WHEEL_DOWN);

	setAction(ACTION_CONFIRM, getDefaultKeys(ACTION_CONFIRM));
	setAction(ACTION_CANCEL, getDefaultKeys(ACTION_CANCEL));
	setAction(ACTION_NEW_FILE, getDefaultKeys(ACTION_NEW_FILE));
	setAction(ACTION_OPEN_FILE, getDefaultKeys(ACTION_OPEN_FILE));
	setAction(ACTION_SAVE_FILES, getDefaultKeys(ACTION_SAVE_FILES));
	setAction(ACTION_ADD_STRING, getDefaultKeys(ACTION_ADD_STRING));
	setAction(ACTION_EDIT_STRING, getDefaultKeys(ACTION_EDIT_STRING));
	setAction(ACTION_DELETE_STRING, getDefaultKeys(ACTION_DELETE_STRING));
	setAction(ACTION_CONTROL, getDefaultKeys(ACTION_CONTROL));
	setAction(ACTION_SAVE_EDIT, getDefaultKeys(ACTION_SAVE_EDIT));
}

vector<KeyCode> ControlsManager::getDefaultKeys(ACTION_TYPE action) {
	switch (action) {
	case ACTION_CONFIRM: return { KEY_SPACE,  KEY_ENTER };
	case ACTION_CANCEL: return { KEY_ESCAPE };
	case ACTION_NEW_FILE: return { KEY_N };
	case ACTION_OPEN_FILE: return { KEY_O };
	case ACTION_SAVE_FILES: return { KEY_S };
	case ACTION_ADD_STRING: return { KEY_A };
	case ACTION_EDIT_STRING: return { KEY_E };
	case ACTION_DELETE_STRING: return { KEY_D };
	case ACTION_CONTROL: return { KEY_CONTROL };
	case ACTION_SAVE_EDIT: return { KEY_SPACE,  KEY_ENTER };
	}
}

void ControlsManager::setSavedKeys() {
	for (auto& action : actionsNamesMap) {
		try {
			auto t = configData[action.second].get<vector<KeyCode>>();
			setAction(action.first, configData[action.second].get<vector<KeyCode>>());
		} catch (const std::exception& e) {
			//default already seted
		}
	}
}

void ControlsManager::saveConfig() {
	for (auto& action : actionsNamesMap) {
		configData[action.second] = actionsMap[action.first];
	}
	auto stream = WriteFile(configPath);
	stream->WriteString(configData.dump(), false);
	stream->Close();
}

void ControlsManager::clearActionUp() {
	for (auto& keyUp : keysUp) {
		keyUp.second = false;
	}
}

void ControlsManager::clearActionDown() {
	for (auto& keyDown : keysDown) {
		keyDown.second = false;
	}
}

void ControlsManager::pollEvents(Event event) {
	if (!event.source || event.source->As<Window>()) {
		//return;//drag n drop is broken with that
	}
	for (auto& actionListener : actionListeners) {
		for (auto it = actionListener.second.begin(); it != actionListener.second.end();) {
			if (it->first.expired()) {
				it = actionListener.second.erase(it);
			} else {
				++it;
			}
		}
	}

	switch (event.id) {
	case EVENT_KEYDOWN:
	{
		auto key = static_cast<KeyCode>(event.data);
		keysDown[key] = true;
		keysUp[key] = false;
		break;
	}
	case EVENT_KEYUP:
	{
		auto key = static_cast<KeyCode>(event.data);
		if (keysUp[key]) return;
		keysUp[key] = true;
		keysDown[key] = false;
		vector<std::function<void(Event)>> useLaterListeners;
		for (auto& actionListener : actionListeners) {
			for (auto& currentKey : actionsMap[actionListener.first]) {
				if (key == currentKey) {
					for (auto& listener : actionListener.second) {
						if (listener.first.lock() == InterfaceManager::getInstance()) {
							useLaterListeners.push_back(listener.second);
						} else {
							listener.second(Event(EVENT_KEYUP, Self(), event.data));
						}
					}
					break;
				}
			}
		}
		for (auto& listener : useLaterListeners) {
			listener(Event(EVENT_KEYUP, Self()));
		}
		break;
	}
	case EVENT_MOUSEWHEEL:
	{
		int mouseWheelData = event.data;
		MouseWheel mouseWheel = MOUSE_WHEEL_NONE;

		if (mouseWheelData > 0) mouseWheel = MOUSE_WHEEL_DOWN;
		else if (mouseWheelData < 0) mouseWheel = MOUSE_WHEEL_UP;

		for (auto& actionListener : actionListeners) {
			if (mouseWheel == actionsWheelMap[actionListener.first] && actionsWheelMap[actionListener.first] != MOUSE_WHEEL_NONE) {
				for (auto& listener : actionListener.second) {
					listener.second(Event(EVENT_MOUSEWHEEL, Self(), mouseWheelData, event.position));
				}
			}
		}
		break;
	}
	case EVENT_MOUSEMOVE:
	{
		for (auto& actionMouseMove : actionsMouseMoveWidgetsMap) {
			for (auto& mouseMoveListener : actionMouseMove.second) {
				if (!mouseMoveListener.widget.lock() || !mouseMoveListener.listener) continue;
				auto widgetShared = mouseMoveListener.widget.lock();
				auto wPosition = widgetShared->GetPosition(true);
				auto wSize = widgetShared->GetSize();
				auto mPosition = event.position;
				if (widgetShared->GetInterface()
					&& !widgetShared->GetInterface()->GetHidden()
					&& !widgetShared->GetHidden()
					&& widgetShared->GetInteractive()
					&& mPosition.x >= wPosition.x && mPosition.x < (wPosition.x + wSize.width)
					&& mPosition.y >= wPosition.y && mPosition.y < (wPosition.y + wSize.height))
					mouseMoveListener.listener(Event(EVENT_MOUSEMOVE, Self(), event.data, event.position));
			}
		}
		break;
	}
	}
}

bool ControlsManager::isActionKeyDown(ACTION_TYPE action) {
	if (actionsMap[action].empty()) return false;
	for (auto& key : actionsMap[action]) {
		if (keysDown[key]) return true;
	}
	return false;
}

bool ControlsManager::isActionKeyUp(ACTION_TYPE action) {
	if (actionsMap[action].empty()) return false;
	for (auto& key : actionsMap[action]) {
		if (keysUp[key]) return true;
	}
	return false;
}

WString ControlsManager::getFirstActionKey(ACTION_TYPE action) {
	if (actionsMap[action].size() == 0) return "0";
	auto keyCode = actionsMap[action][0];
	return getKeyString(keyCode);
}

WString ControlsManager::getKeyString(KeyCode keyCode) {
	switch (keyCode) {
	case (KEY_BACKSPACE): return "BACK";
	case (KEY_TAB): return "TAB";
	case (KEY_ENTER): return "ENTER";
	case (KEY_SHIFT): return "SHIFT";
	case (KEY_CONTROL): return "CTRL";
	case (KEY_ALT): return "ALT";
	case (KEY_CAPSLOCK): return "CAPS";
	case (KEY_ESCAPE): return "ESC";
	case (KEY_SPACE): return "SPACE";
	case (KEY_PAGEUP): return "PU";
	case (KEY_PAGEDOWN): return "PD";
	case (KEY_END): return "END";
	case (KEY_HOME): return "HOME";
	case (KEY_LEFT): return "LEFT";
	case (KEY_UP): return "UP";
	case (KEY_RIGHT): return "RIGHT";
	case (KEY_DOWN): return "DOWN";
	case (KEY_INSERT): return "INSERT";
	case (KEY_DELETE): return "DELETE";
	case (KEY_D0): return "0";
	case (KEY_D1): return "1";
	case (KEY_D2): return "2";
	case (KEY_D3): return "3";
	case (KEY_D4): return "4";
	case (KEY_D5): return "5";
	case (KEY_D6): return "6";
	case (KEY_D7): return "7";
	case (KEY_D8): return "8";
	case (KEY_D9): return "9";
	case (KEY_A): return "A";
	case (KEY_B): return "B";
	case (KEY_C): return "C";
	case (KEY_D): return "D";
	case (KEY_E): return "E";
	case (KEY_F): return "F";
	case (KEY_G): return "G";
	case (KEY_H): return "H";
	case (KEY_I): return "I";
	case (KEY_J): return "J";
	case (KEY_K): return "K";
	case (KEY_L): return "L";
	case (KEY_M): return "M";
	case (KEY_N): return "N";
	case (KEY_O): return "O";
	case (KEY_P): return "P";
	case (KEY_Q): return "Q";
	case (KEY_R): return "R";
	case (KEY_S): return "S";
	case (KEY_T): return "T";
	case (KEY_U): return "A";
	case (KEY_V): return "V";
	case (KEY_W): return "W";
	case (KEY_X): return "X";
	case (KEY_Y): return "Y";
	case (KEY_Z): return "Z";
	case (KEY_F1): return "F1";
	case (KEY_F2): return "F2";
	case (KEY_F3): return "F3";
	case (KEY_F4): return "F4";
	case (KEY_F5): return "F5";
	case (KEY_F6): return "F6";
	case (KEY_F7): return "F7";
	case (KEY_F8): return "F8";
	case (KEY_F9): return "F9";
	case (KEY_F10): return "F10";
	case (KEY_F11): return "F11";
	case (KEY_F12): return "F12";
		//case (KEY_NUMLOCK): return "Num";
	case (KEY_SEMICOLON): return ";";
	case (KEY_EQUALS): return "=";
	case (KEY_COMMA): return ",";
	case (KEY_SUBTRACT): return "0";
	case (KEY_PERIOD): return ".";
	case (KEY_SLASH): return "/";
	case (KEY_TILDE): return "`";
	case (KEY_OPENBRACKET): return "[";
	case (KEY_BACKSLASH): return "\\";
	case (KEY_CLOSEBRACKET): return "]";
	case (KEY_QUOTES): return "'";
	case (KEY_NUMPAD0): return "0";
	case (KEY_NUMPAD1): return "1";
	case (KEY_NUMPAD2): return "2";
	case (KEY_NUMPAD3): return "3";
	case (KEY_NUMPAD4): return "4";
	case (KEY_NUMPAD5): return "5";
	case (KEY_NUMPAD6): return "6";
	case (KEY_NUMPAD7): return "7";
	case (KEY_NUMPAD8): return "8";
	case (KEY_NUMPAD9): return "9";
	case (KEY_NUMPADPERIOD): return ".";
	case (KEY_NUMPADDIVIDE): return "/";
	case (KEY_NUMPADMULTIPLY): return "*";
	case (KEY_NUMPADSUBTRACT): return "-";
	case (KEY_NUMPADADDITION): return "+";
	}
	return "";
}

vector<KeyCode> ControlsManager::getActionKeys(ACTION_TYPE action) {
	return actionsMap[action];
}
