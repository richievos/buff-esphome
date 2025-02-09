#pragma once

#include <queue>

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

// #ifdef BOARD_MKS_DLC32
#include <map>
#include <memory>
// Arduino Libraries
#include <ArduinoJson.h>
// #include <TinyMqtt.h>
// #include <nvs_flash.h>

#include "doser/doser.h"
// #include "inputs.h"

#include "mks-bridge.h"
// #endif


namespace esphome {
namespace buff {

enum class Command {
    ClearCalibration,
    ClearTotalVolumeDosed,
    DoseContinuously,
    DoseVolume,
    DoseVolumeOverTime,
    DoseWithConstantFlowRate,
    None,
    PauseDosing,
    ReadAbsoluteTotalVolumeDosed,
    ReadCalibrationStatus,
    ReadDosing,
    ReadMaxFlowRate,
    ReadPauseStatus,
    ReadPumpVoltage,
    ReadTotalVolumeDosed,
    SetCalibrationVolume,
    StopDosing,
    TypeRead
};

struct QueueableCommand {
    Command command;
    double volume = -1;
    int duration = -1;
};

class BuffDoser : public PollingComponent {
   public:
    void dump_config() override;
    float get_setup_priority() const override { return setup_priority::DATA; };

    void loop() override;
    void update() override;

#ifdef USE_SENSOR
    void set_current_volume_dosed(sensor::Sensor *current_volume_dosed) { current_volume_dosed_ = current_volume_dosed; }
    void set_total_volume_dosed(sensor::Sensor *total_volume_dosed) { total_volume_dosed_ = total_volume_dosed; }
    void set_absolute_total_volume_dosed(sensor::Sensor *absolute_total_volume_dosed) {
        absolute_total_volume_dosed_ = absolute_total_volume_dosed;
    }
    void set_pump_voltage(sensor::Sensor *pump_voltage) { pump_voltage_ = pump_voltage; }
    void set_last_volume_requested(sensor::Sensor *last_volume_requested) {
        last_volume_requested_ = last_volume_requested;
    }
    void set_max_flow_rate(sensor::Sensor *max_flow_rate) { max_flow_rate_ = max_flow_rate; }
#endif

#ifdef USE_BINARY_SENSOR
    void set_is_dosing(binary_sensor::BinarySensor *is_dosing) { is_dosing_ = is_dosing; }
    void set_is_paused(binary_sensor::BinarySensor *is_paused) { is_paused_ = is_paused; }
#endif

#ifdef USE_TEXT_SENSOR
    void set_dosing_mode(text_sensor::TextSensor *dosing_mode) { dosing_mode_ = dosing_mode; }
    void set_calibration_status(text_sensor::TextSensor *calibration_status) { calibration_status_ = calibration_status; }
#endif

    // Actions for BuffDoser
    void dose_continuously();
    void dose_volume(double volume);
    void dose_volume_over_time(double volume, int duration);
    void dose_with_constant_flow_rate(double volume, int duration);
    void set_calibration_volume(double volume);
    void clear_total_volume_dosed();
    void clear_calibration();
    void pause_dosing();
    void stop_dosing();

   protected:
    std::queue<QueueableCommand> queue_;
    QueueableCommand current_command_ = {Command::None};

    // uint32_t start_time_ = 0;
    // uint32_t wait_time_ = 0;
    // bool is_waiting_ = false;
    // bool is_first_read_ = true;

    bool is_paused_flag_ = false;
    bool is_dosing_flag_ = false;

    std::shared_ptr<doser::BuffDosers> buffDosersPtr = nullptr;
    // std::shared_ptr<buff_time::TimeWrapper> timeClient = nullptr;

#ifdef USE_SENSOR
    sensor::Sensor *current_volume_dosed_{nullptr};
    sensor::Sensor *total_volume_dosed_{nullptr};
    sensor::Sensor *absolute_total_volume_dosed_{nullptr};
    sensor::Sensor *pump_voltage_{nullptr};
    sensor::Sensor *max_flow_rate_{nullptr};
    sensor::Sensor *last_volume_requested_{nullptr};
#endif

#ifdef USE_BINARY_SENSOR
    binary_sensor::BinarySensor *is_dosing_{nullptr};
    binary_sensor::BinarySensor *is_paused_{nullptr};
#endif

#ifdef USE_TEXT_SENSOR
    text_sensor::TextSensor *dosing_mode_{nullptr};
    text_sensor::TextSensor *calibration_status_{nullptr};
#endif
};

// Action Templates
template <typename... Ts>
class BuffDoserDoseContinuouslyAction : public Action<Ts...> {
   public:
    BuffDoserDoseContinuouslyAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->dose_continuously(); }

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserDoseVolumeAction : public Action<Ts...> {
   public:
    BuffDoserDoseVolumeAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->dose_volume(this->volume_.value(x...)); }
    TEMPLATABLE_VALUE(double, volume)

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserDoseVolumeOverTimeAction : public Action<Ts...> {
   public:
    BuffDoserDoseVolumeOverTimeAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override {
        this->buff_doser_->dose_volume_over_time(this->volume_.value(x...), this->duration_.value(x...));
    }
    TEMPLATABLE_VALUE(double, volume)
    TEMPLATABLE_VALUE(int, duration)

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserDoseWithConstantFlowRateAction : public Action<Ts...> {
   public:
    BuffDoserDoseWithConstantFlowRateAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override {
        this->buff_doser_->dose_with_constant_flow_rate(this->volume_.value(x...), this->duration_.value(x...));
    }
    TEMPLATABLE_VALUE(double, volume)
    TEMPLATABLE_VALUE(int, duration)

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserSetCalibrationVolumeAction : public Action<Ts...> {
   public:
    BuffDoserSetCalibrationVolumeAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->set_calibration_volume(this->volume_.value(x...)); }
    TEMPLATABLE_VALUE(double, volume)

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserClearTotalVolumeDispensedAction : public Action<Ts...> {
   public:
    BuffDoserClearTotalVolumeDispensedAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->clear_total_volume_dosed(); }

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserClearCalibrationAction : public Action<Ts...> {
   public:
    BuffDoserClearCalibrationAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->clear_calibration(); }

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserPauseDosingAction : public Action<Ts...> {
   public:
    BuffDoserPauseDosingAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->pause_dosing(); }

   protected:
    BuffDoser *buff_doser_;
};

template <typename... Ts>
class BuffDoserStopDosingAction : public Action<Ts...> {
   public:
    BuffDoserStopDosingAction(BuffDoser *buff_doser) : buff_doser_(buff_doser) {}

    void play(Ts... x) override { this->buff_doser_->stop_dosing(); }

   protected:
    BuffDoser *buff_doser_;
};

}  // namespace buff
}  // namespace esphome
