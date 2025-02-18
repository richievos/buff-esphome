#pragma once

#include <map>

// Buff Libraries
#include "doser/doser-config.h"
#include "inputs-board-config.h"

namespace buff {
namespace inputs {

/*******************************
 * INPUTS Board config
 *******************************/
#ifdef BOARD_ESP32
const auto PIN_CONFIG = ESP32_CONFIG;
#elif BOARD_MKS_DLC32
const auto PIN_CONFIG = MKS_DLC32_CONFIG;
#endif

#ifdef UNIT_1
const DoserConfig fillDoserConfig = {.mlPerFullRotation = 0.269, .motorRPM = 120,
                                     //
                                     .microStepType = SIXTEENTH,
                                     .fullStepsPerRotation = 200,
                                     .clockwiseDirectionMultiplier = -1};

const DoserConfig reagentDoserConfig = {.mlPerFullRotation = 0.175, .motorRPM = 60,
                                        //
                                        .microStepType = SIXTEENTH,
                                        .fullStepsPerRotation = 200,
                                        .clockwiseDirectionMultiplier = 1};

const DoserConfig drainDoserConfig = {.mlPerFullRotation = 0.3, .motorRPM = 180,
                                      //
                                      .microStepType = SIXTEENTH,
                                      .fullStepsPerRotation = 200,
                                      .clockwiseDirectionMultiplier = -1};
#elif UNIT_2
const DoserConfig fillDoserConfig = {.mlPerFullRotation = 0.313, .motorRPM = 120,
                                     //
                                     .microStepType = SIXTEENTH,
                                     .fullStepsPerRotation = 200,
                                     .clockwiseDirectionMultiplier = -1};

const DoserConfig reagentDoserConfig = {.mlPerFullRotation = 0.185, .motorRPM = 60,
                                        //
                                        .microStepType = SIXTEENTH,
                                        .fullStepsPerRotation = 200,
                                        .clockwiseDirectionMultiplier = 1};

const DoserConfig drainDoserConfig = {.mlPerFullRotation = 0.276, .motorRPM = 180,
                                      //
                                      .microStepType = SIXTEENTH,
                                      .fullStepsPerRotation = 200,
                                      .clockwiseDirectionMultiplier = -1};
#endif

}  // namespace inputs
}  // namespace buff
