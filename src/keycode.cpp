#include "keycode.h"


const QString ktos( int keycode )
{
	if (keycode > MAXKEY || keycode < 0) keycode = 0;

	if (keycode == 0) return "[NO KEY]";
	
	QString xname = XKeysymToString(XKeycodeToKeysym(display, keycode,0));

//this section of code converts standard X11 keynames into much nicer names
//which are prettier, fit the dialogs better, and are more readily understandable.
//This is really ugly and I wish I didn't have to do this... that's why there
//is a config option to define PLAIN_KEYS and drop this whole section of code,
//instead using the default names for keys.
#ifndef PLAIN_KEYS
	//the following code assumes xname is system independent and always
	//in the same exact format.
	
	QRegExp rx;
	rx.setPattern("^\\w$");
	//"a-z" -> "A-Z"
	if (rx.exactMatch(xname)) return xname.upper();
	
	rx.setPattern("(.*)_(.*)");
	if (rx.exactMatch(xname)) {
		QString first = rx.cap(1);
		QString second = rx.cap(2);
		
		rx.setPattern("^[RL]$");
		//"Control_R" -> "R Control"
		if (rx.exactMatch(second)) return second + " " + first;
		
		rx.setPattern("^(Lock|Enter)$");
		//"Caps_Lock" -> "Caps Lock"
		//"KP_Enter" -> "KP Enter"
		if (rx.exactMatch(second)) return first + " " + second;
		
		//the following assumes all number pads are laid out alike.
		if (xname == "KP_Home")		return "KP 7";
		if (xname == "KP_Up")		return "KP 8";
		if (xname == "KP_Prior")	return "KP 9";
		if (xname == "KP_Subtract")	return "KP -";
		if (xname == "KP_Left")		return "KP 4";
		if (xname == "KP_Begin")	return "KP 5";
		if (xname == "KP_Right")	return "KP 6";
		if (xname == "KP_Add")		return "KP +";
		if (xname == "KP_End")		return "KP 1";
		if (xname == "KP_Down")		return "KP 2";
		if (xname == "KP_Next")		return "KP 3";
		if (xname == "KP_Insert")	return "KP 0";
		if (xname == "KP_Delete")	return "KP .";
		if (xname == "KP_Multiply")	return "KP *";
		if (xname == "KP_Divide")	return "KP /";
		
		return xname;
	}
	
	if (xname == "minus")			return "-";
	if (xname == "equal")			return "=";
	if (xname == "bracketleft")		return "[";
	if (xname == "bracketright")	return "]";
	if (xname == "semicolon")		return ";";
	if (xname == "apostrophe")		return "'";
	if (xname == "grave")			return "`";
	if (xname == "backslash")		return "\\";
	if (xname == "comma")			return ",";
	if (xname == "period")			return ".";
	if (xname == "slash")			return "/";
	if (xname == "space")			return "Space";
	if (xname == "Prior")			return "PageUp";
	if (xname == "Next")			return "PageDown";
#endif
	
	//if none of that succeeded,
	return xname;
}




GetKey::GetKey( QString button, bool m )
		 :QDialog( )
{
	//prepare the dialog
	mouse = m;
	setCaption( "Choose a key" );
	setIcon(QPixmap(ICON24));

	//I'd use a QLabel, but that steals x11Events!
	//So, I'll draw the text directly. That means
	//I need to resolve the size of the dialog by hand:
	Text = "Choose a new key ";
	if (mouse) Text += "or mouse button ";
	Text += "for " + button;
	QRect rect = fontMetrics().boundingRect( Text );
	//I calculate the size based on the first line of text, which is longer.
	//The fontMetrics function is dumb and would treat the string with a
	//newline in it as a continues flow of characters if I did the whole string
	//at once.
	Text += "\n(Ctrl-X for no key)";
	//now I add 20 pixels of padding and double the height to make room for
	//two lines.
	setFixedSize( QSize( rect.width() + 20, rect.height()*2 + 20 ) );
}

bool GetKey::x11Event( XEvent* e )
{
	//keep Qt from closing the dialog upon seeing Esc pressed.
	if (e->type == KeyPress) return true;
	
	//On a key press, return the key and quit
	//Ctrl+X == [No Key]
	if (e->type == KeyRelease) {
		if (XKeycodeToKeysym(display,e->xkey.keycode,0) == XK_x ) {
			if (e->xkey.state & ControlMask) done( 0 );
			else done( e->xkey.keycode );
		}
		else done( e->xkey.keycode );
		return true;
	}
	//if we're accepting mouse clicks and a mouse button was clicked...
	if (mouse && e->type == ButtonRelease) {
		done ( e->xbutton.button + MOUSE_OFFSET);
		return true;
	}

	//any other events we will pass on to the dialog. This allows for closing
	//the window and easy redrawing  :)
	return false;
}

void GetKey::paintEvent ( QPaintEvent * ) {
	//whenever we need to repaint, draw in our text.
	QPainter paint( this );
	paint.drawText( rect(), AlignCenter, Text );
}







KeyButton::KeyButton( QString name, int val, QWidget* parent, bool m, bool nowMouse)
	:QPushButton(nowMouse?"Mouse " + QString::number(val):QString(ktos(val)), parent) {
	mouse = m;
	mouseClicked = nowMouse;
	buttonname = name;
	value = val;
	connect( this, SIGNAL( clicked() ), SLOT( onClick() ));
}

void KeyButton::onClick() {
	//when clicked, ask for a key!
	value = GetKey( buttonname, mouse ).exec();
	//if the return value was a mouse click...
	if (value > MOUSE_OFFSET) {
		mouseClicked = true;
		value -= MOUSE_OFFSET;
		setText( "Mouse " + QString::number(value));
	}
	//otherwise, it was a key press!
	else {
		mouseClicked = false;
		setText( ktos(value));
	}
}
