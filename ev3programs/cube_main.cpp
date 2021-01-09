#include <unistd.h>
#include <iostream>
#include <fstream>  
#include <thread>

#include "ev3dev.h"
#include "cube_algorithm.h"

using namespace std;
using namespace ev3dev;

// constant for medium and large motors
const int medium_motor_max_speed = 1250;
const int large_motor_max_speed = 900;
// tunable parameters
int scanrgb_delay = 15;
int turntable_position = 0;
int turntable_offset = 24;
int tilt_offset = 12;
// other global variables
string data_file_path;

void leds_red_on() {
    led::all_off();
    led::red_left.on();
    led::red_right.on();
}

void leds_red_flash() {
    led::all_off();
    led::red_left.flash(0, 0);
    led::red_right.flash(0, 0);
}

void leds_green_on() {
    led::all_off();
    led::green_left.on();
    led::green_right.on();
}

void leds_green_flash() {
    led::all_off();
    led::green_left.flash(0, 0);
    led::green_right.flash(0, 0);
}

void leds_orange_on() {
    led::all_off();
    led::red_left.on();
    led::red_right.on();
    led::green_left.on();
    led::green_right.on();
}

void leds_orange_flash() {
    led::all_off();
    led::red_left.flash(0, 0);
    led::red_right.flash(0, 0);
    led::green_left.flash(0, 0);
    led::green_right.flash(0, 0);
}

void run_to_position(motor &m, int sleep_ms) {
    m.run_to_abs_pos();
    while (m.state().count("holding") == 0) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    if (sleep_ms > 0) {
        this_thread::sleep_for(chrono::milliseconds(sleep_ms));
    }
}

void scan_away(medium_motor &mm) {
    mm.set_speed_regulation_enabled(motor::speed_regulation_on);
    mm.set_stop_command(motor::stop_command_hold);
    mm.set_speed_sp(medium_motor_max_speed*100/100);
    mm.set_position_sp(-340);
    run_to_position(mm, 0);
}

void reset_scan(medium_motor &mm) {
    cout << "Reset Scan" << endl;
    mm.set_speed_regulation_enabled(motor::speed_regulation_on);
    mm.set_stop_command(motor::stop_command_hold);
    mm.set_speed_sp(medium_motor_max_speed*40/100);
    mm.run_forever();
    int oldp = 0, newp = 1;
    while (oldp != newp) {
        oldp = mm.position();
        this_thread::sleep_for(chrono::milliseconds(200));
        newp = mm.position();
    }
    mm.set_position_sp(newp - 100);
    run_to_position(mm, 0);
    mm.set_speed_sp(medium_motor_max_speed*20/100);
    mm.run_forever();
    oldp = newp + 1;
    while (oldp != newp) {
        oldp = mm.position();
        this_thread::sleep_for(chrono::milliseconds(500));
        newp = mm.position();
    }
    mm.reset();
    scan_away(mm);
}

void tilt_away(large_motor &lm1) {
    lm1.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm1.set_stop_command(motor::stop_command_hold);
    lm1.set_speed_sp(large_motor_max_speed*70/100);
    lm1.set_position_sp(10);
    run_to_position(lm1, 0);
}

void reset_tilt(large_motor &lm1) {
    cout << "Reset Tilt" << endl;
    lm1.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm1.set_stop_command(motor::stop_command_hold);
    lm1.set_speed_sp(large_motor_max_speed*(-20)/100);
    lm1.run_forever();
    int oldp = 0, newp = 1;
    while (oldp != newp) {
        oldp = lm1.position();
        this_thread::sleep_for(chrono::milliseconds(200));
        newp = lm1.position();
    }
    lm1.reset();
    tilt_away(lm1);
}

void scan_RGB(int &r, int &g, int &b, color_sensor &cs) {
    int rgb_sum = -1;
    cs.set_mode(color_sensor::mode_col_reflect);
    while (rgb_sum < 0) {
        this_thread::sleep_for(chrono::milliseconds(scanrgb_delay));
        cs.set_mode(color_sensor::mode_rgb_raw);
        this_thread::sleep_for(chrono::milliseconds(2));
        r = cs.value(0);
        g = cs.value(1);
        b = cs.value(2);
        rgb_sum = r + g + b;
        if (rgb_sum < 0) {
            scanrgb_delay = (scanrgb_delay + 1)%200;
        }
    }
}

void reset_RGB(large_motor &lm2, color_sensor &cs) {
    int r = 0, g = 0, b = 0;
    cout << "Reset RGB" << endl;
    lm2.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm2.set_stop_command(motor::stop_command_hold);
    lm2.set_speed_sp(large_motor_max_speed*(-70)/100);
    lm2.run_forever();
    for (int i = 0; i < 10; i++) {
        scan_RGB(r, g, b, cs);
    }
    lm2.set_speed_sp(large_motor_max_speed*75/100);
    lm2.set_position_sp(turntable_position);
    run_to_position(lm2, 0);    
}

void motor_reset(medium_motor &mm, large_motor &lm1, large_motor &lm2) {
    mm.reset();
    lm1.reset();
    lm2.reset();
}

void initialize(color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2)
{
    leds_red_flash();
    motor_reset(mm, lm1, lm2);
    reset_scan(mm);
    reset_tilt(lm1);
    reset_RGB(lm2, cs);
    cout << "Ready" << endl;
}

void tilt_hold(large_motor &lm1) {
    lm1.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm1.set_stop_command(motor::stop_command_hold);
    lm1.set_speed_sp(large_motor_max_speed*60/100);
    lm1.set_position_sp(tilt_offset + 85);
    run_to_position(lm1, 0);
}

void tilt(int num, large_motor &lm1) {
    if (lm1.position() < 60) {
        tilt_hold(lm1);
    }
    for (int i = 0; i < num; i++) {
        if (i > 0) {
            this_thread::sleep_for(chrono::milliseconds(500));
        }
        lm1.set_speed_regulation_enabled(motor::speed_regulation_on);
        lm1.set_stop_command(motor::stop_command_hold);
        // action 1
        lm1.set_speed_sp(large_motor_max_speed*100/100);
        lm1.set_position_sp(tilt_offset + 195);
        run_to_position(lm1, 0);
        // action 2
        lm1.set_speed_sp(large_motor_max_speed*75/100);
        lm1.set_position_sp(lm1.position()-15);
        run_to_position(lm1, 0);
        this_thread::sleep_for(chrono::milliseconds(70));
        // action 3
        lm1.set_speed_sp(large_motor_max_speed*75/100);
        lm1.set_position_sp(tilt_offset + 60);
        run_to_position(lm1, 0);
        tilt_hold(lm1);
    }
}

// m -> rotate 90
void spin(int m, large_motor &lm1, large_motor &lm2) {
    if (15 - lm1.position() < 0) {
        lm1.set_speed_regulation_enabled(motor::speed_regulation_on);
        lm1.set_stop_command(motor::stop_command_hold);
        lm1.set_speed_sp(large_motor_max_speed*75/100);
        lm1.set_position_sp(15);
        run_to_position(lm1, 0);
    }
    turntable_position = turntable_position - m*(90*3);
    lm2.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm2.set_stop_command(motor::stop_command_hold);
    lm2.set_speed_sp(large_motor_max_speed*100/100);
    lm2.set_position_sp(turntable_position);
    run_to_position(lm2, 0);
}

void scan_piece(int f, int p, int rgb[3][54], color_sensor &cs) {
    int idx = f * 9 + p;
    this_thread::sleep_for(chrono::milliseconds(100));
    scan_RGB(rgb[0][idx], rgb[1][idx], rgb[2][idx], cs);
    scan_RGB(rgb[0][idx], rgb[1][idx], rgb[2][idx], cs);
}

void spin45(large_motor &lm2) {
    lm2.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm2.set_stop_command(motor::stop_command_hold);
    lm2.set_speed_sp(large_motor_max_speed*70/100);
    turntable_position = turntable_position - (45*3);
    lm2.set_position_sp(turntable_position);
    run_to_position(lm2, 0);
}

void scan_middle(int f, int rgb[3][54], color_sensor &cs, medium_motor &mm) {
    this_thread::sleep_for(chrono::milliseconds(100));
    mm.set_speed_regulation_enabled(motor::speed_regulation_on);
    mm.set_stop_command(motor::stop_command_hold);
    mm.set_speed_sp(medium_motor_max_speed*85/100);
    mm.set_position_sp(-720);
    run_to_position(mm, 0);
    scan_piece(f, 0, rgb, cs);
}

void scan_corner(int f, int p, int rgb[3][54], color_sensor &cs, medium_motor &mm, large_motor &lm2) {
    spin45(lm2);
    mm.set_speed_regulation_enabled(motor::speed_regulation_on);
    mm.set_stop_command(motor::stop_command_hold);
    mm.set_speed_sp(medium_motor_max_speed*85/100);
    mm.set_position_sp(-585);
    run_to_position(mm, 0);
    scan_piece(f, p, rgb, cs);
}

void scan_edge(int f, int p, int rgb[3][54], color_sensor &cs, medium_motor &mm, large_motor &lm2) {
    spin45(lm2);
    mm.set_speed_regulation_enabled(motor::speed_regulation_on);
    mm.set_stop_command(motor::stop_command_hold);
    mm.set_speed_sp(medium_motor_max_speed*85/100);
    mm.set_position_sp(-610);
    run_to_position(mm, 0);
    scan_piece(f, p, rgb, cs);
}

void scan_face(int f, int rgb[3][54], color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2) {
    tilt_away(lm1);
    scan_middle(f, rgb, cs, mm);
    for (int i = 0; i < 4; i++) {
        int p = (i * 2 + 1)%8;
        scan_corner(f, p, rgb, cs, mm, lm2);
        p++;
        scan_edge(f, p, rgb, cs, mm, lm2);
    }
    int pos = turntable_position - lm2.position();
    if (pos > 0) {
        turntable_position = turntable_position - (pos - pos%(360*3));
    }
    lm2.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm2.set_stop_command(motor::stop_command_hold);
    lm2.set_speed_sp(large_motor_max_speed*75/100);
    lm2.set_position_sp(turntable_position);
    run_to_position(lm2, 0);
}

bool calculate_color(int rgb[3][54], char color[]) {
    int i = 0;
    int count_y = 0, count_w = 0, count_b = 0, count_g = 0, count_r = 0, color_sum = 0;
    ofstream ofs(data_file_path, ofstream::out);
    for (i = 0; i < 54; i++) {
        if (rgb[2][i] < rgb[0][i] && rgb[2][i] < rgb[1][i] && rgb[0][i] > 80 && rgb[1][i] > 100) {
            color[i] = 'Y';
            count_y++;
        } else if (rgb[0][i] + rgb[1][i] + rgb[2][i] > 400) {
            color[i] = 'W';
            count_w++;
        } else if (rgb[0][i] + rgb[1][i] < rgb[2][i]) {
            color[i] = 'B';
            count_b++;
        } else if (rgb[1][i] > rgb[2][i] && rgb[1][i] > rgb[0][i]) {
            color[i] = 'G';
            count_g++;
        } else if (rgb[2][i] < 80 && rgb[1][i] < 80 && rgb[1][i] + rgb[2][i] < rgb[0][i] + 8) {
            color[i] = 'R';
            count_r++;
        } else {
            color[i] = 'O';
        }
        ofs << rgb[0][i] << "," << rgb[1][i] << "," << rgb[2][i] << "," << color[i] << endl;
    }
    ofs << color << endl;
    ofs.close();
    for (i = 0; i < 46; i+=9) {
        if (color[i] == 'Y') {
            color_sum += 1;
        } else if (color[i] == 'W') {
            color_sum += 2;
        } else if (color[i] == 'B') {
            color_sum += 3;
        } else if (color[i] == 'G') {
            color_sum += 4;
        } else if (color[i] == 'R') {
            color_sum += 5;
        } else {
            color_sum += 6;
        }
    }
    if (color_sum != 21 || count_y != 9 || count_w != 9 || count_b != 9 || count_g != 9 || count_r != 9) {
        return false;
    }
    return true;
}

bool scan_cube(char color[], color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2) {
    int rgb[3][54];
    cout << "Scan the cube" << endl;
    scan_face(0, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    this_thread::sleep_for(chrono::milliseconds(100));
    tilt(1, lm1);
    scan_face(1, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    this_thread::sleep_for(chrono::milliseconds(100));
    tilt(1, lm1);
    scan_face(2, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    this_thread::sleep_for(chrono::milliseconds(100));
    spin(1, lm1, lm2);
    tilt(1, lm1);
    scan_face(3, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    spin(-1, lm1, lm2);
    tilt(1, lm1);
    scan_face(4, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    this_thread::sleep_for(chrono::milliseconds(100));
    tilt(1, lm1);
    scan_face(5, rgb, cs, mm, lm1, lm2);
    scan_away(mm);
    return calculate_color(rgb, color);
}

void turn(int m_step, large_motor &lm1, large_motor &lm2) {
    tilt_hold(lm1);
    int turn_cut = -20;
    if (m_step == 1) {
        turn_cut = 21;
    } else if (m_step == 2) {
        turn_cut = 21;
    }
    turntable_position = turntable_position - m_step*(90*3);
    lm2.set_speed_regulation_enabled(motor::speed_regulation_on);
    lm2.set_stop_command(motor::stop_command_hold);
    lm2.set_speed_sp(large_motor_max_speed*100/100);
    lm2.set_position_sp(turntable_position - turn_cut);
    run_to_position(lm2, 200);
    this_thread::sleep_for(chrono::milliseconds(100));
    lm2.set_position_sp(turntable_position);
    run_to_position(lm1, 200);
}

void apply_moves(string movestr, large_motor &lm1, large_motor &lm2) {
    int m_len = movestr.length();
    for (int i = 0; i < m_len; i += 2) {
        int m_step = 0, n_step = 0;
        if (movestr[i + 1] == '1') {
            m_step = 1;
            n_step = 1;
        } else if (movestr[i + 1] == '2') {
            m_step = 2;
            n_step = 2;
        } else {
            m_step = -1;
            n_step = 3;
        }
        if (movestr[i] == 'S') {
            this_thread::sleep_for(chrono::milliseconds(50));
            spin(m_step, lm1, lm2);
        } else if (movestr[i] == 'R') {
            this_thread::sleep_for(chrono::milliseconds(50));
            tilt(n_step, lm1);
        } else {
            this_thread::sleep_for(chrono::milliseconds(50));
            turn(m_step, lm1, lm2);
        }
    }
}

void scan_and_solve(color_sensor &cs, medium_motor &mm, large_motor &lm1, large_motor &lm2, cube_algorithm &cube_al)
{
    char color[55];
    color[54] = '\0';
    leds_orange_flash();
    for (int i = 0; i < 2; i++) {
        if (scan_cube(color, cs, mm, lm1, lm2)) {
            break;
        } else {
            if (i < (2 - 1)) {
                cout << "Scan error, retry" << endl;
            } else {
                cout << "Scan failed, can't solve" << endl;
                return;
            }
        }
    }
    scan_away(mm);

    cout << "Solving" << endl;
    string color_str(color);
    string movestr = cube_al.set_input(color_str).solve();
    ofstream ofs(data_file_path, ofstream::app);
    ofs << movestr << endl;
    ofs.close();
    if (movestr.find(cube_algorithm::err_prefix) == 0) {
        cout << movestr << endl;
    }

    leds_green_flash();
    apply_moves(movestr, lm1, lm2);
    cout << "Solved" << endl;
    // rotate 2 round
    tilt_away(lm1);
    spin(8, lm1, lm2);
}

int main(int argc, char** argv)
{
    bool escape = false;
    float dist = 0.0;
    bool done = true;
    int f = 0;
    bool remove_cube_msg = false;
    bool insert_cube_msg = false;

    // get data file path
    data_file_path = argv[0];
    size_t p = data_file_path.rfind('/');
    if (p == string::npos) {
        cout << "Error: can't detect file path!" << endl;
        return -1;
    }
    data_file_path = data_file_path.substr(0, p + 1) + "scandata.txt";

    cube_algorithm* cube_al = new cube_algorithm;

    // sensors and motors
    ultrasonic_sensor us(INPUT_1);
    color_sensor cs(INPUT_2);
    medium_motor mm(OUTPUT_C);
    large_motor lm1(OUTPUT_A);
    large_motor lm2(OUTPUT_B);

    // check sensors and motors
    if (!us.connected() || !cs.connected() || !mm.connected() || !lm1.connected() || !lm2.connected()) {
        cout << "Error: devices are not connected!" << endl;
        delete cube_al;
        return -1;
    }

    initialize(cs, mm, lm1, lm2);

    cout << "Click back key to terminate the program" << endl;
    while (1) {
        escape = button::back.pressed();
        if (escape) {
            motor_reset(mm, lm1, lm2);
            leds_green_on();
            break;
        }
        dist = us.distance_centimeters();
        if ((dist >= 6.0) && (dist <= 10.0)) {
            if (done) {
                if (!remove_cube_msg) {
                    cout << "Remove the cube" << endl;
                    remove_cube_msg = true;
                    insert_cube_msg = false;
                }
            } else {
                this_thread::sleep_for(chrono::milliseconds(3000));
                dist = us.distance_centimeters();
                if ((dist >= 6.0) && (dist <= 10.0)) {
                    scan_and_solve(cs, mm, lm1, lm2, *cube_al);
                    done = true;
                } else {
                    if (!insert_cube_msg) {
                        cout << "Insert a cube" << endl;
                        leds_orange_on();
                        done = false;
                        remove_cube_msg = false;
                        insert_cube_msg = true;
                    }
                }
            }
        } else {
            if (!insert_cube_msg) {
                cout << "Insert a cube" << endl;
                leds_orange_on();
                done = false;
                remove_cube_msg = false;
                insert_cube_msg = true;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    delete cube_al;
    return 0;
}
