#include "windowImage.h"
#include "inpainting_algo.h"


WindowImage::WindowImage(QImage* image, QString windowTitle, int windowType)
    : mImage(image), mWindowTitle(windowTitle), mMaskPath(windowTitle), mWindowType(windowType), mImageN(0) {
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    //    loading = new QMovie(":/icons/box.gif");
    mLocale = new QLocale(QLocale::English);

    setWindowTitle(mWindowTitle);

    uiScrollAreaWidgetContents->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    mPixmap = QPixmap::fromImage(*mImage);

    uiLabelImage->setPixmap(mPixmap);

    mMaskPath.replace(QString("input"),QString("mask"));
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

        loadInpaintingImages(
                    mWindowTitle.toStdString(),
                    mMaskPath.toStdString(),
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

void WindowImage::applyHe(int color_type) {
    mAlgoType = WindowImage::He;

    Mat imgMat, maskTem, maskMat, colorMat;
    imgMat = imread(mWindowTitle.toStdString());
    maskTem = imread(mMaskPath.toStdString(), IMREAD_GRAYSCALE);
    threshold(maskTem, maskMat, 100, true, THRESH_BINARY_INV);

    switch (color_type) {
    case 0:
        colorMat = Mat(imgMat.size(), CV_8UC3, Scalar(0,255,255));
        break;
    case 1:
        colorMat = Mat(imgMat.size(), CV_8UC3, Scalar(0,0,0));
        break;
    case 2:
        colorMat = Mat(imgMat.size(), CV_8UC3, Scalar(0,0,255));
        break;
    }

    colorMat.copyTo(imgMat, maskMat);
    cvtColor(imgMat, imgMat, CV_BGR2RGB);
    mImageProcessed = new QImage(mImage->size(), QImage::Format_RGB888);
    *mImageProcessed = QImage((uchar*) imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888).copy();

    mShowOutImg = true;
    showOutImage();
}

/*
void WindowImage::applyNs(size_t range) {
    mAlgoType = WindowImage::Ns;

    if (!mImageProcessed) {
        Mat colorMat, maskMat, imgMat;
        Mat maskTem;

        colorMat = imread(mWindowTitle.toStdString(), IMREAD_COLOR);
        maskTem = imread(mMaskPath.toStdString(), IMREAD_GRAYSCALE);
        threshold(maskTem, maskMat, 100, true, THRESH_BINARY_INV);

        float time = (float) getTickCount();
        lc::pde_ns(colorMat, maskMat, imgMat, range);
        mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
        mShowOutImg = true;

        mImageProcessed = new QImage(mImage->size(), QImage::Format_RGB888);
        *mImageProcessed = QImage((uchar*) imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888).copy();

        showOutImage();
    }

}

void WindowImage::applyTelea(size_t range) {
    mAlgoType = WindowImage::Telea;

    if (!mImageProcessed) {
        Mat colorMat, maskMat, imgMat;
        Mat maskTem;

        colorMat = imread(mWindowTitle.toStdString(), IMREAD_COLOR);
        maskTem = imread(mMaskPath.toStdString(), IMREAD_GRAYSCALE);
        threshold(maskTem, maskMat, 100, true, THRESH_BINARY_INV);

        float time = (float) getTickCount();
        lc::pde_telea(colorMat, maskMat, imgMat, range);
        mImageTime = mLocale->toString((float)((getTickCount()-time)*1000/getTickFrequency()),'f', 2);
        mShowOutImg = true;

        mImageProcessed = new QImage(mImage->size(), QImage::Format_RGB888);
        *mImageProcessed = QImage((uchar*) imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888).copy();

        showOutImage();
    }
}
*/

void WindowImage::resetImage() {
    mAlgoType = WindowImage::none;
    mImageTime.clear();
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
