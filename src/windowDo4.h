#ifndef WINDOWDO4_H
#define WINDOWDO4_H

#include "ui_windowDo4.h"
#include "windowImage.h"

using namespace std;

class WindowDo4 : public QWidget, Ui::windowDo4 {
  Q_OBJECT
  public:
	WindowDo4(QString windowTitle, WindowImage* harrisImage, WindowImage* fastImage, WindowImage* siftImage, WindowImage* surfImage);

  private:
	void changeEvent(QEvent* event);
	
	WindowImage* mHarrisImage;
	WindowImage* mFastImage;
	WindowImage* mSiftImage;
	WindowImage* mSurfImage;
	
	QTimer* mTimer;

  private slots:
	void zoomBestFit();
};

#endif
