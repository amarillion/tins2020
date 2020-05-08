#include "editbox.h"
#include <fstream>
#include "widget.h"
#include <allegro5/allegro_font.h>
#include "util.h"
#include <sstream>
#include "textstyle.h"

using namespace std;

EditBox::EditBox()
	:Label(),
	maxLength(0),
	cursorpos(0),
	offsetpos(0),
	selStart(0),
	selEnd(0),
	firstSel(0),
	oldSel(0)
{
	ClearFlag(D_AUTOSIZE);
}

void EditBox::MsgInitSkin() {
	//TODO
//	if (GetBitmapIndex() == -1) SetBitmap(Skin::PANEL_SUNKEN);
//	if (GetSampleIndex(Skin::SAMPLE_GOTFOCUS) == -1) SetSample(Skin::SAMPLE_GOTFOCUS, Skin::SAMPLE_GOTFOCUS);
//	if (GetSampleIndex(Skin::SAMPLE_LOSTFOCUS) == -1) SetSample(Skin::SAMPLE_LOSTFOCUS, Skin::SAMPLE_LOSTFOCUS);
//	if (GetSampleIndex(Skin::SAMPLE_ACTIVATE) == -1) SetSample(Skin::SAMPLE_ACTIVATE, Skin::SAMPLE_ACTIVATE);
//	if (GetSampleIndex(Skin::SAMPLE_KEY) == -1) SetSample(Skin::SAMPLE_KEY, Skin::SAMPLE_KEY);
	ClearFlag(D_AUTOSIZE);
	for (int i=0; i<4; i++) {
//		if (GetFontColor(i) == -1) SetFontColor(skin->fcol[Skin::INFO_EDITBOX][i], skin->scol[Skin::INFO_EDITBOX][i], i);
//		if (GetFontIndex(i) == -1) SetFont(skin->fnt[Skin::INFO_EDITBOX][i], i);
	}
	UpdateSize();
	Widget::MsgInitSkin();
}


int EditBox::FindCursorPos() {
	int p;
	//TODO
	/*
	int x1 = 4;
	char s[12];
	usprintf(s, uconvert_ascii(".", s));
	int l = ustrlen(text);

	//int mx = GetMousePos().x() + skin->GetCursor(Skin::MOUSE_TEXTSELECT).GetFocus().x();
	int mx = GetMousePos().x() + GetParent()->GetMouse()->GetCursor()->GetFocus().x();
	Font f = GetFont(HasFocus() ? Skin::FOCUS : Skin::NORMAL);

	for (p = offsetpos; p < l; p++) {
		usetat(s, 0, ugetat(text, p));
		x1 += f.TextLength(s);
		if (x1 > mx) {
			break;
		}
	}
	 */
	return p;
}


void EditBox::MsgLPress(const Point &p) {
	int oldCursorpos = cursorpos;

	Label::Select();
	selStart = FindCursorPos();
	selEnd = selStart;
	cursorpos = selStart;
	firstSel = selStart;
	if (cursorpos != oldCursorpos) {
		UpdateOffset();
		Redraw();
	}
	oldSel = 0;
	SetFlag(D_PRESSED);

	Widget::MsgLPress(p);
}


void EditBox::MsgMousemove(const Point &d) {
	if (Flags() & D_PRESSED) {
		selEnd = cursorpos = FindCursorPos();
		if (selEnd <= selStart) {
			selStart = selEnd;
			selEnd = firstSel;
		}
		if (selEnd - selStart != oldSel) {
			oldSel = selEnd - selStart;
		}
		UpdateOffset();
		Redraw();
	}

	Widget::MsgMousemove(d);
}


void EditBox::MsgLRelease(const Point &p) {
	if (Flags() & D_PRESSED) {
		cursorpos = FindCursorPos();
		if (selStart == selEnd) {
			Label::Deselect();
		}
		ClearFlag(D_PRESSED);
		UpdateOffset();
		Redraw();
	}

	Widget::MsgLRelease(p);
}


void EditBox::MsgDClick(const Point &p) {
	selStart = 0;
	selEnd = text.length();
	Label::Select();
	Redraw();
	Widget::MsgDClick(p);
}


bool EditBox::MsgChar(int keycode, int unichar, int key_shifts) {
	bool ret = true;
	bool has_changed = false;
	int l = text.length();

	switch (keycode) {
		case ALLEGRO_KEY_LEFT: {
			if (cursorpos > 0) {
				cursorpos--;
				if (key_shifts & ALLEGRO_KEYMOD_SHIFT) {
					if (!isSelected()) {
						Label::Select();
						selStart = cursorpos+1;
						selEnd = selStart;
					}
					if (selStart > cursorpos) {
						selStart = cursorpos;
					}
					else {
						selEnd = cursorpos;
					}
				}
				else {
					Label::Deselect();
				}
			}
			else if (!(key_shifts & ALLEGRO_KEYMOD_SHIFT)) {
				Label::Deselect();
			}
		}
		break;

		case ALLEGRO_KEY_RIGHT: {
			if (cursorpos < l) {
				cursorpos++;
				if (key_shifts & ALLEGRO_KEYMOD_SHIFT) {
					if (!isSelected()) {
						Label::Select();
						selStart = cursorpos-1;
						selEnd = selStart;
					}
					if (selEnd < cursorpos) {
						selEnd = cursorpos;
					}
					else {
						selStart = cursorpos;
					}
				}
				else {
					Label::Deselect();
				}
			}
			else if (!(key_shifts & ALLEGRO_KEYMOD_SHIFT)) {
				Label::Deselect();
			}
		}
		break;

		case ALLEGRO_KEY_HOME: {
			if (key_shifts & ALLEGRO_KEYMOD_SHIFT) {
				if (!isSelected()) {
					selStart = 0;
					selEnd = cursorpos;
					Label::Select();
				}
				else {
					selEnd = selStart;
					selStart = 0;
					Label::Select();
				}
				cursorpos = 0;
			}
			else {
				Label::Deselect();
				cursorpos = 0;
			}
		}
		break;

		case ALLEGRO_KEY_END: {
			if (key_shifts & ALLEGRO_KEYMOD_SHIFT) {
				if (!isSelected()) {
					selStart = cursorpos;
					selEnd = l;
					Label::Select();
				}
				else {
					selStart = selEnd;
					selEnd = l;
					Label::Select();
				}
				cursorpos = l;
			}
			else {
				Label::Deselect();
				cursorpos = l;
			}
		}
		break;

		case ALLEGRO_KEY_DELETE: {
			if (!TestFlag(D_READONLY)) {
				//TODO
				/*
				if (isSelected()) {
					int i=0;
					for (int p=selEnd; ugetat(text, p); p++) {
						usetat(text, selStart+i, ugetat(text, p));
						i++;
					}
					usetat(text, selStart+i, '\0');
					cursorpos = selStart;
					Label::Deselect();
					// TODO
//					PlaySample(Skin::SAMPLE_KEY);
				}
				else
				*/
				if (cursorpos < l) {
					//uremove(text, cursorpos);
					text = text.substr(0, cursorpos) + text.substr(cursorpos + 1);

					//TODO
//					PlaySample(Skin::SAMPLE_KEY);
					has_changed = true;
				}
			}
			else {
				ret = false;
			}
		}
		break;

		case ALLEGRO_KEY_BACKSPACE: {
			if (!TestFlag(D_READONLY)) {
				//TODO
				/*
				if (Selected()) {
					int i=0;
					for (int p=selEnd; ugetat(text, p); p++) {
						usetat(text, selStart+i, ugetat(text, p));
						i++;
					}
					usetat(text, selStart+i, '\0');
					cursorpos = selStart;
					Label::Deselect();
					//TODO
					// PlaySample(Skin::SAMPLE_KEY);
				}
				else
				*/
				if (cursorpos > 0)		{
					--cursorpos;
					// uremove(text, cursorpos);
					text = text.substr(0, cursorpos) + text.substr(cursorpos + 1);

					if (cursorpos <= offsetpos) offsetpos = 0;

					//TODO
//					PlaySample(Skin::SAMPLE_KEY);
					has_changed = true;
				}
			}
			else {
				ret = false;
			}
		}
		break;

		case ALLEGRO_KEY_ENTER: {
			Label::Deselect();
			Redraw();
			//TODO
//			PlaySample(Skin::SAMPLE_ACTIVATE);
			//TODO
//			GetParent()->HandleEvent(*this, callbackID >= 0 ? callbackID : MSG_ACTIVATE);
		}
		break;

//TODO
/*
		case ALLEGRO_KEY_X: {
			if ((key_shifts & KB_CTRL_FLAG) && Selected()) {
				// copy selected text to clipboard
				char *tmp = new char[selEnd - selStart + 2];
				ustrncpy(tmp, text+selStart, selEnd - selStart + 1);
				Clipboard cb;
				cb.SetText(tmp);
				delete [] tmp;

				// delete selected text
				if (!TestFlag(D_READONLY)) {
					int i=0;
					for (int p=selEnd; ugetat(text, p); p++) {
						usetat(text, selStart+i, ugetat(text, p));
						i++;
					}
					usetat(text, selStart+i, '\0');
					cursorpos = selStart;
					Label::Deselect();
				}
			}
			else {
				ret = false;
			}
		}
		break;

		case ALLEGRO_KEY_C: {
			if ((key_shifts & KB_CTRL_FLAG) && Selected()) {
				// copy selected text to clipboard
				char *tmp = new char[selEnd - selStart + 2];
				ustrncpy(tmp, text+selStart, selEnd - selStart + 1);
				Clipboard cb;
				cb.SetText(tmp);
				delete [] tmp;
			}
			else {
				ret = false;
			}
		}
		break;

		case ALLEGRO_KEY_V: {
			// get text from clipboard
			Clipboard cb;
			if ((key_shifts & KB_CTRL_FLAG) && cb.GetText() && !TestFlag(D_READONLY)) {
				// delete selected text
				if (Selected()) {
					int i=0;
					for (int p=selEnd; ugetat(text,p); p++) {
						usetat(text, selStart+i, ugetat(text, p));
						i++;
					}
					usetat(text, selStart+i, '\0');
					cursorpos = selStart;
					Label::Deselect();
				}

				// copy text from clipboard
				for (int i=0; i<cb.bufSize-1; i++) {
					if (cursorpos < maxLength) {
						uinsert(text, cursorpos, cb.buffer[i]);
						cursorpos++;
					}

					UpdateOffset();
					Redraw();
				}
			}
			else {
				ret = false;
			}
		}
		break;
*/
		default: {
			ret = false;
		}
		break;

	};

	UpdateOffset();
	Redraw();

	Widget::MsgChar(keycode, unichar, key_shifts);

	if (!ret) {
		ret = handleUnicodeCharacter(unichar, key_shifts);
	}
	return ret;
}


bool EditBox::handleUnicodeCharacter(int c, int modifier) {
	bool ret = false;

	//TODO unicode support...

	// int l = ustrlen(text);
	int l = text.length();
	if (c >= ' '
			// && uisok(c)
			&& !TestFlag(D_READONLY)) {
		// replace selected text
		/*
		if (Selected()) {
			int i=0;
			for (int p=selEnd; ugetat(text,p); p++) {
				usetat(text, selStart+i, ugetat(text, p));
				i++;
			}
			usetat(text, selStart+i, '\0');
			cursorpos = selStart;
			Label::Deselect();
		}
		*/
		if (l < maxLength) {
			// uinsert(text, cursorpos, c);
			text = text.substr(0, cursorpos) + (char)c + text.substr(cursorpos);
			cursorpos++;

			//TODO
//			PlaySample(Skin::SAMPLE_KEY);
		}

		UpdateOffset();
		Redraw();
		ret = true;
	}

	return ret;
}


void EditBox::MsgGotmouse() {
	if (!TestFlag(D_READONLY)) {
		Mouse::setCursor(
				theSkin->GetCursor(Skin::MOUSE_TEXTSELECT)
		);
	}
	//TODO
//	Widget::MsgGotmouse();
}


void EditBox::MsgLostmouse() {
	if (!TestFlag(D_READONLY)) {
		Mouse::setCursor(
				theSkin->GetCursor(Skin::MOUSE_NORMAL)
		);
	}
	//TODO
//	Widget::MsgLostmouse();
}

/*
void EditBox::Setup(const char *tex, int len) {
	SetText(tex, len);
}
*/

void EditBox::SetText(const char *tex, int len) {
	string tmp = tex;

	int texLen = text.length();
	int l = len;
	if (l <= 0) l = 256;
	maxLength = l;

	if (texLen > len)
	{
		text = tmp.substr(0, len);
	}
	else {
		text = tmp;
	}
	cursorpos = text.length();
	offsetpos = 0;
}


void EditBox::UpdateOffset() {
	int lastchar;
	Bitmap null;
	while (true) {
		// Try with the current offset, but don't really draw
		//TODO
		GraphicsContext dummy;
		lastchar = DrawText(offsetpos, dummy);

		// need to decrease offset?
		if (offsetpos > cursorpos) {
			offsetpos--;
			if (offsetpos < 0) {
				offsetpos = 0;
				break;
			}
		}
		// need to increase offset?
		else if (cursorpos > lastchar+1) {
			offsetpos++;
		}
		// or is it just right?
		else {
			break;
		}
	}
}


void EditBox::draw(const GraphicsContext &gc) {

	Bitmap bmp = theSkin->GetBitmap(Skin::PANEL_SUNKEN);

	Bitmap canvas = Bitmap(gc.buffer);

	int x1 = gc.xofst + getx();
	int y1 = gc.yofst + gety();
	int x2 = x1 + getw();
	int y2 = y2 + geth();

	// Draw the text on the canvas
	bmp.TiledBlit(canvas, 0, 0, x1, y1, getw(), geth(), 1, 1);

	DrawText(offsetpos, gc);

	// Possibly draw the dotted rectangle
	//TODO

	if (/* skin->drawDots && */ hasFocus()) {
		//   dotted_rect (gc.buffer, x1 + 2, y1 + 2, x2 - 3, y2 - 3, skin->c_font);
		al_draw_rectangle (x1 + 2, y1 + 2, x2 - 3, y2 - 3, GREEN, 1.0);
	}

}

// Helper to draw the editable text
// Returned value is last character number drawn
int EditBox::DrawText(int offsetpos, const GraphicsContext &gc) {
	// get the button state
	int state = isDisabled() ? 2 : (hasFocus() ? 3 : 0);

	// get the button colors and font
	ALLEGRO_COLOR fg = theSkin->fcol[Skin::INFO_EDITBOX][state];
	ALLEGRO_COLOR bg = theSkin->scol[Skin::INFO_EDITBOX][state];
	ALLEGRO_FONT *f = GetFont(state);

	ALLEGRO_COLOR fgs = theSkin->fcol[Skin::SELECT][state];
	ALLEGRO_COLOR bgs = theSkin->fcol[Skin::SELECT][state];

	int	x1 = getx() + gc.xofst + 4;
	int	y1 = gety() + gc.yofst + (geth()>>1) - (al_get_font_line_height(f)>>1) + 1;
	int	x2 = x1 + getw() - 10;

	int	i = offsetpos;
	int	len;
	int cursorx = -1;
	int ii;

	// buffer
	char s[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// Assume that we will go to the end
	int there_is_more = 0;

	// Possibly remember where the cursor should go
	if (i == cursorpos) cursorx = x1;

	//TODO: iterate over unicode characters...
	// possibly using boost::locale or ICU UnicodeString.
	// Or alternatively write a wrapper for ALLEGRO_USTR...
	for (auto it = text.begin(); it != text.end(); ++it) {

		// Find the length of the character
		char c = (*it);
		s[0] = c;

		len = al_get_text_width(f, s);

		// Draw the character, possibly with cursor
		if (isSelected() && i>=selStart && i<selEnd) {
			if (hasFocus()) {
				draw_text_with_background(f, fgs, bgs, x1, y1, ALLEGRO_ALIGN_LEFT, s);
			}
			else {
				draw_text_with_background(f, fg, bg, x1, y1, ALLEGRO_ALIGN_LEFT, s);
			}
		}
		else {
			draw_text_with_background(f, fg, bg, x1, y1, ALLEGRO_ALIGN_LEFT, s);
		}

		x1 += len;

		// Stop drawing if we are out of bounds
		if (x1 > x2) {
			// Remember that there is more to draw
			there_is_more = 1;
			break;
		}
		i++;

		// Possibly remember where the cursor should go
		if (i == cursorpos) cursorx = x1;
	}
	// If we ran off the end before finding the cursor, then its at the end
	if ((there_is_more) && (cursorx < 0)) cursorx = x1;

	// Draw the cursor if we have the focus
	if ((cursorx >= 0) && hasFocus()) {
		int hh = al_get_font_line_height(f);
		al_draw_line(cursorx-1, y1, cursorx-1, y1 + hh, fg, 1.0);
	}

	return i;
}


void EditBox::Select() {
	// TODO
	// MsgDClick();
}


void EditBox::Deselect() {
	selStart = selEnd = firstSel = oldSel = 0;
	offsetpos = cursorpos = 0;
	Label::Deselect();
	Redraw();
}


void EditBox::ScrollTo(int offset) {
	Deselect();
	offset = bound(0, offset, (int)text.length());
	cursorpos = offset;
	UpdateOffset();
}

int EditBox::GetMaxLength() {
	return maxLength;
}

ListItemString::ListItemString(const char * val) { /* TODO */ }

void ListBox::InsertItem(ListItemString *item, int pos) { /* TODO */ }
void ListBox::SetMultiSelect(bool val) { /* TODO */ }

void TextArea::LoadLinesFromFile (const char *filename) {
	ifstream f;
	f.open(filename, ios::in);

	std::stringstream ss;
	while (f) {
		string s;
		getline(f, s);
		ss << s << "\n";
	}

	f.close();

	//TODO
//	text = ss.str();
//	CalculateRange(); //TODO
}
