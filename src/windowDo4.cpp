#include "windowDo4.h"

WindowDo4::WindowDo4(QString windowTitle, WindowImage* CriminisiImage, WindowImage* HeImage, WindowImage* NsImage, WindowImage* TeleaImage)
        : mCriminisiImage(CriminisiImage), mHeImage(HeImage), mNsImage(NsImage), mTeleaImage(TeleaImage), mTimer(new QTimer()) {
	setupUi(this);
	
	setWindowTitle(windowTitle + " - Do4!");
	setAttribute(Qt::WA_DeleteOnClose);
	
    uiHLayout1->insertWidget(1, mCriminisiImage);
    uiHLayout1->insertWidget(3, mHeImage);
    uiHLayout2->insertWidget(1, mNsImage);
    uiHLayout2->insertWidget(3, mTeleaImage);
	
    uiCriminisiTimeLabel->setText(mCriminisiImage->mImageTime);
    uiHeTimeLabel->setText(mHeImage->mImageTime);
    uiNsTimeLabel->setText(mNsImage->mImageTime);
    uiTeleaTimeLabel->setText(mTeleaImage->mImageTime);
	
	connect(uiPushButtonZoomBestFit, &QPushButton::released, this, &WindowDo4::zoomBestFit);
	connect(mTimer, &QTimer::timeout, this, &WindowDo4::zoomBestFit);
	
	// http://wiki.qt.io/Center_a_Window_on_the_Screen
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
	show();
	
	// This timer is to resize the images once the WindowStateChange event is fired because the window is actually resized
	// after the event is fired, i.e., when the window's size is not yet ready and images would get a not fit size.
	mTimer->setSingleShot(true);
	mTimer->setInterval(200);
	mTimer->start();
	
}




void WindowDo4::zoomBestFit() {
    mCriminisiImage->zoomBestFit();
    mHeImage->zoomBestFit();
    mNsImage->zoomBestFit();
    mTeleaImage->zoomBestFit();
}




void WindowDo4::changeEvent(QEvent* event) {
	if (event->type() == QEvent::WindowStateChange) {
		QWindowStateChangeEvent* eventb = static_cast<QWindowStateChangeEvent*>(event);
		if (eventb->oldState() == Qt::WindowMaximized && this->windowState() == Qt::WindowNoState)
			mTimer->start();
		else if (eventb->oldState() == Qt::WindowNoState && this->windowState() == Qt::WindowMaximized)
			mTimer->start();
	}
}
