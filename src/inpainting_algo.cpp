#include "inpainting_algo.h"

int lc::criminisi(cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst, size_t _patchsize)
{
    cv::Mat src = _src.getMat(), mask = _mask.getMat();
    _dst.create(cv::Size(src.cols-2*_patchsize,src.rows-2*_patchsize), CV_8UC3);
    cv::Mat dst = _dst.getMat();

    cv::Mat dst_tem;
    src.copyTo(dst_tem);

    cv::Mat grayMat;
    cv::cvtColor(src, grayMat, CV_BGR2GRAY);

    // confidenceMat - confidence picture + border
    cv::Mat confidenceMat;
    mask.convertTo(confidenceMat, CV_32F);
    confidenceMat /= 255.0f;

    // add borders around mask and confidenceMat
    cv::copyMakeBorder(mask, mask,
        _patchsize, _patchsize, _patchsize, _patchsize,
        cv::BORDER_CONSTANT, 255);
    cv::copyMakeBorder(confidenceMat, confidenceMat,
        _patchsize, _patchsize, _patchsize, _patchsize,
        cv::BORDER_CONSTANT, 0.0001f);

    // ---------------- start the algorithm -----------------
    contours_t contours;            // mask contours
    hierarchy_t hierarchy;          // contours hierarchy

    cv::Mat priorityMat(			// priorityMat - priority values for all contour points + border
        confidenceMat.size(),
        CV_32FC1
    );  // priority value matrix for each contour point

    cv::Point psiHatP;          // psiHatP - point of highest confidence

    cv::Mat psiHatPColor;       // color patch around psiHatP

    cv::Mat psiHatPConfidence;  // confidence patch around psiHatP
    double confidence;          // confidence of psiHatPConfidence

    cv::Point psiHatQ;          // psiHatQ - point of closest patch

    cv::Mat result;             // holds result from template matching
    cv::Mat erodedMask;         // eroded mask

    cv::Mat templateMask;       // mask for template match (3 channel)

                                // eroded mask is used to ensure that psiHatQ is not overlapping with target
    cv::erode(mask, erodedMask, cv::Mat(), cv::Point(-1, -1), _patchsize);

    // main loop
    const size_t area = mask.total();

    while (cv::countNonZero(mask) != area)   // end when target is filled
    {
        // set priority matrix to -.1, lower than 0 so that border area is never selected
        priorityMat.setTo(-0.1f);

        // get the contours of mask
        getContours((mask == 0), contours, hierarchy);

        // compute the priority for all contour points
        computePriority(contours, grayMat, confidenceMat, priorityMat);

        // get the patch with the greatest priority
        cv::minMaxLoc(priorityMat, NULL, NULL, NULL, &psiHatP);
        psiHatPColor = getPatch(dst_tem, psiHatP);
        psiHatPConfidence = getPatch(confidenceMat, psiHatP);

        cv::Mat confInv = (psiHatPConfidence != 0.0f);
        confInv.convertTo(confInv, CV_32F);
        confInv /= 255.0f;
        // get the patch in source with least distance to psiHatPColor wrt source of psiHatP
        cv::Mat mergeArrays[3] = { confInv, confInv, confInv };
        cv::merge(mergeArrays, 3, templateMask);
        result = computeSSD(psiHatPColor, dst_tem, templateMask);

        // set all target regions to 1.1, which is over the maximum value possilbe
        // from SSD
        result.setTo(1.1f, erodedMask == 0);
        // get minimum point of SSD between psiHatPColor and dst
        cv::minMaxLoc(result, NULL, NULL, &psiHatQ);

        assert(psiHatQ != psiHatP);

        // updates
        // copy from psiHatQ to psiHatP for each colorspace
        transferPatch(psiHatQ, psiHatP, grayMat, (mask == 0));
        transferPatch(psiHatQ, psiHatP, dst_tem, (mask == 0));

        // fill in confidenceMat with confidences C(pixel) = C(psiHatP)
        confidence = computeConfidence(psiHatPConfidence);
        assert(0 <= confidence && confidence <= 1.0f);
        // update confidence
        psiHatPConfidence.setTo(confidence, (psiHatPConfidence == 0.0f));
        // update mask
        mask = (confidenceMat != 0.0f);
    }

    dst_tem.convertTo(dst_tem, CV_8UC3, 255.0);
    cv::Mat dst_roi = dst_tem(cv::Range(_patchsize,src.rows-_patchsize),cv::Range(_patchsize,src.cols-_patchsize));
    dst_roi.copyTo(dst); // low efficiency? but how to get this? when this function over, dst_tem was destroyed.

    return 0;
}

int lc::pde_bertamio(cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst)
{
	return 1;
}

int lc::pde_ns(cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst, double _inpaintRange)
{
	cv::inpaint( _src,  _mask,  _dst, _inpaintRange, cv::INPAINT_NS);
	return 0;
}

int lc::pde_telea(cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst, double _inpaintRange)
{
	cv::inpaint( _src, _mask, _dst, _inpaintRange, cv::INPAINT_TELEA);
	return 0;
}
