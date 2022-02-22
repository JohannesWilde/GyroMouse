/**
 * This sketch shall read in data from an MPU-6500 and command a PC mouse
 * accordingly [from an Arduino UNO].
 */

// For development purposes only - this hard-codes the Arduino UNO MCU, as my IDE
// somehow fails to detect this automatically via CMake.
#define __AVR_ATmega328P__

#include <Arduino.h>
#include <Wire.h>

#include <MPU6500_WE.h>
int constexpr mpu6500Address = 0x68;
MPU6500_WE motionProcessingUnit = MPU6500_WE(mpu6500Address);


void setup()
{
    Serial.begin(115200);
    Wire.begin();
    if (!motionProcessingUnit.init())
    {
        Serial.println("MPU6500 does not respond");
    }
    else
    {
        Serial.println("MPU6500 is connected");
    }

    /* The slope of the curve of acceleration vs measured values fits quite well to the theoretical
     * values, e.g. 16384 units/g in the +/- 2g range. But the starting point, if you position the
     * MPU6500 flat, is not necessarily 0g/0g/1g for x/y/z. The autoOffset function measures offset
     * values. It assumes your MPU6500 is positioned flat with its x,y-plane. The more you deviate
     * from this, the less accurate will be your results.
     * The function also measures the offset of the gyroscope data. The gyroscope offset does not
     * depend on the positioning.
     * This function needs to be called at the beginning since it can overwrite your settings!
     */
    Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
    delay(1000);
    motionProcessingUnit.autoOffsets();
    Serial.println("Done!");

    /*  This is a more accurate method for calibration. You have to determine the minimum and maximum
     *  raw acceleration values of the axes determined in the range +/- 2 g.
     *  You call the function as follows: setAccOffsets(xMin,xMax,yMin,yMax,zMin,zMax);
     *  Use either autoOffset or setAccOffsets, not both.
     */
    //motionProcessingUnit.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

    /*  The gyroscope data is not zero, even if you don't move the MPU6500.
     *  To start at zero, you can apply offset values. These are the gyroscope raw values you obtain
     *  using the +/- 250 degrees/s range.
     *  Use either autoOffset or setGyrOffsets, not both.
     */
    //motionProcessingUnit.setGyrOffsets(45.0, 145.0, -105.0);

    /*  You can enable or disable the digital low pass filter (DLPF). If you disable the DLPF, you
     *  need to select the bandwdith, which can be either 8800 or 3600 Hz. 8800 Hz has a shorter delay,
     *  but higher noise level. If DLPF is disabled, the output rate is 32 kHz.
     *  MPU9250_BW_WO_DLPF_3600
     *  MPU9250_BW_WO_DLPF_8800
     */
    motionProcessingUnit.enableGyrDLPF();
    //motionProcessingUnit.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800); // bandwdith without DLPF

    /*  Digital Low Pass Filter for the gyroscope must be enabled to choose the level.
     *  MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7
     *
     *  DLPF    Bandwidth [Hz]   Delay [ms]   Output Rate [kHz]
     *    0         250            0.97             8
     *    1         184            2.9              1
     *    2          92            3.9              1
     *    3          41            5.9              1
     *    4          20            9.9              1
     *    5          10           17.85             1
     *    6           5           33.48             1
     *    7        3600            0.17             8
     *
     *    You achieve lowest noise using level 6
     */
    motionProcessingUnit.setGyrDLPF(MPU9250_DLPF_6);

    /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
     *  Sample rate = Internal sample rate / (1 + divider)
     *  It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
     *  Divider is a number 0...255
     */
    motionProcessingUnit.setSampleRateDivider(5);

    /*  MPU9250_GYRO_RANGE_250       250 degrees per second (default)
     *  MPU9250_GYRO_RANGE_500       500 degrees per second
     *  MPU9250_GYRO_RANGE_1000     1000 degrees per second
     *  MPU9250_GYRO_RANGE_2000     2000 degrees per second
     */
    motionProcessingUnit.setGyrRange(MPU9250_GYRO_RANGE_250);

    /*  MPU9250_ACC_RANGE_2G      2 g   (default)
     *  MPU9250_ACC_RANGE_4G      4 g
     *  MPU9250_ACC_RANGE_8G      8 g
     *  MPU9250_ACC_RANGE_16G    16 g
     */
    motionProcessingUnit.setAccRange(MPU9250_ACC_RANGE_2G);

    /*  Enable/disable the digital low pass filter for the accelerometer
     *  If disabled the bandwidth is 1.13 kHz, delay is 0.75 ms, output rate is 4 kHz
     */
    motionProcessingUnit.enableAccDLPF(true);

    /*  Digital low pass filter (DLPF) for the accelerometer, if enabled
     *  MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7
     *   DLPF     Bandwidth [Hz]      Delay [ms]    Output rate [kHz]
     *     0           460               1.94           1
     *     1           184               5.80           1
     *     2            92               7.80           1
     *     3            41              11.80           1
     *     4            20              19.80           1
     *     5            10              35.70           1
     *     6             5              66.96           1
     *     7           460               1.94           1
     */
    motionProcessingUnit.setAccDLPF(MPU9250_DLPF_6);

    /* You can enable or disable the axes for gyroscope and/or accelerometer measurements.
     * By default all axes are enabled. Parameters are:
     * MPU9250_ENABLE_XYZ  //all axes are enabled (default)
     * MPU9250_ENABLE_XY0  // X, Y enabled, Z disabled
     * MPU9250_ENABLE_X0Z
     * MPU9250_ENABLE_X00
     * MPU9250_ENABLE_0YZ
     * MPU9250_ENABLE_0Y0
     * MPU9250_ENABLE_00Z
     * MPU9250_ENABLE_000  // all axes disabled
     */
    //motionProcessingUnit.enableAccAxes(MPU9250_ENABLE_XYZ);
    //motionProcessingUnit.enableGyrAxes(MPU9250_ENABLE_XYZ);

    while (true)
    {
        xyzFloat gValue = motionProcessingUnit.getGValues();
        xyzFloat gyr = motionProcessingUnit.getGyrValues();
        float temp = motionProcessingUnit.getTemperature();
        float resultantG = motionProcessingUnit.getResultantG(gValue);

        Serial.println("Acceleration in g (x,y,z):");
        Serial.print(gValue.x);
        Serial.print("   ");
        Serial.print(gValue.y);
        Serial.print("   ");
        Serial.println(gValue.z);
        Serial.print("Resultant g: ");
        Serial.println(resultantG);

        Serial.println("Gyroscope data in degrees/s: ");
        Serial.print(gyr.x);
        Serial.print("   ");
        Serial.print(gyr.y);
        Serial.print("   ");
        Serial.println(gyr.z);

        Serial.print("Temperature in °C: ");
        Serial.println(temp);

        Serial.println("********************************************");

        delay(1000);
    }
}



void loop()
{
    // intentionally empty
}
