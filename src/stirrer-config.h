#pragma once

namespace buff {

const short UNSET_STIRRER_PIN_VALUE = -1;

struct StirrerConfig {
    short STIRRER_PIN = UNSET_STIRRER_PIN_VALUE;

    bool enabled() const {
        return STIRRER_PIN != UNSET_STIRRER_PIN_VALUE;
    }
};

}
