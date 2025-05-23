#include "UltraEngine.h"
#include "CustomTextArea.h"
#include "../../Managers/SettingsManager.h"

CustomTextArea::CustomTextArea() {
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
	stringHeight = 16;
	offsetY = 0;
	warpMode = WARP_NONE;
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
	setStringHeight(_stringHeight);
	textIndent = Round((float)indent / 2.0f);
	return flag;
}

void CustomTextArea::setStringHeight(int newStringHeight) {
	stringHeight = newStringHeight;
	float fonstScale = (float)stringHeight / (float)SettingsManager::getInstance()->fontHeight;
	SetFontScale(fonstScale);
}

static void Split(WString const& text, WString const& delimiter, std::vector<WString>& result) {
	WString tempText = text;
	while (!tempText.empty()) {
		int delimiterIndex = tempText.Find(delimiter);
		if (delimiterIndex == -1) {
			result.push_back(tempText);
			break;
		}
		result.push_back(tempText.Left(delimiterIndex));
		tempText = tempText.Right((int)tempText.length() - delimiterIndex - (int)delimiter.length());
		if (tempText.empty() && delimiterIndex != -1) {
			result.push_back("");
		}
	}
}

int CustomTextArea::GetCharAtPosition(iVec2 position, const bool clickOnChar) {
	auto text = this->text;
	if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'�');

	int indentX = textIndent;
	if (position.x < 0) {
		position.x = 0;
	}
	if (position.y < 0) {
		position.y = 0;
	}
	vector<WString> lines;
	Split(text, "\n", lines);
	int lineIndex = position.y / stringHeight;
	if (lines.empty()) {
		lineIndex = 0;
		lines.emplace_back("");
	} else if (lineIndex >= lines.size()) {
		lineIndex = lines.size() - 1;
	}
	int lastCharWidth = 0;
	//total char index
	int lineBegin = 0;
	for (int i = 0; i < lineIndex; i++) {
		lineBegin = lineBegin + lines[i].size() + 1;
	}
	if (lines[lineIndex].empty()) {
		return lineBegin;
	}
	int count = lineBegin + lines[lineIndex].size();
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
		text = wstring(this->text.size(), L'�');
	}
	int indentX = textIndent;
	int count = Min((int)caret, (int)text.length());
	auto tempText = text.Replace("\r", "");
	vector<WString> lines;
	Split(text, "\n", lines);
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

void CustomTextArea::MouseDown(const MouseButton button, const int _x, const int _y) {
	lastMousePosition.x = _x;
	lastMousePosition.y = _y;
	int x = _x;
	int y = _y;
	if (button == MOUSE_LEFT) {
		x -= offsetX;
		y += offsetY;
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
		resetCursorBlinking();
		Redraw();
	}
}

void CustomTextArea::DoubleClick(const MouseButton button, const int _x, const int _y) {
	//if ((style & CUSTOM_TEXT_FIELD_READONLY) != 0) return;

	if (button != MOUSE_LEFT) return;
	int x = _x - offsetX;
	int y = _y + offsetY;
	if (Abs(lastMousePosition.x - x) <= doubleClickRange and Abs(lastMousePosition.y - y) <= doubleClickRange) {
		WString badchars = "<>,./\\?[]{}!@#$%^&*()-_=+| 	\n";

		//Select the word at the mouse position
		auto text = this->text;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) text = wstring(this->text.size(), L'�');

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


void CustomTextArea::MouseMove(const int _x, const int _y) {
	int x = _x;
	int y = _y;
	if (pressed) {
		//Select range of characters
		x = x - offsetX;
		y = y + offsetY;
		int currentcaretpos = caretPosition;
		int prevcaretpos = caretPosition + sellen;
		caretPosition = GetCharAtPosition(iVec2(x, y), false);
		if (caretPosition != currentcaretpos) {
			sellen = prevcaretpos - caretPosition;
			Redraw();
		}
		UpdateOffset();
	}
	Widget::MouseMove(_x, _y);
}

void CustomTextArea::UpdateOffset() {
	int currentY = GetCaretCoord().y;
	doWarpText();
	if (GetCaretCoord().y != currentY) {
		caretPosition++;
	}
	int width = GetSize().x;
	int height = GetSize().y;
	auto character = text.Right(1);
	auto const& font = GetInterface()->font;
	int textWidth = GetInterface()->GetTextWidth(font, fontscale, text, fontweight);
	if (textWidth + textIndent * 2 > width) {
		auto fragment = GetSelectedText();
		int fragmentWidth = GetInterface()->GetTextWidth(font, fontscale, fragment, fontweight);
		if (fragmentWidth + textIndent * 2 > width) {
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
		if (offsetX + textWidth < width - textIndent * 2) {
			offsetX = (width - textWidth - textIndent * 2);
		}
	} else {
		offsetX = 0;
	}
	vector<WString> lines;
	Split(text, "\n", lines);	
	int totalCharCount = 0;
	if (lines.size() * stringHeight > height) {
		for (int i = 0; i < lines.size(); i++) {
			//found current line?
			if (caretPosition <= (totalCharCount + lines[i].size())) {
				auto caretPositionHeight = (i + 1) * stringHeight;
				if (caretPositionHeight < height + offsetY && caretPositionHeight > offsetY) {
					//cursor is visible
					break;
				} else if (caretPositionHeight > height) {
					offsetY = caretPositionHeight - height;
					break;
				} else if (offsetY > 0 && caretPositionHeight <= offsetY) {
					//if cursor upper than visible part
					for (int j = 0; j < lines.size(); j++) {
						offsetY = offsetY - stringHeight;
						if (offsetY == 0 || (caretPositionHeight < height + offsetY && caretPositionHeight > offsetY)) {
							break;
						}
					}
					break;
				} else {
					offsetY = 0;
				}
				break;
			}
			totalCharCount = totalCharCount + (int)lines[i].size() + 1;
		}
	} else {
		offsetY = 0;
	}
	if (offsetY < 0) {
		offsetY = 0;
	}
	resetCursorBlinking();
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
		vector<WString> lines;
		Split(text, "\n", lines);	
		if (lines.size() > 1 && caretPosition > lines[0].size()) {
			int totalCharCount = lines[0].size() + 1;
			for (int i = 1; i < lines.size(); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					//char index in current line
					int lineCaretPos = caretPosition - totalCharCount;
					auto currentLineText = text.Mid(totalCharCount, lineCaretPos);
					//width til char
					auto currentLineWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, currentLineText, fontweight);
					int prevCaretPosition = caretPosition + sellen;
					caretPosition = GetCharAtPosition(iVec2(currentLineWidth, stringHeight * i - stringHeight / 2), true);
					if (shiftPressed) {
						sellen = prevCaretPosition - caretPosition;
					} else {
						sellen = 0;
					}
					UpdateOffset();
					Redraw();
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}
	} else if (key == KEY_DOWN) {
		vector<WString> lines;
		Split(text, "\n", lines);
		if (lines.size() > 1) {
			int totalCharCount = 0;
			for (int i = 0; i < (lines.size() - 1); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					//char index in current line
					int lineCaretPos = caretPosition - totalCharCount;
					auto currentLineText = text.Mid(totalCharCount, lineCaretPos);
					//width til char
					auto currentLineWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, currentLineText, fontweight);
					int prevCaretPosition = caretPosition + sellen;
					caretPosition = GetCharAtPosition(iVec2(currentLineWidth, stringHeight * (i + 1) + stringHeight / 2), true);				
					if (shiftPressed) {
						sellen = prevCaretPosition - caretPosition;
					} else {
						sellen = 0;
					}
					UpdateOffset();
					Redraw();
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}
	} else if (key == KEY_HOME) {
		vector<WString> lines;
		Split(text, "\n", lines);
		if (lines.size() == 1) {
			caretPosition = 0;
		} else {
			int totalCharCount = 0;
			for (int i = 0; i < (lines.size()); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					caretPosition = totalCharCount;
					sellen = 0;
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}	
		UpdateOffset();
		Redraw();
	} else if (key == KEY_END) {
		vector<WString> lines;
		Split(text, "\n", lines);
		if (lines.size() == 1) {
			caretPosition = text.size();
		} else {
			int totalCharCount = 0;
			for (int i = 0; i < (lines.size()); i++) {
				if (caretPosition < (totalCharCount + lines[i].size() + 1)) {
					caretPosition = totalCharCount + lines[i].size();
					sellen = 0;
					break;
				}
				totalCharCount = totalCharCount + lines[i].size() + 1;
			}
		}
		UpdateOffset();
		Redraw();
	} else if (key == KEY_ENTER) {
		if (!ctrlPressed) {
			KeyChar('\n');
		}
	} else if (key == KEY_DELETE) {
		del();
	} else if (key == KEY_TAB) {
		KeyChar('\t');
	}
	if (wasSymbolTyped && doTriggerValueChangeOnType && valueChangelistener) {
		valueChangelistener(Event(EVENT_WIDGETACTION, Self()->As<CustomTextArea>(), getIntegerValue(), 0, 0, nullptr, text));
	}
	return CustomWidget::KeyDown(key);
}

void CustomTextArea::KeyChar(const int keychar) {
	if (keychar == ' ' && ctrlPressed) {
		return;
	} else {
		CustomTextField::KeyChar(keychar);
	}
}

void CustomTextArea::SetText(const WString& text) {
	bool isSameText = GetText() == text;
	CustomWidget::SetText(text);
	if (!isSameText) {
		UpdateOffset();
	}
}

void CustomTextArea::SetHidden(const bool hide) {
	resetStates();
	CustomTextField::SetHidden(hide);
}

void CustomTextArea::resetStates() {
	ctrlPressed = false;
	shiftPressed = false;
	pressed = false;
	caretPosition = 0;
	sellen = 0;
	offsetX = 0;
	offsetY = 0;
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
		vector<WString> lines;
		Split(text, "\n", lines);
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
					AddBlock(iVec2(caretCoord1.x + offsetX, caretCoord1.y - offsetY), iVec2(caretCoord2.x - caretCoord1.x, stringHeight), color[WIDGETCOLOR_RAISED], false);
					break;
				//selection starts on this line and ends later on another one
				} else {
					auto caretCoord1 = GetCaretCoord(c1);
					int fw = gui->GetTextWidth(gui->font, fontscale, lines[i].Right(lines[i].size() - (c1 - totalCharCount)), fontweight);
					AddBlock(iVec2(caretCoord1.x + offsetX, caretCoord1.y - offsetY), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
					doStartSelection = true;
				}
			//full line selections between start and end lines
			} else if (doStartSelection && c2 > totalCharCount + lines[i].size()) { 
				int fw = !lines[i].empty() ? gui->GetTextWidth(gui->font, fontscale, lines[i], fontweight) : indent;
				AddBlock(iVec2(textIndent + offsetX, i * stringHeight - offsetY), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
			//end selection
			} else if (doStartSelection) {
				auto caretCoord2 = GetCaretCoord(c2);
				int fw = gui->GetTextWidth(gui->font, fontscale, lines[i].Left(c2 - totalCharCount), fontweight);
				AddBlock(iVec2(textIndent + offsetX, caretCoord2.y - offsetY), iVec2(fw, stringHeight), color[WIDGETCOLOR_RAISED], false);
				break;
			}
			totalCharCount = totalCharCount + lines[i].size() + 1;
		}
	}

	//Caret
	if ((style & CUSTOM_TEXT_FIELD_READONLY) == 0 && gui->GetFocus() == Self() && !doHideCursor) {
		auto caretCoord = GetCaretCoord();
		AddBlock(iVec2(caretCoord.x + offsetX, caretCoord.y - offsetY), iVec2(1, stringHeight), color[WIDGETCOLOR_FOREGROUND], false);
	}

	//Label
	if (!text.empty()) {
		auto textPos = iVec2(0) + cornerradius;
		textPos.x = textPos.x + textIndent + offsetX;
		textPos.y = textPos.y - offsetY;
		auto textSize = size;
		textSize.x = textSize.x - cornerradius * textIndent;
		textSize.x = textSize.x - offsetX;
		textSize.y = textSize.y + offsetY;
		WString blockText;
		if ((style & CUSTOM_TEXT_FIELD_PASSWORD) != 0) {
			blockText = std::wstring(text.size(), L'�');
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

void CustomTextArea::Paste() {
	if ((CUSTOM_TEXT_FIELD_READONLY & style) != 0) {
		return;
	}
	if (sellen != 0) {
		KeyChar('\b');
	}
	auto text = GetClipboardText();
	text = text.Replace("\r", "");
	if (text == "") {
		return;
	}
	WString left = this->text.Left(caretPosition);
	WString right = this->text.Right(this->text.size() - caretPosition);
	caretPosition += text.size();
	SetText(left + text + right);
}

void CustomTextArea::setWarpMode(TextAreaWarpMode _warpMode) {
	warpMode = _warpMode;
}


void CustomTextArea::doWarpText() {
	auto width = getWidth();
	if (warpMode == WARP_NONE) {
		return;
	}
	bool doWarp;
	vector<WString> lines;
	Split(text, "\n", lines);
	int totalCharCount = 0;
	vector<WString> words;
	WString lastWord;
	int currentLineWidth;
	bool doChangeText = false;
	do {
		doWarp = false;
		for (int i = 0; i < lines.size(); i++) {
			currentLineWidth = GetInterface()->GetTextWidth(GetInterface()->font, fontscale, lines[i], fontweight);
			if (currentLineWidth > (width - textIndent * 2)) {
				if (warpMode == WARP_WORD) {
					words.clear();
					Split(lines[i], " ", words);
				}
				if (words.size() > 1 || warpMode == WARP_CHAR) {
					lastWord = warpMode == WARP_CHAR || words[words.size() - 1].empty() ? lines[i].Right(1) : words[words.size() - 1];
					if (i < (lines.size() - 1)) {
						if (lines[i + 1].empty()) {
							lines[i + 1] = lastWord;
						} else {
							lines[i + 1] = lastWord == " " || lines[i + 1].Left(1) == " " || warpMode == WARP_CHAR ? lastWord + lines[i + 1] : lastWord + " " + lines[i + 1];
						}
						lines[i] = lines[i].Left(lines[i].size() - lastWord.size());
						doWarp = true;
						break;
					} else {
						lines[i] = lines[i].Left(lines[i].size() - lastWord.size());
						lines.push_back(lastWord);
						doWarp = true;
						break;
					}
				} else {
					doWarp = false;
					break;
				}
			}
			totalCharCount = totalCharCount + (int)lines[i].size() + 1;
		}
		if (doWarp) {
			doChangeText = true;
		}
	} while (doWarp);
	if (doChangeText) {
		WString newText = "";
		for (int i = 0; i < lines.size(); i++) {
			newText = newText + lines[i];
			if (i < (lines.size() - 1)) {
				newText = newText + "\n";
			}
		}
		m_text = newText;
	}	
	return;
}