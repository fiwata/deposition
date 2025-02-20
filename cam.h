#ifndef CAM_H
#define CAM_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "spiralshape.h"
#include "MyUi.h"
#include <vector>
#include <string>
#include "DaqSignal.h"
#include "areaSelection.h"
#include "constants.h"
#include <math.h>

class Cam {
private:

	MyUI myUIInstance;
	cv::Mat frame;
	cv::VideoCapture cap;
	cv::VideoWriter videoWriter;
	std::ifstream filepre;
	std::vector<std::vector<std::string>> rows;
	std::string csv_file;
	std::string csv_line;
	std::atomic<bool> running;
	std::mutex frameMutex;
	std::condition_variable frameCond;
	std::atomic<bool> initialized;
	cv::Mat dframe, tmpFrameOriginal, tmpFrameCropped, tmpcalcFrame, ppttzz, combinedFrame, sdgrphlayout, newFrame;
	cv::Mat x_stage_out, y_stage_out, z_stage_out, cfc;
	std::set<std::string> windows; // Set of window names
	std::deque<double> b_store, bd_store, x_store, y_store, z_store, null,sec;
	bool isDrawingCompleted = false;
	cv::Rect box; // Rectangle to be resized
	bool drawing_box = false;
	//Spiral mySpiral;
	cv::Rect boxSecond; // Rectangle to be resized
	bool drawing_boxSecond = false;
	int cameraid = 0;
	std::string filepath;
	std::string vi_file;
	int looping_index = 0;
	void captureLoop() {
		try {
			cameraIndex();
			if (getCameraId() != -1) {
				cap.open(getCameraId());
				//setcvflip(TRUE);
			}
			else {
				cap.open(getFilePath());
				//setcvflip(FALSE);
			}

			if (!cap.isOpened()) {
				throw std::runtime_error("Error: Could not open camera");
			}
			initialized = true; // Indicate that the camera has been initialized

			while (running) {
				cap >> newFrame; // Capture a new frame
				videoWriter << newFrame;
				//pztmax = key_double(PZT_KEY);
				cameraIndex();
				//time = key_double(TIME_KEY);
				if (newFrame.empty()) {
					continue; // Skip if frame is empty
				}
				{
					std::lock_guard<std::mutex> lock(frameMutex);
					if (getprefString(CameraONoFF) == "OFF") {
						break;
					}

					if (getcvflip()) {
						cv::flip(newFrame, frame, 1);
					}
					else {
						frame = newFrame.clone();
					}

					//frame = newFrame.clone(); // Update the frame
				}
				frameCond.notify_all(); // Notify that a new frame is available
			}
			cap.release(); // Release the camera
			videoWriter.release();
		}
		catch (...) {
			myUIInstance.messi("kkaa");
		}
	}

public:
	Cam() : running(false), initialized(false) {
	}

	~Cam() {
		stop();
	}
	std::string getFilePath() const {
		return filepath;
	}
	void cameraIndex() {
		std::string indexString = getprefString(CameraIndex);
		std::string result;
		for (char c : indexString) {
			if (c == '\\') {
				result += "\\\\";
			}
			else if (c != '"') {
				result += c;
			}
		}
		if (indexString == "0") {
			cameraid = 0;
			filepath = result; // No filepath when camera index is 0
		}
		else if (indexString == "1") {
			cameraid = 1;
			filepath = result; // No filepath when camera index is 1
		}
		else {
			cameraid = -1; // Indicate invalid camera index
			filepath = result; // Set the filepath
		}
	}
	int getCameraId() const {
		return cameraid;
	}
	void start() {
		if (!running) {
			running = true;
			std::thread(&Cam::captureLoop, this).detach();
		}
	}

	void stop() {
		destroyWindow("Camera");
		if (running) {
			running = false;
			std::unique_lock<std::mutex> lock(frameMutex);
			frameCond.notify_all(); // Wake up any waiting threads to exit
		}
	}
	void writeFrameToVideo(cv::VideoWriter& videoWriter, const cv::Mat& frame) {
		if (frame.empty()) {
			std::cerr << "Error: Frame is empty!" << std::endl;
			return;
		}
		vi_file = getprefString(CURRENT_VIDEO_FOLDER) + "\\" + getprefString(CURRENT_FILENAME_KEY) + ".mp4";
		if (!isFilexists(vi_file)) {
			videoWriter.open(vi_file, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30.0, cv::Size(frame.cols, frame.rows));
			if (!videoWriter.isOpened()) {
				myUIInstance.messi("Error: Couldn't open the output video file for writing!");
				return;
			}
		}
		if (getprefString(AUTORECORD_KEY) == "on" || getprefString(DEPOSITION_ON_OFF) == "ON") {
			videoWriter.write(frame);//Logger("Frame written to video.");
		}

	}
	cv::Mat getFrame() {
		std::unique_lock<std::mutex> lock(frameMutex);
		frameCond.wait(lock, [this]() { return !frame.empty() || !running; });
		cv::Mat annotatedFrame = frame.clone();
		cv::Mat ichi = letsdo(annotatedFrame);
		writeFrameToVideo(videoWriter, ichi);
		return running ? ichi : cv::Mat();
		// return running ? frame.clone() : cv::Mat();
	}
	double key_double(const std::string& key) {
		double result = std::stod(getprefString(key));
		return result;
	}
	int fScrnX_w(double percent) {
		const int camwidth = std::round(percent * GetSystemMetrics(SM_CXSCREEN));
		return camwidth;
	}
	int fScrnY_h(double percent) {
		const int rowheight = std::round(percent * GetSystemMetrics(SM_CYSCREEN));
		return rowheight;
	}

	double brightness;

	void setBrightness(cv::Mat& tmpcalcFrame) {
		brightness = run_fun(tmpcalcFrame);
	}
	const double getBrightness() {
		return brightness;
	}
	double finalCalculation() {
		if (getprefString(ADORDIFF_KEY) == "on") {
			return stdev(getBrightness());
		}
		else {
			return sum_of(getBrightness());
		}
	}

	double i = 0;
	void setINC(double ii) {
		i = ii;
	}
	double x = 0;
	void setX(double xx
	) {
		if (xx > 5) {
			x = 5.0;
		}
		else if (xx < 0) {
			x = 0.0;
		}
		else {
			x = xx;
		}
	}
	double y = 0;
	void setY(double yy) {
		if (yy > 5) {
			y = 5.0;
		}
		else if (yy < 0) {
			y = 0.0;
		}
		else {
			y = yy;
		}
	}
	double z = 0;
	void setZ(double zz) {
		if (zz > 5) {
			z = 5.0;
		}
		else if (zz < 0) {
			z = 0.0;
		}
		else {
			z = zz;
		}
	}
	double getINC() {
		return i;
	}
	double getX() {
		return x;
	}
	double getY() {
		return y;
	}
	double getZ() {
		return z;
	}

	//速度計算
	void vcalc() {
		double v;
		double t;
		t = (double)(other_time+miliseconds_deposition);
		v = total_height * 6000 / (total_step * t / 1000);
		velocity = (int)v;
	}

	void set_count() {
		count = (int)count_redeposition;
	}

	cv::Mat letsdo(cv::Mat frame) {
		try {
			int msqx1 = key_double(MSQX1_KEY);
			int	msqy1 = key_double(MSQY1_KEY);
			int msqx2 = key_double(MSQX2_KEY);
			int msqy2 = key_double(MSQY2_KEY);//mainBigCrop
			int sqx1 = key_double(SQW_KEY);
			int	sqy1 = key_double(SQH_KEY);
			int	sqw = key_double(SQX1_KEY);
			int	sqh = key_double(SQY1_KEY);//calculationarea

			cv::resize(frame, frame, cv::Size(fScrnX_w(0.25) - 10, fScrnY_h(0.45) - 10));
			cv::cvtColor(frame, tmpFrameOriginal, cv::COLOR_BGR2BGRA);

			cv::rectangle(tmpFrameOriginal, cv::Point(msqx1, msqy1), cv::Point(msqx1 + msqx2, msqy1 + msqy2), RED, 1);
			cv::Mat croppedFrame = frame(cv::Rect(msqx1, msqy1, msqx2, msqy2));

			cv::cvtColor(croppedFrame, tmpFrameCropped, cv::COLOR_BGR2BGRA);
			cv::resize(tmpFrameCropped, tmpFrameCropped, cv::Size(fScrnX_w(0.25) - 10, fScrnY_h(0.45) - 10));


						/*
			std::vector<cv::Point> polygonPoints = {

			   cv::Point(50, 110),//A

			   cv::Point(95, 110),//B

			   cv::Point(75, 205),//B
			   cv::Point(80, 210),//B

			   cv::Point(100, 115),//B

			   cv::Point(100, 300),//C
			   cv::Point(110, 300),//D

			   cv::Point(110, 110),//E

			   cv::Point(200, 110),//F
			   cv::Point(200, 100),//G

			   cv::Point(110, 100),//H

			   cv::Point(110, 0),//I
			   cv::Point(100, 0),//J

			   cv::Point(100, 100),//K

			   cv::Point(50, 100)//L
			};*/

			int p_hi = (tmpFrameCropped.rows);
			int p_wi = (tmpFrameCropped.cols);

			//計測範囲の形状
			int wide = 30;
			int hight = 30;

			std::vector<cv::Point> polygonPoints = {

			   cv::Point(p_wi / 2 + wide, hight + p_hi / 2),//A

			   cv::Point(p_wi / 2 - wide, hight + p_hi / 2),//J

			   cv::Point(p_wi / 2 - wide, (p_hi / 2) - hight),//K

			   cv::Point(p_wi / 2 + wide, (p_hi / 2) - hight)//L
			};



			// Call the function to draw the polygon and highlight the area
			cv::Mat result = drawPolygonAndHighlightArea(tmpFrameCropped,polygonPoints);

			//cv::Mat star_roi = get_star_roi(tmpFrameCropped);
			setBrightness(result);
			//std::this_thread::sleep_for(std::chrono::milliseconds(50));

			cv::Mat xstage_graph(200, 200, CV_8UC3, GREEN);
			cv::cvtColor(xstage_graph, x_stage_out, cv::COLOR_BGR2BGRA);
			cv::resize(x_stage_out, x_stage_out, cv::Size(tmpFrameOriginal.cols, 0.33 * tmpFrameOriginal.rows));

			cv::Mat ystage_graph(200, 200, CV_8UC3, GREEN);
			cv::cvtColor(ystage_graph, y_stage_out, cv::COLOR_BGR2BGRA);
			cv::resize(y_stage_out, y_stage_out, cv::Size(tmpFrameOriginal.cols, 0.33 * tmpFrameOriginal.rows));

			cv::Mat zstage_graph(200, 200, CV_8UC3, GREEN);
			cv::cvtColor(zstage_graph, z_stage_out, cv::COLOR_BGR2BGRA);
			cv::resize(z_stage_out, z_stage_out, cv::Size(tmpFrameOriginal.cols, 0.33 * tmpFrameOriginal.rows));

			cv::Mat sdgraph(200, 200, CV_8UC3, GREEN);
			cv::cvtColor(sdgraph, sdgrphlayout, cv::COLOR_BGR2BGRA);
			cv::resize(sdgrphlayout, sdgrphlayout, cv::Size(tmpFrameOriginal.cols, 0.66 * tmpFrameOriginal.rows));

			cv::Mat pztgraph(200, 200, CV_8UC3, GREEN);
			cv::cvtColor(pztgraph, ppttzz, cv::COLOR_BGR2BGRA);
			cv::resize(ppttzz, ppttzz, cv::Size(tmpFrameOriginal.cols, 0.33 * tmpFrameOriginal.rows));

			x_store.push_back(getX());
			y_store.push_back(getY());
			z_store.push_back(getZ());
			null.push_back(d_length*30/total_step);
			sec.push_back(total_sec);

			b_store.push_back(getBrightness());
			bd_store.push_back(finalCalculation());

			double epv = getEV();
			SchmittTrigger naresh(getuth(), getlth());
			//set_schmit_bool(naresh.processInput(finalCalculation()));

			if (getuth() <= finalCalculation()) {
				set_schmit_bool(true);
			}else if (getlth() >= finalCalculation()) {
				set_schmit_bool(false);
			}

			if (this->drawing_box) {
				cv::rectangle(tmpFrameCropped, this->box, cv::Scalar(255, 0, 0), 1);
				putInt_Mem(SQX1_KEY, this->box.x);
				putInt_Mem(SQY1_KEY, this->box.y);
				putInt_Mem(SQW_KEY, this->box.width);
				putInt_Mem(SQH_KEY, this->box.height);
			}
			if (this->drawing_boxSecond) {
				cv::rectangle(tmpFrameOriginal, this->boxSecond, cv::Scalar(2, 0, 20), 1);
			}

			allgraph(x_stage_out, x_store, max_radius * 5, "X");
			allgraph(y_stage_out, y_store, max_radius * 5, "Y");
			allgraph(z_stage_out, z_store, std::stod(getprefString(PZT_KEY)), "PZT");
			allgraph(sdgrphlayout, bd_store, graph_height, "BD");
			allgraph(ppttzz, null, std::stod(getprefString(PZT_KEY))*6, "PZT");

			CCSV_reading();//deposition function
						
			if (is_threshold_applied()) {
				thresh_line(sdgrphlayout, 0, (1 - (getuth() / graph_height)) * sdgrphlayout.rows, sdgrphlayout.cols, (1 - (getuth() / graph_height)) * sdgrphlayout.rows);
				thresh_line(sdgrphlayout, 0, (1 - (getlth() / graph_height)) * sdgrphlayout.rows, sdgrphlayout.cols, (1 - (getlth() / graph_height)) * sdgrphlayout.rows);
			}
			if (getEV() == 0) {
				cv::putText(sdgrphlayout, "EPV OFF", cv::Point(sdgrphlayout.cols - 100, 20), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 255));
			}
			else {
				cv::putText(sdgrphlayout, "EPV ON", cv::Point(sdgrphlayout.cols - 100, 20), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));
			}
			if (isRede()) {
				cv::putText(sdgrphlayout, "Redeposition ON", cv::Point(sdgrphlayout.cols - 100, 35), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));
			}
			else {
				cv::putText(sdgrphlayout, "Redeposition OFF", cv::Point(sdgrphlayout.cols - 100, 35), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 255));
			}
			if (get_schmit_bool()) {
				cv::putText(sdgrphlayout, "Signal ON", cv::Point(sdgrphlayout.cols - 100, 50), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));
			}
			else {
				cv::putText(sdgrphlayout, "Signal OFF", cv::Point(sdgrphlayout.cols - 100, 50), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 255));
			}
			cv::putText(sdgrphlayout, "Redepo Count: "+ std::to_string(count), cv::Point(sdgrphlayout.cols - 100, 65), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));
			cv::putText(sdgrphlayout, "velocity: " + std::to_string(velocity), cv::Point(sdgrphlayout.cols - 100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));
			cv::putText(sdgrphlayout, "tan: " + std::to_string(tan_now), cv::Point(sdgrphlayout.cols - 100, 95), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 0, 0));

			const std::string xyz = "X:" + std::to_string(getX()) + "  Y:" + std::to_string(getY()) + "  Z:" + std::to_string(getZ());
			cv::putText(tmpFrameOriginal, xyz, cv::Point(10, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
			cv::putText(tmpFrameOriginal, getprefString(CURRENT_FILENAME_KEY), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 10, 10), 1);
			cv::putText(tmpFrameOriginal, getprefString(MOUSE_X) + "," + getprefString(MOUSE_Y), cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 10), 1);
			cv::putText(tmpFrameOriginal, getprefString(MSQX1_KEY) + "," + getprefString(MSQY1_KEY), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 10), 1);

			cv::Mat combinedFrame(tmpFrameOriginal.rows * 2, tmpFrameOriginal.cols * 2, tmpFrameOriginal.type());

			int g_row1 = 0.5 * combinedFrame.cols;

			tmpFrameOriginal.copyTo(combinedFrame(cv::Rect(0, 0, tmpFrameOriginal.cols, tmpFrameOriginal.rows)));
			tmpFrameCropped.copyTo(combinedFrame(cv::Rect(tmpFrameOriginal.cols, 0, tmpFrameOriginal.cols, tmpFrameOriginal.rows)));

			x_stage_out.copyTo(combinedFrame(cv::Rect(0, tmpFrameOriginal.rows, g_row1, 0.33 * tmpFrameOriginal.rows)));
			y_stage_out.copyTo(combinedFrame(cv::Rect(0, 1.33 * tmpFrameOriginal.rows, g_row1, 0.33 * tmpFrameOriginal.rows)));
			z_stage_out.copyTo(combinedFrame(cv::Rect(0, 1.66 * tmpFrameOriginal.rows, g_row1, 0.33 * tmpFrameOriginal.rows)));

			sdgrphlayout.copyTo(combinedFrame(cv::Rect(g_row1, tmpFrameOriginal.rows, g_row1, 0.66 * tmpFrameOriginal.rows)));
			ppttzz.copyTo(combinedFrame(cv::Rect(g_row1, 1.66 * tmpFrameOriginal.rows, g_row1, 0.33 * tmpFrameOriginal.rows)));

			if (getprefString(AUTOGRAPH_KEY) == "on" || getprefString(DEPOSITION_ON_OFF) == "ON") {
				saveCSV(sec,bd_store, x_store, y_store, z_store, null ,getprefString(CURRENT_FILENAME_KEY));
			}
			if (getprefString(EBOXONOFF_KEY) == "on" && !getDes()) {
				createWindow("ORI", combinedFrame);
				cv::setMouseCallback("ORI", &Cam::mouse_callback, this);
				setDes(FALSE); // Mark the window as shown
			}
			if (getprefString(EBOXONOFF_KEY) == "off" && getDes()) {
				if (windowExists("ORI")) {
					destroyWindow("ORI");
				}
				setDes(TRUE); // Mark the window as hidden
			}
			if (getprefString(DEPOSITION_ON_OFF) == "OFF") {
				setEV(0);
				return_normal_position();
			}
			std::vector<float64> vout = { epv , getZ(), getX(), getY() };
			DDaq(vout, NI_DEVICESk(), NI_PORTS);
			return running ? combinedFrame : cv::Mat();
		}
		catch (const std::exception& e) {
			myUIInstance.messi(e.what());
		}
		catch (...) {
			myUIInstance.mess(L"Unknown error occurred.");
		}
	}

	void createWindow(const std::string& win_name, cv::Mat& coom) {
		cv::namedWindow(win_name);
		cv::imshow(win_name, coom);
		windows.insert(win_name);
	}
	// Destroy a window if it exists in the set
	void destroyWindow(const std::string& win_name) {
		if (windows.find(win_name) != windows.end()) {
			cv::destroyWindow(win_name);
			windows.erase(win_name);
		}
	}
	// Check if a window exists
	bool windowExists(const std::string& win_name) const {
		return windows.find(win_name) != windows.end();
	}

	bool putInt_Mem(const std::string& key, const int& v) {
		if (SetPreference(key, std::to_string((v)))) {
			return true;
		}
		else {
			return false;
		}
	}

	void allgraph(cv::Mat& frame, std::deque<double>& graphValues, double upperLimit, const std::string& yxix) {
		int startPointX = 30;
		if (graphValues.empty()) {
			return;
		}
		int height = frame.rows;
		int width = frame.cols;
		if (graphValues.size() >= static_cast<size_t>(width - startPointX)) {
			int elementsToSkip = graphValues.size() - (width - startPointX);
			startPointX -= elementsToSkip;
		}
		std::string cu;
		cv::Point startPoint(startPointX, height * 0.5);
		frame = cv::Scalar(255, 255, 255);
		line(frame, cv::Point(10, 10), cv::Point(width * 0.9, 10), cv::Scalar(0, 255, 0), 1);
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << upperLimit / 2; // Format double with 2 decimal places
		std::string myString = oss.str();
		cv::putText(frame, myString, cv::Point(0, height * 0.5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(10, 10, 10), 1);


		line(frame, cv::Point(10, height - 10), cv::Point(width * 0.9, height - 10), cv::Scalar(0, 0, 255), 1);
		for (int i = 0; i < graphValues.size(); ++i) {
			cu = std::to_string(graphValues[i]);
			double y = (graphValues[i] / upperLimit) * (height * 0.8) + 10;
			cv::Point endPoint(i + startPointX, height - static_cast<int>(y));
			line(frame, startPoint, endPoint, cv::Scalar(0, 0, 0), 1);
			startPoint = endPoint;
		}
		cv::putText(frame, cu, cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 10, 10), 1);
	}
	void thresh_line(cv::Mat& graph_frame, const int& x1, const int& y1, const int& x2, const int& y2) {
		line(graph_frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(200, 10, 200), 1);
	}
	double getlth() {
		return key_double(LTH_KEY);
	}
	double getuth() {
		return key_double(UTH_KEY);
	}
	void setDepOn(bool stop) {
		if (stop) {
			SetPreference(DEPOSITION_ON_OFF, "ON");
			Logger("Deposition is set on.", "INFO");
		}
		else {
			rows.clear();
			is_csv_ready = false;
			SetPreference(DEPOSITION_ON_OFF, "OFF");
			Logger("Deposition is set off.", "INFO");
		}
	}
	bool getDes() {
		return getprefString(CV_DESTROY) == "ON";
	}
	void setDes(bool tf) {
		if (tf) {
			SetPreference(CV_DESTROY, "ON");
		}
		else {
			SetPreference(CV_DESTROY, "OFF");
		}
	}
	bool rd = FALSE;
	bool isRede() const {
		return rd;
	}
	void setRede(bool t) {
		rd = t;
		std::string ll = std::to_string(t);
		Logger("Redeposition is set "+ll, "INFO");
	}
	/*
	double bd_height() {
		double iio = 10;
		if (getuth() > iio) {
			return getuth();
		}
		if (getuth() < iio) {
			return iio;
		}
	}
	*/
	bool schmitBool;
	void set_schmit_bool(bool bl) {
		schmitBool = bl;
	}
	bool get_schmit_bool() {
		return schmitBool;
	}
	double epv = 0;
	void setEV(double ephv) {
		epv = ephv;
	}
	double getEV() {
		return epv;
	}
	bool cvflip = TRUE;
	bool getcvflip() {
		return cvflip;
	}
	void setcvflip(bool stop) {
		cvflip = stop;
	}
	bool iscapture = FALSE;
	void setCaptureScreenBool(bool stop) {
		iscapture = stop;
	}
	bool getCaptureScreenBool() {
		return iscapture;
	}
	void return_normal_position() {
		reset_csv_rows();
		if (getX() > max_radius) {
			x -= 0.05;
			setX(x);
		}
		if (getX() < max_radius) {
			x += 0.05;
			setX(x);
		}
		if (getY() > max_radius) {
			y -= 0.05;
			setY(y);
		}
		if (getY() < max_radius) {
			y += 0.05;
			setY(y);
		}
		if (getX() == max_radius) {
			setX(max_radius);
		}
		if (getY() == max_radius) {
			setY(max_radius);
		}
		if (getZ() > 0) {
			z -= 0.1;
			setZ(z);
		}
		if (getZ() < 0 || getZ() == 0) {
			setZ(0);
			setINC(0);
		}
	}
	static void Cam::mouse_callback(int event, int x, int y, int flags, void* param) {
		auto self = static_cast<Cam*>(param);
		int w = std::round(0.25 * GetSystemMetrics(SM_CXSCREEN) - 10);
		int h = std::round(0.45 * GetSystemMetrics(SM_CYSCREEN) - 10);
		switch (event) {
		case cv::EVENT_MOUSEMOVE://x2y2
			if (self->drawing_box) {
				if (x >= w && y >= 0 && y < h) {
					self->box.width = x - w - self->box.x;
					self->box.height = y - self->box.y;
				}
			}
			if (self->drawing_boxSecond) {
				if (x >= 0 && y >= 0 && x < w && y < h) {
					self->boxSecond.width = x - self->boxSecond.x;
					self->boxSecond.height = y - self->boxSecond.y;
				}
			}
			break;
		case cv::EVENT_LBUTTONDOWN://x1y1
			if (x >= w && y >= 0 && y < h) {
				self->drawing_box = true;
				self->box = cv::Rect(x - w, y, 0, 0);
			}
			if (x >= 0 && y >= 0 && x < w && y < h) {
				self->drawing_boxSecond = true;
				self->boxSecond = cv::Rect(x, y, 0, 0);
			}
			break;
		case cv::EVENT_LBUTTONUP:
			if (self->drawing_boxSecond) {
				self->putInt_Mem(MSQX1_KEY, self->boxSecond.x);
				self->putInt_Mem(MSQY1_KEY, self->boxSecond.y);
				self->putInt_Mem(MSQX2_KEY, self->boxSecond.width);
				self->putInt_Mem(MSQY2_KEY, self->boxSecond.height);
				self->drawing_boxSecond = false;
			}
			self->drawing_box = false;
			break;
		}
	}
	
	int row_position;
	int direction;
	bool is_csv_ready = false;
	bool is_redeposition_count = true;
	bool is_threshold_applied() {
		return getprefString("STAG_TOGGLE_KEY") == "on";
	}
	bool is_deposition_on() {
		return getprefString(DEPOSITION_ON_OFF) == "ON";
	}

	//ステージ制御
	void CCSV_reading() {
		if (is_deposition_on()) {
			setEV(std::stod(getprefString(EPV_KEY)));
			if (!is_csv_ready && rows.empty()) {
				rows = readCSV(getprefString("XYZ_CSV_FILENAME"));
				is_csv_ready = true;
				row_position = -1;
				direction = 1;
				Logger("CSV is read. Ready for new deposition.","INFO");
				setRede(false);
				vcalc();
			}

			if (getprefString("CameraONoFF") == "OFF") {
				Logger("CCameraONoFF...", "INFO");
				rows.clear();
				is_csv_ready = false;
				return;
			}
			
			reduce_time();
			set_count();
			total_time();

			if (rows.size() > (row_position + direction)) {
				row_position += direction;
				d_length += direction;
				if (waiting_time >= waiting_count) {
					if (!isRede()) {
						std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_re_deposition));
						direction = 0;
						waiting_count++;
					}
					else {
						waiting_time = waiting_restart;
						std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_re_deposition));
						direction = restart_back;
						waiting_count++;
						is_redeposition_count = true;
					}
				}
				else {
					if (get_schmit_bool()) {
						if (!is_redeposition_count) {
							waiting_count = 0;
						}
						else {
							std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_deposition));
							direction = 1;
							is_redeposition_count = true;
						}

						/*if (isRede()) {
							Logger("This is redeposition after lost..", "INFO");


						}
						else {
							std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_deposition));
							direction = 1;
							is_redeposition_count = true;
							Logger("This is normal deposition before lost..", "INFO");
						}*/
						//Logger("Current brightness difference is above threshold value", "INFO");
					}
					else {
						//Logger("Current brightness difference is below threshold value", "INFO");
						if (!is_threshold_applied()) {
							std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_deposition));
							direction = 1;
							is_redeposition_count = true;
							Logger("Deposition on for open feedback..", "INFO");
						}
						else {
							if ((row_position + direction) < start_dash && !isRede()) {
								std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_deposition));
								direction = 1;
								is_redeposition_count = true;
								Logger("This is starting 9 steps..", "INFO");
							}
							else {
								//std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds_deposition));
								Logger("This is after 9 steps..", "INFO");
								setRede(true);
								direction = -2;  // Change the direction
								Logger("signal lost and stage down", "INFO");
								if (is_redeposition_count) {
									if ((row_position + direction) >= start_dash) {
										count_redeposition++;
										is_redeposition_count = false;
										delay_time();
										Logger("Redeposition count : " + std::to_string(miliseconds_deposition), "INFO");
									}

								}
							}
						}
					}
				}
				if (rows[row_position][0] == "x" && rows[row_position][1] == "y" && rows[row_position][2] == "z") {
					Logger("X,Y,Z detected.", "INFO");
				}
				else {
					setX(std::stod(rows[row_position][0]));
					setY(std::stod(rows[row_position][1]));
					setZ(std::stod(rows[row_position][2]));
					cal_TAN();
					//Logger(rows[row_position][0] + "-" + rows[row_position][1] + "-" + rows[row_position][2]);
				}
			}
			else {
				setRede(false);
				count_redeposition = 0;
				Logger("Out of csv size.. It is going to stop deposition..", "INFO");
				rows.clear();
				is_csv_ready = false;
				setDepOn(false);
			}
			
		}
		else {
			//Logger("Deposition is off.","INFO");
		}
	}
	void reset_csv_rows() {
		rows.clear();
		is_csv_ready = false;
		row_position = 0;
		direction = 1;
	}

	//加工時間計算
	void total_time() {
		if (!get_schmit_bool()) {
			total_sec = total_sec + (miliseconds_re_deposition + other_time) * 0.001;
		}
		else {
			total_sec = total_sec + (miliseconds_deposition + other_time) * 0.001;
		}
	}

	//待機時間増加(速度低下)
	void delay_time() {
		double d, t;
		d = atan(tan_now) / 2;
		t = tan(d);
		initial_value = pow(ratio, count_redeposition) + pow(t, 2) * redep_pluse * count_redeposition;
		miliseconds_deposition = (int)initial_value;
		vcalc();
	}

	//待機時間現象(速度上昇)
	void reduce_time() {
		if (!get_schmit_bool()) {
			reduce_count = 0;
		}
		else {
			reduce_count++;
		}
		if (reduce_count >= reduce_threshold) {
			count_redeposition = count_redeposition - red_value;
			if (count_redeposition <= 0) {
				count_redeposition = 0;
			}
			set_count();
			delay_time();
			reduce_count = 0;
		}
	}

	//現在の角度入手
	void cal_TAN() {
		double L,HH,H;
		if (x - px == 0 && y - py == 0) {
			tan_now = 0;
		}
		else {
			HH = pow(x - px, 2) + pow(y - py, 2);
			L = fabs(z - pz);
			H = sqrt(HH);
			tan_now = H / L;
		}
		px = x;
		py = y;
		pz = z;
	}

	std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
		std::ifstream file(filename);
		std::vector<std::vector<std::string>> rows;
		std::string line;
		while (std::getline(file, line)) {
			std::stringstream ss(line);
			std::string cell;
			std::vector<std::string> row;

			while (std::getline(ss, cell, ',')) {
				row.push_back(cell);
			}
			rows.push_back(row);
		}
		return rows;
	}





};

#endif // CAM_H
