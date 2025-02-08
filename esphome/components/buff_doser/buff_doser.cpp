#include "buff_doser.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

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

void BuffDoser::update() {
  if (this->is_waiting_) {
    return;
  }
}

void BuffDoser::loop() {
  // TODO: this all is not thread safe
  Command current_command = this->current_command_;
  if (this->current_command_.command == Command::None) {
    if (!this->queue->empty()) {
      current_command = this->queue->pop();
      this->current_command_ = current_command;
    }
  }

  switch (current_command) {
    case Command::ChangeI2CAddress:
      ESP_LOGI("Command", "Received ChangeI2CAddress command");
      break;

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

    case Command::ExecArbitraryCommandAddress:
      ESP_LOGI("Command", "Received ExecArbitraryCommandAddress command");
      break;

    case Command::Find:
      ESP_LOGI("Command", "Received Find command");
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

    case Command::ReadSingleReport:
      ESP_LOGI("Command", "Received ReadSingleReport command");
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

    case Command::TypeRead:
      ESP_LOGI("Command", "Received TypeRead command");
      break;

    default:
      ESP_LOGE("Command", "Unknown command received");
      break;
  }

  // this->is_waiting_ = true;
  // this->start_time_ = millis();
  // this->wait_time_ = wait_time_for_command;
}

void BuffDoser::queue_command_(Command command, double volume, int duration) {
  this->queue->push(command);
}

// Actions

void BuffDoser::find() { this->queue_command_(BUFF_DOSER_COMMAND_FIND); }

void BuffDoser::dose_continuously() {
  this->queue_command_(BUFF_DOSER_COMMAND_DOSE_CONTINUOUSLY, 0, 0);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_DOSING);
}

void BuffDoser::dose_volume(double volume) {
  this->queue_command_(BUFF_DOSER_COMMAND_DOSE_VOLUME, volume, 0);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_DOSING);
}

void BuffDoser::dose_volume_over_time(double volume, int duration) {
  this->queue_command_(BUFF_DOSER_COMMAND_DOSE_VOLUME_OVER_TIME, volume, duration);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_DOSING);
}

void BuffDoser::dose_with_constant_flow_rate(double volume, int duration) {
  this->queue_command_(BUFF_DOSER_COMMAND_DOSE_WITH_CONSTANT_FLOW_RATE, volume, duration);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_DOSING);
}

void BuffDoser::set_calibration_volume(double volume) {
  this->queue_command_(BUFF_DOSER_COMMAND_SET_CALIBRATION_VOLUME, volume, 0);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_CALIBRATION_STATUS);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_MAX_FLOW_RATE);
}

void BuffDoser::clear_total_volume_dosed() {
  this->queue_command_(BUFF_DOSER_COMMAND_CLEAR_TOTAL_VOLUME_DOSED);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_SINGLE_REPORT);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_TOTAL_VOLUME_DOSED);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_ABSOLUTE_TOTAL_VOLUME_DOSED);
}

void BuffDoser::clear_calibration() {
  this->queue_command_(BUFF_DOSER_COMMAND_CLEAR_CALIBRATION);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_CALIBRATION_STATUS);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_MAX_FLOW_RATE);
}

void BuffDoser::pause_dosing() {
  this->queue_command_(BUFF_DOSER_COMMAND_PAUSE_DOSING);
  this->queue_command_(BUFF_DOSER_COMMAND_READ_PAUSE_STATUS);
}

void BuffDoser::stop_dosing() { this->queue_command_(BUFF_DOSER_COMMAND_STOP_DOSING); }

}  // namespace buff
}  // namespace esphome
