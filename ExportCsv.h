#ifndef EXPORT_CSV_H
#define EXPORT_CSV_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <deque>
#include <chrono>
#include "preferencemanager.h"
#include "Resource.h"
#include <vector>

struct Csvlist {
	std::string nameapp;
};

void saveCSV(std::deque<double> first, std::deque<double> second, std::deque<double> third, std::deque<double> fourth, std::deque<double> fifth, std::deque<double> sixth, std::string fileName) {
    std::string folderName = getprefString(CURRENT_FOLDER);
    std::string filename_ = folderName + "\\" + fileName + ".csv";
    std::ofstream file(filename_);
    if (!file.is_open()) {
        return;
    }
    file << "sn,sec,bd,x,y,z,l" << std::endl;
    for (size_t i = 0; i < second.size(); i++) {
        file << static_cast<double>(i)/10 << "," << first[i] << "," << second[i] << "," << third[i] << "," << fourth[i] << "," << fifth[i] << "," << sixth[i] << "\n";
    }
    file.close();
}

void saveCSV_head(const std::vector<std::string>& header, const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& col : header) {
        file << col << ",";
    }
    file << "\n";
    for (size_t i = 0; i < x.size(); ++i) {
        file << x[i] << "," << y[i] << "," << z[i] << "\n";
    }
    file.close();
}

#endif