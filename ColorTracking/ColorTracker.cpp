#pragma once
#include "ColorTracker.h"


void ColorTracker::CaptureFrame()
{
	m_capture.read(m_img);
	cv::cvtColor(m_img, m_imgHSV, cv::COLOR_BGR2HSV);
}

void ColorTracker::CreateTrackbars()
{
	cv::namedWindow("Trackbars");
	cv::createTrackbar("Hue Min", "Trackbars", &m_hmin, 179);
	cv::createTrackbar("Sat Min", "Trackbars", &m_smin, 255);
	cv::createTrackbar("Val Min", "Trackbars", &m_vmin, 255);
	cv::createTrackbar("Hue Max", "Trackbars", &m_hmax, 179);
	cv::createTrackbar("Sat Max", "Trackbars", &m_smax, 255);
	cv::createTrackbar("Val Max", "Trackbars", &m_vmax, 255);
}

void ColorTracker::GetMask()
{
	cv::Scalar lower(m_hmin, m_smin, m_vmin);
	cv::Scalar upper(m_hmax, m_smax, m_vmax);
	cv::inRange(m_imgHSV, lower, upper, m_mask);
}

cv::Mat ColorTracker::ExtractColor()
{
	GetMask();
	cv::Mat result(m_imgHSV.rows, m_imgHSV.cols, m_imgHSV.type(), cv::Scalar(0, 0, 0));
	for (int i = 0; i < m_imgHSV.rows; i++)
		for (int j = 0; j < m_imgHSV.cols; j++)
			if (m_mask.at<uchar>(i, j) == 255)
				result.at<cv::Vec3b>(i, j) = m_imgHSV.at<cv::Vec3b>(i, j);
	return result;
}

void ColorTracker::CalibrateColor()
{
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::Mat imgDilate, imgClose, imgErode, imgOpen;
	int key = -1;
	while (key != 113) // q
	{
		CaptureFrame();
		cv::Mat imgExtract = ExtractColor();
		cv::erode(imgExtract, imgErode, kernel);
		cv::dilate(imgErode, imgOpen, kernel);
		cv::imshow("Cam", m_img);
		cv::imshow("Color extractiom", imgOpen);
		key = cv::waitKey(1);
	}
	cv::destroyWindow("Color extraction");
}
void ColorTracker::TrackColor()
{
	int i = 0;
	cv::Point point = GetPoint();
	for (auto& p : m_points)
		p = point;
	while (true)
	{
		CaptureFrame();
		point = GetPoint();
		m_points[i] = point;
		i++;
		if (i == m_pointsNumber)
			i = 0;
		for (const auto& p : m_points)
			cv::circle(m_img, p, 10, cv::Scalar(20, 255, 36), cv::FILLED);
		cv::imshow("Tracking", m_img);
		cv::waitKey(1);
	}
}
cv::Point ColorTracker::GetPoint()
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	GetMask();
	cv::findContours(m_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::vector<std::vector<cv::Point>> contourPolygons(contours.size());
	std::vector<cv::Rect> boundingRectangles(contours.size());
	double areaMax = cv::contourArea(contours[0]);
	int iMax = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > areaMax)
		{
			areaMax = area;
			iMax = i;
		}
			

		if (area >= 20)
		{
			double length = cv::arcLength(contours[i], true); // true -> closed contour
			double accuracy = 0.05;
			cv::approxPolyDP(contours[i], contourPolygons[i], accuracy * length, true);
			boundingRectangles[i] = cv::boundingRect(contourPolygons[i]);
			cv::drawContours(m_img, contourPolygons, i, cv::Scalar(0, 0, 255), 2);
			cv::rectangle(m_img, boundingRectangles[i], cv::Scalar(36, 23, 88));
		}

	}
	cv::Point point(0, 0);
	if (boundingRectangles.data())
		point = 0.5 * (boundingRectangles[iMax].tl() + boundingRectangles[iMax].br());
	return point;
}

void ColorTracker::Run()
{
	CreateTrackbars();
	CalibrateColor();
	TrackColor();
}



