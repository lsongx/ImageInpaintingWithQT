#ifndef WINDOWIMAGE_H
#define WINDOWIMAGE_H

#include <QtWidgets>
#include <opencv2/opencv.hpp>
#include "ui_windowImage.h"

using namespace std;
using namespace cv;

class WindowImage : public QScrollArea, Ui::windowImage {
    Q_OBJECT
public:
    WindowImage(QImage* image, QString windowTitle, int windowType=normal);
    void zoomIn();
    void zoomOut();
    void zoomOriginal();
    void zoomBestFit();
    void resetImage();
    void showOutImage();
    void applyCriminisi(size_t patchsize);
    void applyHe(int color_type);
    void applyNs(size_t range);
    void applyTelea(size_t range);

    enum windowType {normal=0, duplicated=1, fromWebcam=2, do4=3};
    enum AlgoType {none=0, criminisi=1, He=2, Ns=3, Telea=4};
    enum colorType {yellow = 0, black = 1, red = 2};
    QImage* mImage;
    QImage* mImageProcessed;
    QPixmap mPixmap;
    QString mImageZoom;
    QString mImageTime;
    QString mImageDimensions;
    QString mImageSize;
    QString mWindowTitle;
    QString mMaskPath;
    QString mUid;
    QString mOriginalUid;
    float mCurrentFactor;
    int mWindowType, mAlgoType, mImageN;
    bool mShowOutImg;

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    void scaleImage();
    void adjustScrollBar(QScrollBar* scrollBar);

    //    QMovie *loading;
    QSize mOriginalSize;
    QPoint mLastPoint;
    QLocale* mLocale;
    int mOriginalWidth, mOiginalHeight;
    float mScaleFactorAbove100, mScaleFactorUnder100, mFactorIncrement;
};

#endif
