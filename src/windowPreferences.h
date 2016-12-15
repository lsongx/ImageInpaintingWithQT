#ifndef WINDOWPREFERENCES_H
#define WINDOWPREFERENCES_H

#include "ui_windowPreferences.h"
#include "windowMain.h"

class WindowMain; // http://stackoverflow.com/questions/2133250/does-not-name-a-type-error-in-c

class WindowPreferences : public QDialog, private Ui::windowPreferences {
  Q_OBJECT
  public:
	WindowPreferences(WindowMain* windowMain);

  private:
	WindowMain* mWindowMain;
	QSettings* mSettings;

  private slots:
	void clearRecentFilesPrompt();
	void clearRecentFiles();
	void restore();
	void save();
};

#endif
