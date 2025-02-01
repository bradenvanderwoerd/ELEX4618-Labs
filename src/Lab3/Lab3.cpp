#include "stdafx.h"
#include "Lab3.h"
#include <iomanip>
#include <conio.h>

#define SERVO_MAX 180

Lab3::Lab3() {

}

Lab3::~Lab3() {
}

void Lab3::lab3main() {
    _ctrl.init_com();
    
    int cmd = -1;
    do
    {
        print_menu();

        std::cin >> cmd;
        switch (cmd)
        {
        case 1: test_analog(); break;
        case 2: test_digital(); break;
        case 3: test_button(); break;
        case 4: test_servo(); break;
        }
    } while (cmd != 0);
}

void Lab3::print_menu() {
    std::cout << "(1) Test Analog\n";
    std::cout << "(2) Test Digital\n";
    std::cout << "(3) Test Button\n";
    std::cout << "(4) Test Servo\n";
    std::cout << "(0) Exit Lab 3\n";
    std::cout << "CMD>";
}

void Lab3::test_analog() {
    do {
        int resultX, resultY;

        if (!_ctrl.get_data(ANALOG, JOY_X, resultX) || !_ctrl.get_data(ANALOG, JOY_Y, resultY))
            _ctrl.init_com();
        else
            std::cout << "ANALOG TEST : CH" << JOY_X << " = " << resultX
                      << " (" << std::fixed << std::setprecision(1) << _ctrl.get_analog(JOY_X)
                      << "%) CH" << JOY_Y << " = " << resultY
                      << " (" << std::fixed << std::setprecision(1) << _ctrl.get_analog(JOY_Y)
                      << "%)\n";
    } while (!_kbhit());
}

void Lab3::test_digital() {
    do {
        int result = -1;
        if (!_ctrl.get_data(DIGITAL, BUTTON1, result))
            _ctrl.init_com();
        else {
            if (result == 0)
                _ctrl.set_data(DIGITAL, GREEN_LED, 1);
            else
                _ctrl.set_data(DIGITAL, GREEN_LED, 0);
            std::cout << "DIGITAL TEST : CH" << BUTTON1 << " = " << result << std::endl;
        }
    } while (!_kbhit());
}

void Lab3::test_button() {
    int button_presses = 0;
    do {
        int dummy;
        if (!_ctrl.get_data(DIGITAL, BUTTON1, dummy)) {
            _ctrl.init_com();
        }
        else if (_ctrl.get_button(BUTTON1) == 0) {
            button_presses++;
            std::cout << "Button Test: " << button_presses << "\n";
        }
    } while (!_kbhit());
}

void Lab3::test_servo() {
    for (int position = 0; !_kbhit(); position++) {
        if (!_ctrl.set_data(SERVO, 0, position % SERVO_MAX))
            _ctrl.init_com();
        else
            std::cout << "Servo Test: CH0 POS " << position << "\n";

        if (position == SERVO_MAX)
            position = 0;
    }
}