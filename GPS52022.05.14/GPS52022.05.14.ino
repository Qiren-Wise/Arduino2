
//arduino-variometer/gps_time_analysis/gps_time_analysis.ino

   
/* gps_time_analysis -- GPS 频率确定
 *
* 版权所有 2016-2019 巴蒂斯特·佩莱格林
 * 
* 此文件是 GNUVario 的一部分。
 *
GNUVario是自由软件：你可以重新发布它和/或修改
* 根据 GNU 通用公共许可证的条款，由
* 自由软件基金会，许可证版本 3，或
*（由您选择）任何更高版本。
 *
* GNUVario 的分发是希望它有用，
* 但没有任何保证;甚至没有暗示的保证
* 适销性或特定用途的适用性。查看
* GNU 通用公共许可证了解更多详情。
 *
* 您应该已经收到 GNU 通用公共许可证的副本
*与此程序一起。如果没有，请参阅<https://www.gnu.org/licenses/>。
 */

#include <Arduino.h>
#include <SPI.h>
#include <VarioSettings.h>
#include <IntTW.h>
#include <vertaccel.h>
#include <EEPROM.h>
#include <LightInvensense.h>
#include <avr/pgmspace.h>
#include <varioscreen.h>
#include <digit.h>
#include <LightSdCard.h>
#include <LightFat16.h>
#include <SerialNmea.h>
#include <NmeaParser.h>
#include <LxnavSentence.h>
#include <LK8Sentence.h>
#include <IGCSentence.h>
#include <FirmwareUpdater.h>

/********************************************************/
/*此草图按顺序给出 ： */
/* -> 最后一个 GPS 周期 */
/* -> 平均 GPS 周期 */
/* -> 发送 RMC 和 GGA 句子所需的时间 */
/********************************************************/

/* 输出在哪里 ？*/
#define SERIAL_OUTPUT
#difine SDCARD_OUTPUT

#difine OUTPUT_PRECISION 1
#difine MAX_SILENT_DURATION 50
#difine PERIOD_MEAN_FILTER_SIZE 20


/***************/
/*IMU 对象 */
/***************/
Vertaccel vertaccel;


/*****************/
/* 屏幕显示 */
/*****************/
#difine HAVE_SCREEN
#difine VARIOSCREEN_GPS_PERIOD_ANCHOR_X 62
#difine VARIOSCREEN_GPS_PERIOD_ANCHOR_Y 0
#difine VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_X 62
#difine VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_Y 2
#difine VARIOSCREEN_GPS_DURATION_ANCHOR_X 62
#difine VARIOSCREEN_GPS_DURATION_ANCHOR_Y 4
#difine VARIOSCREEN_SAT_ANCHOR_X 68
#difine VARIOSCREEN_SAT_ANCHOR_Y 0

可变屏幕（VARIOSCREEN_DC_PIN，VARIOSCREEN_CS_PIN，VARIOSCREEN_RST_PIN）;
ScreenDigit gpsperiodDigit（屏幕， VARIOSCREEN_GPS_PERIOD_ANCHOR_X， VARIOSCREEN_GPS_PERIOD_ANCHOR_Y， OUTPUT_PRECISION， false);
ScreenDigit gpsMeanPeriodDigit（屏幕，VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_X，VARIOSCREEN_GPS_MEAN_PERIOD_ANCHOR_Y，OUTPUT_PRECISION，假);
屏幕数字gpsDuration数字（屏幕，VARIOSCREEN_GPS_DURATION_ANCHOR_X，VARIOSCREEN_GPS_DURATION_ANCHOR_Y，OUTPUT_PRECISION，假);
SATLevel satLevel（屏幕，VARIOSCREEN_SAT_ANCHOR_X，VARIOSCREEN_SAT_ANCHOR_Y）;

无符号长最后一级显示 = 0;
#difine SAT_LEVEL_DISPLAY_DELAY 1000

ScreenSchedulerObject displayList[] = { {&gpsPeriodDigit， 0}， {&gpsMeanPeriodDigit， 0}， {&gpsDurationDigit， 0}， {&satLevel， 0} };
ScreenScheduler varioScreen（screen， displayList， sizeof（displayList）/sizeof（ScreenSchedulerObject）， 0， 0）;
#endif //HAVE_SCREEN

/***************/
/* 全球定位系统对象 */
/***************/
NmeaParser nmeaParser;

/*******************/
/*SD 卡对象 */
/*******************/
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）

字符文件名[] = “TIMES00”;
#定义FILE_NAME_SIZE 7

光脂肪16 文件;

#定义SD_CARD_STATE_INITIAL 0
#定义SD_CARD_STATE_INIT 1
#定义SD_CARD_STATE_BEGIN 2
int sdcardState = SD_CARD_STATE_INITIAL;

#endif //HAVE_SDCARD

#if （defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）） ||已定义（SERIAL_OUTPUT）
数字值数字;
#endif


/*******************/
/* 期间措施 */
/*******************/
无符号长 gpsLastLock 时间戳;
未签名的长 gpsLastRelease 时间戳;
无符号长周期持续;
无符号长锁持续时间;
无符号的长周期周期[PERIOD_MEAN_FILTER_SIZE];
int lastPeriodPos = 0;
双周期含义;
bool needOutput = false;


#if （defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）） ||已定义（SERIAL_OUTPUT）
void writeNumbers（ double* values， int count ） {

#ifdefSERIAL_OUTPUT
SerialNmea.锁();
#endif

  /* 写每个数字 */
  for（int i = 0; i<count; i++） {

值数字。begin（values[i]， OUTPUT_PRECISION）;
    while（valueDigit.可用()) {

      uint8_t c = valueDigit。获取();
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
      if（ sdcardState == SD_CARD_STATE_BEGIN ） {
文件。写（c）;
      }
#endif
#ifdefSERIAL_OUTPUT
SerialNmea.写（c）;
#endif
    }
    
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
    if（ sdcardState == SD_CARD_STATE_BEGIN ） {
文件。写（'\n');
    }
#endif
#ifdefSERIAL_OUTPUT
SerialNmea.写（'\n');
#endif
  }

#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
  if（ sdcardState == SD_CARD_STATE_BEGIN ） {
文件。写（'\n');
文件。同步();
  }
#endif
#ifdefSERIAL_OUTPUT
SerialNmea.写（'\n');
SerialNmea.释放();
#endif

}
#endif


void computeMeanPeriod（void) {

  /* 保存新值 */
lastPeriodDurations[lastPeriodPos] = periodDuration;
lastPeriodPos = （lastPeriodPos + 1） % PERIOD_MEAN_FILTER_SIZE;

  /* 计算平均值 */
  无符号长周期Sum = 0;
  for（ int i = 0; i<PERIOD_MEAN_FILTER_SIZE; i++） {
周期和 += 最后周期持续时间[i];
  }

周期含义 = （双）周期和/（双）PERIOD_MEAN_FILTER_SIZE;
}


布尔 gpsCheckLock（void) {

  /* 保存时间 */
  无符号长接收时间戳 = 串行Nmea。获取接收时间戳();
  
  if（ gpsLastLockTimestamp ！= receiveTimestamp ） {
periodDuration = receiveTimestamp - gpsLastLockTimestamp;
lockDuration = gpsLastReleaseTimestamp - gpsLastLockTimestamp;
gpsLastLockTimestamp = receiveTimestamp;
gpsLastReleaseTimestamp = gpsLastLockTimestamp;
    computeMeanPeriod();
    返回 true;
  } 
  返回 false;
}


void gpsSaveRelease（void) {
gpsLastReleaseTimestamp = millis();
}

#if （defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）） ||已定义（SERIAL_OUTPUT）
void 输出结果（void) {

  /* sd卡/串行输出*/
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
  /* sd卡输出 */
  if（ sdcardState > SD_CARD_STATE_INITIAL） {

    /* 创建文件 */
    if（ sdcardState == SD_CARD_STATE_INIT） {
文件。初始化();
文件。开始（文件名， FILE_NAME_SIZE）;
sdcardState = SD_CARD_STATE_BEGIN;
    }
  }
#endif

#如果定义（SERIAL_OUTPUT） ||已定义（SDCARD_OUTPUT）
  双输出值[] = {（双）周期持续时间， 周期含义， （双）锁定持续时间};
  writeNumbers（outputValues， 3);
#endif
}   
#endif

 
空隙设置() {

  /*****************************/
  /* 等待设备开机 */
  /*****************************/
  延迟（VARIOMETER_POWER_ON_DELAY）;

  /**********************/
  /* 初始化加速度计 */
  /**********************/
intTW.开始();
#ifdefHAVE_ACCELEROMETER
vertaccel.初始化();
  if（ 固件更新第二步() ) {
   固件更新();
  }
#endif //HAVE_ACCELEROMETER

  /************/
  /* 初始化 SPI */
  /************/ 

  /* 在与设备通信之前设置所有 SPI CS 线路 */
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
文件。使能SPI();
#endif //HAVE_SDCARD

#ifdefHAVE_SCREEN
屏幕。使能SPI();
#endif //HAVE_SCREEN

  /****************/
  /* 初始化 SD 卡 */
  /****************/
#if defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）
  if（ 文件。init（） >= 0 ) {
sdcardState = SD_CARD_STATE_INIT;
  }
#endif //HAVE_SDCARD
 
  /***************/
  /* 初始化屏幕 */
  /***************/
#ifdefHAVE_SCREEN
屏幕。开始（VARIOSCREEN_CONTRAST）;
#endif //HAVE_SCREEN
  
  /************/
  /* 初始化 gps */
  /************/
SerialNmea.开始（GPS_BLUETOOTH_BAUDS，真);

}

空隙循环() {

  if（ gpsCheckLock() ) {
    
    /* 屏幕输出 */
gpsPeriodDigit.setValue（（double）periodDuration）;
gpsMeanPeriodDigit.setValue（periodMean）;
gpsDurationDigit.setValue（（double）lockDuration）;

    /* 串行/sd 卡稍后输出 */
    /* 作为串行输出当前正在使用中 */
需要输出 = 真;
    
  }

#if （defined（HAVE_SDCARD） && defined（SDCARD_OUTPUT）） ||已定义（SERIAL_OUTPUT）
  if（ （millis（） - serialNmea.getLastReceiveTimestamp（） > MAX_SILENT_DURATION） && needOutput ） {
    输出结果();
需要输出 = 假;
  }
#endif

  /* 尽可能解析，发布时检查 */
  bool needOutputTag = false;
  if（ serialNmea.锁定() ) {
nmeaParser.开始RMC();
需要输出标记 = 真;
  }

  if( serialNmea.lockGGA() ) {
    nmeaParser.beginGGA();
    needOutputTag = true;
  }

#if defined(HAVE_SDCARD) && defined(SDCARD_OUTPUT)
  if( needOutputTag ) {
    if( sdcardState == SD_CARD_STATE_BEGIN ) {
      serialNmea.addTagToRead();
      
      uint8_t c;
      do {
        c = serialNmea.read();
        file.write(c);
      } while( c != ',' );
      nmeaParser.feed(c);
    }
     
    needOutputTag = false;
  }
#endif

  if( nmeaParser.isParsing() ) {
    while( nmeaParser.isParsing() ) {
      uint8_t c = serialNmea.read();
        
      /* parse sentence */        
      nmeaParser.feed( c );

#if defined(HAVE_SDCARD) && defined(SDCARD_OUTPUT)
      /* output to SD card */
      if( sdcardState == SD_CARD_STATE_BEGIN ) {
        file.write(c);
      }
#endif
    }

#if defined(HAVE_SDCARD) && defined(SDCARD_OUTPUT)
      /* output to SD card */
      if( sdcardState == SD_CARD_STATE_BEGIN ) {
        uint8_t c;
        do {
          c = serialNmea.read();
          file.write(c);
        } while( c != '\n' );
      }
#endif
    serialNmea.release();
    gpsSaveRelease();
  }

#ifdef HAVE_SCREEN
  /* screen update */
  varioScreen.displayStep();
  if( millis() - lastSatLevelDisplay > SAT_LEVEL_DISPLAY_DELAY ) {
    lastSatLevelDisplay = millis();
    satLevel.setSatelliteCount( nmeaParser.satelliteCount );
  }
#endif
}
