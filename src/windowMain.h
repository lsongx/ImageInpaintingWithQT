#ifndef WINDOWMAIN_H
#define WINDOWMAIN_H

#include <QtDebug>
#include <QtCore>
#include <QtWidgets>
#include "ui_windowMain.h"
#include "ui_barCriminisi.h"
#include "ui_barHe.h"
#include "ui_barPDE_NS.h"
#include "ui_barTELEA.h"
#include "windowImage.h"
#include "windowCaptureWebcam.h"
#include "windowPreferences.h"
#include "windowAbout.h"
#include "windowStartup.h"
#include "windowDo4.h"

class WindowMain : public QMainWindow, public Ui::windowMain {
  Q_OBJECT
  public:
	WindowMain();
	void updateRecentFilesMenu();
	void showWindowImage(WindowImage*);

	QToolButton* mToolButtonOpenRecent;
	QMenu* mMenuRecentFiles;
	int mCapturedWebcamImages;
	int mTotalImages;

  public slots:
	void open();
    void captureWebcam();

  private:
	void applyCommonTasks();
	void loadFile(QString);
	void closeEvent(QCloseEvent*);
	void saveSettings();
	void setRecentFile(QString);
	void removeRecentFile(QString);

	bool mSeparatorOpenWindowsAdded; // Adding the separator on Qt Designer doesn't work
	QSettings* mSettings;
	QAction* mActionExit;
	QAction* mActionSeparatorRecentFiles;
	QSignalMapper* mSignalMapper;
	QActionGroup* mActionGroupZoom;
	QActionGroup* mActionGroupAlgo;
	QActionGroup* mActionGroupWindow;
    QButtonGroup* mButtonGroupMask;
	enum { maxRecentFiles=8 };
	QAction* mActionRecentFiles[maxRecentFiles];
    QAction* mCriminisiAction;
    QAction* mHeAction;
    QAction* mNSAction;
    QAction* mTeleaAction;
    QAction* mCurrentAlgoAction;
	QList<QAction*>* mSubwindowActions;
    Ui::barCriminisi mUICriminisi;
    Ui::barHe mUIHe;
    Ui::barPDE_NS mUIpde_ns;
    Ui::barTELEA mUItelea;
    QWidget* mCriminisiToolBar;
    QWidget* mHeToolBar;
    QWidget* mNSToolBar;
    QWidget* mTeleaToolBar;

	QMdiSubWindow* mActiveWindow;
	WindowImage* mActiveWindowImage;
    QIcon* mIconCriminisi;
    QIcon* mIconHe;
    QIcon* mIconNs;
    QIcon* mIconTelea;
	QLabel* mStatusBarLabelZoom;
	QLabel* mStatusBarLabelDimensions;
	QLabel* mStatusBarLabelSize;
	QLabel* mStatusBarLabelTime;
	QLabel* mStatusBarLabelIcon;
	QLabel* mStatusBarLabelSpaceRight;
	QLabel* mStatusBarLabelSpaceLeft;
	QFrame* mStatusBarLine;
	QFrame* mStatusBarLine2;
	QFrame* mStatusBarLine3;

  private slots:
	void saveCopyAs();
	void preferences();
	void exit();
	
	void copy();
	void resetImage();
	
	void startupDialog();
	void zoom();
	
    void showOutImage();
    void changeProcessedUi();

    void showCriminisiToolBar();
    void applyCriminisi();
    void saveCriminisiParams();
    void resetCriminisiParams();
	

    void showHeToolBar();
    void showMask();

    void showNsToolBar();
    void applyNs();
    void saveNsParams();
    void resetNsParams();
	
    void showTeleaToolBar();
    void applyTelea();
    void saveTeleaParams();
    void resetTeleaParams();
	
    void do4();
    /*
*/
	void tile();
	void cascade();
	void duplicate();
	void closeActiveSubWindow();
	void closeAllSubWindows();
	
	void website();
	void about();

	void updateWindowMenu(QMdiSubWindow*);
	void openRecentFile();
	void setActiveSubWindow(QWidget*);
};

#endif
