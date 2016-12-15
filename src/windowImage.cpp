#include "windowImage.h"
#include "inpainting_algo.h"


WindowImage::WindowImage(QImage* image, QString windowTitle, int windowType)
    : mImage(image), mWindowTitle(windowTitle), mWindowType(windowType), mImageN(0) {
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

//    loading = new QMovie(":/icons/box.gif");
    mLocale = new QLocale(QLocale::English);

    setWindowTitle(mWindowTitle);

    uiScrollAreaWidgetContents->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    mPixmap = QPixmap::fromImage(*mImage);

    uiLabelImage->setPixmap(mPixmap);

    mImageProcessed = NULL;
    mScaleFactorAbove100 = 0.5;
    mScaleFactorUnder100 = 0.25;
    mFactorIncrement = 0;
    mCurrentFactor = 1.0;

    mOriginalSize = mImage->size();
    mOriginalWidth = mImage->width();
    mOiginalHeight = mImage->height();

    mImageZoom = tr("%1%").arg((int)(mCurrentFactor*100));
    mImageDimensions = tr("%1x%2 px").arg(mOriginalWidth).arg(mOiginalHeight);
    float sizeInKiB = mImage->byteCount()/(float)1024;
    if (sizeInKiB > 1024)
        mImageSize = mLocale->toString(sizeInKiB/(float)1024,'f', 2).append(" MiB");
    else mImageSize = mLocale->toString(sizeInKiB,'f', 2).append(" KiB");
}




void WindowImage::zoomIn() {
    if (mCurrentFactor>=1.0) {
        mFactorIncrement = (mCurrentFactor+mScaleFactorAbove100)/mCurrentFactor;
        mCurrentFactor += mScaleFactorAbove100;
    } else {
        mFactorIncrement = (mCurrentFactor+mScaleFactorUnder100)/mCurrentFactor;
        mCurrentFactor += mScaleFactorUnder100;
    }
    scaleImage();
}




void WindowImage::zoomOut() {
    if (mCurrentFactor>1.0) {
        mFactorIncrement = (mCurrentFactor-mScaleFactorAbove100)/mCurrentFactor;
        mCurrentFactor -= mScaleFactorAbove100;
    } else {
        mFactorIncrement = (mCurrentFactor-mScaleFactorUnder100)/mCurrentFactor;
        mCurrentFactor -= mScaleFactorUnder100;
    }
    scaleImage();
}




void WindowImage::zoomBestFit() {
    float correctF = 0.99; // This correction factor allows the image to fit the subwindow area without scrollbars.
    int scrollWidth = width();
    int scrollHeight = height();

    float relationScroll = scrollWidth/(float)scrollHeight;
    float relationImage = mOriginalWidth/(float)mOiginalHeight;

    float scaleWidth = scrollWidth/(float)mOriginalWidth;
    float scaleHeight = scrollHeight/(float)mOiginalHeight;

    if (relationScroll > relationImage) {
        if (correctF*scaleHeight>1.0)
            mFactorIncrement = correctF*scaleHeight/mCurrentFactor;
        else mFactorIncrement = correctF*scaleHeight/mCurrentFactor;
        mCurrentFactor = correctF*scaleHeight;
    } else {
        if (correctF*scaleWidth>1.0)
            mFactorIncrement = correctF*scaleWidth/mCurrentFactor;
        else mFactorIncrement = correctF*scaleWidth/mCurrentFactor;
        mCurrentFactor = correctF*scaleWidth;
    }

    scaleImage();
}




void WindowImage::zoomOriginal() {
    mFactorIncrement = 1/mCurrentFactor;
    mCurrentFactor = 1.0;
    scaleImage();
}

void WindowImage::showOutImage() {
    if (!mImageProcessed) {
        QMessageBox::information(NULL, "Oh...sorry", "The image has not been processed yet.");
    } else if (mImageProcessed && mShowOutImg) {
        mPixmap = QPixmap::fromImage(*mImageProcessed);
        uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        mShowOutImg = false;
    } else {
        mPixmap = QPixmap::fromImage(*mImage);
        uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        mShowOutImg = true;
    }
}

void WindowImage::applyCriminisi(size_t patchsize) {
    mAlgoType = WindowImage::criminisi;

    if (!mImageProcessed)
    {
//        uiLabelImage->setMovie(loading);
//        loading->start();

        Mat colorMat, maskMat, grayMat, outMat;
        QString maskPath = mWindowTitle;
        maskPath.replace(QString("input"),QString("mask"));

        loadInpaintingImages(
                    mWindowTitle.toStdString(),
                    maskPath.toStdString(),
                    colorMat,
                    maskMat,
                    grayMat
                    );

        float time = (float) getTickCount();
        lc::criminisi(colorMat, maskMat, outMat, patchsize);
        mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
        mShowOutImg = true;

        cvtColor(outMat, outMat, CV_BGR2RGB);
        // mImageProcessed = new QImage((uchar*) outMat.data, outMat.cols, outMat.rows, outMat.step, QImage::Format_RGB888);
        // ---> this do not work, maybe just define a ref, and when outMat is freed, mImageP get nothing
        // have to keep the original cv::Mat image in memory
        // need to call copy(), else the qimage will share the buffer of cv::Mat, and I have:
        mImageProcessed = new QImage(mImage->size(), QImage::Format_RGB888);
        *mImageProcessed = QImage((uchar*) outMat.data, outMat.cols, outMat.rows, outMat.step, QImage::Format_RGB888).copy();

//        loading->stop();
        showOutImage();
    }
}


/*
void WindowImage::applyFast(int threshold, bool nonMaxSuppression) {
    mAlgoType = WindowImage::PDE_NS;
    if (mModified)
        mPixmap = mPixmapOriginal;

    Mat image(mImage->height(), mImage->width(), CV_8UC4, mImage->bits(), mImage->bytesPerLine()); // With CV_8UC3 it doesn't work
    Mat imageGrey(mImage->height(), mImage->width(), CV_8UC1);
    cvtColor(image, imageGrey, CV_RGB2GRAY);

    vector<KeyPoint> keypoints;
    float time = (float) getTickCount();
    FAST(imageGrey, keypoints, threshold, nonMaxSuppression);

    mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
    mImageKeypoints = mLocale->toString((float)keypoints.size(),'f', 0);

    mPainter->begin(&mPixmap);
    QPen pen(QColor::fromRgb(255, 0, 0));
    pen.setWidth(2);
    mPainter->setPen(pen);
    mPainter->setRenderHint(QPainter::Antialiasing);
    for (int n=0; n<keypoints.size(); ++n)
        mPainter->drawEllipse((int)keypoints.at(n).pt.x, (int)keypoints.at(n).pt.y, 4, 4);
    mPainter->end();

    mModified = true;
    uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void WindowImage::applySift(double threshold, double edgeThreshold, int nOctaves, int nOctaveLayers, bool showOrientation) {
    mAlgoType = WindowImage::sift;
    if (mModified)
        mPixmap = mPixmapOriginal;

    Mat image(mImage->height(), mImage->width(), CV_8UC4, mImage->bits(), mImage->bytesPerLine()); // With CV_8UC3 it doesn't work
    Mat imageGrey(mImage->height(), mImage->width(), CV_8UC1);
    cvtColor(image, imageGrey, CV_RGB2GRAY);

    vector<KeyPoint> keypoints;
    float time = (float) getTickCount();
    Ptr<Algo2D> Algo = SIFT::create(nOctaveLayers, nOctaves, threshold, edgeThreshold);
    Algo->detect(imageGrey, keypoints);

    mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
    mImageKeypoints = mLocale->toString((float)keypoints.size(),'f', 0);

    QPoint center;
    int radius;
    mPainter->begin(&mPixmap);
    mPainter->setRenderHint(QPainter::Antialiasing);
    for (int n=0; n<keypoints.size(); ++n) {
        center.setX((int) (keypoints.at(n).pt.x));
        center.setY((int) (keypoints.at(n).pt.y));
        radius = (int) (keypoints.at(n).size); // radius = (int)(keypoints->at(n).size*1.2/9.*2); = 0.266666
        if (showOrientation) {
            mPainter->setPen(QColor::fromRgb(255, 0, 0));
            mPainter->drawLine(QLineF(keypoints.at(n).pt.x,
                    keypoints.at(n).pt.y,
                    keypoints.at(n).pt.x + keypoints.at(n).size*qCos(keypoints.at(n).angle*3.14159265/180),
                    keypoints.at(n).pt.y + keypoints.at(n).size*qSin(keypoints.at(n).angle*3.14159265/180)));
        }
        mPainter->setPen(QColor::fromRgb(0, 0, 255));
        mPainter->drawEllipse(center, radius, radius);
    }
    mPainter->end();

    mModified = true;
    uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}




void WindowImage::applySurf(double threshold, int nOctaves, int nOctaveLayers, bool showOrientation) {
    mAlgoType = WindowImage::surf;
    if (mModified)
        mPixmap = mPixmapOriginal;

    Mat image(mImage->height(), mImage->width(), CV_8UC4, mImage->bits(), mImage->bytesPerLine()); // With CV_8UC3 it doesn't work
    Mat imageGrey(mImage->height(), mImage->width(), CV_8UC1);
    cvtColor(image, imageGrey, CV_RGB2GRAY);

    vector<KeyPoint> keypoints;
    float time = getTickCount();
    Ptr<Algo2D> Algo = SURF::create(threshold, nOctaves, nOctaveLayers, false, false);
    Algo->detect(imageGrey, keypoints);

    mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
    mImageKeypoints = mLocale->toString((float) keypoints.size(),'f', 0);

    QPoint center;
    int radius;
    mPainter->begin(&mPixmap);
    mPainter->setRenderHint(QPainter::Antialiasing);
    for (int n=0; n<keypoints.size(); ++n) {
        center.setX((int) (keypoints.at(n).pt.x));
        center.setY((int) (keypoints.at(n).pt.y));
        radius = (int) (keypoints.at(n).size); // radius = (int)(keypoints->at(n).size*1.2/9.*2); = 0.266666
        if (showOrientation) {
            mPainter->setPen(QColor::fromRgb(255, 0, 0));
            mPainter->drawLine(QLineF(keypoints.at(n).pt.x,
                    keypoints.at(n).pt.y,
                    keypoints.at(n).pt.x + keypoints.at(n).size*qCos(keypoints.at(n).angle*3.14159265/180),
                    keypoints.at(n).pt.y + keypoints.at(n).size*qSin(keypoints.at(n).angle*3.14159265/180)));
        }
        mPainter->setPen(QColor::fromRgb(0, 0, 255));
        mPainter->drawEllipse(center, radius, radius);
    }
    mPainter->end();

    mModified = true;
    uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}
*/

void WindowImage::resetImage() {
    mAlgoType = WindowImage::none;
    mImageTime.clear();
    mImageKeypoints.clear();
    if (mImageProcessed){
        delete mImageProcessed;
        mImageProcessed = NULL;
    }
    mPixmap = QPixmap::fromImage(*mImage);
    uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}



void WindowImage::scaleImage() {
    uiLabelImage->setPixmap(mPixmap.scaled(mCurrentFactor*mOriginalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    adjustScrollBar(horizontalScrollBar());
    adjustScrollBar(verticalScrollBar());
    mImageZoom = tr("%1%").arg((int)(mCurrentFactor*100));
}




void WindowImage::adjustScrollBar(QScrollBar* scrollBar) {
    scrollBar->setValue(int(mFactorIncrement*scrollBar->value()+(mFactorIncrement-1)*scrollBar->pageStep()/2));
}




void WindowImage::mousePressEvent(QMouseEvent* event) {
    mLastPoint = event->pos();
    setCursor(Qt::ClosedHandCursor);
}




void WindowImage::mouseMoveEvent(QMouseEvent* event) {
    QPoint myPos = event->pos();
    int hValue = horizontalScrollBar()->value();
    int vValue = verticalScrollBar()->value();
    horizontalScrollBar()->setValue(hValue+(mLastPoint.x()-myPos.x()));
    verticalScrollBar()->setValue(vValue+(mLastPoint.y()-myPos.y()));
    mLastPoint = myPos;
}




void WindowImage::mouseReleaseEvent(QMouseEvent* event) {
    unsetCursor();
}




void WindowImage::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button()==Qt::LeftButton)
        zoomIn();
    else zoomOut();
}
