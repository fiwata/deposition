#ifndef DAQ_SIGNAL_H
#define DAQ_SIGNAL_H

#include "opencv2/opencv.hpp"
#include <NIDAQmx.h>
#include <Windows.h>
#include "preferencemanager.h"
#include "MyUi.h"
#include "ExportCsv.h"
#include <opencv2/videoio.hpp>
#include <d2d1.h>
#include <d2d1helper.h>
#include <string>

MyUI ui;
bool laserState = false;
void DExe(std::string e) {
	char temp[2048] = "";
	DAQmxGetExtendedErrorInfo(temp, 2048);
	std::string errorMsg = "Error in " + e + ":" + std::string(temp);
	ui.messi(errorMsg);
}

void DExe(std::string e, int32 err) {
	char temp[2048] = "";
	DAQmxGetExtendedErrorInfo(temp, 2048);
	std::string errorMsg = "Error in " + e + ":" + std::string(temp);
	ui.messi(errorMsg);
}

void DDaq(const std::vector<float64>& vouts, const std::vector<std::string>& devices, const std::vector<int>& ports) {
	for (size_t i = 0; i < devices.size(); ++i) {
		TaskHandle epvtask = nullptr;
		int32 err = DAQmxCreateTask("", &epvtask);
		if (err != 0) {
			DExe("DAQmxCreateTask", err);
			continue;
		}

		std::string channel = devices[i] + "/ao" + std::to_string(ports[i]);
		err = DAQmxCreateAOVoltageChan(epvtask, channel.c_str(), "ao_channel", 0.0, 5.0, DAQmx_Val_Volts, nullptr);
		if (err != 0) {
			DExe("DAQmxCreateAOVoltageChan", err);
			DAQmxClearTask(epvtask);
			continue;
		}

		err = DAQmxWriteAnalogF64(epvtask, 1, true, 10.0, DAQmx_Val_GroupByChannel, &vouts[i], nullptr, nullptr);
		if (err != 0) {
			DExe("DAQmxWriteAnalogF64", err);
		}

		DAQmxClearTask(epvtask);
	}
}
void Digitaloutput(const uInt32& datau) {
	TaskHandle thandl = nullptr;

	int32 err = DAQmxCreateTask("", &thandl);
	if (err != 0) {
		DExe("DAQmxCreateTask", err);
	}
	std::string channel = digital_dev2();
	err = DAQmxCreateDOChan(thandl, channel.c_str(), "", DAQmx_Val_ChanForAllLines);
	if (err != 0) {
		DExe("DAQmxCreateDOChan", err);
	}

	/*uInt32 datay = (laserState ? false : true);
	err = DAQmxWriteDigitalU32(taskHandle, 1, 1, 10.0, DAQmx_Val_GroupByChannel, &datay, nullptr, nullptr);
	if (err != 0) {
		DExe("DAQmxWriteDigitalU32", err);
	}*/
		
	err = DAQmxWriteDigitalScalarU32(thandl, true, 10.0, datau, nullptr);
	if (err != 0) {
		DExe("DAQmxWriteDigitalScalarU32", err);
	}
	err = DAQmxStartTask(thandl);
	if (err != 0) {
		DExe("DAQmxStartTask", err);
	}
	err = DAQmxStopTask(thandl);
	if (err != 0) {
		DExe("DAQmxStopTask", err);
	}

	err = DAQmxClearTask(thandl);
	if (err != 0) {
		DExe("DAQmxClearTask", err);
	}

}





#endif