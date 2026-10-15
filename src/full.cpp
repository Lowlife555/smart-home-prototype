
#include "mbed.h"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <string>
#include <iostream>
using namespace std::chrono;
#define LIGHT_T 6
// 4 LEDs引脚定义（D10-D13对应PB6, PA7, PA6, PA5）
DigitalOut led1(PB_6);
DigitalOut led2(PA_7);
DigitalOut led3(PA_6);
DigitalOut led4(PA_5);

// 4 switches引脚定义（D6-D9对应PB10, PA8, PA9, PC7）
DigitalIn sw1(PB_10);
DigitalIn sw2(PA_8);
DigitalIn sw3(PA_9);
DigitalIn sw4(PC_7);

// 其他设备引脚定义
DigitalIn user_button(BUTTON1);  // 按键
//PwmOut servo(PA_3);            // 舵机（示例引脚，可根据实际调整）
BufferedSerial pc(USBTX, USBRX);   // 使用BufferedSerial
AnalogIn ldr(PC_0);            // LDR传感器（A5）
AnalogIn accel_x(PA_0);        // 加速度计X轴（A0）
AnalogIn accel_y(PA_1);        // 加速度计Y轴（A1）
AnalogIn accel_z(PA_4);        // 加速度计Z轴（A2）
DigitalOut red_led(PA_10);     // 三色LED - 红（D2）
DigitalOut blue_led(PB_3);     // 三色LED - 蓝（D3）
DigitalOut green_led(PB_5);    // 三色LED - 绿（D4）
AnalogIn potentiometer(PC_1);  // 电位器（A4）


class DS1621 {
    public:
        DS1621(PinName sda, PinName scl) : i2c(sda, scl) {

        }
    
        float read() {
            char cmd[2];
            float data=0;
            cmd[0] = 0xAC;
            i2c.write(addr,cmd,1,false);
            cmd[0] = 0x02;
            i2c.write(addr,cmd,1,false);
            ThisThread::sleep_for(milliseconds(20));
            cmd[0] = 0xEE;
            i2c.write(addr,cmd,1,false);
            ThisThread::sleep_for(milliseconds(20));
            cmd[0] = 0xAA;
            i2c.write(addr,cmd,1,false);
            i2c.read(addr,cmd,2,false);
            data = static_cast<int>(cmd[0])+(float)((cmd[1])==128)*0.5;
            return data;
        }
    
    private:
        I2C i2c;
        const int addr=0x9E;
    
};
    
DS1621 temp_sensor(D14, D15);

int last_button=1;

// 舵机角度控制函数
void set_servo_angle(float angle) {
    float duty = (angle / 180.0f) * 0.15f + 0.05f;
    //servo.write(duty);
}

// 获取光照强度等级（0-4级）0最强
int get_light_level() {
    //最强光0.03 最弱0.31
    float val = ldr.read()-0.1;
    if(val<=0) val = 0;
    float level = val * 20;
    return (level >= 3.7) ? 4 : static_cast<int>(level);
}

// 跌倒检测
bool detect_fall() {
    float x = accel_x.read() * 3.3f;
    float y = accel_y.read() * 3.3f;
    float z = accel_z.read() * 3.3f;
    float g_x = (x - 1.65f) / 0.330f;
    float g_y = (y - 1.65f) / 0.330f;
    float g_z = (z - 1.65f) / 0.330f;
    float magnitude = sqrt(g_x * g_x + g_y * g_y + (g_z-1) * (g_z-1));
    return (magnitude > 1.8);
}

// 跌倒报警
void fall_alarm(int reset_step) {
    static int step=0;
    static int alarm_state=0;
    if(reset_step){
        alarm_state=1;
        step%=(LIGHT_T*3);
    }
    if(!alarm_state) return;
    switch(step%(LIGHT_T*3)){
        case 0:
            blue_led = 0; green_led = 0;
            red_led=1;
            break;
        case LIGHT_T:
            red_led = 0;green_led = 0;
            blue_led=1;
            break;
        case LIGHT_T*2:
            red_led = 0; blue_led = 0;
            green_led=1;
            break;
    }
    ThisThread::sleep_for(milliseconds(50));
    step++;
    if(step>=LIGHT_T*9){
        alarm_state=0;
        red_led = 0; blue_led = 0;green_led = 0;
    }
}

int main() {
    //servo.period(0.02);  // 50Hz周期
    pc.set_baud(9600);  // 设置波特率

    while (1) {
        // 自动照明控制
        char buf[100]={0};
        memset(buf,0,100);
        int light_level = get_light_level();
        //sprintf(buf,"light level:%d\n",light_level);
        //pc.write(buf, strlen(buf));
        led1 = (light_level >= 1);
        led2 = (light_level >= 2);
        led3 = (light_level >= 3);
        led4 = (light_level >= 4);

        // 跌倒检测
        if (detect_fall()) {
            fall_alarm(1);
        }
        else{
            fall_alarm(0);
        }

        // 按键功能处理
        int this_button = user_button.read();
        if (last_button && !this_button) {
            ThisThread::sleep_for(milliseconds(50));  // 消抖
            if((this_button = user_button.read())) break;
            int switch_val = (sw4.read() << 3) | (sw3.read() << 2) |
                            (sw2.read() << 1) | sw1.read();
            switch_val &=0x0f;
            switch (switch_val) {
                case 0x08: {
                    float temp = temp_sensor.read();
                    sprintf(buf, "Temp: %.1f C\n", temp);
                    pc.write(buf, strlen(buf));
                    if(temp<25){
                        pc.write("Start heating, 135 degrees angle\n", 34);
                    }
                    else{
                        pc.write("Stop heating, 0 degrees\n",25);
                    }
                    break;
                }
                case 0x04: {  // 读取光照强度
                    pc.write("Light Level: ", 12);
                    for (int i = 5; i > light_level; --i) {
                        char star[] = "*";
                        pc.write(star, 1);
                    }
                    pc.write("\n", 1);
                    //float k = ldr.read();
                    //sprintf(buf,"light level num:%.2f\n",k);
                    //pc.write(buf, strlen(buf));
                    break;
                }
                case 0x02: {  // 读取加速度计数据
                    float x = (accel_x.read() * 3.3f - 1.65f) / 0.330f;
                    float y = (accel_y.read() * 3.3f - 1.65f) / 0.330f;
                    float z = (accel_z.read() * 3.3f - 1.65f) / 0.330f;
                    sprintf(buf,"Accelerometer: X=%.2f, Y=%.2f, Z=%.2f g\n", x, y, z);
                    pc.write(buf, strlen(buf));
                    break;
                }
                case 0x01: {  // 读取电位器电压
                    float voltage = potentiometer.read() * 3.3f;
                    sprintf(buf,"Potentiometer Voltage: %.2f V\n", voltage);
                    pc.write(buf, strlen(buf));
                    break;
                }
                default:
                    pc.write("Invalid switch combination\n", 25);
            }
        }
        last_button = this_button;
    }
}
