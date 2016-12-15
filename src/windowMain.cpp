#include "windowMain.h"

WindowMain::WindowMain() : mTotalImages(0) {
	setupUi(this);
	
	mSubwindowActions = new QList<QAction*>();
	mSeparatorOpenWindowsAdded = false;
	
    mIconCriminisi = new QIcon(":/icons/Criminisi.png");
    mIconHe = new QIcon(":/icons/He.png");
    mIconNs = new QIcon(":/icons/Ns.png");
    mIconTelea = new QIcon(":/icons/Telea.png");
	
	setContextMenuPolicy(Qt::PreventContextMenu);
	
    mSettings = new QSettings("imageInpaintingSettings.ini", QSettings::IniFormat);
	resize(mSettings->value("size", QSize(700, 480)).toSize());
	move(mSettings->value("pos", QPoint(150, 40)).toPoint());
	uiToolBarFile->setVisible(mSettings->value("uiToolBarFile", true).toBool());
	uiToolBarZoom->setVisible(mSettings->value("uiToolBarZoom", true).toBool());
    uiToolBarAlgos->setVisible(mSettings->value("uiToolBarAlgos", true).toBool());

	mMenuRecentFiles = new QMenu(this);
	for (int n=0; n<maxRecentFiles; ++n) {
		mActionRecentFiles[n] = new QAction(this);
		mActionRecentFiles[n]->setVisible(false);
		connect(mActionRecentFiles[n], &QAction::triggered, this, &WindowMain::openRecentFile);
		uiMenuFile->addAction(mActionRecentFiles[n]);
	}
	mActionSeparatorRecentFiles = uiMenuFile->addSeparator();
	mActionSeparatorRecentFiles->setVisible(false);
	updateRecentFilesMenu();
	mActionExit = new QAction(this);
	mActionExit->setObjectName(QString::fromUtf8("actionExit"));
	mActionExit->setText(QApplication::translate("mainWindow", "Exit", 0));
	mActionExit->setShortcut(QApplication::translate("mainWindow", "Ctrl+Q", 0));
    mActionExit->setIcon(QIcon(":/icons/window-close.svg"));
	uiMenuFile->addAction(mActionExit);

	mToolButtonOpenRecent = new QToolButton(this);
	mToolButtonOpenRecent->setFocusPolicy(Qt::NoFocus);
	mToolButtonOpenRecent->setPopupMode(QToolButton::MenuButtonPopup);
	mToolButtonOpenRecent->setMenu(mMenuRecentFiles);
	mToolButtonOpenRecent->setToolButtonStyle(Qt::ToolButtonIconOnly);
	mToolButtonOpenRecent->setAutoRaise(true);
	mToolButtonOpenRecent->setDefaultAction(uiActionOpen);
    uiToolBarFile->insertWidget(uiActionCaptureWebcam, mToolButtonOpenRecent);

	mActionGroupZoom = new QActionGroup(this);
	mActionGroupZoom->setEnabled(false);
	mActionGroupZoom->addAction(uiActionZoomIn);
	mActionGroupZoom->addAction(uiActionZoomOut);
	mActionGroupZoom->addAction(uiActionZoomOriginal);
	mActionGroupZoom->addAction(uiActionZoomBestFit);
	
	mActionGroupAlgo = new QActionGroup(this);
	mActionGroupAlgo->setEnabled(false);
    mActionGroupAlgo->addAction(uiActionCriminisi);
    mActionGroupAlgo->addAction(uiActionHe);
    mActionGroupAlgo->addAction(uiActionPDE_NS);
    mActionGroupAlgo->addAction(uiActionTELEA);
    mActionGroupAlgo->addAction(uiActionDo4);

	mActionGroupWindow = new QActionGroup(this);
	mActionGroupWindow->setEnabled(false);
	mActionGroupWindow->addAction(uiActionTile);
	mActionGroupWindow->addAction(uiActionCascade);
	mActionGroupWindow->addAction(uiActionNext);
	mActionGroupWindow->addAction(uiActionPrevious);
	mActionGroupWindow->addAction(uiActionDuplicate);
	mActionGroupWindow->addAction(uiActionClose);
	mActionGroupWindow->addAction(uiActionCloseAll);

	mStatusBarLabelZoom = new QLabel(uiStatusBar);
	mStatusBarLabelZoom->setFrameShape(QFrame::NoFrame);
	mStatusBarLabelZoom->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelDimensions = new QLabel(uiStatusBar);
	mStatusBarLabelDimensions->setFrameShape(QFrame::NoFrame);
	mStatusBarLabelDimensions->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelSize = new QLabel(uiStatusBar);
	mStatusBarLabelSize->setFrameShape(QFrame::NoFrame);
	mStatusBarLabelSize->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelTime = new QLabel(uiStatusBar);
	mStatusBarLabelTime->setFrameShape(QFrame::NoFrame);
	mStatusBarLabelTime->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelIcon = new QLabel(uiStatusBar);
	mStatusBarLabelIcon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	mStatusBarLabelIcon->setMinimumSize(QSize(16, 16));
	mStatusBarLabelIcon->setMaximumSize(QSize(16, 16));
	mStatusBarLabelIcon->setScaledContents(true);
	mStatusBarLabelIcon->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelIcon->setVisible(false);
	mStatusBarLabelKeypoints = new QLabel(uiStatusBar);
	mStatusBarLabelKeypoints->setAlignment(Qt::AlignHCenter);
	mStatusBarLabelSpaceRight = new QLabel(uiStatusBar);
	mStatusBarLabelSpaceRight->setMinimumSize(0, 1);
	mStatusBarLabelSpaceLeft = new QLabel(uiStatusBar);
	mStatusBarLabelSpaceLeft->setMinimumSize(0, 1);
	mStatusBarLine = new QFrame(uiStatusBar);
	mStatusBarLine->setVisible(false);
	mStatusBarLine->setFrameShape(QFrame::VLine);
	mStatusBarLine->setFrameShadow(QFrame::Sunken);
	mStatusBarLine2 = new QFrame(mStatusBarLabelTime);
	mStatusBarLine2->setFrameShape(QFrame::VLine);
	mStatusBarLine2->setFrameShadow(QFrame::Sunken);
	mStatusBarLine2->setVisible(false);
	mStatusBarLine3 = new QFrame(mStatusBarLabelTime);
	mStatusBarLine3->setFrameShape(QFrame::VLine);
	mStatusBarLine3->setFrameShadow(QFrame::Sunken);
	mStatusBarLine3->setVisible(false);
	uiStatusBar->addWidget(mStatusBarLabelSpaceLeft);
	uiStatusBar->addWidget(mStatusBarLabelZoom);
	uiStatusBar->addPermanentWidget(mStatusBarLabelTime);
	uiStatusBar->addPermanentWidget(mStatusBarLine2);
	uiStatusBar->addPermanentWidget(mStatusBarLabelIcon);
	uiStatusBar->addPermanentWidget(mStatusBarLabelKeypoints);
	uiStatusBar->addPermanentWidget(mStatusBarLine3);
	uiStatusBar->addPermanentWidget(mStatusBarLabelDimensions);
	uiStatusBar->addPermanentWidget(mStatusBarLine);
	uiStatusBar->addPermanentWidget(mStatusBarLabelSize);
	uiStatusBar->addPermanentWidget(mStatusBarLabelSpaceRight);
	
    mCriminisiToolBar = new QWidget();
    mCriminisiToolBar->setVisible(false);
    mUICriminisi.setupUi(mCriminisiToolBar);
    mUICriminisi.uiComboBoxPatchSize->setCurrentIndex(mSettings->value("Criminisi/PatchSize", 1).toInt());
    connect(mUICriminisi.uiPushButtonShowProcessedImage, &QAbstractButton::clicked, this, &WindowMain::showOutImage);
    connect(mUICriminisi.uiComboBoxPatchSize, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &WindowMain::saveCriminisiParams);
    connect(mUICriminisi.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyCriminisi);
    connect(mUICriminisi.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetCriminisiParams);
    mCriminisiAction = uiToolBarParameters->addWidget(mCriminisiToolBar);

	
    mHeToolBar = new QWidget();
    mHeToolBar->setVisible(false);
//    mUIHe.setupUi(mHeToolBar);
//    mUIHe.uiSpinBoxThreshold->setValue(mSettings->value("He/threshold", 50).toInt());
//    mUIHe.uiPushButtonNonMax->setChecked(mSettings->value("He/nonMaxSuppression", true).toBool());
//    connect(mUIHe.uiSpinBoxThreshold, &QSpinBox::editingFinished, this, &WindowMain::saveHeParams);
//    connect(mUIHe.uiPushButtonNonMax, &QPushButton::toggled, this, &WindowMain::saveHeParams);
//    connect(mUIHe.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyHe);
//    connect(mUIHe.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::restHeParams);
//    mHeAction = uiToolBarParameters->addWidget(mHeToolBar);

    mNSToolBar = new QWidget();
    mNSToolBar->setVisible(false);
//    mUIpde_ns.setupUi(mNSToolBar);
//    mUIpde_ns.uiDoubleSpinBoxThreshold->setValue(mSettings->value("Ns/threshold", 0.04).toDouble());
//    mUIpde_ns.uiDoubleSpinBoxEdgeThreshold->setValue(mSettings->value("Ns/edgeThreshold", 10.0).toDouble());
//    mUIpde_ns.uiSpinBoxAlgos->setValue(mSettings->value("Ns/Algos", 0).toInt());
//    mUIpde_ns.uiSpinBoxLayers->setValue(mSettings->value("Ns/layers", 3).toInt());
//    mUIpde_ns.uiPushButtonShowOrientation->setChecked(mSettings->value("Ns/showOrientation", true).toBool());
//    connect(mUIpde_ns.uiDoubleSpinBoxThreshold, &QSpinBox::editingFinished, this, &WindowMain::saveNsParams);
//    connect(mUIpde_ns.uiDoubleSpinBoxEdgeThreshold, &QSpinBox::editingFinished, this, &WindowMain::saveNsParams);
////	connect(mUIpde_ns.uiSpinBoxOctaves, &QSpinBox::editingFinished, this, &WindowMain::saveNsParams);
//    connect(mUIpde_ns.uiSpinBoxLayers, &QSpinBox::editingFinished, this, &WindowMain::saveNsParams);
//    connect(mUIpde_ns.uiPushButtonShowOrientation, &QPushButton::toggled, this, &WindowMain::saveNsParams);
////	connect(mUIpde_ns.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyNs);
//    connect(mUIpde_ns.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetNsParams);
    mNSAction = uiToolBarParameters->addWidget(mNSToolBar);
	
    mTeleaToolBar = new QWidget();
    mTeleaToolBar->setVisible(false);
//    mUItelea.setupUi(mTeleaToolBar);
//    mUItelea.uiSpinBoxThreshold->setValue(mSettings->value("Telea/threshold", 100).toInt());
//    mUItelea.uiSpinBoxOctaves->setValue(mSettings->value("Telea/octaves", 4).toInt());
//    mUItelea.uiSpinBoxLayers->setValue(mSettings->value("Telea/layers", 3).toInt());
//    mUItelea.uiPushButtonShowOrientation->setChecked(mSettings->value("Telea/showOrientation", true).toBool());
//    connect(mUItelea.uiSpinBoxThreshold, &QSpinBox::editingFinished, this, &WindowMain::saveTeleaParams);
//    connect(mUItelea.uiSpinBoxOctaves, &QSpinBox::editingFinished, this, &WindowMain::saveTeleaParams);
//    connect(mUItelea.uiSpinBoxLayers, &QSpinBox::editingFinished, this, &WindowMain::saveTeleaParams);
//    connect(mUItelea.uiPushButtonShowOrientation, &QPushButton::toggled, this, &WindowMain::saveTeleaParams);
////	connect(mUItelea.uiButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &WindowMain::applyTelea);
//    connect(mUItelea.uiButtonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &WindowMain::resetTeleaParams);
    mTeleaAction = uiToolBarParameters->addWidget(mTeleaToolBar);
	
	switch (mSettings->value("startupParameters", 0).toInt()) {
        case 0: showCriminisiToolBar(); break;
//        case 1: showHeToolBar(); break;
//        case 2: showNsToolBar(); break;
//        case 3: showTeleaToolBar(); break;
	}
	
    connect(uiActionOpen, &QAction::triggered, this, &WindowMain::open);
    connect(uiActionCaptureWebcam, &QAction::triggered, this, &WindowMain::captureWebcam);
    connect(uiActionSaveCopyAs, &QAction::triggered, this, &WindowMain::saveCopyAs);
    connect(uiActionPreferences, &QAction::triggered, this, &WindowMain::preferences);
    connect(mActionExit, &QAction::triggered, this, &WindowMain::exit);
    connect(uiActionCopy, &QAction::triggered, this, &WindowMain::copy);
    connect(uiActionResetImage, &QAction::triggered, this, &WindowMain::resetImage);
    connect(uiActionStartupDialog, &QAction::triggered, this, &WindowMain::startupDialog);
    connect(uiActionZoomIn, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomOut, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomOriginal, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionZoomBestFit, &QAction::triggered, this, &WindowMain::zoom);
    connect(uiActionCriminisi, &QAction::triggered, this, &WindowMain::showCriminisiToolBar);
//    connect(uiActionHe, &QAction::triggered, this, &WindowMain::showHeToolBar);
//    connect(uiActionPDE_NS, &QAction::triggered, this, &WindowMain::showNsToolBar);
//    connect(uiActionTELEA, &QAction::triggered, this, &WindowMain::showTeleaToolBar);
//	connect(uiActionDo4, &QAction::triggered, this, &WindowMain::do4);
    connect(uiActionTile, &QAction::triggered, this, &WindowMain::tile);
    connect(uiActionCascade, &QAction::triggered, this, &WindowMain::cascade);
    connect(uiActionNext, &QAction::triggered, uiMdiArea, &QMdiArea::activateNextSubWindow);
    connect(uiActionPrevious, &QAction::triggered, uiMdiArea, &QMdiArea::activatePreviousSubWindow);
    connect(uiActionDuplicate, &QAction::triggered, this, &WindowMain::duplicate);
    connect(uiActionClose, &QAction::triggered, this, &WindowMain::closeActiveSubWindow);
    connect(uiActionCloseAll, &QAction::triggered, this, &WindowMain::closeAllSubWindows);
    connect(uiActionWebsite, &QAction::triggered, this, &WindowMain::website);
    connect(uiActionAbout, &QAction::triggered, this, &WindowMain::about);
    connect(uiMdiArea, &QMdiArea::subWindowActivated, this, &WindowMain::updateWindowMenu);
    mSignalMapper = new QSignalMapper(this); // for the Open Windows menu entries
    connect(mSignalMapper, static_cast<void (QSignalMapper::*)(QWidget*)>(&QSignalMapper::mapped), this, &WindowMain::setActiveSubWindow);

	
	if (mSettings->value("maximized", true).toBool())
		showMaximized();
	else show();
	
	if (mSettings->value("startupDialog", true).toBool())
		startupDialog();
}




void WindowMain::open() {
	loadFile(QFileDialog::getOpenFileName(this, tr("Open File"), "/home", tr("Images (*.png *.bmp *.jpg)")));
}




void WindowMain::captureWebcam() {
    new WindowCaptureWebcam(this);
}




void WindowMain::saveCopyAs() {
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save Copy As"), QFileInfo(mActiveWindowImage->mWindowTitle).baseName().append(".png"), tr("Images (*.bmp *.png)"));
	if (!fileName.isEmpty())
		mActiveWindowImage->mPixmap.save(fileName);
}




void WindowMain::exit() {
	saveSettings();
	close();
}




void WindowMain::copy() {
	QApplication::clipboard()->setPixmap(mActiveWindowImage->mPixmap);
}




void WindowMain::preferences() {
	new WindowPreferences(this);
}




void WindowMain::startupDialog() {
	new WindowStartup(this);
}




void WindowMain::zoom() {
	if (sender()==uiActionZoomIn)
		mActiveWindowImage->zoomIn();
	else if (sender()==uiActionZoomOut)
		mActiveWindowImage->zoomOut();
	else if (sender()==uiActionZoomOriginal)
		mActiveWindowImage->zoomOriginal();
	else if (sender()==uiActionZoomBestFit)
		mActiveWindowImage->zoomBestFit();
	
	uiActionZoomIn->setEnabled(mActiveWindowImage->mCurrentFactor < 3.0);
	uiActionZoomOut->setEnabled(mActiveWindowImage->mCurrentFactor > 0.25);
	mStatusBarLabelZoom->setText(mActiveWindowImage->mImageZoom);
}

void WindowMain::showOutImage() {
        mActiveWindowImage->showOutImage();
}

void WindowMain::showCriminisiToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mCriminisiAction;
    mCurrentAlgoAction->setVisible(true);
	mSettings->setValue("startupParameters", 0);
    uiActionCriminisi->setChecked(true);
}

void WindowMain::resetCriminisiParams() {
    mUICriminisi.uiComboBoxPatchSize->setCurrentIndex(1);
    saveCriminisiParams();
    mActiveWindowImage->resetImage();
}

void WindowMain::applyCriminisi() {
    mActiveWindowImage->applyCriminisi(mSettings->value("Criminisi/CriminisiApertureSize", 4).toInt());
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Criminisi.png")));
    mActiveWindow->setWindowIcon(*mIconCriminisi);
	applyCommonTasks();
}

void WindowMain::saveCriminisiParams() {
    mSettings->setValue("Criminisi/PatchSize", mUICriminisi.uiComboBoxPatchSize->currentIndex());
}



/*
void WindowMain::showHeToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mHeAction;
    mCurrentAlgoAction->setVisible(true);
	mSettings->setValue("startupParameters", 1);
    uiActionHe->setChecked(true);
}




void WindowMain::restHeParams() {
    mUIHe.uiSpinBoxThreshold->setValue(50);
    mUIHe.uiPushButtonNonMax->setChecked(true);
    saveHeParams();
}




void WindowMain::applyHe() {
    mActiveWindowImage->applyHe(mSettings->value("He/threshold", 50).toInt(), mSettings->value("He/nonMaxSuppression", true).toBool());
    mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/He.png")));
    mActiveWindow->setWindowIcon(*mIconHe);
	applyCommonTasks();
}




void WindowMain::saveHeParams() {
    mSettings->setValue("He/threshold", mUIHe.uiSpinBoxThreshold->value());
    mSettings->setValue("He/nonMaxSuppression", mUIHe.uiPushButtonNonMax->isChecked());
}




void WindowMain::showNsToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mNSAction;
    mCurrentAlgoAction->setVisible(true);
	mSettings->setValue("startupParameters", 2);
    uiActionNs->setChecked(true);
}




void WindowMain::resetNsParams() {
    mUIpde_ns.uiDoubleSpinBoxThreshold->setValue(0.014);
    mUIpde_ns.uiDoubleSpinBoxEdgeThreshold->setValue(10.0);
//	mUIpde_ns.uiSpinBoxOctaves->setValue(3);
    mUIpde_ns.uiSpinBoxLayers->setValue(1);
    mUIpde_ns.uiPushButtonShowOrientation->setChecked(true);
    saveNsParams();
}




//void WindowMain::applyNs() {
//	mActiveWindowImage->applyNs(mSettings->value("Ns/threshold", 0.04).toDouble(),
//			mSettings->value("Ns/edgeThreshold", 10.0).toDouble(),
//			mSettings->value("Ns/Algos", 0).toInt(),
//			mSettings->value("Ns/layers", 3).toInt(),
//			mSettings->value("Ns/showOrientation", true).toBool());
//	mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Ns.png")));
//	mActiveWindow->setWindowIcon(*mIconNs);
//	applyCommonTasks();
//}




void WindowMain::saveNsParams() {
    mSettings->setValue("Ns/threshold", mUIpde_ns.uiDoubleSpinBoxThreshold->value());
    mSettings->setValue("Ns/edgeThreshold", mUIpde_ns.uiDoubleSpinBoxEdgeThreshold->value());
//	mSettings->setValue("Ns/Algos", mUIpde_ns.uiSpinBoxOctaves->value());
    mSettings->setValue("Ns/layers", mUIpde_ns.uiSpinBoxLayers->value());
    mSettings->setValue("Ns/showOrientation", mUIpde_ns.uiPushButtonShowOrientation->isChecked());
}




void WindowMain::showTeleaToolBar() {
    if (mCurrentAlgoAction)
        mCurrentAlgoAction->setVisible(false);
    mCurrentAlgoAction = mTeleaAction;
    mCurrentAlgoAction->setVisible(true);
	mSettings->setValue("startupParameters", 3);
    uiActionTelea->setChecked(true);
}




void WindowMain::resetTeleaParams() {
    mUItelea.uiSpinBoxThreshold->setValue(4000);
    mUItelea.uiSpinBoxOctaves->setValue(3);
    mUItelea.uiSpinBoxLayers->setValue(1);
    mUItelea.uiPushButtonShowOrientation->setChecked(true);
    saveTeleaParams();
}




//void WindowMain::applyTelea() {
//	mActiveWindowImage->applyTelea(mSettings->value("Telea/threshold", 100).toInt(),
//			mSettings->value("Telea/octaves", 4).toInt(),
//			mSettings->value("Telea/layers", 3).toInt(),
//			mSettings->value("Telea/showOrientation", true).toBool());
//	mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Telea.png")));
//	mActiveWindow->setWindowIcon(*mIconTelea);
//	applyCommonTasks();
//}




void WindowMain::saveTeleaParams() {
    mSettings->setValue("Telea/threshold", mUItelea.uiSpinBoxThreshold->value());
    mSettings->setValue("Telea/octaves", mUItelea.uiSpinBoxOctaves->value());
    mSettings->setValue("Telea/layers", mUItelea.uiSpinBoxLayers->value());
    mSettings->setValue("Telea/showOrientation", mUItelea.uiPushButtonShowOrientation->isChecked());
}
*/


// apply result params
void WindowMain::applyCommonTasks() {
	uiActionResetImage->setEnabled(true);
	mStatusBarLabelTime->setText(mActiveWindowImage->mImageTime + " ms");
//	mStatusBarLabelKeypoints->setText(mActiveWindowImage->mImageKeypoints + " keypoints");
	mStatusBarLabelIcon->setVisible(true);
	mStatusBarLine2->setVisible(true);
	mStatusBarLine3->setVisible(true);
}




void WindowMain::resetImage() {
	mActiveWindowImage->resetImage();
    uiActionResetImage->setEnabled(false);
	mActiveWindow->setWindowIcon(QApplication::windowIcon());
	mStatusBarLabelIcon->clear();
	mStatusBarLabelIcon->setVisible(false);
	mStatusBarLabelTime->clear();
	mStatusBarLabelKeypoints->clear();
	mStatusBarLine2->setVisible(false);
	mStatusBarLine3->setVisible(false);
}



/*
void WindowMain::do4() {
    WindowImage* harrisImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* HeImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* NsImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
    WindowImage* TeleaImage = new WindowImage(mActiveWindowImage->mImage, mActiveWindowImage->mWindowTitle);
	
    int sobelApertureSize = 0;
    switch (mSettings->value("harris/sobelApertureSize", 1).toInt()) {
        case 0: sobelApertureSize=1; break;
        case 1: sobelApertureSize=3; break;
        case 2: sobelApertureSize=5; break;
        case 3: sobelApertureSize=7;
    }
    harrisImage->applyHarris(sobelApertureSize,
            mSettings->value("harris/harrisApertureSize", 2).toInt(),
            mSettings->value("harris/kValue", 0.01).toDouble(),
            mSettings->value("harris/threshold", 64).toInt(),
            mSettings->value("harris/showProcessedImage", false).toBool());

    HeImage->applyHe(mSettings->value("He/threshold", 50).toInt(), mSettings->value("He/nonMaxSuppression", true).toBool());

    NsImage->applyNs(mSettings->value("Ns/threshold", 0.04).toDouble(),
            mSettings->value("Ns/edgeThreshold", 10.0).toDouble(),
            mSettings->value("Ns/Algos", 0).toInt(),
            mSettings->value("Ns/layers", 3).toInt(),
            mSettings->value("Ns/showOrientation", true).toBool());

    TeleaImage->applyTelea(mSettings->value("Telea/threshold", 100).toInt(),
            mSettings->value("Telea/octaves", 4).toInt(),
            mSettings->value("Telea/layers", 3).toInt(),
            mSettings->value("Telea/showOrientation", true).toBool());
	
    new WindowDo4(mActiveWindowImage->mWindowTitle, harrisImage, HeImage, NsImage, TeleaImage);
}

*/


void WindowMain::tile() {
	uiMdiArea->tileSubWindows();
	if (mSettings->value("bestFit").toBool()) {
		QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
		for (int n=0; n<subwindows.size(); ++n)
			qobject_cast<WindowImage*>(subwindows.at(n)->widget())->zoomBestFit();
	}
}




void WindowMain::cascade() {
	uiMdiArea->cascadeSubWindows();
	if (mSettings->value("bestFit").toBool()) {
		QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
		for (int n=0; n<subwindows.size(); ++n)
			qobject_cast<WindowImage*>(subwindows.at(n)->widget())->zoomBestFit();
	}
}




void WindowMain::duplicate() {
	WindowImage* imageOriginal = uiMdiArea->findChild<WindowImage*>(QString(mActiveWindowImage->mOriginalUid));
	++imageOriginal->mImageN;
	WindowImage* imageDuplicated = new WindowImage(mActiveWindowImage->mImage,
							imageOriginal->mWindowTitle + QString(" (Duplicated %1)").arg(imageOriginal->mImageN),
							WindowImage::duplicated);
	imageDuplicated->mOriginalUid = imageOriginal->mUid;
	showWindowImage(imageDuplicated);
}




void WindowMain::closeActiveSubWindow() {
	uiMdiArea->closeActiveSubWindow();
}




void WindowMain::closeAllSubWindows() {
	uiMdiArea->closeAllSubWindows();
}




void WindowMain::website() {
    QDesktopServices::openUrl(QUrl::fromEncoded("http://opencv.org/"));
}




void WindowMain::about() {
	new WindowAbout(this);
}




// http://doc.qt.io/qt-5/qtwidgets-mainwindows-mdi-example.html
void WindowMain::updateWindowMenu(QMdiSubWindow* mdiSubWindow) {
	if (!mSeparatorOpenWindowsAdded) { // Adding the separator on Qt Designer doesn't work
		uiMenuWindow->addSeparator();
		mSeparatorOpenWindowsAdded = true;
	}
	for (int n=0; n<mSubwindowActions->size(); ++n) {
// 		uiMenuWindow->removeAction(mSubwindowActions->at(n)); // Makes not to trigger new actions added
		mSubwindowActions->at(n)->setVisible(false);
	}
	mSubwindowActions->clear();
	
	if (mdiSubWindow != 0) {
		mActiveWindow = mdiSubWindow;
		mActiveWindowImage = qobject_cast<WindowImage*> (mdiSubWindow->widget());
		
		uiActionSaveCopyAs->setEnabled(true);
		uiActionCopy->setEnabled(true);
		mActionGroupZoom->setEnabled(true);
		mActionGroupAlgo->setEnabled(true);
		mActionGroupWindow->setEnabled(true);
		uiActionZoomIn->setEnabled(mActiveWindowImage->mCurrentFactor < 3.0);
		uiActionZoomOut->setEnabled(mActiveWindowImage->mCurrentFactor > 0.25);
		
		if (!mActiveWindowImage->mImageTime.isEmpty()) {
			uiActionResetImage->setEnabled(true);
            switch (mActiveWindowImage->mAlgoType) {
                case WindowImage::criminisi: mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/Criminisi.png"))); break;
                case WindowImage::He: mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/Telea.png"))); break;
                case WindowImage::PDE_NS: mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/:/icons/He.png"))); break;
                case WindowImage::PDE_TELEA: mStatusBarLabelIcon->setPixmap(QPixmap::fromImage(QImage(":/icons/Ns.png"))); break;
			}
			mStatusBarLabelIcon->setVisible(true);
			mStatusBarLabelTime->setText(mActiveWindowImage->mImageTime + " ms");
//			mStatusBarLabelKeypoints->setText(mActiveWindowImage->mImageKeypoints + " keypoints");
			mStatusBarLine2->setVisible(true);
			mStatusBarLine3->setVisible(true);
		} else {
			uiActionResetImage->setEnabled(false);
			mStatusBarLabelIcon->setVisible(false);
			mStatusBarLabelTime->clear();
			mStatusBarLabelKeypoints->clear();
			mStatusBarLine2->setVisible(false);
			mStatusBarLine3->setVisible(false);
		}
		
		mStatusBarLabelZoom->setText(mActiveWindowImage->mImageZoom);
		mStatusBarLabelDimensions->setText(mActiveWindowImage->mImageDimensions);
		mStatusBarLabelSize->setText(mActiveWindowImage->mImageSize);
		mStatusBarLine->setVisible(true);
		
		QList<QMdiSubWindow*> subwindows = uiMdiArea->subWindowList();
		for (int n=0; n<subwindows.size(); ++n) {
			WindowImage* windowImage = qobject_cast<WindowImage*> (subwindows.at(n)->widget());
			QString actionName;
			if (n<9)
				actionName = tr("&%1 %2").arg(n+1).arg(windowImage->windowTitle());
			else actionName = tr("%1 %2").arg(n+1).arg(windowImage->windowTitle());
			QAction* actionSubwindow = uiMenuWindow->addAction(actionName);
			mSubwindowActions->append(actionSubwindow);
			actionSubwindow->setCheckable(true);
			if (uiMdiArea->activeSubWindow())
				actionSubwindow->setChecked(windowImage == mActiveWindowImage);
			else actionSubwindow->setChecked(false);
			mActionGroupWindow->addAction(actionSubwindow);
			mSignalMapper->setMapping(actionSubwindow, subwindows.at(n));
			connect(actionSubwindow, &QAction::triggered, mSignalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
		}
	} else if (uiMdiArea->subWindowList().size()==0) {
		uiActionSaveCopyAs->setEnabled(false);
		uiActionCopy->setEnabled(false);
		mActionGroupZoom->setEnabled(false);
		mActionGroupAlgo->setEnabled(false);
		mActionGroupWindow->setEnabled(false);
		uiActionResetImage->setEnabled(false);
		uiToolBarParameters->setEnabled(false);

		mStatusBarLabelTime->clear();
		mStatusBarLabelIcon->setVisible(false);
		mStatusBarLabelKeypoints->clear();
		mStatusBarLabelZoom->clear();
		mStatusBarLabelDimensions->clear();
		mStatusBarLabelSize->clear();
		mStatusBarLine->setVisible(false);
		mStatusBarLine2->setVisible(false);
		mStatusBarLine3->setVisible(false);
	}
}




void WindowMain::loadFile(QString filePath) {
	if (!filePath.isEmpty()) {
		QImage* image = new QImage(filePath);
		if (!image->isNull()) {
			setRecentFile(filePath);
			showWindowImage(new WindowImage(image, filePath));
		} else {
			removeRecentFile(filePath);
            QMessageBox::warning(this, tr("Image Inpainting"), tr("Cannot open %1.").arg(filePath));
		}
	}
}




void WindowMain::showWindowImage(WindowImage* windowImage) {
	windowImage->mUid = ++mTotalImages;
	if (windowImage->mWindowType != WindowImage::duplicated)
		windowImage->mOriginalUid = mTotalImages;
	windowImage->setObjectName(QString(mTotalImages));
	uiMdiArea->addSubWindow(windowImage);
    windowImage->parentWidget()->setGeometry(0, 0, windowImage->mImage->width()+8, windowImage->mImage->height()+31); // 8 and 31 are hardcoded values for the decorations of the subwindow
	if (windowImage->mImage->width() > uiMdiArea->width())
		windowImage->parentWidget()->setGeometry(0, 0, uiMdiArea->width(), windowImage->parentWidget()->height());
	if (windowImage->mImage->height() > uiMdiArea->height())
		windowImage->parentWidget()->setGeometry(0, 0, windowImage->parentWidget()->width(), uiMdiArea->height());
	windowImage->show();
	uiToolBarParameters->setEnabled(true);
}




void WindowMain::setRecentFile(QString filePath) {
	if (mSettings->value("rememberRecentFiles", true).toBool()) {
		QStringList files = mSettings->value("recentFiles").toStringList();
		files.removeAll(filePath);
		files.prepend(filePath);
		while (files.size()>maxRecentFiles)
			files.removeLast();
		mSettings->setValue("recentFiles", files);
		updateRecentFilesMenu();
	}
}




void WindowMain::removeRecentFile(QString filePath) {
	QStringList files = mSettings->value("recentFiles").toStringList();
	files.removeAll(filePath);
	mSettings->setValue("recentFiles", files);
	updateRecentFilesMenu();
}




// http://doc.qt.io/qt-5/qtwidgets-mainwindows-recentfiles-example.html
void WindowMain::updateRecentFilesMenu() {
	QStringList files = mSettings->value("recentFiles").toStringList();
	int numRecentFiles;
	if (files.size() < maxRecentFiles)
		numRecentFiles = files.size();
	else numRecentFiles = maxRecentFiles;
	mActionSeparatorRecentFiles->setVisible(numRecentFiles>0);
	mMenuRecentFiles->clear();
	for (int n=0; n<numRecentFiles; ++n) {
		mActionRecentFiles[n]->setText(tr("&%1 %2").arg(n+1).arg(QFileInfo(files[n]).fileName()));
		mActionRecentFiles[n]->setData(files[n]);
		mActionRecentFiles[n]->setVisible(true);
		mMenuRecentFiles->addAction(mActionRecentFiles[n]);
	}
	for (int n=numRecentFiles; n<maxRecentFiles; ++n)
		mActionRecentFiles[n]->setVisible(false);
}




void WindowMain::openRecentFile() {
	loadFile(qobject_cast<QAction*>(sender())->data().toString());
}




void WindowMain::setActiveSubWindow(QWidget* subWindow) {
	uiMdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(subWindow));
}




void WindowMain::closeEvent(QCloseEvent* eventConstr) {
	saveSettings();
	QWidget::closeEvent(eventConstr);
}




void WindowMain::saveSettings() {
	mSettings->setValue("maximized", isMaximized());
	if (!isMaximized()) {
		mSettings->setValue("pos", pos());
		mSettings->setValue("size", size());
	}
}
