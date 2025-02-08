#include "UltraEngine.h"
#include "CustomTextField.h"

CustomTextField::CustomTextField() {
	blocks.resize(5);//text, value
	ctrlPressed = false;
	doubleClickRange = 1;
	pressed = false;
	offsetX = 0;
	sellen = 0;
	caretPosition = 0;
	textIndent = 4;
	hoverState = false;
	shiftPressed = false;
	cornerradius = 0;
	doHideCursor = false;
	oldText = "";
	doTriggerValueChangeOnType = false;
}


shared_ptr<CustomTextField> CustomTextField::create(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style, WString text) {
	struct Struct : public CustomTextField {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, parent, style, text);
	return instance;
}

int CustomTextField::getIntegerValue() {
	if (!hasWidgetStyle(CUSTOM_TEXT_FIELD_INTEGER) || text.empty()) return 0;
	return text.ToInt();
}

void CustomTextField::setValueChangeListener(std::function<void(Event)> valueChangelistener) {
	CustomTextField::valueChangelistener = valueChangelistener;
}

void CustomTextField::setFocusListener(std::function<void(Event)> focusListenerToSet) {
	CustomTextField::focusListener = focusListenerToSet;
}

bool CustomTextField::Initialize(const int x, const int y, const int width, const int height, shared_ptr<Widget> parent, const int style, WString text) {
	bool flag = CustomWidget::Initialize(text, x, y, width, height, parent, style);
	setAutoFontScale();
	textIndent = Round((float)indent / 2.0f);
	return flag;
}

bool CustomTextField::OK() {
	KeyDown(KEY_ENTER);
	return true;
}

int CustomTextField::GetCharAtPosition(const int position, const bool clickOnChar) {
	auto text = this->text;
	if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'•');

	float indentX = textIndent; // Round((float)indent / 2.0f);
	int count = text.length();
	int lastCharWidth = 0;
	for (int n = 0; n < count; ++n) {
		auto c = text.Mid(n, 1);
		lastCharWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, c, fontweight);
		if (clickOnChar) {
			if (indentX >= position) return n;
		} else {
			if (indentX >= position - (float)lastCharWidth / 2.0f) return n;
		}
		indentX += lastCharWidth;
	}
	return count;
}

int CustomTextField::GetCaretCoord() {
	return GetCaretCoord(caretPosition);
}

int CustomTextField::GetCaretCoord(const int caret) {
	auto text = this->text;
	if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'•');
	float indentX = textIndent;
	int count = Min((int)caret, (int)text.length());
	for (int n = 0; n < count; ++n) {
		auto c = text.Mid(n, 1);
		indentX += GetInterface()->GetTextWidth(GetInterface()->font, fontscale, c, fontweight);
	}
	return indentX;
}

void CustomTextField::MouseDown(const MouseButton button, const int x_, const int y) {
	lastMousePosition.x = x_;
	lastMousePosition.y = y;
	int x = x_;
	if (button == MOUSE_LEFT) {
		x -= offsetX;
		lastMousePosition.x = x;
		lastMousePosition.y = y;

		//Position caret under mouse click
		int prevcaretposition = caretPosition + sellen;
		caretPosition = GetCharAtPosition(x, false);
		pressed = true;
		if (shiftPressed) {
			sellen = prevcaretposition - caretPosition;
		} else {
			sellen = 0;
		}
		resetCursorBlinking();
		Redraw();
	}
}

void CustomTextField::DoubleClick(const MouseButton button, const int x_, const int y) {
	//if ((style & CUSTOM_TEXT_FIELD_READONLY) != 0) return;

	if (button != MOUSE_LEFT) return;
	int x = x_ - offsetX;
	if (Abs(lastMousePosition.x - x) <= doubleClickRange and Abs(lastMousePosition.y - y) <= doubleClickRange) {
		WString badchars = "<>,./\\?[]{}!@#$%^&*()-_=+| 	";

		//Select the word at the mouse position
		auto text = this->text;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'•');

		int l = text.length();
		int c = GetCharAtPosition(x, true);
		caretPosition = c;
		sellen = -1;
		auto chr = text.Mid(c - 1, 1);

		if (chr == L" " or chr == L"	") {
			//Select spaces in this word before the clicked character
			for (int n = c - 2; n >= 0; n--) {
				if (text.Mid(n, 1) != L" " and text.Mid(n, 1) != L"	") {
					break;
				} else {
					sellen--;
				}
			}

			//Select spaces in this word after the clicked character
			for (int n = c; n < l; n++) {
				if (text.Mid(n, 1) != L" " and text.Mid(n, 1) != L"	") {
					break;
				} else {
					caretPosition++;
					sellen--;
				}
			}
		} else if (badchars.Find(chr) > -1) {
			//Stop here
		} else {
			//Select characters in this word before the clicked character
			for (int n = c - 2; n >= 0; n--) {
				chr = text.Mid(n, 1);
				if (badchars.Find(chr) > -1) {
					break;
				} else {
					sellen--;
				}
			}

			//Select characters in this word after the clicked character
			for (int n = c; n < l; n++) {
				chr = text.Mid(n, 1);
				if (badchars.Find(chr) > -1) {
					break;
				} else {
					caretPosition++;
					sellen--;
				}
			}
		}
		//self.gui:ResetCursorBlink()
		//cursorblinkmode = true
		pressed = false;
		Redraw();
	}
}

void CustomTextField::TripleClick(const MouseButton button, const int x, const int y) {
	//if ((style & CUSTOM_TEXT_FIELD_READONLY) != 0) return;
	if (button != MOUSE_LEFT) return;
	int l = text.length();
	caretPosition = l;
	sellen = -int(text.size());
	Redraw();
}

void CustomTextField::MouseUp(const MouseButton button, const int x, const int y) {
	if (button == MOUSE_LEFT) {
		pressed = false;
	}
}

void CustomTextField::MouseMove(const int x_, const int y) {
	int x = x_;
	if (pressed) {
		//Select range of characters
		x = x - offsetX;
		int currentcaretpos = caretPosition;
		int prevcaretpos = caretPosition + sellen;
		caretPosition = GetCharAtPosition(x, false);
		if (caretPosition != currentcaretpos) {
			sellen = prevcaretpos - caretPosition;
			Redraw();
		}
		UpdateOffset();
	}
	Widget::MouseMove(x_, y);
}

void CustomTextField::UpdateOffset() {
	int width = GetSize().x;
	auto c = text.Right(1);
	auto font = GetInterface()->font;
	int tw = GetInterface()->GetTextWidth(font, fontscale, text, fontweight);
	if (tw + textIndent * 2 > width) {
		auto fragment = GetSelectedText();
		int fw = GetInterface()->GetTextWidth(font, fontscale, fragment, fontweight);
		if (fw + textIndent * 2 > width) {
			int coord = GetCaretCoord();
			if (offsetX + coord - textIndent < 0) {
				offsetX = -coord + textIndent;
			} else if (offsetX + coord > width - textIndent) {
				offsetX = -(coord - (width - textIndent));
			}
		} else {
			int c1 = Min(caretPosition, caretPosition + sellen);
			int c2 = Max(caretPosition, caretPosition + sellen);
			int coord1 = GetCaretCoord(c1);
			int coord2 = GetCaretCoord(c2);
			if (offsetX + coord1 - textIndent < 0) {
				offsetX = -coord1 + textIndent;
			} else if (offsetX + coord2 > width - textIndent) {
				offsetX = -(coord2 - (width - textIndent));
			}
		}
		if (offsetX + tw < width - textIndent * 2) {
			offsetX = (width - tw - textIndent * 2);
		}
	} else {
		offsetX = 0;
	}
	resetCursorBlinking();
}

void CustomTextField::fixResult() {
	if (hasWidgetStyle(CUSTOM_TEXT_FIELD_INTEGER)) {
		int value = getIntegerValue();
		if (value < minValue) SetText(minValue);
		if (value > maxValue && maxValue != 0) SetText(maxValue);
	}
	if (text.GetSize() < minSymbols && minSymbols != 0 && !oldText.empty()) {
		SetText(oldText);
	} else {
		oldText = text;
	}
}


bool CustomTextField::drawCallback(const UltraEngine::Event& ev, shared_ptr<UltraEngine::Object> extra) {
	auto textField = extra->As<CustomTextField>();
	textField->doHideCursor = !textField->doHideCursor;
	textField->Redraw();
	return false;
}


void CustomTextField::resetCursorBlinking() {
	doHideCursor = false;
	if (timer) {
		timer->Stop();
		timer.reset();
	}
	timer = UltraEngine::CreateTimer(500);
	ListenEvent(EVENT_TIMERTICK, timer, drawCallback, Self());
}

void CustomTextField::GainFocus() {
	resetCursorBlinking();
	if (!hasWidgetStyle(CUSTOM_TEXT_FIELD_READONLY) && focusListener) {
		focusListener(Event(EVENT_NONE, Self()->As<CustomTextField>(), getIntegerValue(), 0, 0, nullptr, text));
	}
}

void CustomTextField::LoseFocus() {
	if (timer) timer->Stop();
	timer = nullptr;
	hoverState = false;
	fixResult();
	if (!hasWidgetStyle(CUSTOM_TEXT_FIELD_READONLY) && !doTriggerValueChangeOnType && valueChangelistener) {
		valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<CustomTextField>(), getIntegerValue(), 0, 0, nullptr, text));
	}
}

void CustomTextField::MouseEnter(const int x, const int y) {
	CustomWidget::MouseEnter(x, y);
	hoverState = true;
	Redraw();
}

void CustomTextField::MouseLeave(const int x, const int y) {
	CustomWidget::MouseLeave(x, y);
	hoverState = false;
	Redraw();
}

void CustomTextField::KeyUp(const KeyCode key) {
	if (key == KEY_CONTROL) ctrlPressed = false;
	if (key == KEY_SHIFT) shiftPressed = false;
}

bool CustomTextField::KeyDown(const KeyCode key) {
	bool wasSymbolTyped = true;
	if (key == KEY_CONTROL) {
		ctrlPressed = true;
		wasSymbolTyped = false;
	}
	if (key == KEY_A and ctrlPressed == true) {
		caretPosition = text.size();
		sellen = -caretPosition;
		Redraw();
	}
	if (key == KEY_SHIFT) {
		shiftPressed = true;
		wasSymbolTyped = false;
	}
	if (key == KEY_LEFT or key == KEY_UP) {
		moveCaretLeft();
		wasSymbolTyped = false;
	} else if (key == KEY_RIGHT or key == KEY_DOWN) {
		moveCaretRight();
		wasSymbolTyped = false;
	} else if (key == KEY_HOME) {
		caretPosition = 0;
		UpdateOffset();
		Redraw();
	} else if (key == KEY_END) {
		caretPosition = text.size();
		UpdateOffset();
		Redraw();
	} else if (key == KEY_ENTER) {
		sellen = 0;
		offsetX = 0;
		fixResult();
		if (valueChangelistener) {
			valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<CustomTextField>(), getIntegerValue(), 0, 0, nullptr, text));
		}
		GetInterface()->SetFocus(nullptr);
		wasSymbolTyped = false;
	} else if (key == KEY_DELETE) {
		del();
	}
	if (wasSymbolTyped && doTriggerValueChangeOnType && valueChangelistener) {
		valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<CustomTextField>(), getIntegerValue(), 0, 0, nullptr, text));
	}
	return CustomWidget::KeyDown(key);
}

void CustomTextField::moveCaretLeft() {
	if (shiftPressed == false and sellen != 0) {
		//Move the caret to the left side of the selection
		caretPosition = Min(caretPosition, caretPosition + sellen);
		sellen = 0;
		UpdateOffset();
		Redraw();
	} else {
		//Move the caret one character left
		if (caretPosition > 0) {
			caretPosition--;
			if (shiftPressed) {
				sellen++;
			} else {
				sellen = 0;
			}
			UpdateOffset();
			Redraw();
		}
	}
}

void CustomTextField::moveCaretRight() {
	if (shiftPressed == false and sellen != 0) {
		//Move the caret to the right side of the selection
		caretPosition = Max(caretPosition, caretPosition + sellen);
		sellen = 0;
		UpdateOffset();
		Redraw();
	} else {
		//Move the caret one character right
		if (caretPosition < text.length()) {
			caretPosition++;
			if (shiftPressed) {
				sellen--;
			} else {
				sellen = 0;
			}
			UpdateOffset();
			Redraw();
		}
	}
}

void CustomTextField::del() {
	auto s = text;
	if (!s.empty()) {
		if (sellen == 0) {
			if (caretPosition == s.length()) {
				return;
			} else if (caretPosition == 0) {
				s = s.Right(s.length() - 1);
			} else if (caretPosition > 0) {
				s = s.Left(caretPosition) + s.Right(s.length() - caretPosition - 1);
			}
		} else {
			auto c1 = Min(caretPosition, caretPosition + sellen);
			auto c2 = Max(caretPosition, caretPosition + sellen);
			s = s.Left(c1) + s.Right(s.length() - c2);
			caretPosition = c1;
			sellen = 0;
		}
		m_text = s;
		UpdateOffset();
		Redraw();
	}

}

void CustomTextField::Paste() {
	if ((CUSTOM_TEXT_FIELD_READONLY & style) != 0) return;
	if (sellen != 0) KeyChar('\b');
	auto text = GetClipboardText();
	text = text.Replace("\n", "");
	text = text.Replace("\r", "");
	if (text == "") return;
	WString left = this->text.Left(caretPosition);
	WString right = this->text.Right(this->text.size() - caretPosition);
	caretPosition += text.size();
	SetText(left + text + right);
}

void CustomTextField::Copy() {
	if ((CUSTOM_TEXT_FIELD_PASSWORD & style) != 0) return;
	auto text = GetText(true);
	if (text == "") return;
	SetClipboardText(text);
}

void CustomTextField::Cut() {
	if ((CUSTOM_TEXT_FIELD_PASSWORD & style) != 0) return;
	if (sellen == 0) return;
	if ((CUSTOM_TEXT_FIELD_READONLY & style) != 0) return;
	Copy();
	KeyChar('\b');
}

void CustomTextField::KeyChar(const int charcode) {
	if ((CUSTOM_TEXT_FIELD_READONLY & style) != 0) return;

	auto s = text;
	auto c = WChr(charcode);
	if (c == "\x1a") return;// undo
	if (c == "\x16") return;// paste
	if (c == "\b") {
		//Backspace
		if (s.length() > 0) {
			if (sellen == 0) {
				if (caretPosition == s.length()) {
					s = s.Left(s.length() - 1);
				} else if (caretPosition > 0) {
					s = s.Left(caretPosition - 1) + s.Right(s.length() - caretPosition);
				}
				caretPosition = caretPosition - 1;
				caretPosition = Max(0, caretPosition);
			} else {
				auto c1 = Min(caretPosition, caretPosition + sellen);
				auto c2 = Max(caretPosition, caretPosition + sellen);
				s = s.Left(c1) + s.Right(s.length() - c2);
				caretPosition = c1;
				sellen = 0;
			}
			if (text != s) {
				m_text = s;
				UpdateOffset();
				Redraw();
			}
		}
	} else if (c != "\r" and c != "") {
		if ((style & CUSTOM_TEXT_FIELD_INTEGER) != 0) {
			if (!isdigit(charcode)) return;
		}

		//Insert a new character
		auto c1 = Min(caretPosition, caretPosition + sellen);
		auto c2 = Max(caretPosition, caretPosition + sellen);
		s = s.Left(c1) + c + s.Right(s.length() - c2);
		if (maxSymbols != 0 && (s.size()) > maxSymbols) return;
		caretPosition = caretPosition + 1;
		if (sellen < 0) caretPosition = caretPosition + sellen;
		sellen = 0;
		if (text != s) {
			m_text = s;
			UpdateOffset();
			Redraw();
		}
	}
}

WString CustomTextField::GetText(const bool selected) {
	if (selected) {
		if (sellen > 0) {
			return text.Mid(caretPosition, sellen);
		} else if (sellen < 0) {
			return text.Mid(caretPosition + sellen, -sellen);
		} else {
			return "";
		}
	} else {
		return CustomWidget::GetText(selected);
	}
}

void CustomTextField::SetText(const WString& text) {
	CustomWidget::SetText(text.Replace("\n", "").Replace("\r", ""));
}

void CustomTextField::SelectText(const int position, const int length) {
	caretPosition = Max(position, 0);
	if (length == -1) {
		sellen = text.size() - caretPosition;
	} else {
		sellen = Min((int)length, (int)text.size()) - caretPosition;
	}
	sellen = Max(0, sellen);
	caretPosition += sellen;
	sellen *= -1;
}

WString CustomTextField::GetSelectedText() {
	if (sellen == 0) return "";
	if (Abs(sellen) == text.size()) return text;
	int c1 = Min(caretPosition, caretPosition + sellen);
	int c2 = Max(caretPosition, caretPosition + sellen);
	return text.Mid(c1, c2 - c1);
}

void CustomTextField::Draw(const int x, const int y, const int width, const int height) {
	auto gui = GetInterface();
	//Background
	blocks[0].color = color[WIDGETCOLOR_SUNKEN];
	blocks[0].position = iVec2(0);
	blocks[0].size = size;
	blocks[0].radius = cornerradius;
	blocks[0].hidden = false;

	//Selection
	blocks[1].hidden = true;
	if (sellen != 0) {
		blocks[1].color = color[WIDGETCOLOR_RAISED];
		auto fragment = GetSelectedText();
		int c1 = Min(caretPosition, caretPosition + sellen);
		int c2 = Max(caretPosition, caretPosition + sellen);
		auto prefix = text.Left(c1);
		blocks[1].hidden = false;
		auto font = GetInterface()->font;
		blocks[1].position.x = textIndent + gui->GetTextWidth(font, fontscale, prefix, fontweight) + offsetX;
		blocks[1].position.y = (size.y - gui->GetFontHeight(font, fontscale, fontweight) * fontscale) / 2;
		blocks[1].size.x = gui->GetTextWidth(font, fontscale, fragment, fontweight);
		blocks[1].size.y = gui->GetFontHeight(font, fontscale, fontweight) * fontscale;
	}

	//Caret
	if ((style & CUSTOM_TEXT_FIELD_READONLY) == 0 && gui->GetFocus() == Self()) {
		if (!doHideCursor) {
			blocks[2].hidden = false;
			blocks[2].position.x = GetCaretCoord() + offsetX;
			blocks[2].position.y = (size.y - gui->GetFontHeight(GetInterface()->font, fontscale, fontweight) * fontscale) / 2 - 1;
			blocks[2].size.x = 1;
			blocks[2].size.y = gui->GetFontHeight(GetInterface()->font, fontscale, fontweight) * fontscale + 2;
			blocks[2].color = color[WIDGETCOLOR_FOREGROUND];
		} else {
			blocks[2].hidden = true;
		}
	} else {
		blocks[2].hidden = true;
	}

	//Label
	if (text.empty()) {
		blocks[3].hidden = true;
	} else {
		blocks[3].color = color[WIDGETCOLOR_FOREGROUND];
		blocks[3].position = iVec2(0) + cornerradius;
		blocks[3].position.x += textIndent + offsetX;
		blocks[3].textdefined = true;
		blocks[3].size = size - cornerradius * textIndent;
		//	blocks[3].size.x -= 4;
		blocks[3].size.x -= offsetX;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) {
			blocks[3].text = std::wstring(text.size(), L'•');
		} else {
			blocks[3].text = text;
		}
		blocks[3].textalignment = TEXT_LEFT | TEXT_MIDDLE;
		blocks[3].hidden = false;
	}

	//Border
	if (hoverState || gui->GetFocus() == Self() and ((style & CUSTOM_TEXT_FIELD_READONLY) == 0)) {
		blocks[4].color = color[WIDGETCOLOR_SELECTION];
	} else {
		blocks[4].color = color[WIDGETCOLOR_BORDER];
	}
	blocks[4].position = iVec2(0);
	blocks[4].size = size;
	blocks[4].wireframe = true;
	blocks[4].radius = cornerradius;
	blocks[4].hidden = false;
}