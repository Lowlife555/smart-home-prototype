# Smart Home System Prototype — Fall Detection, Intelligent Lighting and Automatic Heating Control

An **mbed OS 6** firmware prototype of an elderly-assistance smart-home controller on a **NUCLEO-STM32L476RG**.
Year-1 individual coursework (EL1242 – Digital Electronics): all hardware, firmware, calibration, testing and the demonstration video were done by a single author.

## Features

| Feature | How it is implemented |
|---|---|
| **Intelligent lighting** | An LDR is sampled on an analog input and quantised into **5 light levels (0–4)**. Four GPIO LEDs light up as a bar graph (level ≥ 1 … ≥ 4): the darker the room, the more lamps switch on. The same level is printed to the serial console as a star bar — more stars, less light. |
| **Automatic heating control** | A hand-written `DS1621` I²C driver class (7-bit address `0x9E`) runs the start-conversion / read-temperature command sequence. The reading is compared against a **25 °C** threshold: below 25 °C the firmware commands the heating actuator to **135°**, at or above 25 °C it commands **0°**. The decision is reported on the serial console (the servo object is disabled in this build). |
| **Fall detection** | An **ADXL335** 3-axis analog accelerometer (X/Y/Z) is converted to *g* using a 1.65 V midpoint and 330 mV/g sensitivity. The resultant deviation from 1 g is computed and a fall is declared when it exceeds **1.8 g**. |
| **Fall alarm** | On a fall the on-board RGB LED cycles red → blue → green, holding each colour for 6 refresh steps of 50 ms (~300 ms per colour, ≈2.7 s total), then all channels switch off. |
| **Serial parameter menu** | The user button (50 ms debounce) plus the four switches select what is streamed at **9600 baud**: temperature and heating state (`0x08`), light level as a star bar (`0x04`), the three accelerometer axes in *g* (`0x02`), or the potentiometer voltage (`0x01`). Any other combination prints `Invalid switch combination`. |
| **Servo output** | A 50 Hz PWM helper (`set_servo_angle`) maps 0–180° to a duty cycle; the object is declared but left disabled in the committed build. |

## Repository layout

```
src/full.cpp     the complete application (single translation unit, 205 lines)
media/demo.mp4   demo video of the prototype running
```

## Hardware and toolchain

- **Board:** NUCLEO-STM32L476RG, running **mbed OS 6** (uses `DigitalOut`, `AnalogIn`, `I2C`, `BufferedSerial`, `ThisThread::sleep_for`).
- **Sensors and actuators:** ADXL335 3-axis analog accelerometer, DS1621 digital temperature sensor (I²C, `0x9E`), LDR light sensor, potentiometer, 4 LEDs, RGB status LED, 4 DIP switches, push button.
- **Calibration:** the LDR threshold and the accelerometer zero offset were calibrated experimentally per board (the 1.65 V / 330 mV/g constants above come from that calibration).
- **Build:** create an mbed OS 6 project (mbed CLI / Keil Studio) and use `src/full.cpp` as `main.cpp`.

## Notes and limitations

- Pin names (`PB_6`, `PA_7`, …) follow the mbed board's `PinNames.h`; porting to another board means remapping them.
- The heating output is a commanded servo angle rather than a real relay, so the heating behaviour is observable on the serial console and through the RGB/LED state rather than on a mains load.
- The serial console runs at 9600 baud; most debug prints are commented out in the committed version.

---

# 智能家居系统原型 —— 跌倒检测、智能照明与自动加热控制

基于 **mbed OS 6** 的居家养老辅助智能家居控制器固件原型，运行在 **NUCLEO-STM32L476RG** 上。
大一课程作业（EL1242 数电），**个人独立完成**：硬件搭建、固件开发、传感器标定、功能测试与演示视频全部由一人完成。

## 功能

| 功能 | 实现方式 |
|---|---|
| **智能照明** | 用模拟口读 LDR，量化成 **5 个光照等级（0–4）**；4 个 GPIO LED 做条形显示（等级 ≥1 … ≥4）—— 房间越暗，点亮的"灯"越多。同一等级还会以星号条形式打印到串口：越暗星号越多。 |
| **自动加热控制** | 手写 `DS1621` I²C 驱动类（7 位地址 `0x9E`），按"启动转换 → 读温度"指令序列读取。读数与 **25 °C** 阈值比较：低于 25 °C 时向加热执行器输出 **135°**，达到或高于 25 °C 输出 **0°**；判定结果打印到串口（本版本中舵机对象处于停用状态）。 |
| **跌倒检测** | **ADXL335** 三轴模拟加速度计（X/Y/Z）按中点 1.65 V、灵敏度 330 mV/g 换算成重力加速度；计算合加速度相对 1 g 的偏离量，超过 **1.8 g** 判定为跌倒。 |
| **跌倒报警** | 判定跌倒后板载 RGB LED 按 红 → 蓝 → 绿 循环闪烁，每色保持 6 个刷新步、每步 50 ms（约 300 ms/色，整轮约 2.7 s），随后全部关闭。 |
| **串口参数菜单** | 按键（50 ms 消抖）配合 4 个拨动开关选择串口（**9600 baud**）输出内容：温度与加热状态（`0x08`）、光照等级星号条（`0x04`）、三轴加速度 g 值（`0x02`）、电位器电压（`0x01`）；其他组合打印 `Invalid switch combination`。 |
| **舵机输出** | 50 Hz PWM 角度换算函数 `set_servo_angle`（0–180° → 占空比）；对象已声明，但本提交版本中处于停用状态。 |

## 仓库结构

```
src/full.cpp     全部应用代码（单编译单元，205 行）
media/demo.mp4   原型运行的演示视频
```

## 硬件与工具链

- **开发板**：NUCLEO-STM32L476RG，运行 **mbed OS 6**（使用 `DigitalOut`、`AnalogIn`、`I2C`、`BufferedSerial`、`ThisThread::sleep_for`）。
- **传感器与执行器**：ADXL335 三轴模拟加速度计、DS1621 数字温度传感器（I²C，`0x9E`）、LDR 光敏电阻、电位器、4 个 LED、RGB 状态灯、4 个拨动开关、按键。
- **标定**：LDR 阈值与加速度计零偏均为逐板实验标定（上述 1.65 V / 330 mV/g 常数即来自标定结果）。
- **编译**：新建 mbed OS 6 工程（mbed CLI / Keil Studio），把 `src/full.cpp` 作为 `main.cpp` 使用。

## 说明与限制

- 引脚命名（`PB_6`、`PA_7` …）遵循该 mbed 开发板的 `PinNames.h`；换板需要重新映射。
- 加热输出是"命令舵机角度"而不是真实继电器，因此加热行为体现在串口输出与 LED/RGB 状态上，并未驱动实际负载。
- 串口波特率 9600；本提交版本里多数调试打印已被注释掉。
