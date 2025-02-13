import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_DURATION,
    CONF_VOLUME,
)
from esphome import automation
from esphome.automation import maybe_simple_id

CODEOWNERS = ["@richievos"]
DEPENDENCIES = []
# Allow for setting up multiple of these dosers
MULTI_CONF = True

CONF_VOLUME_PER_MINUTE = "volume_per_minute"

buff_ns = cg.esphome_ns.namespace('::esphome::buff')
BuffDoser = buff_ns.class_('BuffDoser', cg.Component)

####################################
# Core libraries
cg.add_library(
    name="arduino-libraries/NTPClient",
    version=None,
)
cg.add_library(
    name="adafruit/Adafruit BusIO",
    version="^1.14.1",
)
cg.add_library(
    name="Wire",
    version=None,
)
cg.add_library(
    name="SPI",
    version=None,
)
cg.add_library(
    name="ESP32SSDP",
    repository="https://github.com/richievos/ESP32SSDP",
    version=None,
)
cg.add_library(
    name="ReefBuff",
    repository="https://github.com/richievos/buff-esphome",
    version=None,
)

####################################
# Stepper libraries
cg.add_library(
    name="StepperDriver",
    repository="https://github.com/richievos/StepperDriver",
    version=None,
)
# ; Using a fork of this to get cleaner compilation on desktop
# ; waspinator/AccelStepper@^1.64
cg.add_library(
    name="ArduinoJson",
    repository="https://github.com/bblanchon/ArduinoJson.git",
    version="^6.20.1",
)

####################################
# Libraries for the display
# TODO: figure out how to set the lv_conf appropriately
#     build_flags: '-std=gnu++17 -DLV_CONF_SKIP -D LV_CONF_INCLUDE_SIMPLE'
# the easiest option might be fork lvgl and create a version with the lv_conf
# committed in it.
# cg.add_library(
#     name="lvgl",
#     version="^8.3.7",
# )
# cg.add_library(
#     name="SPI",
#     version="^8.3.7",
# )
# cg.add_library(
#     name="TFT_eSPI",
#     repository="https://github.com/richievos/TFT_eSPI",
#     version=None,
# )
# cg.add_library(
#     name="adafruit/Adafruit GFX Library",
#     version="^1.11.5",
# )
# cg.add_library(
#     name="adafruit/Adafruit SSD1306",
#     version="^2.5.7",
# )

####################################
# Redundant due to esphome libraries

# MQTT is redundant here since esphome will cover it
# ; my main fixes a connectivity issue: https://github.com/hsaturn/TinyMqtt/pull/72
# ; and a memory leak: https://github.com/hsaturn/TinyMqtt/pull/74
# https://github.com/richievos/TinyMqtt.git#main
# cg.add_library(
#     name="TinyMqtt",
#     repository="https://github.com/richievos/TinyMqtt.git",
#     version="main",
# )
# cg.add_library(
#     name="arduino-esp/WiFi",
#     version="^1.2.7",
# )
# Webserver also is somewhat redundant given esphome handles that
# cg.add_library(
#     # name="me-no-dev/ESP Async WebServer",
#     name="ESP32Async/ESPAsyncWebServer",
#     version="^1.2.3",
# )

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BuffDoser),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)


BUFF_DOSER_NO_ARGS_ACTION_SCHEMA = maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(BuffDoser),
    }
)

# Actions that do not require more arguments

BuffDoserClearTotalVolumeDispensedAction = buff_ns.class_(
    "BuffDoserClearTotalVolumeDispensedAction", automation.Action
)
BuffDoserClearCalibrationAction = buff_ns.class_(
    "BuffDoserClearCalibrationAction", automation.Action
)
BuffDoserPauseDosingAction = buff_ns.class_(
    "BuffDoserPauseDosingAction", automation.Action
)
BuffDoserStopDosingAction = buff_ns.class_("BuffDoserStopDosingAction", automation.Action)
BuffDoserDoseContinuouslyAction = buff_ns.class_(
    "BuffDoserDoseContinuouslyAction", automation.Action
)

# Actions that require more arguments
BuffDoserDoseVolumeAction = buff_ns.class_("BuffDoserDoseVolumeAction", automation.Action)
BuffDoserDoseVolumeOverTimeAction = buff_ns.class_(
    "BuffDoserDoseVolumeOverTimeAction", automation.Action
)
BuffDoserDoseWithConstantFlowRateAction = buff_ns.class_(
    "BuffDoserDoseWithConstantFlowRateAction", automation.Action
)
BuffDoserSetCalibrationVolumeAction = buff_ns.class_(
    "BuffDoserSetCalibrationVolumeAction", automation.Action
)

@automation.register_action(
    "buff_doser.dose_continuously",
    BuffDoserDoseContinuouslyAction,
    BUFF_DOSER_NO_ARGS_ACTION_SCHEMA,
)
async def buff_doser_dose_continuously_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "buff_doser.clear_total_volume_dosed",
    BuffDoserClearTotalVolumeDispensedAction,
    BUFF_DOSER_NO_ARGS_ACTION_SCHEMA,
)
async def buff_doser_clear_total_volume_dosed_to_code(
    config, action_id, template_arg, args
):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "buff_doser.clear_calibration",
    BuffDoserClearCalibrationAction,
    BUFF_DOSER_NO_ARGS_ACTION_SCHEMA,
)
async def buff_doser_clear_calibration_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "buff_doser.pause_dosing", BuffDoserPauseDosingAction, BUFF_DOSER_NO_ARGS_ACTION_SCHEMA
)
async def buff_doser_pause_dosing_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "buff_doser.stop_dosing", BuffDoserStopDosingAction, BUFF_DOSER_NO_ARGS_ACTION_SCHEMA
)
async def buff_doser_stop_dosing_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


# Actions that require Multiple Args

BUFF_DOSER_DOSE_VOLUME_ACTION_SCHEMA = cv.All(
    {
        cv.Required(CONF_ID): cv.use_id(BuffDoser),
        cv.Required(CONF_VOLUME): cv.templatable(
            cv.float_range()
        ),  # Any way to represent as proper volume (vs. raw int)
    }
)


@automation.register_action(
    "buff_doser.dose_volume", BuffDoserDoseVolumeAction, BUFF_DOSER_DOSE_VOLUME_ACTION_SCHEMA
)
async def buff_doser_dose_volume_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_ = await cg.templatable(config[CONF_VOLUME], args, cg.double)
    cg.add(var.set_volume(template_))

    return var


BUFF_DOSER_DOSE_VOLUME_OVER_TIME_ACTION_SCHEMA = cv.All(
    {
        cv.Required(CONF_ID): cv.use_id(BuffDoser),
        cv.Required(CONF_VOLUME): cv.templatable(
            cv.float_range()
        ),  # Any way to represent as proper volume (vs. raw int)
        cv.Required(CONF_DURATION): cv.templatable(
            cv.int_range(1)
        ),  # Any way to represent it as minutes (vs. raw int)
    }
)


@automation.register_action(
    "buff_doser.dose_volume_over_time",
    BuffDoserDoseVolumeOverTimeAction,
    BUFF_DOSER_DOSE_VOLUME_OVER_TIME_ACTION_SCHEMA,
)
async def buff_doser_dose_volume_over_time_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_ = await cg.templatable(config[CONF_VOLUME], args, cg.double)
    cg.add(var.set_volume(template_))

    template_ = await cg.templatable(config[CONF_DURATION], args, int)
    cg.add(var.set_duration(template_))

    return var


BUFF_DOSER_DOSE_WITH_CONSTANT_FLOW_RATE_ACTION_SCHEMA = cv.All(
    {
        cv.Required(CONF_ID): cv.use_id(BuffDoser),
        cv.Required(CONF_VOLUME_PER_MINUTE): cv.templatable(
            cv.float_range()
        ),  # Any way to represent as proper volume (vs. raw int)
        cv.Required(CONF_DURATION): cv.templatable(
            cv.int_range(1)
        ),  # Any way to represent it as minutes (vs. raw int)
    }
)


@automation.register_action(
    "buff_doser.dose_with_constant_flow_rate",
    BuffDoserDoseWithConstantFlowRateAction,
    BUFF_DOSER_DOSE_WITH_CONSTANT_FLOW_RATE_ACTION_SCHEMA,
)
async def buff_doser_dose_with_constant_flow_rate_to_code(
    config, action_id, template_arg, args
):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_ = await cg.templatable(config[CONF_VOLUME_PER_MINUTE], args, cg.double)
    cg.add(var.set_volume(template_))

    template_ = await cg.templatable(config[CONF_DURATION], args, int)
    cg.add(var.set_duration(template_))

    return var


BUFF_DOSER_SET_CALIBRATION_VOLUME_ACTION_SCHEMA = cv.All(
    {
        cv.Required(CONF_ID): cv.use_id(BuffDoser),
        cv.Required(CONF_VOLUME): cv.templatable(
            cv.float_range()
        ),  # Any way to represent as proper volume (vs. raw int)
    }
)


@automation.register_action(
    "buff_doser.set_calibration_volume",
    BuffDoserSetCalibrationVolumeAction,
    BUFF_DOSER_SET_CALIBRATION_VOLUME_ACTION_SCHEMA,
)
async def buff_doser_set_calibration_volume_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_ = await cg.templatable(config[CONF_VOLUME], args, cg.double)
    cg.add(var.set_volume(template_))

    return var
