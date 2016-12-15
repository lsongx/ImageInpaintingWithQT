#ifndef WINDOWSTARTUP_H
#define WINDOWSTARTUP_H

#include "ui_windowStartup.h"
#include "windowMain.h"

class WindowMain; // http://stackoverflow.com/questions/2133250/does-not-name-a-type-error-in-c

class WindowStartup : public QDialog, private Ui::windowStartup {
  Q_OBJECT
  public:
	WindowStartup(WindowMain* windowMain);

  private:
	WindowMain* mWindowMain;
	QSettings* mSettings;

  private slots:
	void open();
	void captureWebcam();
//	void fastRT();
	void saveSettings();
};

#endif
