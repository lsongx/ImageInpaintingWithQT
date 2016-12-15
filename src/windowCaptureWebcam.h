#ifndef WINDOWCAPTUREWEBCAM_H
#define WINDOWCAPTUREWEBCAM_H

#include <opencv2/opencv.hpp>
#include "ui_windowCaptureWebcam.h"
#include "windowMain.h"
#include "windowImage.h"

using namespace cv;

class WindowMain; // http://stackoverflow.com/questions/2133250/does-not-name-a-type-error-in-c

class WindowCaptureWebcam : public QDialog, private Ui::windowCaptureWebcam {
  Q_OBJECT
  public:
	WindowCaptureWebcam(WindowMain*);
	void closeEvent(QCloseEvent*);
	
	WindowMain* mWindowMain;
	
  private:
	QTimer* mTimer;
	VideoCapture mCamera;
	Mat mImageRT;
	QImage* mImage;

  private slots:
	void capture();
	void ok();
	void compute();
	void close();
};

#endif
