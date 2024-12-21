#include "UltraEngine.h"
#include "CustomTextArea.h"
#include "../../Managers/SettingsManager.h"

CustomTextArea::CustomTextArea() {
	ctrlPressed = false;
	doubleClickRange = 1;
	pressed = false;
	offsetX = 0;
	x = 0;
	sellen = 0;
	caretPosition = 0;
	textIndent = 4;
	hoverState = false;
	shiftPressed = false;
	cornerradius = 0;
	doHideCursor = false;
	oldText = "";
	doTriggerValueChangeOnType = false;
	stringHeight = 16;
}


shared_ptr<CustomTextArea> CustomTextArea::create(const int x, const int y, const int width, const int height, const int _stringHeight, shared_ptr<Widget> parent, const int style, WString text) {
	struct Struct : public CustomTextArea {
	};
	auto instance = std::make_shared<Struct>();
	instance->Initialize(x, y, width, height, _stringHeight, parent, style, text);
	return instance;
}

bool CustomTextArea::Initialize(const int x, const int y, const int width, const int height, const int _stringHeight, shared_ptr<Widget> parent, const int style, WString text) {
	bool flag = CustomWidget::Initialize(text, x, y, width, height, parent, style);
	stringHeight = _stringHeight;
	float fonstScale = (float)stringHeight / (float)SettingsManager::getInstance()->fontHeight;
	SetFontScale(fonstScale);
	textIndent = Round((float)indent / 2.0f);
	return flag;
}

int CustomTextArea::GetCharAtPosition(const iVec2 position, const bool clickOnChar) {
	auto text = this->text;
	if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'•');

	float indentX = textIndent;
	vector<WString> lines = text.Split("\n");
	int lineIndex = position.y / stringHeight;
	if (lines.empty()) {
		lineIndex = 0;
		lines.emplace_back("");
	} else if (lineIndex >= lines.size()) {
		lineIndex = lines.size() - 1;
	}
	int count = text.length();
	int lastCharWidth = 0;
	int lineBegin = 0;
	for (int i = 0; i < lineIndex; i++) {
		lineBegin = lineBegin + lines[i].size() + 1;
	}
	if (lines[lineIndex].empty()) {
		return lineBegin;
	}
	for (int n = lineBegin; n < count; ++n) {
		auto c = text.Mid(n, 1);
		lastCharWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, c, fontweight);
		if (clickOnChar && indentX >= position.x) {
			return n;
		} else if (!clickOnChar && indentX >= position.x - (float)lastCharWidth / 2.0f) {
			return n;
		}
		indentX += lastCharWidth;
	}
	return count;
}

iVec2 CustomTextArea::GetCaretCoord() {
	return GetCaretCoord(caretPosition);
}

iVec2 CustomTextArea::GetCaretCoord(const int caret) {
	auto text = this->text;
	if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) {
		text = wstring(this->text.size(), L'•');
	}
	int indentX = textIndent;
	int count = Min((int)caret, (int)text.length());
	auto tempText = text.Replace("\r", "");
	vector<WString> lines = text.Split("\n");
	int currentCount = 0;
	int lineIndex = 0;
	for (auto& line : lines) {
		currentCount = currentCount + line.size();
		if (count > currentCount + lineIndex) {
			lineIndex++;
			continue;
		}
		int lineBegin = currentCount - line.size() + lineIndex;
		indentX = indentX + GetInterface()->GetTextWidth(GetInterface()->font, fontscale, text.Mid(currentCount - line.size() + lineIndex, caret - lineBegin), fontweight);
		break;
	}

	return iVec2(indentX, lineIndex * stringHeight);
}

void CustomTextArea::MouseDown(const MouseButton button, const int x_, const int y) {
	lastMousePosition.x = x;
	lastMousePosition.y = y;
	int x = x_;
	if (button == MOUSE_LEFT) {
		x -= offsetX;
		lastMousePosition.x = x;
		lastMousePosition.y = y;

		//Position caret under mouse click
		int prevcaretposition = caretPosition + sellen;
		caretPosition = GetCharAtPosition(iVec2(x, y), false);
		pressed = true;
		if (shiftPressed) {
			sellen = prevcaretposition - caretPosition;
		} else {
			sellen = 0;
		}
		Redraw();
	}
}

void CustomTextArea::DoubleClick(const MouseButton button, const int x_, const int y) {
	//if ((style & CUSTOM_TEXT_FIELD_READONLY) != 0) return;

	if (button != MOUSE_LEFT) return;
	int x = x_ - offsetX;
	if (Abs(lastMousePosition.x - x) <= doubleClickRange and Abs(lastMousePosition.y - y) <= doubleClickRange) {
		WString badchars = "<>,./\\?[]{}!@#$%^&*()-_=+| 	\n";

		//Select the word at the mouse position
		auto text = this->text;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'•');

		int l = text.length();
		int c = GetCharAtPosition(iVec2(x, y), true);
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
		pressed = false;
		Redraw();
	}
}

void CustomTextArea::TripleClick(const MouseButton button, const int x, const int y) {
	//if ((style & CUSTOM_TEXT_FIELD_READONLY) != 0) return;
	if (button != MOUSE_LEFT) return;
	int l = text.length();
	caretPosition = l;
	sellen = -int(text.size());
	Redraw();
}


void CustomTextArea::MouseMove(const int x_, const int y) {
	int x = x_;
	if (pressed) {
		//Select range of characters
		x = x - offsetX;
		int currentcaretpos = caretPosition;
		int prevcaretpos = caretPosition + sellen;
		caretPosition = GetCharAtPosition(iVec2(x, y), false);
		int oldsellen = sellen;
		if (caretPosition != currentcaretpos) {
			sellen = prevcaretpos - caretPosition;
			Redraw();
		}
		UpdateOffset();
	}
	Widget::MouseMove(x_, y);
}

void CustomTextArea::UpdateOffset() {
	int width = GetSize().x;
	auto c = text.Right(1);
	auto font = GetInterface()->font;
	int cw = GetInterface()->GetTextWidth(font, fontscale, c, fontweight);
	int tw = GetInterface()->GetTextWidth(font, fontscale, text, fontweight);
	if (tw + textIndent * 2 > width) {
		auto fragment = GetSelectedText();
		int fw = GetInterface()->GetTextWidth(font, fontscale, fragment, fontweight);
		if (fw + textIndent * 2 > width) {
			int coord = GetCaretCoord().x;
			if (offsetX + coord - textIndent < 0) {
				offsetX = -coord + textIndent;
			} else if (offsetX + coord > width - textIndent) {
				offsetX = -(coord - (width - textIndent));
			}
		} else {
			int c1 = Min(caretPosition, caretPosition + sellen);
			int c2 = Max(caretPosition, caretPosition + sellen);
			int coord1 = GetCaretCoord(c1).x;
			int coord2 = GetCaretCoord(c2).x;
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
}

bool CustomTextArea::KeyDown(const KeyCode key) {
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
	if (key == KEY_LEFT) {
		moveCaretLeft();
		wasSymbolTyped = false;
	} else if (key == KEY_RIGHT) {
		moveCaretRight();
		wasSymbolTyped = false;
	} else if (key == KEY_UP) {
		vector<WString> lines = text.Split("\n");
		if (lines.size() > 1 && caretPosition > lines[0].size()) {
			int totalCharCount = lines[0].size() + 1;
			for (int i = 1; i < lines.size(); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					int lineCaretPos = caretPosition - totalCharCount;
					if (lines[i - 1].size() >= lineCaretPos) {
						caretPosition = totalCharCount - lines[i - 1].size() - 1 + lineCaretPos;
					} else {
						caretPosition = totalCharCount - 1;
					}
					if (caretPosition < 0) {
						caretPosition = 0;
					}
					sellen = 0;
					UpdateOffset();
					Redraw();
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}
	} else if (key == KEY_DOWN) {
		vector<WString> lines = text.Split("\n");
		if (lines.size() > 1) {
			int totalCharCount = 0;
			for (int i = 0; i < (lines.size() - 1); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					int lineCaretPos = caretPosition - totalCharCount;
					if (lines[i + 1].size() >= lineCaretPos) {
						caretPosition = totalCharCount + lines[i].size() + 1 + lineCaretPos;
					} else {
						caretPosition = totalCharCount + lines[i].size() + 1 + lines[i + 1].size();
					}
					sellen = 0;
					UpdateOffset();
					Redraw();
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}
	} else if (key == KEY_ENTER) {
		KeyChar('\n');
	} else if (key == KEY_DELETE) {
		del();
	}
	if (wasSymbolTyped && doTriggerValueChangeOnType && valueChangelistener) {
		valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<CustomTextArea>(), getIntegerValue(), 0, 0, nullptr, text));
	}
	return CustomWidget::KeyDown(key);
}

void CustomTextArea::SetText(const WString& text) {
	CustomWidget::SetText(text);
}

void CustomTextArea::Draw(const int x, const int y, const int width, const int height) {
	auto gui = GetInterface();
	blocks.clear();
	//Background
	AddBlock(iVec2(0), size, color[WIDGETCOLOR_SUNKEN], false, cornerradius);

	//Selection
	if (sellen != 0) {
		int c1 = Min(caretPosition, caretPosition + sellen);
		int c2 = Max(caretPosition, caretPosition + sellen);
		bool doStartSelection = false;
		vector<WString> lines = text.Split("\n");
		int totalCharCount = 0;
		for (int i = 0; i < lines.size(); i++) {
			if (!doStartSelection && c1 > totalCharCount + lines[i].size()) {
				totalCharCount = totalCharCount + lines[i].size() + 1;
				continue;
			}
			if (!doStartSelection) {
				//in same line
				if (c2 <= totalCharCount + lines[i].size()) {
					auto caretCoord1 = GetCaretCoord(c1);
					auto caretCoord2 = GetCaretCoord(c2);
					AddBlock(iVec2(caretCoord1.x, caretCoord1.y), iVec2(caretCoord2.x - caretCoord1.x, stringHeight), color[WIDGETCOLOR_RAISED], false);
					break;
				//selection starts on this line and ends later on another one
				} else {
					auto caretCoord1 = GetCaretCoord(c1);
					int fw = gui->GetTextWidth(gui->font, fontscale, lines[i].Right(lines[i].size() - c1 - totalCharCount), fontweight);
					AddBlock(iVec2(caretCoord1.x, caretCoord1.y), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
					doStartSelection = true;

				}
			//full line selections between start and end lines
			} else if (doStartSelection && c2 > totalCharCount + lines[i].size()) { 
				int fw = !lines[i].empty() ? gui->GetTextWidth(gui->font, fontscale, lines[i], fontweight) : indent;
				AddBlock(iVec2(textIndent, i * stringHeight), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
			//end selection
			} else if (doStartSelection) {
				auto caretCoord2 = GetCaretCoord(c2);
				int fw = gui->GetTextWidth(gui->font, fontscale, lines[i].Left(c2 - totalCharCount), fontweight);
				AddBlock(iVec2(textIndent, caretCoord2.y), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
				break;
			}
			totalCharCount = totalCharCount + lines[i].size() + 1;
		}
	}

	//Caret
	if ((style & CUSTOM_TEXT_FIELD_READONLY) == 0 && gui->GetFocus() == Self() && !doHideCursor) {
		auto caretCoord = GetCaretCoord();
		AddBlock(iVec2(caretCoord.x + offsetX, caretCoord.y), iVec2(1, stringHeight), color[WIDGETCOLOR_FOREGROUND], false);
	}

	//Label
	if (!text.empty()) {
		auto textPos = iVec2(0) + cornerradius;
		textPos.x = textPos.x + textIndent + offsetX;
		auto textSize = size - cornerradius * textIndent;
		textSize = textSize - offsetX;
		WString blockText;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) {
			blockText = std::wstring(text.size(), L'•');
		} else {
			blockText = text;
		}
		auto textBlock = AddBlock(blockText, textPos, textSize, color[WIDGETCOLOR_FOREGROUND], TEXT_LEFT | TEXT_LEFT);
		blocks[textBlock].textdefined = true;
	}

	//Border
	Vec4 borderColor;
	if (hoverState || gui->GetFocus() == Self() and ((style & CUSTOM_TEXT_FIELD_READONLY) == 0)) {
		borderColor = color[WIDGETCOLOR_SELECTION];
	} else {
		borderColor = color[WIDGETCOLOR_BORDER];
	}
	AddBlock(iVec2(0), size, borderColor, true, cornerradius);
}