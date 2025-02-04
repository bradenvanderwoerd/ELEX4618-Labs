#include "stdafx.h"
#include "Lab5\\CBase4618Threaded.h"


CBase4618Threaded::CBase4618Threaded() {
	_exit_flag = 0;
}
CBase4618Threaded::~CBase4618Threaded() {

}

void CBase4618Threaded::run() {
	char key;
	do {
		gpio();
		update();
		draw();

		key = cv::waitKey(1);
	} while (key != 'q' && !_exit_flag);
}