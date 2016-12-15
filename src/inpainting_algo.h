#pragma once

#include "opencv2/opencv.hpp"

#include "utils.hpp"

#ifndef DEBUG
	#define DEBUG 0
#endif

namespace lc
{

int criminisi( cv::InputArray  _src, cv::InputArray _mask, cv::OutputArray _dst, size_t _patchsize = RADIUS);

int pde_ns( cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst, double _inpaintRange);

int pde_bertamio( cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst);

int pde_telea( cv::InputArray _src, cv::InputArray _mask, cv::OutputArray _dst, double _inpaintRange);

}