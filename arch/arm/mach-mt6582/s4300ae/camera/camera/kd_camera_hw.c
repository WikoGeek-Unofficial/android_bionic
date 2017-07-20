#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>
#include <linux/kernel.h>

#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg)
static u32 subCamPDNStatus = 0;	//LINE <> <20140906> <resolve the Bg that can't change to subCamera on little phones> paul

#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         xlog_printk(ANDROID_LOG_ERR, PFX , fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                   xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif

kal_bool searchMainSensor = KAL_TRUE;

int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
    u32 pinSetIdx = 0;//default main sensor
    u32 pinSetIdxTmp = 0;

#define IDX_PS_CMRST 0
#define IDX_PS_CMPDN 4

#define IDX_PS_MODE 1
#define IDX_PS_ON   2
#define IDX_PS_OFF  3


    u32 pinSet[2][8] =
    {
        //for main sensor
        {
            GPIO_CAMERA_CMRST_PIN,
            GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
            GPIO_OUT_ONE,                   /* ON state */
            GPIO_OUT_ZERO,                  /* OFF state */
            GPIO_CAMERA_CMPDN_PIN,
            GPIO_CAMERA_CMPDN_PIN_M_GPIO,
            GPIO_OUT_ONE,
            GPIO_OUT_ZERO,
        },
        //for sub sensor
        {
            GPIO_CAMERA_CMRST1_PIN,
            GPIO_CAMERA_CMRST1_PIN_M_GPIO,
            GPIO_OUT_ONE,
            GPIO_OUT_ZERO,
            GPIO_CAMERA_CMPDN1_PIN,
            GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
            GPIO_OUT_ONE,
            GPIO_OUT_ZERO,
        },
    };

    if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
    {
        pinSetIdx = 0;
        searchMainSensor = KAL_TRUE;
    }
    else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx)
    {
        pinSetIdx = 1;
        searchMainSensor = KAL_FALSE;
    }


    //power ON
    if (On)
    {
        PK_DBG("kdCISModulePowerOn -on:currSensorName=%s\n",currSensorName);
        PK_DBG("kdCISModulePowerOn -on:pinSetIdx=%d\n",pinSetIdx);

        if ((currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5648_MIPI_RAW_DARLING, currSensorName)))
            ||(currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5648_MIPI_RAW, currSensorName))))
        {
            PK_DBG("kdCISModulePowerOn get in--- OV5648 \n");
            //enable active sensor
            //RST pin
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");
                }
                mdelay(10);

                //PDN pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }
            }

            //DOVDD
            // PK_DBG("[ON_general 1.8V]sensorIdx:%d \n",SensorIdx);
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);
            //AVDD
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);

            //DVDD
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1500,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);

            //enable active sensor
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                //PDN pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }


                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");
                }
                mdelay(1);

            }

            //disable inactive sensor
            if(pinSetIdx == 0 || pinSetIdx == 2)  //disable sub
            {
                if (GPIO_CAMERA_INVALID != pinSet[1][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_ON]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
            }
            else
            {
                if (GPIO_CAMERA_INVALID != pinSet[0][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
                if (GPIO_CAMERA_INVALID != pinSet[2][IDX_PS_CMRST])
                {
                    /*if(mt_set_gpio_mode(pinSet[2][IDX_PS_CMRST],pinSet[2][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                    if(mt_set_gpio_mode(pinSet[2][IDX_PS_CMPDN],pinSet[2][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                    if(mt_set_gpio_dir(pinSet[2][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                    if(mt_set_gpio_dir(pinSet[2][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                    if(mt_set_gpio_out(pinSet[2][IDX_PS_CMRST],pinSet[2][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
                    if(mt_set_gpio_out(pinSet[2][IDX_PS_CMPDN],pinSet[2][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
                    */
                }
            }//BEGIN<s4700><vedio call fatal exception>panchenjun
        }


        /******************Paul add************************/
        else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_HI704_YUV,currSensorName)))
        {
            PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_HI704_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio 			   mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio			   failed!! \n");
                }
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA			   SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed\n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio 			   failed!! \n");
                }
                mdelay(2);
            }


            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);
            //enable active sensor
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");
                }
                mdelay(10);
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");
                }
                mdelay(1);
                //PDN pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }
            }

            //disable inactive sensor
            if(pinSetIdx == 0 || pinSetIdx == 2)
            {
                //disable sub
                if (GPIO_CAMERA_INVALID != pinSet[1][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_ON]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
            }
            else
            {
                if (GPIO_CAMERA_INVALID != pinSet[0][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
            }

        }
        /******************Paul add************************/
        else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_SP0A19_YUV,currSensorName)))
        {
            //specical config for sp0a19, the power sequence is unnormally
            if(pinSetIdx ==0)
                return 0;
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {

                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }

                //RST pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");
                }
            }

            //must AVDD pull high first
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(5);

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            mdelay(5);

            //PDN/STBY pin
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                mdelay(10);
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }
                mdelay(150);
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");
                }
                mdelay(10);

            }

            //disable inactive sensor
            if(pinSetIdx == 0 || pinSetIdx == 2)  //disable sub
            {
                if (GPIO_CAMERA_INVALID != pinSet[1][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[1][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMRST],pinSet[1][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[1][IDX_PS_CMPDN],pinSet[1][IDX_PS_CMPDN+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
            }
            else
            {
                if (GPIO_CAMERA_INVALID != pinSet[0][IDX_PS_CMRST])
                {
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_mode(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_MODE]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMRST],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_dir(pinSet[0][IDX_PS_CMPDN],GPIO_DIR_OUT))
                    {
                        PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMRST],pinSet[0][IDX_PS_CMRST+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                    }
                    if(mt_set_gpio_out(pinSet[0][IDX_PS_CMPDN],pinSet[0][IDX_PS_CMPDN+IDX_PS_OFF]))
                    {
                        PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                    }
                }
                if (GPIO_CAMERA_INVALID != pinSet[2][IDX_PS_CMRST])
                {
                    //PK_DBG("kdCISModulePowerOn 8AA close index[2]\n");

                    /*if(mt_set_gpio_mode(pinSet[2][IDX_PS_CMRST],pinSet[2][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                    if(mt_set_gpio_mode(pinSet[2][IDX_PS_CMPDN],pinSet[2][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                    if(mt_set_gpio_dir(pinSet[2][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                    if(mt_set_gpio_dir(pinSet[2][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                    if(mt_set_gpio_out(pinSet[2][IDX_PS_CMRST],pinSet[2][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
                    if(mt_set_gpio_out(pinSet[2][IDX_PS_CMPDN],pinSet[2][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
                    */
                }
            }
        }
        else
        {

            PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn Please your sensor power on code here\n" );
        }
    }
    else  //power OFF
    {

        PK_DBG("kdCISModulePowerOn -off:currSensorName=%s\n",currSensorName);
		if (subCamPDNStatus == 1){
		
		  if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
		  if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
		  if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //Low == power down lens module
		
		   }//close Hi704
		  //END<> <20140906> <resolve the Bg that can't change to subCamera on little phones> Paul  

        if ((currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5648_MIPI_RAW_DARLING, currSensorName)))
            ||(currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV5648_MIPI_RAW, currSensorName))))
        {
            PK_DBG("kdCISModulePower--off get in---OV5648 \n");
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                }
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
        }
        else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_SP0A19_YUV,currSensorName)))
        {
            //specical config for sp0a19, the power sequence is unnormally
            if(pinSetIdx ==0)
                return 0;

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }
        }
        else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_HI704_YUV,currSensorName)))
        {
            PK_DBG("kdCISModulePower--off get in---HI704 \n");
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST])
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE]))
                {
                    PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT))
                {
                    PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA LENS] set gpio failed!! \n");   //high == power down lens module
                }
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF]))
                {
                    PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");   //low == reset sensor
                }
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }
        }
        else
        {
            PK_DBG("kdCISModulePower--off add your sensor power off code here \n");
        }
    }
    return 0;

_kdCISModulePowerOn_exit_:
    return -EIO;
}

EXPORT_SYMBOL(kdCISModulePowerOn);


//!--
//

void SubCameraDigtalPDNCtrl(u32 onoff){
    subCamPDNStatus = onoff;
}
EXPORT_SYMBOL(SubCameraDigtalPDNCtrl);




