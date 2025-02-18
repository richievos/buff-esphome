#pragma once

#include <map>
#include <string>

// Buff Libraries
#include "ph-robotank-sensor.h"
#include "readings/ph.h"

// Other inputs
// const std::string wifiSSID;
// const std::string wifiPassword;
#include "../inputs/creds.h"

namespace buff {

namespace inputs {

/*******************************
 * Basics (don't need to change)
 *******************************/
const int MQTT_BROKER_PORT = 1883;
const float DEFAULT_TRIGGER_OUTPUT_ML = 3.0;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define BUFF_NAME TOSTRING(OPT_BUFF_NAME)

// hostname attempted to be used for wifi dhcp
const std::string hostname = BUFF_NAME;

/*******************************
 * PH Calibrations
 * TODO: fill in
 *******************************/

// I2C address of the PH reading board (from the circuit board's docs)
const std::map<unsigned char, unsigned char> boardIDToPort = {
    {5, 98},
    {6, 94},
    {7, 95},
    {8, 96},
    {10, 98},
};

// const auto roboTankPHSensorI2CAddress = 98l;
const unsigned char boardId = 10;
const auto roboTankPHSensorI2CAddress = boardIDToPort.at(boardId);
defineRoboTankSignalReaderFunc(roboTankPHSensorI2CAddress);

const ph::PHReadConfig phReadConfig = {
    // how often to read a new ph value
    .readIntervalMS = 1000,

    .phReadFunc = nameForRoboTankSignalReaderFunc(roboTankPHSensorI2CAddress)};


/*
  B10: BRS Sensor
*/
struct PHCalibs {
    ph::PHCalibrator::CalibrationPoint phHighPoint;
    ph::PHCalibrator::CalibrationPoint phLowPoint;
};
const PHCalibs PH_B10_BRS = {
    .phHighPoint = {.actualPH = 7.0, .readPH = 7.19},
    .phLowPoint = {.actualPH = 4.0, .readPH = 5.13}
};

const PHCalibs PH_BASE = {
    .phHighPoint = {.actualPH = 7.0, .readPH = 6.73},
    .phLowPoint = {.actualPH = 4.0, .readPH = 4.7}
};

const auto PH_SENSOR = PH_B10_BRS;

// calibrate the ph probe and enter in the settings here
// const ph::PHCalibrator::CalibrationPoint phHighPoint = {.actualPH = 10.0, .readPH = 9.53};
const ph::PHCalibrator::CalibrationPoint phHighPoint = PH_SENSOR.phHighPoint;
const ph::PHCalibrator::CalibrationPoint phLowPoint = PH_SENSOR.phLowPoint;


ph::PHCalibrator phCalibrator(phLowPoint, phHighPoint);

/*******************************
 * INPUTS Board config
 *******************************/
#ifdef BOARD_ESP32
const auto PIN_CONFIG = ESP32_CONFIG;
#elif BOARD_MKS_DLC32
const auto PIN_CONFIG = MKS_DLC32_CONFIG;
#endif

#include "inputs-dosers.h"

const std::map<MeasurementDoserType, std::shared_ptr<AccelStepper>> doserSteppers = {
    {MeasurementDoserType::FILL, std::make_shared<AccelStepper>(AccelStepper::DRIVER, PIN_CONFIG.FILL_WATER_STEP_PIN, PIN_CONFIG.FILL_WATER_DIR_PIN)},
    {MeasurementDoserType::REAGENT, std::make_shared<AccelStepper>(AccelStepper::DRIVER, PIN_CONFIG.REAGENT_STEP_PIN, PIN_CONFIG.REAGENT_DIR_PIN)},
    {MeasurementDoserType::DRAIN, std::make_shared<AccelStepper>(AccelStepper::DRIVER, PIN_CONFIG.DRAIN_WATER_STEP_PIN, PIN_CONFIG.DRAIN_WATER_DIR_PIN)},
};

std::map<MeasurementDoserType, std::shared_ptr<doser::Doser>> doserInstances = {
    {MeasurementDoserType::FILL, std::make_shared<doser::AccelStepperDoser>(fillDoserConfig, doserSteppers.at(MeasurementDoserType::FILL))},
    {MeasurementDoserType::REAGENT, std::make_shared<doser::AccelStepperDoser>(reagentDoserConfig, doserSteppers.at(MeasurementDoserType::REAGENT))},
    {MeasurementDoserType::DRAIN, std::make_shared<doser::AccelStepperDoser>(drainDoserConfig, doserSteppers.at(MeasurementDoserType::DRAIN))},
};

alk_measure::AlkMeasurementConfig alkMeasureConf = {
    .primeTankWaterFillVolumeML = 1.0,
    .primeReagentReverseVolumeML = -2.6,
    .primeReagentVolumeML = 2.9,

    .measurementTankWaterVolumeML = 200,

    // .measurementTankWaterVolumeML = 200,
    // .extraPurgeVolumeML = 50,

    // .initialReagentDoseVolumeML = 3.0,
    // .incrementalReagentDoseVolumeML = 0.1,

    // .stirAmountML = 3.0,
    // .stirTimes = 10,

    // .reagentStrengthMoles = 0.1,

    // Adjustment for the manual 0.1 HCL mix
    .calibrationMultiplier = 1.0};

}  // namespace inputs
}  // namespace buff
