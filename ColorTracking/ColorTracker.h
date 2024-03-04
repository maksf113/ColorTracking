#pragma once
#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
class ColorTracker
{
private:
	cv::Mat m_img;
	cv::Mat m_imgHSV;
	cv::Mat m_mask;
	cv::VideoCapture m_capture;
	int m_hmin, m_smin, m_vmin;
	int m_hmax, m_smax, m_vmax;
	std::vector<cv::Point> m_points;
	int m_pointsNumber;

	void CaptureFrame();
	void CreateTrackbars();
	void GetMask();
	cv::Mat ExtractColor();
	void CalibrateColor();
	void TrackColor();
	cv::Point GetPoint();

public:
	ColorTracker() : 
		m_capture(0), m_hmin(0), m_smin(0), m_vmin(0),
		m_hmax(179), m_smax(255), m_vmax(255), m_pointsNumber(100)
	{
		m_points.resize(m_pointsNumber);
	}

	void Run();
};
