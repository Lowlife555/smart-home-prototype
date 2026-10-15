# Smart Home System Prototype

An **mbed OS** firmware prototype of a smart-home / assisted-living controller for an ARM Cortex-M board.
Written as Year-1 undergraduate coursework (EL1242 – Digital Electronics) and reused later as the sensing base of a wearable health-monitoring project.

## Features

| Feature | How it is implemented |
|---|---|
| **Automatic lighting** | An LDR is sampled on an analog input and quantised into 5 light levels; four GPIO LEDs light up as a bar graph (level ≥ 1 … ≥ 4). The darker the room, the more lamps are switched on. |
| **Fall detection** | A 3-axis analog accelerometer (X / Y / Z) is converted to *g* using a per-axis midpoint (1.65 V) and sensitivity (0.330 V/g). The resultant deviation from 1 g is computed and a fall is declared when it exceeds **1.8 g**. |
| **Fall alarm** | On a fall the on-board RGB LED cycles red → blue → green, holding each colour for 6 refresh steps over 9 cycles, then all channels are turned off. |
| **Room temperature** | A hand-written `DS1621` I²C driver class (7-bit address `0x9E`) issues the start-conversion / read-temperature command sequence and returns °C with 0.5 °C resolution. |
| **User input** | Four switches, the user button and a potentiometer are read for manual control and threshold adjustment. |
| **Servo output** | A 50 Hz PWM helper (`set_servo_angle`) maps 0–180° to a duty cycle (left disabled in the committed build). |

## Repository layout

```
src/full.cpp     the complete application (single translation unit)
media/demo.mp4   demo video of the prototype running
```

## Hardware and toolchain

- **Board:** ARM Cortex-M Nucleo-class board running **mbed OS 6** (uses `DigitalOut`, `AnalogIn`, `I2C`, `BufferedSerial`, `ThisThread::sleep_for`).
- **Peripherals:** LDR, 3-axis analog accelerometer, DS1621 I²C temperature sensor, RGB LED, 4 discrete LEDs, 4 switches, potentiometer, push button, optional servo.
- **Build:** create an mbed OS 6 project (mbed CLI / Keil Studio) and use `src/full.cpp` as `main.cpp`.

## Notes and limitations

- Pin names (`PB_6`, `PA_7`, …) follow the mbed board's `PinNames.h`; porting to another board means remapping them.
- The accelerometer constants (1.65 V midpoint, 0.330 V/g) are specific to the sensor fitted on the prototype.
- The serial console runs at 9600 baud; most debug prints are commented out in the committed version.

---

# 智能家居系统原型（Smart Home System Prototype）

基于 **mbed OS** 的智能家居 / 居家辅助控制器固件原型，运行在 ARM Cortex-M 开发板上。
大一课程作业（EL1242 数电），后来又被复用到可穿戴健康监测项目的传感部分。

## 功能

| 功能 | 实现方式 |
|---|---|
| **自动照明** | 用模拟口读 LDR，量化成 5 个光照等级；4 个 GPIO LED 做条形显示（等级 ≥1 … ≥4）。房间越暗，点亮的"灯"越多。 |
| **跌倒检测** | 三轴模拟加速度计（X / Y / Z）按各轴中点 1.65 V、灵敏度 0.330 V/g 换算成重力加速度；计算合加速度相对 1 g 的偏离量，超过 **1.8 g** 判定为跌倒。 |
| **跌倒报警** | 判定跌倒后板载 RGB LED 按 红 → 蓝 → 绿 循环闪烁，每色保持 6 个刷新步、共 9 轮，随后全部关闭。 |
| **室温测量** | 手写 `DS1621` I²C 驱动类（7 位地址 `0x9E`），按"启动转换 → 读温度"指令序列读取，分辨率 0.5 °C。 |
| **用户输入** | 4 个拨动开关、用户按键、电位器，用于手动控制与阈值调整。 |
| **舵机输出** | 50 Hz PWM 角度换算函数 `set_servo_angle`（0–180° → 占空比）；本提交版本中处于注释停用状态。 |

## 仓库结构

```
src/full.cpp     全部应用代码（单编译单元）
media/demo.mp4   原型运行的演示视频
```

## 硬件与工具链

- **开发板**：ARM Cortex-M Nucleo 系列，运行 **mbed OS 6**（使用 `DigitalOut`、`AnalogIn`、`I2C`、`BufferedSerial`、`ThisThread::sleep_for`）。
- **外设**：LDR 光敏电阻、三轴模拟加速度计、DS1621 I²C 温度传感器、RGB LED、4 个独立 LED、4 个开关、电位器、按键、可选舵机。
- **编译**：新建 mbed OS 6 工程（mbed CLI / Keil Studio），把 `src/full.cpp` 作为 `main.cpp` 使用。

## 说明与限制

- 引脚命名（`PB_6`、`PA_7` …）遵循该 mbed 开发板的 `PinNames.h`；换板需要重新映射。
- 加速度计常数（中点 1.65 V、0.330 V/g）是针对原型所用传感器的，换传感器需重新标定。
- 串口波特率 9600；本提交版本里多数调试打印已被注释掉。
