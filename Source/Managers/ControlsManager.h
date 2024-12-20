#pragma once

#include "UltraEngine.h"
#include "../UI/CustomWidgets/CustomWidget.h"

using namespace UltraEngine;

using json = nlohmann::json;

enum ACTION_TYPE {
	ACTION_NONE,
	ACTION_CONFIRM,
	ACTION_CANCEL,
	ACTION_SCROLL_UP,
	ACTION_SCROLL_DOWN,
	ACTION_DRAG_N_DROP,
	ACTION_NEW_FILE,
	ACTION_OPEN_FILE,
	ACTION_SAVE_FILES,
	ACTION_ADD_STRING,
	ACTION_EDIT_STRING,
	ACTION_DELETE_STRING,
	ACTION_CONTROL
};


enum MouseWheel {
	MOUSE_WHEEL_NONE,
	MOUSE_WHEEL_UP,
	MOUSE_WHEEL_DOWN
};

struct MouseMoveListener {
	std::weak_ptr<CustomWidget> widget;
	std::function<void(Event)> listener;

	bool operator==(const MouseMoveListener& other) const {
		return widget.lock() == other.widget.lock();
	}

	MouseMoveListener(const std::weak_ptr<CustomWidget>& widget, const std::function<void(Event)>& listener)
		: widget(widget), listener(listener) {
	}
};

class ControlsManager : public Object {
protected:
	ControlsManager();
	WString configPath;
	json configData;
	map<KeyCode, bool> keysDown;
	map<KeyCode, bool> keysUp;
	map<ACTION_TYPE, vector<KeyCode>> actionsMap;
	map<ACTION_TYPE, MouseWheel> actionsWheelMap;
	map<ACTION_TYPE, vector<MouseMoveListener>> actionsMouseMoveWidgetsMap;
	map<ACTION_TYPE, map<std::weak_ptr<Object>, std::function<void(Event)>, std::owner_less<std::weak_ptr<Object>>>> actionListeners;
	map<ACTION_TYPE, vector<std::function<void(Event)>>> actionDownListeners;//for GUI
public:
	map<ACTION_TYPE, string> actionsNamesMap;
	ControlsManager(ControlsManager const&) = delete;
	ControlsManager& operator=(ControlsManager const&) = delete;
	void init();
	static std::shared_ptr<ControlsManager> getInstance();
	void saveConfig();
	void setAction(ACTION_TYPE action, vector<KeyCode> keys);
	void setAction(ACTION_TYPE action, MouseWheel mouseWheel);
	void setActionListener(ACTION_TYPE action, std::weak_ptr<Object> id, std::function<void(Event)> listener);
	void addActionListener(ACTION_TYPE action, std::weak_ptr<Object> id, std::function<void(Event)> listener);
	void removeActionListener(ACTION_TYPE action, std::weak_ptr<Object> id);
	void removeActionListener(ACTION_TYPE action);

	void addMouseMoveActionListener(ACTION_TYPE action, std::weak_ptr<CustomWidget> widget, std::function<void(Event)> listener);
	void clearActionListener(ACTION_TYPE action);
	void setDefaultKeys();
	vector<KeyCode> getDefaultKeys(ACTION_TYPE action);

	void setSavedKeys();
	void clearActionUp();
	void clearActionDown();
	void pollEvents(Event event);
	bool isActionKeyDown(ACTION_TYPE action);
	bool isActionKeyUp(ACTION_TYPE action);
	WString getFirstActionKey(ACTION_TYPE action);
	static WString getKeyString(KeyCode keyCode);
	vector<KeyCode> getActionKeys(ACTION_TYPE action);
};