#include <BMI088.h>
#include <Wire.h>

#define I2C1_SDA 60
#define I2C1_SCL 59
TwoWire i2c1 = TwoWire(I2C1_SDA, I2C1_SCL);

#define BMI_I2C_BUS  i2c1

#define BMI_ACC_I2C_ADDR 0x18
#define BMI_GYR_I2C_ADDR 0x68

Bmi088Accel bmi_accel  = Bmi088Accel(BMI_I2C_BUS, BMI_ACC_I2C_ADDR);
Bmi088Gyro bmi_gyro   = Bmi088Gyro(BMI_I2C_BUS, BMI_GYR_I2C_ADDR);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("BMI088 gyro and accel test");

    if (bmi_accel.begin() == -1) {
        Serial.println("Could not find valid BMI088 accel sensor.");
        while(1);
    }
    if (bmi_gyro.begin() == -1) {
        Serial.println("Could not find valid BMI088 gyro sensor.");
        while(1);
    }
    
    Serial.println("Both sensors found.");
    
    //bmi_accel.setOdr(bmi_accel.ODR_200HZ_BW_80HZ);
    //bmi_gyro.setOdr(bmi_gyro.ODR_200HZ_BW_64HZ);
}

void loop() {
    Serial.println("Reading accel data...");

    if (!bmi_accel.getDrdyStatus()) {
            Serial.println("Failed to read accel data.");
            while(1);
    }
    
    bmi_accel.readSensor();

    Serial.print("Accel_x: ");
    Serial.println(bmi_accel.getAccelX_mss());
    Serial.print("Accel_y: ");
    Serial.println(bmi_accel.getAccelY_mss());
    Serial.print("Accel_z: ");
    Serial.println(bmi_accel.getAccelZ_mss());
    Serial.println();

    Serial.println("Reading gyro data...");

    if (!bmi_gyro.getDrdyStatus()) {
        Serial.println("Failed to read gyro data.");
        while(1);
    }
    
    bmi_gyro.readSensor();

    Serial.print("Gyro_x: ");
    Serial.println(bmi_gyro.getGyroX_rads());
    Serial.print("Gyro_y: ");
    Serial.println(bmi_gyro.getGyroY_rads());
    Serial.print("Gyro_z: ");
    Serial.println(bmi_gyro.getGyroZ_rads());
    Serial.println();
    
    delay(2000);
}