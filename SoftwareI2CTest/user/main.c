#include "stm32f10x.h"

/* 函数声明 */
void My_SI2C_Init(void);
void scl_write(uint8_t level);
void sda_write(uint8_t level);
uint8_t sda_read(void);
void delay_us(uint32_t us);
void SendStart(void);
void SendStop(void);
uint8_t SendByte(uint8_t Byte);
uint8_t ReceiveByte(uint8_t Ack);

int My_SI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size);
int My_SI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size);

int main(void)
{
    My_SI2C_Init();

    // OLED 初始化命令序列（你的原有数据）
    uint8_t commands[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
    My_SI2C_SendBytes(0x78, commands, 5);

    while(1)
    {
    }
}

/**
 * @brief  模拟I2C初始化：使能GPIOA时钟，配置PA0(SCL)和PA1(SDA)为开漏输出
 */
void My_SI2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;  // SCL -> PA0, SDA -> PA1
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;        // 开漏输出，适合I2C总线
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;         // I2C速率不高，2MHz足够

    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 初始状态：总线空闲，SCL和SDA均置高（开漏输出写1即释放总线）
    GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
}

/**
 * @brief  控制SCL线电平
 * @param  level: 0 = 低电平, 非0 = 高电平
 */
void scl_write(uint8_t level)
{
    if(level == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
    }
}

/**
 * @brief  控制SDA线电平（输出模式）
 * @param  level: 0 = 低电平, 非0 = 高电平
 */
void sda_write(uint8_t level)
{
    if(level == 0)
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
    }
    else
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
    }
}

/**
 * @brief  读取SDA线电平（输入模式，开漏输出也可直接读输入寄存器）
 * @return 1: 高电平, 0: 低电平
 */
uint8_t sda_read(void)
{
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief  微秒级延时（粗略循环延时，需根据主频调整循环次数）
 * @param  us: 延时的微秒数
 */
void delay_us(uint32_t us)
{
    uint32_t n = us * 8;         // 假设72MHz下，循环8次约1us（实际需校准）
    for(uint32_t i = 0; i < n; i++);
}

/**
 * @brief  产生I2C起始条件：SCL高时，SDA由高变低
 */
void SendStart(void)
{
    // 标准起始时序：先确保总线空闲（SCL高，SDA高），然后SDA拉低，最后SCL拉低
    scl_write(1);               // 确保SCL为高
    sda_write(1);               // 确保SDA为高（释放总线）
    delay_us(1);                // 满足建立时间
    sda_write(0);               // SDA拉低，产生起始条件
    delay_us(1);                // 保持时间
    scl_write(0);               // 拉低SCL，准备发送数据
    delay_us(1);
}

/**
 * @brief  产生I2C停止条件：SCL高时，SDA由低变高
 */
void SendStop(void)
{
    scl_write(0);               // 先将SCL拉低，确保SDA变化发生在SCL低电平期间
    sda_write(0);               // SDA也拉低，为停止做准备
    delay_us(1);
    scl_write(1);               // 拉高SCL
    delay_us(1);                // 满足建立时间
    sda_write(1);               // SDA由低变高，产生停止条件
    delay_us(1);                // 总线释放后保持一段时间
    // 停止后SCL和SDA均为高，总线空闲
}

/**
 * @brief  发送一个字节，并接收从机的应答位
 * @param  Byte: 要发送的8位数据
 * @return 0: 收到应答(ACK), 1: 未收到应答(NACK)
 */
uint8_t SendByte(uint8_t Byte)
{
    uint8_t ack;

    // 先发送8个数据位，高位在前
    for(int i = 7; i >= 0; i--)
    {
        scl_write(0);                           // SCL低电平，允许改变SDA
        if(Byte & (0x01 << i))
            sda_write(1);                       // 数据位为1，SDA置高
        else
            sda_write(0);                       // 数据位为0，SDA置低
        delay_us(1);                            // 数据建立时间

        scl_write(1);                           // SCL上升沿，从机锁存数据
        // 注意：此处原有 sda_write(1); 是错误的！SCL高电平时SDA必须保持稳定，不能变化。
        delay_us(1);                            // 高电平保持时间
    }

    // 第9个时钟脉冲：接收应答位
    scl_write(0);                               // SCL拉低，准备应答
    sda_write(1);                               // 主机释放SDA（开漏输出写1），由从机控制SDA
    delay_us(1);
    scl_write(1);                               // SCL第9个上升沿，从机将应答位放到SDA上
    delay_us(1);
    ack = sda_read();                           // 读取SDA状态（0=ACK，1=NACK）
    scl_write(0);                               // 应答时钟结束，拉低SCL
    sda_write(1);                               // 释放SDA（为下一字节或停止做准备）
    delay_us(1);

    return ack;                                 // 返回应答情况
}

/**
 * @brief  接收一个字节，并发送应答/非应答
 * @param  Ack: 0 = 发送非应答(NACK, 结束接收), 1 = 发送应答(ACK, 继续接收)
 * @return 接收到的8位数据
 */
uint8_t ReceiveByte(uint8_t Ack)
{
    uint8_t byte = 0;

    // 先接收8个数据位，高位在前
    for(int8_t i = 7; i >= 0; i--)
    {
        scl_write(0);                           // SCL低电平，从机准备数据
        sda_write(1);                           // 主机释放SDA，让从机控制
        delay_us(1);
        scl_write(1);                           // SCL上升沿，锁存数据
        delay_us(1);

        if(sda_read() != 0)
        {
            byte |= (0x01 << i);                // 如果SDA为高，该位置1
        }
    }

    // 第9个时钟：发送应答/非应答位
    scl_write(0);                               // SCL拉低，准备改变SDA
    if(Ack)
        sda_write(0);                           // ACK = 0（低电平应答）
    else
        sda_write(1);                           // NACK = 1（高电平非应答）
    delay_us(1);
    scl_write(1);                               // SCL上升沿，从机读取应答
    delay_us(1);
    scl_write(0);                               // 应答时钟结束，拉低SCL
    sda_write(1);                               // 释放SDA
    delay_us(1);

    return byte;
}

/**
 * @brief  向指定从机地址连续发送多个字节（常用于写命令/数据）
 * @param  Addr:  7位从机地址（已左移1位，即包含了读写位，调用时应传入写地址，如0x78）
 * @param  pData: 待发送数据缓冲区指针
 * @param  Size:  发送字节数
 * @return 0: 成功, -1: 从机未应答地址, -2: 数据发送过程中从机未应答
 */
int My_SI2C_SendBytes(uint8_t Addr, uint8_t *pData, uint16_t Size)
{
    SendStart();                                // 产生起始条件

    // 发送从机地址（写操作，最低位应为0，调用时已保证）
    if(SendByte(Addr) != 0)
    {
        SendStop();                             // 地址无应答，发送停止条件
        return -1;
    }

    // 连续发送数据字节
    for(uint32_t i = 0; i < Size; i++)
    {
        if(SendByte(pData[i]) != 0)
        {
            SendStop();                         // 某个字节无应答，停止传输
            return -2;
        }
    }

    SendStop();                                 // 所有数据发送完毕，产生停止条件
    return 0;
}

/**
 * @brief  从指定从机地址连续读取多个字节
 * @param  Addr:    7位从机地址（已左移1位，调用时应传入读地址，如0x79）
 * @param  pBuffer: 接收数据缓冲区指针
 * @param  Size:    要读取的字节数
 * @return 0: 成功, -1: 从机未应答地址
 */
int My_SI2C_ReceiveBytes(uint8_t Addr, uint8_t *pBuffer, uint16_t Size)
{
    SendStart();                                // 产生起始条件

    // 发送从机地址（读操作，最低位为1）
    if(SendByte(Addr | 0x01) != 0)
    {
        SendStop();
        return -1;
    }

    // 读取前 Size-1 个字节，每个字节后发送ACK(0)表示继续读取
    for(uint32_t i = 0; i < Size - 1; i++)
    {
        pBuffer[i] = ReceiveByte(1);            // 1 = 发送ACK
    }
    // 最后一个字节后发送NACK(0)表示读取结束
    pBuffer[Size - 1] = ReceiveByte(0);

    SendStop();
    return 0;
}
