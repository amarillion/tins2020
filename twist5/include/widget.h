#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "component.h"
#include "wrappers.h"
#include "skin.h"
#include <functional>
#include "deprecated.h"

// copied from a4
/* return values for the dialog procedures */
#define D_O_K           0        /* normal exit status */

#define TWIST_START_EVENT 1024    /* value for event.type */

// copied from a4
#define MSG_START       1        /* start the dialog, initialise */
#define MSG_END         2        /* dialog is finished - cleanup */
#define MSG_DRAW        3        /* draw the object */
#define MSG_CLICK       4        /* mouse click on the object */
#define MSG_DCLICK      5        /* double click on the object */
#define MSG_KEY         6        /* keyboard shortcut */
#define MSG_CHAR        7        /* other keyboard input */
#define MSG_UCHAR       8        /* unicode keyboard input */
#define MSG_XCHAR       9        /* broadcast character to all objects */
#define MSG_WANTFOCUS   10       /* does object want the input focus? */
#define MSG_GOTFOCUS    11       /* got the input focus */
#define MSG_LOSTFOCUS   12       /* lost the input focus */
#define MSG_GOTMOUSE    13       /* mouse on top of object */
#define MSG_LOSTMOUSE   14       /* mouse moved away from object */
#define MSG_IDLE        15       /* update any background stuff */
#define MSG_RADIO       16       /* clear radio buttons */
#define MSG_WHEEL       17       /* mouse wheel moved */
#define MSG_LPRESS      18       /* mouse left button pressed */
#define MSG_LRELEASE    19       /* mouse left button released */
#define MSG_MPRESS      20       /* mouse middle button pressed */
#define MSG_MRELEASE    21       /* mouse middle button released */
#define MSG_RPRESS      22       /* mouse right button pressed */
#define MSG_RRELEASE    23       /* mouse right button released */
#define MSG_WANTMOUSE   24       /* does object want the mouse? */
#define MSG_USER        25       /* from here on are free... */

/**
	sent when a widget has been activated (button pressed, checkbox toggled, a list entry double clicked, etc)
*/
#define		MSG_ACTIVATE		MSG_USER+3

/**
 * Sent when a component wants the focus, for example after it is clicked or after the mouse entered.
 */
#define MSG_REQUEST_FOCUS	MSG_USER + 4

/**
	sent when a widget that has the D_EXIT flag set was selected
*/
#define		MSG_CLOSE			MSG_USER+10

/** message from viewport to contents that the viewport has been resized */
#define		MSG_VIEWPORT_RESIZED	MSG_USER+11

/**
 * message from child to parent that the child has resized and that the parent should adjust to match.
 * Used for e.g. menu items, when font or label has changed.
 */
#define		MSG_CHILD_RESIZED	MSG_USER+12

/**
 * Message from a menu item to the menu, that a submenu should be popped up.
 */
#define		MSG_CHILD_POPUP		MSG_USER+13

/** message from animating component to parent that the animation has finished */
#define		MSG_ANIMATION_DONE		MSG_USER+14

/**
	this is the first user available message.

	MASkinG uses messages for communication between widgets and dialogs.
	For this it uses both the message constants it defines and those defined
	by Allegro itself. All messages above MSG_SUSER may be used by the user.
*/
#define		MSG_SUSER			MSG_USER+15

const int ALLEGRO_EVENT_TWIST_MOUSE_ENTER_WIDGET = ALLEGRO_GET_EVENT_TYPE('t','m','e','w');
const int ALLEGRO_EVENT_TWIST_MOUSE_LEAVE_WIDGET = ALLEGRO_GET_EVENT_TYPE('t','m','l','w');

class Dialog;

class Widget : public Component
{
private:
	int key;
	ALLEGRO_BITMAP *buffer;
protected:
	bool bufferSizeMismatch()
	{
		return buffer != NULL && (al_get_bitmap_width(buffer) != getw() || al_get_bitmap_height(buffer) != geth());
	}

	/** initialise back buffer for this component. Call during init and resize */
	void resetBuffer();
	/** draw current state of component to the buffer */
	void updateBuffer();
	virtual void doDraw (const GraphicsContext &gc) {}

	Bitmap border;
	/** called automatically by UpdateSize(). Override this to implement special adjustment after screen resize*/
	virtual void onResize() {}
public:
	Widget() : key(0), buffer(NULL), border() {
		SetFlag(D_DIRTY);
	}

	virtual ~Widget() {
		if (buffer != NULL) al_destroy_bitmap(buffer);
	}

	int GetCallbackID() { return key; }

	void SetTooltipText (const char *txt);
	void SetCallbackID(int id);
	DEPRECATED void Redraw();
	DEPRECATED void Redraw(const Rect &rect);

	/** Set a border by skin index. Note that this only takes effect if child widgets make use of the value in their draw() method */
	void setBorder(int i) { border = GetSkin()->GetBitmap(i); SetFlag(D_DIRTY); }

	/** use setVisible(false) instead */
	DEPRECATED void Hide() { setVisible(false); }
	/** use setVisible(true) instead */
	DEPRECATED void Unhide() { setVisible(true); }

	virtual void draw(const GraphicsContext &gc) override;

	virtual bool MsgClose();
	virtual void MsgTick() final; // TODO same as update!
	virtual void MsgTimer(int id);

	virtual void HandleEvent( Widget &source, int msg, int value);

	virtual void MsgLPress (const Point &p) {}
	virtual void MsgRPress (const Point &p) {}
	virtual void MsgLRelease (const Point &p) {}
	virtual void MsgRRelease (const Point &p) {}
	virtual void MsgClick (const Point &p) {}
	virtual void MsgDClick (const Point &p) {}

	/** use makeToggleButton instead */
	DEPRECATED void setToggle(bool value) { makeToggleButton(value); }

	/** make this a toggle button (or menu item), meaning it can toggle between selected and deselected state.
	 * Controls with this flag are usually drawn differently, e.g. they can have a checkmark */
	void makeToggleButton(bool value) { setFlagVal(D_TOGGLE, value); }
	bool isToggleButton() { return TestFlag(D_TOGGLE); }

	/** toggle between selected and deselected. */
	void Toggle() {
		ToggleFlag(D_SELECTED);
		SetFlag(D_DIRTY);
	}

	bool isSelected() const { return TestFlag(D_SELECTED); }
	void Select() { SetFlag(D_SELECTED); SetFlag(D_DIRTY); }
	void Deselect() { ClearFlag(D_SELECTED); SetFlag(D_DIRTY); }

	bool hasFocus() { return TestFlag(D_GOTFOCUS); }
	void setFocus(bool val) { if (hasFocus() != val) { ToggleFlag(D_GOTFOCUS); SetFlag(D_DIRTY); } }

	/** deprecated: use isDisabled() instead */
	DEPRECATED bool Disabled() const { return isDisabled(); }
	/** deprecated: use isSelected() instead */
	DEPRECATED bool Selected() const { return isSelected(); }
	/** deprecated: use hasFocus instead */
	DEPRECATED bool HasFocus() { return hasFocus(); }

	void setEnabled (bool val) { if (isEnabled() != val) { ToggleFlag(D_DISABLED); SetFlag(D_DIRTY); } }
	bool isDisabled() const { return TestFlag(D_DISABLED); }
	bool isEnabled() { return !TestFlag(D_DISABLED); }

	virtual void MsgStart ();
	virtual void MsgMousemove(const Point &d) {};
	virtual void MsgDraw ();
	virtual bool HasMouse ();
	virtual void MsgInitSkin ();

	/**
	 * MsgChar is called for a keypress,
	 * or periodically while a key is held.
	 *
	 * @param keycode is one of the ALLEGRO_KEY_*** constants
	 * @param unicode is the unicode codepoint representing this character
	 * @param modifier is a combination of the ALLEGRO_KEYMOD_*** constants.
	 */
	virtual bool MsgChar(int keycode, int unicode, int modifier) { return false; };

    /** called whenever the mouse pointer enters the widget area */
    virtual void onMouseEnter() {}

    /** called whenever the mouse pointer leaves the widget area.
     * Every mouse enter event is guaranteed to be paired with a mouse leave event */
    virtual void onMouseLeave() {}

	Skin *GetSkin();
	ALLEGRO_COLOR GetFontColor(int state);
	ALLEGRO_COLOR GetShadowColor(int state);
	ALLEGRO_FONT *GetFont(int state);


	virtual void handleEvent(ALLEGRO_EVENT &evt) override;

	/**
	   Tests if two references to widgets are one and the same.
	*/
	bool operator==(const Widget& obj) const;
	/**
	   Tests if two references to widgets are not the same.
	*/
	bool operator!=(const Widget& obj) const;

	virtual std::string const className() const override { return "Widget"; }
	virtual void UpdateSize() override { onResize(); }
};

class TextWidget : public Widget
{
protected:
	std::string text;
public:
	TextWidget() : text() {}
	TextWidget(const std::string &text) : text(text) { }
	void SetText(const char *val);
};

typedef std::function< void()> ActionFunc;
typedef std::shared_ptr<Widget> WidgetPtr;

#endif
