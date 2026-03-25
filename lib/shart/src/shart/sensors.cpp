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

  if (!lsm.begin_I2C(LSM_I2C_ADDR, &LSM_I2C_BUS)) {
    UPDATE_STATUS(ICMStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
    ERROR("LSM initialization failed!", MAIN_SERIAL_PORT)
    return;
  }

  lsm.setAccelRange(LSM6DSO32_ACCEL_RANGE_32_G);
  lsm.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
  lsm.setAccelDataRate(LSM6DS_RATE_208_HZ);
  lsm.setGyroDataRate(LSM6DS_RATE_208_HZ);

  UPDATE_STATUS(LSMStatus, AVAILABLE, MAIN_SERIAL_PORT)
}

// void Shart::initICM20948() {

//   icm20948_instance = 0;
  
//   if (!icm.init()) {
//     UPDATE_STATUS(ICMStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
//     ERROR("ICM initialization failed!", MAIN_SERIAL_PORT)
//     return;
//   }
//   UPDATE_STATUS(ICMStatus, AVAILABLE, MAIN_SERIAL_PORT)

// }
void Shart::initLIS3MDL() {
  if (!lis.begin_I2C(LIS_CS, &LIS_I2C_BUS)) {          // hardware I2C mode, can pass in address & alt Wire
  //if (! lis3mdl.begin_SPI(LIS3MDL_CS)) {  // hardware SPI mode
  //if (! lis3mdl.begin_SPI(LIS3MDL_CS, LIS3MDL_CLK, LIS3MDL_MISO, LIS3MDL_MOSI)) { // soft SPI
    UPDATE_STATUS(BMPStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
    ERROR("BMP initialization failed!", MAIN_SERIAL_PORT)
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
  UPDATE_STATUS(BMPStatus, AVAILABLE, MAIN_SERIAL_PORT)
}

// Initialize the BMP SPI connection
// TODO: consider power cycling to avoid issues with clock synchronization
void Shart::initBMP388() {

  // Initialize in hardware SPI mode
  //delay(100);
  //if (!bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {
  if (!bmp.begin_SPI(BMP_CS, &BMP_SPI_BUS)) {
    UPDATE_STATUS(BMPStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
    ERROR("BMP initialization failed!", MAIN_SERIAL_PORT)
    return;
  }

  // Turn off oversampling, instead just take data at 200Hz (we can process noise later)
  //bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  //bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_200_HZ);

  UPDATE_STATUS(BMPStatus, AVAILABLE, MAIN_SERIAL_PORT)

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

  if (!bmi_accel.begin() || !bmi_gyro.begin()) {
    UPDATE_STATUS(BMIStatus, UNINITIALIZED, MAIN_SERIAL_PORT);
    ERROR("BMI initialization failed!", MAIN_SERIAL_PORT);
    return;
  }

  UPDATE_STATUS(BMIStatus, AVAILABLE, MAIN_SERIAL_PORT);
}

/*******************************************************************************
* Status checkers
*
*   Before collecting data, check and update the status of sensors. Depending on
*   the determined status, init or collect functions may be flagged.
*
*******************************************************************************/

void Shart::updateStatusBMP388() {

  // The chipID() function has been modified to ACTUALLY read the chip_id register
  if (bmp.chipID() != BMP_CHIP_ID) {
    UPDATE_STATUS(BMPStatus, UNAVAILABLE, MAIN_SERIAL_PORT)
    ERROR("BMP not found!", MAIN_SERIAL_PORT)
    return;
  }

  UPDATE_STATUS(BMPStatus, AVAILABLE, MAIN_SERIAL_PORT);
}

// See if icm is connected (under the covers, checks device id)
void Shart::updateStatusLIS3MDL() {
  
  // if (!lis.connected()) {
  //   UPDATE_STATUS(ICMStatus, UNINITIALIZED, MAIN_SERIAL_PORT)
  //   ERROR("ICM reading failed!", MAIN_SERIAL_PORT)
  //   return;
  // }

  UPDATE_STATUS(ICMStatus, AVAILABLE, MAIN_SERIAL_PORT)
  
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

// // collect data from the ICM w/ modified ZaneL's library
// void Shart::collectDataICM20948() {

//   // float gyro_x, gyro_y, gyro_z;
//   // float accel_x, accel_y, accel_z;
//   float mag_x, mag_y, mag_z;

//   // we don't wait for mag here because it only comes at 70Hz (we can oversample)
//   //while (!icm.accelDataIsReady()||!icm.gyroDataIsReady())//||!icm.magDataIsReady())
//   icm.task();

//   // icm.readGyroData(&gyro_x, &gyro_y, &gyro_z);
//   // icm.readAccelData(&accel_x, &accel_y, &accel_z);
//   icm.readMagData(&mag_x, &mag_y, &mag_z);
 
//   // sensor_packet.data.acc_x = accel_x;
//   // sensor_packet.data.acc_y = accel_y;
//   // sensor_packet.data.acc_z = accel_z;
//   // sensor_packet.data.gyr_x = gyro_x;
//   // sensor_packet.data.gyr_y = gyro_y;
//   // sensor_packet.data.gyr_z = gyro_z;
//   sensor_packet.data.mag_x = mag_x;
//   sensor_packet.data.mag_y = mag_y;
//   sensor_packet.data.mag_z = mag_z;
  
// }
void Shart::collectDataLIS3MDL() {
  lis.read();
  sensors_event_t event; 
  lis.getEvent(&event);
  sensor_packet.data.mag_x = event.magnetic.x;
  sensor_packet.data.mag_y = event.magnetic.y;
  sensor_packet.data.mag_z = event.magnetic.z;
}

// collect data from the BMP388 over SPI
// VERY IMPORTANT: "the_sensor.settings.op_mode = BMP3_MODE_NORMAL" in begin_SPI in Adafruit_BMP3XX.cpp or else very slow
void Shart::collectDataBMP388() {

  // take temperature and pressure, ignore altitude estimate to avoid expensive calculations
  bmp.performReading();
  sensor_packet.data.temp = bmp.temperature; // in *C
  sensor_packet.data.pres = bmp.pressure; // in HPa

}

void Shart::collectDataBMI088() {

  bmi_accel.readSensor();
  sensor_packet.data.acc_x = bmi_accel.getAccelX_mss();
  sensor_packet.data.acc_y = bmi_accel.getAccelY_mss();
  sensor_packet.data.acc_z = bmi_accel.getAccelZ_mss();

  bmi_gyro.readSensor();
  sensor_packet.data.gyr_x = bmi_gyro.getGyroX_rads();
  sensor_packet.data.gyr_y = bmi_gyro.getGyroY_rads();
  sensor_packet.data.gyr_z = bmi_gyro.getGyroZ_rads(); 

}