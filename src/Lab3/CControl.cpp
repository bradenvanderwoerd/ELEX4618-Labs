#include "stdafx.h"
#include "Lab3\\CControl.h"
#include <regex>
#include <iostream>

#define DEBOUNCE_TIME 200
#define TIMEOUT_TIME 1000

CControl::CControl() {

}

CControl::~CControl() {
	set_data(DIGITAL, RED_LED, 0);
	set_data(DIGITAL, GREEN_LED, 0);
	set_data(DIGITAL, BLUE_LED, 0);
}

void CControl::init_com(int comport) {
	_com.open("COM" + std::to_string(comport));
	_com.flush();
}

void CControl::init_com() {
	int result;
	int comport = 4;
	bool port_detected = false;

	std::cout << "Attempting communication on COM" << comport << std::endl;
	_com.open("COM" + std::to_string(comport));
	_com.flush();
	port_detected = get_data(DIGITAL, 0, result);

	if (port_detected) {
		std::cout << "Successfully established communication on COM" << comport << std::endl;
		return;
	}

	comport = -1;

	while (!port_detected && comport < 10) {
		comport++;
		std::cout << "Attempting communication on COM" << comport << std::endl;
		_com.open("COM" + std::to_string(comport));
		_com.flush();
		port_detected = get_data(DIGITAL, 0, result);
	}
	
	if (port_detected)
		std::cout << "Successfully established communication on COM" << comport << std::endl;
	else {
		std::cout << "Auto-detection failed. Enter COM number or (r) to retry: ";
		char cmd = ' ';
		std::cin >> cmd;
		if (cmd == 'r' || cmd == 'R')
			init_com();
		else
			init_com(cmd);
	}
}

bool CControl::get_data(int type, int channel, int& result) {
	std::string tx_str = "G " + std::to_string(type) + " " + std::to_string(channel) + "\n";
	std::string rx_str = "";

	_com.write(tx_str.c_str(), tx_str.length());
	Sleep(10);

	char buff[2];
	buff[0] = 0;

	float timer = cv::getTickCount();

	while (buff[0] != '\n' && (cv::getTickCount() - timer) / cv::getTickFrequency() * 1000 < TIMEOUT_TIME)
		if (_com.read(buff, 1) > 0)
			rx_str = rx_str + buff[0];
	
	std::regex result_exp("(\\d+)$");
	std::smatch match;
	if (std::regex_search(rx_str, match, result_exp))
		result = std::stoi(match.str(1));
	else
		return false;

	if (rx_str == "A " + std::to_string(type) + " " + std::to_string(channel) + " " + std::to_string(result) + "\n")
		return true;

	return false;
}

bool CControl::set_data(int type, int channel, int val) {
	std::string tx_str = "S " + std::to_string(type) + " " + std::to_string(channel) + " " + std::to_string(val) + "\n";
	std::string rx_str = "";

	_com.write(tx_str.c_str(), tx_str.length());
	Sleep(10);

	char buff[2];
	buff[0] = 0;

	float timer = cv::getTickCount();

	while (buff[0] != '\n' && (cv::getTickCount() - timer) / cv::getTickFrequency() * 1000 < TIMEOUT_TIME)
		if (_com.read(buff, 1) > 0)
			rx_str = rx_str + buff[0];

	if (rx_str == "A " + std::to_string(type) + " " + std::to_string(channel) + " " + std::to_string(val) + "\n")
		return true;

	return false;
}

float CControl::get_analog(int analog_channel) {
	int result = -1;
	float percent = -1;

	if (get_data(ANALOG, analog_channel, result))
		percent = ((float) result / 4096) * 100;
	
	return percent;
}

int CControl::get_button(int button_number) {
	static int last_stable_state = 1;
	static float last_button_press = cv::getTickCount();
	
	int button_state = -1;
	if (!get_data(DIGITAL, button_number, button_state))
		return -1;

	if (button_state != last_stable_state && (cv::getTickCount() - last_button_press) * 1000 / cv::getTickFrequency() > DEBOUNCE_TIME) {
		last_stable_state = button_state;
		last_button_press = cv::getTickCount();
		return button_state;
	}

	return 1;
}