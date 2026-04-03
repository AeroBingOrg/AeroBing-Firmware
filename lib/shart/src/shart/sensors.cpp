/*******************************************************************************
* File Name: sensors.cpp
*
* Description:
*   Implementation for all sensor-specific methods
*
* Author: Andrew Shen-Costello
*
* Spring, 2024
*
* TODO: Consider switch to LSM6DSO32 (6DoF) + a magnetometer instead of a 9DoF chip
* LSM6DSO32 must be set to high-performance mode
* Another option: LSM9DS1 9DoF, mag only 100Hz, separate SPI chip select pins for
* acc/gyr, mag
* RM3100 magnetometer? kinda sketch but ppl us it
* bought this shit asap $60 baby EMPTY WALLET WOOO, we still need a got 6DoF
* 
*
* Version:  1.0.0
*
*******************************************************************************/

#include "shart.h"

/*******************************************************************************
* Initializers
*
*   Initialize and configure sensors
*
*******************************************************************************/


//
void Shart::initLSM6DSO32() {

  if (!lsm.begin_SPI(LSM_CS, &LSM_SPI_BUS)) {
    UPDATE_STATUS(LSMStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
    ERROR("LSM initialization failed!", MAIN_SERIAL_PORT)
    return;
  }

  lsm.setAccelRange(LSM6DSO32_ACCEL_RANGE_32_G);
  lsm.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
  lsm.setAccelDataRate(LSM6DS_RATE_208_HZ);
  lsm.setGyroDataRate(LSM6DS_RATE_208_HZ);

  UPDATE_STATUS(LSMStatus, AVAILABLE, MAIN_SERIAL_PORT)

}

void Shart::initLIS3MDL() {
  if (!lis.begin_SPI(LIS_CS, &LIS_SPI_BUS)) {          // hardware I2C mode, can pass in address & alt Wire
    UPDATE_STATUS(LISStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
    ERROR("LIS Initialization failed!", MAIN_SERIAL_PORT)
  }
  lis.setPerformanceMode(LIS3MDL_MEDIUMMODE);
  lis.setOperationMode(LIS3MDL_CONTINUOUSMODE);
  lis.setDataRate(LIS3MDL_DATARATE_155_HZ);
  lis.setRange(LIS3MDL_RANGE_4_GAUSS);
  lis.setIntThreshold(500);
  lis.configInterrupt(false, false, true, // enable z axis
                          true, // polarity
                          false, // don't latch
                          true); // enabled!
  UPDATE_STATUS(LISStatus, AVAILABLE, MAIN_SERIAL_PORT)
}

// ADXL375 range is fixed at +/-200G
// TODO: maybe try power cycling here: clock synchonization gets messed up sometimes if SCL gets unplugged
void Shart::initADXL375() {

  if (!adxl.begin()) {
    UPDATE_STATUS(ADXLStatus, UNINITIALIZED, MAIN_SERIAL_PORT);
    ERROR("ADXL initialization failed!", MAIN_SERIAL_PORT)
    return;
  }

  UPDATE_STATUS(ADXLStatus, AVAILABLE, MAIN_SERIAL_PORT)

}

void Shart::initBMI088() {

  if (bmi_accel.begin() == -1 || bmi_gyro.begin() == -1) {
    UPDATE_STATUS(BMIStatus, UNINITIALIZED, MAIN_SERIAL_PORT);
    ERROR("BMI initialization failed!", MAIN_SERIAL_PORT);
    return;
  }

  UPDATE_STATUS(BMIStatus, AVAILABLE, MAIN_SERIAL_PORT);

}

void Shart::initMS5611() {

  if (!ms5.begin()) {
    UPDATE_STATUS(MSStatus, UNINITIALIZED, MAIN_SERIAL_PORT);
    ERROR("MS initialization failed!", MAIN_SERIAL_PORT);
    return;
  }

  UPDATE_STATUS(MSStatus, AVAILABLE, MAIN_SERIAL_PORT);

}
/*******************************************************************************
* Status checkers
*
*   Before collecting data, check and update the status of sensors. Depending on
*   the determined status, init or collect functions may be flagged.
*
*******************************************************************************/

// See if icm is connected (under the covers, checks device id)
void Shart::updateStatusLIS3MDL() {
  
  // if (!lis.connected()) {
  //   UPDATE_STATUS(ICMStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
  //   ERROR("ICM reading failed!", MAIN_SERIAL_PORT)
  //   return;
  // }

  UPDATE_STATUS(LISStatus, AVAILABLE, MAIN_SERIAL_PORT)
  
}

// Check if device id matches expected
void Shart::updateStatusADXL375() {

  if (adxl.getDeviceID() != ADXL_CHIP_ID) {
    UPDATE_STATUS(ADXLStatus, UNAVAILABLE, MAIN_SERIAL_PORT);
    ERROR("ADXL not found!", MAIN_SERIAL_PORT)
    return;
  }

  UPDATE_STATUS(ADXLStatus, AVAILABLE, MAIN_SERIAL_PORT)

}

void Shart::updateStatusLSM6DSO32() {

  if (lsm.chipID() != LSM_CHIP_ID) {
    UPDATE_STATUS(LSMStatus, UNAVAILABLE, MAIN_SERIAL_PORT);
    ERROR("LSM not found!", MAIN_SERIAL_PORT)
    return;
  }

  UPDATE_STATUS(LSMStatus, AVAILABLE, MAIN_SERIAL_PORT)
}

void Shart::updateStatusBMI088() {

  if (bmi_accel.begin() == -1 || bmi_gyro.begin() == -1) {
      UPDATE_STATUS(BMIStatus, UNAVAILABLE, MAIN_SERIAL_PORT);
      ERROR("BMI not found!", MAIN_SERIAL_PORT);
      return;
  }

  UPDATE_STATUS(BMIStatus, AVAILABLE, MAIN_SERIAL_PORT);
}

void Shart::updateStatusMS5611() {
  
  if (ms5.getDeviceID() != MS5_DEVICE_ID) {
    UPDATE_STATUS(MSStatus, UNAVAILABLE, MAIN_SERIAL_PORT);
    ERROR("MS not found!", MAIN_SERIAL_PORT);
    return;
  }

  UPDATE_STATUS(MSStatus, AVAILABLE, MAIN_SERIAL_PORT);
  
}
/*******************************************************************************
* Collectors
*
*   This is the meat of the project, but really the simplest part. Most of this
*   code is copied from somewhere else or taken from a library.
*   The increments for index calculations were a choice, it was very inconvenient
*   to change every entry every time something needed to be changed. Very minimal
*   performance tradeoff.
*
*******************************************************************************/

// collect data from the ADXL375, 49mG per LSB so multiply by 49/1000 = 0.049 for units in G
void Shart::collectDataADXL375() {

  // Collect raw data from axis registers
  int16_t x, y, z;
  adxl.getXYZ(x, y, z);
  
  sensor_packet.data.adxl_acc_x = x;
  sensor_packet.data.adxl_acc_y = y;
  sensor_packet.data.adxl_acc_z = z;

}

//lsm data collection
void Shart::collectDataLSM6DSO32(){
  
  lsm.getRaw();

  sensor_packet.data.acc_x = lsm.rawAccX;
  sensor_packet.data.acc_y = lsm.rawAccY;
  sensor_packet.data.acc_z = lsm.rawAccZ;
  sensor_packet.data.gyr_x = lsm.rawGyroX;
  sensor_packet.data.gyr_y = lsm.rawGyroY;
  sensor_packet.data.gyr_z = lsm.rawGyroZ;
  //sensor_packet.data.temp_lsm  = temp.temperature;
  
}

void Shart::collectDataLIS3MDL() {
  lis.read();
  sensors_event_t event; 
  lis.getEvent(&event);
  sensor_packet.data.mag_x = event.magnetic.x;
  sensor_packet.data.mag_y = event.magnetic.y;
  sensor_packet.data.mag_z = event.magnetic.z;
}

void Shart::collectDataBMI088() {

  bmi_accel.readSensor();
  // sensor_packet.data.acc_x = bmi_accel.getAccelX_mss();
  // sensor_packet.data.acc_y = bmi_accel.getAccelY_mss();
  // sensor_packet.data.acc_z = bmi_accel.getAccelZ_mss();

  bmi_gyro.readSensor();
  // sensor_packet.data.gyr_x = bmi_gyro.getGyroX_rads();
  // sensor_packet.data.gyr_y = bmi_gyro.getGyroY_rads();
  // sensor_packet.data.gyr_z = bmi_gyro.getGyroZ_rads(); 

}

void Shart::collectDataMS5611() {

  ms5.read();
  sensor_packet.data.temp = ms5.getTemperature();
  sensor_packet.data.pres = ms5.getPressurePascal(); //in Pa
  
}