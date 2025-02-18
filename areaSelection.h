#ifndef AREA_SELECTION_H
#define AREA_SELECTION_H
#include <opencv2/opencv.hpp>

cv::Point2f rotatePoint(const cv::Point2f& point, const cv::Point2f& center, float angle) {
	float rad = angle * CV_PI / 180.0;
	float cosA = cos(rad);
	float sinA = sin(rad);
	cv::Point2f rotated;
	rotated.x = cosA * (point.x - center.x) - sinA * (point.y - center.y) + center.x;
	rotated.y = sinA * (point.x - center.x) + cosA * (point.y - center.y) + center.y;
	return rotated;
}

cv::Mat dorotate(const cv::Mat& ff, const std::string win_name, const  float& angle) {
	int x1 = 0.25 * ff.cols; 
	int y1 = 0.45 * ff.rows;

	int x2 = 0.75 * ff.cols; 
	int y2 = y1+ slection_width;

	cv::Scalar color = cv::Scalar(255, 0, 255);  // Blue color in BGR
	int thickness = 1;
	const  float& angle1 = 45; const  float& angle2 = 315;
	// Define the four corners of the rectangle
	std::vector<cv::Point2f> points1;
	points1.push_back(cv::Point2f(x1, y1));
	points1.push_back(cv::Point2f(x2, y1));
	points1.push_back(cv::Point2f(x2, y2));
	points1.push_back(cv::Point2f(x1, y2)); 
	
	std::vector<cv::Point2f> points2;
	points2.push_back(cv::Point2f(x1, y1));
	points2.push_back(cv::Point2f(x2, y1));
	points2.push_back(cv::Point2f(x2, y2));
	points2.push_back(cv::Point2f(x1, y2));
	// Calculate the center of the rectangle
	cv::Point2f center((x1 + x2) / 2.0, (y1 + y2) / 2.0);
	// Rotate the points
	for (auto& point1 : points1) {
		point1 = rotatePoint(point1, center, angle1);
	}
	for (auto& point2 : points2) {
		point2 = rotatePoint(point2, center, angle2);
	}
	// Draw the rotated rectangle as a quadrilateral
	for (size_t i = 0; i < points1.size(); i++) {
		cv::line(ff, points1[i], points1[(i + 1) % points1.size()], color, thickness);//can we draw a rectangle and line with same size, please help here?
	}
	for (size_t i = 0; i < points2.size(); i++) {
		cv::line(ff, points2[i], points2[(i + 1) % points2.size()], color, thickness);//can we draw a rectangle and line with same size, please help here?
	}
	// Draw the rotated rectangle on a mask
	cv::Mat mask = cv::Mat::zeros(ff.size(), CV_8UC1);
	std::vector<cv::Point> contour1(points1.begin(), points1.end());
	std::vector<cv::Point> contour2(points2.begin(), points2.end());
	std::vector<std::vector<cv::Point>> contours1 = { contour1 };
	std::vector<std::vector<cv::Point>> contours2 = { contour2 };
	cv::drawContours(mask, contours1, 0, cv::Scalar(255), cv::FILLED);
	cv::drawContours(mask, contours2, 0, cv::Scalar(255), cv::FILLED);
	// Create a black background image
	cv::Mat blackBackground(ff.size(), ff.type(), cv::Scalar(0, 0, 0));
	// Copy the rotated rectangle area from the input frame to the black background
	ff.copyTo(blackBackground, mask);
	//cv::cvtColor(blackBackground, result, cv::COLOR_BGR2BGRA);
	// Show the result
	//cv::imshow(win_name, blackBackground);
	int widt = x2 - x1;
	int high = y2 - y1;

	int area = 2 * widt * high;// substract the overlap area please

	return blackBackground;

}



cv::Mat get_star_roi(cv::Mat& inputFrame) {
	int x1 = 0.25 * inputFrame.cols;
	int y1 = 0.45 * inputFrame.rows;

	int x11 = 0.49 * inputFrame.cols;
	int y11 = 0.15 * inputFrame.rows;

	// Ensure the ROIs are within the image boundaries
	int width1 = 200;
	int height1 = slection_width;

	int width2 = slection_width;
	int height2 = 250;
	// Make sure the ROI dimensions do not exceed the image dimensions
	width1 = std::min(width1, inputFrame.cols - x1);
	height1 = std::min(height1, inputFrame.rows - y1);
	width2 = std::min(width2, inputFrame.cols - x11);
	height2 = std::min(height2, inputFrame.rows - y11);
	// Extract the ROIs
	cv::Mat roi1 = inputFrame(cv::Rect(x1, y1, width1, height1));
	cv::Mat roi2 = inputFrame(cv::Rect(x11, y11, width2, height2));
	cv::Mat right = dorotate(inputFrame, "Right", 315);
	cv::rectangle(inputFrame, cv::Point(x1, y1), cv::Point(x1 + width1, y1 + height1), cv::Scalar(255), 1);
	cv::rectangle(inputFrame, cv::Point(x11, y11), cv::Point(x11 + width2, y11 + height2), cv::Scalar(255), 1);
	// Create an empty image with the same type as inputFrame
	cv::Mat outputImage = cv::Mat::zeros(inputFrame.size(), inputFrame.type());
	cv::Rect leftRect(0, 0, inputFrame.cols, inputFrame.rows);
	right.copyTo(outputImage(leftRect));
	roi1.copyTo(outputImage(cv::Rect(x1, y1, roi1.cols, roi1.rows)));
	roi2.copyTo(outputImage(cv::Rect(x11, y11, roi2.cols, roi2.rows)));
	
	cv::Rect crr(x1, y11, width1,height2);
	cv::Mat crop = outputImage(crr);
	return crop;
}


double delta(double contrast) {
		static std::deque<double> contrastValues; // Static to retain values between function calls
		double sum = 0;
		int expectedSize = 30;
		contrastValues.push_back(contrast);
		while (contrastValues.size() > expectedSize) {
			contrastValues.pop_front();
		}
		if (contrastValues.size() > 1) {
			auto it = contrastValues.begin();
			double prevVal = *it;
			++it;
			while (it != contrastValues.end()) {
				double currentVal = *it;
				sum += std::abs(currentVal - prevVal);
				prevVal = currentVal;
				++it;
			}
		}
		return sum;
	}

	double stdev(double contrast) {
		static std::deque<double> contrastValues;
		double sum = 0;
		int expectedSize = 30;
		contrastValues.push_back(contrast);
		while (contrastValues.size() > expectedSize) {
			contrastValues.pop_front();
		}
		int countLastN = contrastValues.size();
		for (double val : contrastValues) {
			sum += val;
		}
		double mean = (countLastN > 0) ? (sum / countLastN) : 0.0;
		double variance = 0.0;
		for (double val : contrastValues) {
			variance += std::pow(val - mean, 2);
		}
		variance /= countLastN;
		double stdev = std::sqrt(variance);
		return stdev;
	}

	double run_fun(cv::Mat& iframe) {
		static cv::Mat f1, f2;
		try {
			cv::Mat grayFrame;
			cv::cvtColor(iframe, grayFrame, cv::COLOR_BGR2GRAY);
			//cv::imshow("LLAA",grayFrame);
			int height = grayFrame.rows;
			int width = grayFrame.cols;

			if (f1.empty()) {
				f1 = grayFrame.clone();
				Logger("First frame..", "INFO");
				return 0.0;
			}

			f2 = f1;
			f1 = grayFrame.clone();

			double totalDiff = 0.0;
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					const int pixel1 = f1.at<uchar>(y, x);
					const int pixel2 = f2.at<uchar>(y, x);
					totalDiff += std::abs(pixel1 - pixel2);
				}
			}
			double result = std::stod(getprefString(SELECTED_AREA_POLYGON));
			return totalDiff/(result);
		}
		catch (const std::exception& e) {
			Logger(e.what(), "ERROR");
		}
		catch (...) {
			Logger("Unknown error occurred in BrightnessCalculation.", "ERROR");
		}
		return 0.0; // In case of error, return 0.0
	}

double sum_of(double dd) {
	static std::deque<double> values;  // A deque to store the last 30 values
	static double current_sum = 0;     // The current sum of the last 30 values
	static size_t max_size = 30;       // Maximum number of values to store

	// If the deque has reached the maximum size, remove the oldest value
	if (values.size() >= max_size) {
		current_sum -= values.front(); // Subtract the oldest value from the sum
		values.pop_front();            // Remove the oldest value from the deque
	}

	// Add the new value to the deque and update the sum
	values.push_back(dd);
	current_sum += dd;

	return current_sum;
}



cv::Mat drawPolygonAndHighlightArea(const cv::Mat& frame, const std::vector<cv::Point>& polygonPoints) {
	// Clone the original frame to draw the polygon
	cv::Mat frameWithPolygon = frame.clone();

	// Draw the polygon
	const cv::Scalar color = cv::Scalar(255, 0, 255); // Magenta color in BGR
	const int thickness = 1;
	const int lineType = cv::LINE_AA;

	// Draw the polygon lines
	for (size_t i = 0; i < polygonPoints.size(); i++) {
		cv::line(frame, polygonPoints[i], polygonPoints[(i + 1) % polygonPoints.size()], color, thickness, lineType);
	}

	// Create a mask for the polygon area
	cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
	std::vector<std::vector<cv::Point>> contours = { polygonPoints };
	cv::drawContours(mask, contours, 0, cv::Scalar(255), cv::FILLED);

	// Create a result frame with the polygon area highlighted
	cv::Mat highlightedFrame;
	frame.copyTo(highlightedFrame, mask);

	// Find the bounding rectangle of the polygon
	cv::Rect boundingRect = cv::boundingRect(contours[0]);

	// Crop the highlighted area using the bounding rectangle
	cv::Mat croppedHighlightedFrame = highlightedFrame(boundingRect);
	double area = cv::contourArea(polygonPoints);
	SetPreference(SELECTED_AREA_POLYGON, std::to_string(area));

	return croppedHighlightedFrame;
}

#endif