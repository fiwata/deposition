#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "opencv2/opencv.hpp"
#include <NIDAQmx.h>

#include <vector>
#include <string>
#include "preferencemanager.h"
#include "Resource.h"

std::vector<int> NI_PORTS = { 0, 1 ,0 ,1 };
std::vector<float64> NI_VOLTS_0 = { 0, 0 , 0 , 0 };

const cv::Scalar RED = cv::Scalar(0, 0, 255);
const cv::Scalar BLUE = cv::Scalar(0, 255, 0);
const cv::Scalar GREEN = cv::Scalar(255, 0, 0);

std::vector<std::string> NI_DEVICESk() {
	if (getprefString(MENU_SUB_VIR_REA) == "ON") {
		return { "Dsim", "Dsim", "Dsim", "Dsim" };
	}
	else {
		return { "Dev2", "Dev2", "Dev3", "Dev3" };
	}
}
std::string digital_dev2() {
	if (getprefString(MENU_SUB_VIR_REA) == "ON") {
		return "Dsim/port0/line0";
	}
	else {
		return "Dev2/port0/line0";
	}
}

int total_bits = 300;

double total_height = 5;
double base_height = 1;
double Spiral_height = 2;
double total_step = 1000;//CSV�t�@�C���̃X�e�b�v��

double resolution = 1;
double max_radius = 1.5;
double count_redeposition = 0;
double base_steps = (base_height / total_height )* total_bits;
double move_r_steps = (Spiral_height / total_height) * total_bits;

double current_height = 0;
double radiusX = 0;
double radiusY = 0;
double radiusZ = 0;

int miliseconds_deposition = 1;//�X�e�[�W���~
double initial_value = 1;
double redep_pluse = 150;//��������̌���
double ratio = 2;//���䐔��̌���
int miliseconds_re_deposition = 1;//�X�e�[�W�㏸
int graph_height = 500;
int start_dash = 150;
int count = 0;
int waiting_time = 60;//�����̑ҋ@
int waiting_count = 0;
int d_length = 0;
int velocity = 0;
int other_time = 63;//�ҋ@�ȊO�̎���
int waiting_restart = 0;//�ĊJ���i�w��~64ms�ҋ@or�w��~30nm���ׂ�j
int restart_back=0;//�ĊJ��0�̂Ƃ��ҋ@�A-1�̂Ƃ����ׂ�
double total_sec = 0;
int reduce_threshold = 50;//���炷�Ԋu
int reduce_count = 0;
double tan_now = 0;
double px = 0, py = 0, pz = 0;
double red_value = 0.5;//���炷��




int slection_width = 100;


#endif