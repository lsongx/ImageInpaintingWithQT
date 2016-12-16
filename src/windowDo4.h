#ifndef WINDOWDO4_H
#define WINDOWDO4_H

#include "ui_windowDo4.h"
#include "windowImage.h"

using namespace std;

class WindowDo4 : public QWidget, Ui::windowDo4 {
  Q_OBJECT
  public:
    WindowDo4(QString windowTitle, WindowImage* CriminisiImage, WindowImage* HeImage, WindowImage* NsImage, WindowImage* TeleaImage);

  private:
	void changeEvent(QEvent* event);
	
    WindowImage* mCriminisiImage;
    WindowImage* mHeImage;
    WindowImage* mNsImage;
    WindowImage* mTeleaImage;
	
	QTimer* mTimer;

  private slots:
	void zoomBestFit();
};

#endif
