#ifndef BUTTON_EDIT_H
#define BUTTON_EDIT_H

//used to build the dialog:
#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlayout.h>

//we need to edit a Button
#include "button.h"
//to get a new key for the button.
#include "keycode.h"

class ButtonEdit : public QDialog {
	Q_OBJECT
	public:
		ButtonEdit(Button* butt);
		void show();
	protected slots:
		void accept();
	protected:
		Button *button;
		KeyButton *BKKey;
		QPushButton *BKey, *BMouse, *BOkay, *BCancel;
		QPushButton **BMKey;
		QCheckBox *CSticky, *CRapid;
};


#endif
