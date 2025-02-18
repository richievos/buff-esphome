#include "buff_doser.h"

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace buff {

static const char *const TAG = "buff-doser";

static const std::string DOSING_MODE_NONE = "None";
static const std::string DOSING_MODE_VOLUME = "Volume";
static const std::string DOSING_MODE_VOLUME_OVER_TIME = "Volume/Time";
static const std::string DOSING_MODE_CONSTANT_FLOW_RATE = "Constant Flow Rate";
static const std::string DOSING_MODE_CONTINUOUS = "Continuous";

void BuffDoser::dump_config() {
    if (this->is_failed()) {
        ESP_LOGE(TAG, "Communication with Buff-Doser circuit failed!");
    }
    LOG_UPDATE_INTERVAL(this);
}

void BuffDoser::setup() {
    Serial.begin(115200);
    Wire.begin(inputs::PIN_CONFIG.I2C_SDA, inputs::PIN_CONFIG.I2C_SCL);

    _buffDosers = std::move(doser::setupDosers(inputs::PIN_CONFIG.STEPPER_DISABLE_PIN, inputs::doserInstances, inputs::doserSteppers));
    // TODO: make this configurable
    setupPH_RoboTankPHBoard();

    // trigger a NTP refresh
    _ntpClient = std::move(ntp::setupNTP());
    _timeClient = std::make_shared<ntp::NTPTimeWrapper>(_ntpClient);

    // controller::setupController(mqttBroker, mqttClient, buffDosers, phReader, inputs::alkMeasureConf, publisher, timeClient);
      // monitoring_display::setupDisplay(readingStore, publisher);


    // if (this->is_waiting_) {
    //   return;
    // }
}

void BuffDoser::update() {
    // if (this->is_waiting_) {
    //   return;
    // }
}

void BuffDoser::loop() {
    auto phReadingPtr = phReader->readNewPHSignalIfTimeAndUpdate<STANDARD_PH_MAVG_LENGTH>(phReadingStats);
    if (phReadingPtr != nullptr) {
        phReadingPtr->asOfAdjustedSec = timeClient->getAdjustedTimeSeconds();
        // publisher->publishPH(*phReadingPtr);
    }

    ntp::loopNTP(_ntpClient);

    // controller::loopController();
        // monitoring_display::loopDisplay();


    // TODO: this all is not thread safe
    auto current_command = this->current_command_;
    if (this->current_command_.command == Command::None) {
        if (!this->queue_.empty()) {
            current_command = this->queue_.front();
            this->queue_.pop();
            this->current_command_ = current_command;
        }
    }

    switch (current_command.command) {
        case Command::ClearCalibration:
            ESP_LOGI("Command", "Received ClearCalibration command");
            break;

        case Command::ClearTotalVolumeDosed:
            ESP_LOGI("Command", "Received ClearTotalVolumeDosed command");
            break;

        case Command::DoseContinuously:
            ESP_LOGI("Command", "Received DoseContinuously command");
            break;

        case Command::DoseVolume:
            ESP_LOGI("Command", "Received DoseVolume command");
            break;

        case Command::DoseVolumeOverTime:
            ESP_LOGI("Command", "Received DoseVolumeOverTime command");
            break;

        case Command::DoseWithConstantFlowRate:
            ESP_LOGI("Command", "Received DoseWithConstantFlowRate command");
            break;

        case Command::None:
            ESP_LOGD("Command", "Received None command");
            break;

        case Command::PauseDosing:
            ESP_LOGI("Command", "Received PauseDosing command");
            break;

        case Command::ReadAbsoluteTotalVolumeDosed:
            ESP_LOGI("Command", "Received ReadAbsoluteTotalVolumeDosed command");
            break;

        case Command::ReadCalibrationStatus:
            ESP_LOGI("Command", "Received ReadCalibrationStatus command");
            break;

        case Command::ReadDosing:
            ESP_LOGI("Command", "Received ReadDosing command");

            // TODO: this logic is weird
            // if (parsed_third_parameter.has_value())
            //   this->is_dosing_flag_ = parsed_third_parameter.value_or(0) == 1;
#ifdef USE_BINARY_SENSOR
            if (this->is_dosing_)
                this->is_dosing_->publish_state(this->is_dosing_flag_);
#endif
#ifdef USE_SENSOR
            // TODO:
            float last_volume_requested = 0.0f;
            this->last_volume_requested_->publish_state(last_volume_requested);
#endif
#ifdef USE_TEXT_SENSOR
            if (!this->is_dosing_flag_ && !this->is_paused_flag_) {
                // If pump is not paused and not dispensing
                if (this->dosing_mode_ && this->dosing_mode_->state != DOSING_MODE_NONE)
                    this->dosing_mode_->publish_state(DOSING_MODE_NONE);
            }
#endif
            break;

        case Command::ReadMaxFlowRate:
            ESP_LOGI("Command", "Received ReadMaxFlowRate command");
            break;

        case Command::ReadPauseStatus:
            ESP_LOGI("Command", "Received ReadPauseStatus command");
            break;

        case Command::ReadPumpVoltage:
            ESP_LOGI("Command", "Received ReadPumpVoltage command");
            break;

        case Command::ReadTotalVolumeDosed:
            ESP_LOGI("Command", "Received ReadTotalVolumeDosed command");
            break;

        case Command::SetCalibrationVolume:
            ESP_LOGI("Command", "Received SetCalibrationVolume command");
            break;

        case Command::StopDosing:
            ESP_LOGI("Command", "Received StopDosing command");
            break;

        default:
            ESP_LOGE("Command", "Unknown command received");
            break;
    }

    // this->is_waiting_ = true;
    // this->start_time_ = millis();
    // this->wait_time_ = wait_time_for_command;
}

// Actions
void BuffDoser::dose_continuously() {
    this->queue_.push({Command::DoseContinuously, 0, 0});
    this->queue_.push({Command::ReadDosing});
}

void BuffDoser::dose_volume(double volume) {
    this->queue_.push({Command::DoseVolume, volume, 0});
    this->queue_.push({Command::ReadDosing});
}

void BuffDoser::dose_volume_over_time(double volume, int duration) {
    this->queue_.push({Command::DoseVolumeOverTime, volume, duration});
    this->queue_.push({Command::ReadDosing});
}

void BuffDoser::dose_with_constant_flow_rate(double volume, int duration) {
    this->queue_.push({Command::DoseWithConstantFlowRate, volume, duration});
    this->queue_.push({Command::ReadDosing});
}

void BuffDoser::set_calibration_volume(double volume) {
    this->queue_.push({Command::SetCalibrationVolume, volume, 0});
    this->queue_.push({Command::ReadCalibrationStatus});
    this->queue_.push({Command::ReadMaxFlowRate});
}

void BuffDoser::clear_total_volume_dosed() {
    this->queue_.push({Command::ClearTotalVolumeDosed});
    this->queue_.push({Command::ReadTotalVolumeDosed});
    this->queue_.push({Command::ReadAbsoluteTotalVolumeDosed});
}

void BuffDoser::clear_calibration() {
    this->queue_.push({Command::ClearCalibration});
    this->queue_.push({Command::ReadCalibrationStatus});
    this->queue_.push({Command::ReadMaxFlowRate});
}

void BuffDoser::pause_dosing() {
    this->queue_.push({Command::PauseDosing});
    this->queue_.push({Command::ReadPauseStatus});
}

void BuffDoser::stop_dosing() {
    this->queue_.push({Command::StopDosing});
}

}  // namespace buff
}  // namespace esphome
