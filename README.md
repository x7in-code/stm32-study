# stm32-study
STM32嵌入式学习仓库，Keil MDK，C语言练习，包含各类外设实验。

## 📂 实验列表
- BlinkyLED：LED点灯基础实验
- BreathingLED：呼吸灯PWM实验
- ButtonTest：按键检测
- ButtonLibTest：按键库测试
- ClockTreeTest：系统时钟配置
- I2CTest / I2CLibTest：I2C通信
- SoftI2Cxxx：软件I2C
- SPITest：SPI外设
- USART系列：串口查询、中断、库函数测试
- OLEDTest：OLED屏幕驱动
- PrintfTest：串口重定向printf

## 🛠 环境
IDE：Keil MDK-ARM
芯片：STM32F103c8t6
语言：C

## ✨ 说明
- .gitignore 自动过滤Keil编译产物（axf/o/Objects等）
- 每个文件夹独立Keil工程，可单独打开编译