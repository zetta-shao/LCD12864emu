#ifndef __INCL_SW35XX_H__
#define __INCL_SW35XX_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "swi2c.h"
#include <stdint.h>
//#include <Wire.h>

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define NOT_FAST_CHARGE	0
#define QC2				1
#define QC3				2
#define FCP				3
#define SCP				4
#define PD_FIX			5
#define PD_PPS			6
#define MTKPE1			7
#define MTKPE2			8
#define LVDC			9
#define SFCP			10
#define AFC				11

#define HARDRESET		1

#define QC_CONF_NONE	0
#define QC_CONF_PE		(1 << 0)
#define QC_CONF_SCP		(1 << 2)
#define QC_CONF_FCP		(1 << 3)
#define QC_CONF_QC		(1 << 4)
#define QC_CONF_PD		(1 << 5)
#define QC_CONF_PORT2	(1 << 6)
#define QC_CONF_PORT1	(1 << 7)
#define QC_CONF_AFC		(1 << 14)
#define QC_CONF_SFCP	(1 << 15)
#define QC_CONF_ALL	(QC_CONF_PE | QC_CONF_SCP | QC_CONF_FCP | QC_CONF_QC | QC_CONF_PD | QC_CONF_PORT1 | QC_CONF_PORT2 | QC_CONF_AFC | QC_CONF_SFCP)

#define	QC_PWR_9V		0
#define QC_PWR_12V		1
#define QC_PWR_20V_1	2
#define QC_PWR_20V_2	3

#define ADC_VIN			1
#define ADC_VOUT		2
#define ADC_IOUT_USB_C	3
#define ADC_IOUT_USB_A	4
#define ADC_TEMPERATURE	6

typedef struct t_sw35xx {
    swi2c_t     *pDev;
    uint16_t    vin_mV; /*  @brief 输入电压 */
    uint16_t    vout_mV; /* @brief 输出电压 */
    uint16_t	iout_usbc_mA; /*@brief 输出电流1(type-C) */
    uint16_t	iout_usba_mA; /*@brief 输出电流2(type-A) */
    uint8_t 	fastChargeType; /*@brief 快充协议*/
    uint8_t 	PDVersion; /*@brief PD版本(2或者3) */
    uint8_t 	rsv1;
    uint8_t 	init;
} sw35xx_t;

  int __sw35xx_i2cReadReg8(struct t_sw35xx *d, const uint8_t reg);
  int __sw35xx_i2cWriteReg8(struct t_sw35xx *d, const uint8_t reg, const uint8_t data);

  void SW35xx_unlock_i2c_write(struct t_sw35xx *d);
  void SW35xx_lock_i2c_write(struct t_sw35xx *d);

  uint16_t SW35xx_readADCDataBuffer(struct t_sw35xx *d, const uint8_t type);
  void SW35xx_begin(struct t_sw35xx *d);
  /**
   * @brief 读取当前充电状态
   * 
   */
  void SW35xx_readStatus(struct t_sw35xx *d, uint8_t useADCDataBuffer);
  /**
   * @brief Returns the voltage of the NTC in mV
   */
  float SW35xx_readTemperature(struct t_sw35xx *d, uint8_t useADCDataBuffer);
  /**
   * @brief 发送PD命令
   * 
   * @note 这个芯片似乎可以发送很多种PD命令，但是寄存器文档里只有hardreset. 如果你有PD抓包工具，可以尝试2~15的不同参数，摸索出对应的命令。记得开个pr告诉我!
   */
  void SW35xx_sendPDCmd(struct t_sw35xx *d, uint8_t PDcmd);
  /**
   * @brief 重新广播PDO. 改变最大电流后需要调用此函数或者重新插拔USB线来让设置生效.
   */
  void SW35xx_rebroadcastPDO(struct t_sw35xx *d);
  /**
   * @brief Enable or disable the support for certain quick charge features
   * @param flags Multiple values of QuickChargeConfig combined with bitwise or
   */
  void SW35xx_setQuickChargeConfiguration(struct t_sw35xx *d, const uint16_t flags,
      uint16_t QuickChargePowerClass);
  /**
   * @brief 把PD所有组别的电流设置成5A. 如果你的芯片不是sw3518s请慎重使用
   */
  void SW35xx_setMaxCurrent5A(struct t_sw35xx *d);
   /**
   * @brief 设置固定电压组别的最大输出电流
   * 
   * @param ma_xx 各组别的最大输出电流,单位毫安,最小分度50ma,设为0则关闭
   * @note 5v无法关闭
   */
  void SW35xx_setMaxCurrentsFixed(struct t_sw35xx *d, uint32_t ma_5v, uint32_t ma_9v, uint32_t ma_12v, uint32_t ma_15v, uint32_t ma_20v);
  /**
   * @brief 设置PPS组别的最大输出电流
   * 
   * @param ma_xxx 各组别最大输出电流,单位毫安,最小分度50ma,设为0则关闭
   * @note 注意 PD 配置的最大功率大于 60W 时, pps1 将不会广播 (TODO:datasheet这么写的，没试过)
   *       pps1 的最高电压需要大于 pps0 的最高电压，否则 pps1 不会广播;
   */
  void SW35xx_setMaxCurrentsPPS(struct t_sw35xx *d, uint32_t ma_pps1, uint32_t ma_pps2);
  /**
  //  * @brief 重置最大输出电流
  //  * 
  //  * @note 20v组别的电流不会被重置
  //  */
  // void resetMaxCurrents();
  // /**
  //  * @brief 启用Emarker检测
  //  */
  // void enableEmarker();
  // /**
  //  * @brief 禁用Emarker检测
  //  */
  // void disableEmarker();

  uint8_t sw35xx_version(struct t_sw35xx *d);
  //uint8_t sw35xx_unlock_i2c(struct t_sw35xx *d);
  //uint8_t sw35xx_lock_i2c(struct t_sw35xx *d);
  uint8_t sw35xx_init(struct t_sw35xx *d, swi2c_t *pDev);
#ifdef __cplusplus
}
#endif
#endif
