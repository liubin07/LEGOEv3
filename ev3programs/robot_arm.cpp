#include <unistd.h>
#include <cstdio>

#include <thread>
#include "ev3dev.h"

using namespace std;
using namespace ev3dev;

void motor_common_init(motor &m, int power, bool regulation)
{
    // for large motor
    int maxpower = 900;
    if (m.driver_name() == motor::motor_medium) {
        // for medium motor
        maxpower = 1200;
    }
    if (regulation) {
        m.set_speed_regulation_enabled(motor::speed_regulation_on);
        m.set_speed_sp(power*maxpower/100);
    } else {
        m.set_speed_regulation_enabled(motor::speed_regulation_off);
        m.set_duty_cycle_sp(power);
    }
    m.set_stop_command(motor::stop_command_brake);
}

void motor_run_timed(motor &m, int power, int time_sc, bool regulation = false)
{
    motor_common_init(m, power, regulation);

    m.set_time_sp(time_sc*1000);
    m.run_timed();
    int count = 0;
    while (m.state().count("running") || count > time_sc*10) {
        this_thread::sleep_for(chrono::milliseconds(100));
        count++;
    }
    if (count > time_sc*10) {
        m.stop();
    }
    this_thread::sleep_for(chrono::milliseconds(200));
}

void motor_run_to_position(motor &m, int power, int position, bool regulation = false)
{
    motor_common_init(m, power, regulation);

    if (power < 0) {
        power = -power;
        position = -position;
    }

    int oldp = m.position();
    int newp = 0;
    m.set_position_sp(position);
    m.run_to_rel_pos();
    while (m.state().count("running")) {
        this_thread::sleep_for(chrono::milliseconds(100));
        newp = m.position();
        if (oldp == newp) {
            m.stop();
            break;
        }
    }
    this_thread::sleep_for(chrono::milliseconds(200));
}

bool check_color_sensor(sensor &s)
{
    s.set_mode(color_sensor::mode_col_reflect);
    return (s.value(0) > 25);
}

bool check_touch_sensor(sensor &s)
{
    s.set_mode(touch_sensor::mode_touch);
    return s.value(0);
}

void motor_run_wait_sensor(motor &m, int power, sensor &s, std::function<bool(sensor &)> callback, bool regulation = false)
{
    motor_common_init(m, power, regulation);

    m.run_forever();
    while (1) {
        if (callback(s)) {
            m.stop();
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    this_thread::sleep_for(chrono::milliseconds(200));
}

void init_devices(touch_sensor &ts, color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2)
{
    // Large moto 1 reset position
    motor_run_wait_sensor(lm1, -50, cs, check_color_sensor);

    // Medium motor reset position
    motor_run_timed(mm, 30, 1);
    motor_run_to_position(mm, 50, -90);

    // Large moto 2 reset position
    motor_run_wait_sensor(lm2, 50, ts, check_touch_sensor);
    motor_run_to_position(lm2, 50, -360*0.86);
}

void click_operation(touch_sensor &ts, color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2, bool up)
{
    int position1 = -312;
    int position2 = 300;
    if (!up) {
        position1 = 300;
        position2 = -312;
    }

    // 1. h-rotate
    motor_run_to_position(lm2, 65, position1);
    this_thread::sleep_for(chrono::milliseconds(500));

    // 2. down
    motor_run_to_position(lm1, 20, 275);
    this_thread::sleep_for(chrono::milliseconds(1500));

    // 3. grab
    motor_run_timed(mm, 30, 1, 1200);
    this_thread::sleep_for(chrono::milliseconds(500));

    // 4. up
    motor_run_wait_sensor(lm1, -55, cs, check_color_sensor);
    this_thread::sleep_for(chrono::milliseconds(500));

    // 5. h-back
    motor_run_to_position(lm2, 65, position2);
    this_thread::sleep_for(chrono::milliseconds(500));

    // 6. down
    motor_run_to_position(lm1, 20, 275);
    this_thread::sleep_for(chrono::milliseconds(1500));

    // 7. loose
    motor_run_to_position(mm, 30, -90, 1200);
    this_thread::sleep_for(chrono::milliseconds(500));

    // 8. up
    motor_run_wait_sensor(lm1, -55, cs, check_color_sensor);
}

int main()
{
    bool escape = false, up = false, down = false;

    // sensors and motors
    touch_sensor ts(INPUT_1);
    color_sensor cs(INPUT_3);
    medium_motor mm(OUTPUT_A);
    large_motor lm1(OUTPUT_B);
    large_motor lm2(OUTPUT_C);

    if (!ts.connected() || !cs.connected() || !mm.connected() || !lm1.connected() || !lm2.connected()) {
        printf("Error: devices are not connected!\n");
        return -1;
    }

    printf("Initialize devices...\n");
    init_devices(ts, cs, mm, lm1, lm2);

    printf("Click up or down key to operate the arm.\n");
    printf("Click back key to terminate the program.\n");
    while (1) {
        escape = button::back.pressed();
        up = button::up.pressed();
        down = button::down.pressed();
        if (escape) {
            printf("Back clicked\n");
            break;
        } else if (up) {
            printf("Up clicked\n");
            click_operation(ts, cs, mm, lm1, lm2, true);
        } else if (down) {
            printf("Down clicked\n");
            click_operation(ts, cs, mm, lm1, lm2, false);
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    return 0;
}
