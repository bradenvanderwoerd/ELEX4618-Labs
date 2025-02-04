#include "stdafx.h"
#include "Lab4\\CBase4618.h"


CBase4618::CBase4618() {
	_exit_flag = 0;
}
CBase4618::~CBase4618() {

}

void CBase4618::run() {
	char key;
	do {
		gpio();
		update();
		draw();

		key = cv::waitKey(1);
	} while (key != 'q' && !_exit_flag);
}