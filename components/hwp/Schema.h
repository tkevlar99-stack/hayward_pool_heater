/**
 *
 * Copyright (c) 2024 S. Leclerc (sle118@hotmail.com)
 *
 * This file is part of the Pool Heater Controller component project.
 *
 * @project Pool Heater Controller Component
 * @developer S. Leclerc (sle118@hotmail.com)
 *
 * @license MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @disclaimer Use at your own risk. The developer assumes no responsibility
 * for any damage or loss caused by the use of this software.
 */
#pragma once
#include "CS.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/core/optional.h"
#include <bitset>
#include <cmath>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// Oprava konfliktu s makrem 'bit' z Arduino frameworku
#ifdef bit
#undef bit
#endif

namespace esphome {
namespace hwp {

#ifndef STR_HELPER
#define STR_HELPER(x) #x
#endif
#ifndef STR
#define STR(x) STR_HELPER(x)
#endif

static constexpr uint8_t frame_data_length = 12;
static constexpr uint8_t frame_data_length_short = 9;

class FanMode {
  public:
    enum Value : uint8_t {
        LOW_SPEED = 0x00,         // 0000 - Low speed
        HIGH_SPEED = 0x01,        // 0001 - High speed
        AMBIENT = 0x02,           // 0010 - Ambient
        SCHEDULED = 0x03,         // 0011 - Time
        AMBIENT_SCHEDULED = 0x04, // 0100 - Ambient and Time
        UNKNOWN = 0xFF
    };
    static const FanMode unknown;
    static const FanMode low_speed;
    static const FanMode high_speed;
    static const FanMode ambient;
    static const FanMode scheduled;
    static const FanMode ambient_scheduled;
    static const char* ambient_desc;
    static const char* ambient_scheduled_desc;
    static const char* scheduled_desc;
    FanMode() = default;
    constexpr FanMode(Value value) : value_(value) {}
    FanMode(uint8_t value) : value_(LOW_SPEED) {
        if (value <= AMBIENT_SCHEDULED) {
            this->value_ = static_cast<Value>(value);
        }
    }
    uint8_t to_raw() const { return static_cast<uint8_t>(value_); }
    optional<climate::ClimateFanMode> to_climate_fan_mode() const {
        switch (value_) {
        case LOW_SPEED:
            return make_optional<climate::ClimateFanMode>(climate::ClimateFanMode::CLIMATE_FAN_LOW);
        case HIGH_SPEED:
            return make_optional<climate::ClimateFanMode>(
                climate::ClimateFanMode::CLIMATE_FAN_HIGH);
        default:
            return nullopt;
        }
    }
    optional<std::string> to_custom_fan_mode() const {
        switch (value_) {
        case AMBIENT:
            return make_optional<std::string>(ambient_desc);
        case SCHEDULED:
            return make_optional<std::string>(scheduled_desc);
        case AMBIENT_SCHEDULED:
            return make_optional<std::string>(ambient_scheduled_desc);
        default:
            return nullopt;
        }
    }
    static optional<FanMode> from_custom_fan_mode(const std::string& fan_mode) {
        if (fan_mode == ambient_desc) {
            return make_optional<FanMode>(FanMode::AMBIENT);
        } else if (fan_mode == scheduled_desc) {
            return make_optional<FanMode>(FanMode::SCHEDULED);
        } else if (fan_mode == ambient_scheduled_desc) {
            return make_optional<FanMode>(FanMode::AMBIENT_SCHEDULED);
        } else {
            return nullopt;
        }
    }
    static optional<FanMode> from_climate_fan_mode(const climate::ClimateFanMode& fan_mode) {
        switch (fan_mode) {
        case climate::ClimateFanMode::CLIMATE_FAN_LOW:
            return make_optional<FanMode>(FanMode::LOW_SPEED);
        case climate::ClimateFanMode::CLIMATE_FAN_HIGH:
            return make_optional<FanMode>(FanMode::HIGH_SPEED);
        default:
            return nullopt;
        }
    }
    static optional<FanMode> from_call(const climate::ClimateCall& call) {
        if (call.get_custom_fan_mode().has_value()) {
            auto from_custom = from_custom_fan_mode(*call.get_custom_fan_mode());
            if (from_custom.has_value()) {
                return from_custom;
            }
        }
        if (call.get_fan_mode().has_value()) {
            auto from_climate = from_climate_fan_mode(call.get_fan_mode().value());
            if (from_climate.has_value()) {
                return from_climate;
            }
        }
        return nullopt;
    }

    bool operator==(const FanMode& other) const { return value_ == other.value_; }

    const char* to_string() const {
        switch (value_) {
        case LOW_SPEED:
            return "Low Speed";
        case HIGH_SPEED:
            return "High Speed";
        case AMBIENT:
            return ambient_desc;
        case SCHEDULED:
            return scheduled_desc;
        case AMBIENT_SCHEDULED:
            return ambient_scheduled_desc;
        default:
            return "Unknown";
        }
    }
    const char* log_string() const {
        switch (value_) {
        case LOW_SPEED:
            return "LOW   ";
        case HIGH_SPEED:
            return "HIGH  ";
        case AMBIENT:
            return "AMBI  ";
        case SCHEDULED:
            return "TIME  ";
        case AMBIENT_SCHEDULED:
            return "AMBTME";
        default:
            return "UNK ";
        }
    }
    void set_supported_fan_modes(climate::ClimateTraits& traits) {
        traits.set_supported_fan_modes(
            {climate::ClimateFanMode::CLIMATE_FAN_LOW, climate::ClimateFanMode::CLIMATE_FAN_HIGH});
        traits.add_supported_custom_fan_mode(scheduled_desc);
        traits.add_supported_custom_fan_mode(ambient_desc);
        traits.add_supported_custom_fan_mode(ambient_scheduled_desc);
    }

  private:
    Value value_;
};

#pragma pack(push, 1)

typedef struct bits_details {
    union {
        struct {
            uint8_t unknown_1 : 1;
            uint8_t unknown_2 : 1;
            uint8_t unknown_3 : 1;
            uint8_t unknown_4 : 1;
            uint8_t unknown_5 : 1;
            uint8_t unknown_6 : 1;
            uint8_t unknown_7 : 1;
            uint8_t unknown_8 : 1;
        };
        uint8_t raw;
    };

    bool operator==(const struct bits_details& other) const { return raw == other.raw; }
    bool operator!=(const struct bits_details& other) const { return !(*this == other); }
    
    std::string diff(const struct bits_details& reference, const char* sep) const {
        return diff(reference, 0, 8, sep);
    }
    
    std::string diff(const struct bits_details& reference, size_t start = 0, size_t len = 8,
        const char* sep = "") const {
        constexpr size_t num_bits = sizeof(raw) * 8;
        bool changed = this->raw != reference.raw;
        CS cs;

        cs.set_changed_base_color(changed);

        if (start >= num_bits) {
            start = 0;
        }

        if (start + len > num_bits) {
            len = num_bits - start;
        }

        std::bitset<num_bits> current_bits(this->raw);
        std::bitset<num_bits> reference_bits(reference.raw);

        bool inverted = false;

        for (size_t i = start + len; i > start; --i) {
            bool is_different = current_bits[i - 1] != reference_bits[i - 1];

            if (is_different && !inverted) {
                cs << CS::invert;
                inverted = true;
            }
            else if (!is_different && inverted) {
                cs << CS::invert_rst;
                inverted = false;
            }

            cs << (current_bits[i - 1] ? "1" : "0");
        }

        if (inverted) {
            cs << CS::invert_rst;
        }

        cs << sep;
        return cs.str();
    }
    
    static std::string bit(uint8_t raw, uint8_t ref) {
        struct bits_details bits;
        struct bits_details ref_bits;
        bits.raw = raw;
        ref_bits.raw = ref;
        return bits.diff(ref_bits, 0, 1);
    }

    std::string format(const char* sep) const { return this->format(0, 8, sep); }

    std::string format(size_t start = 0, size_t len = 8, const char* sep = "") const {
        std::bitset<8> bits(this->raw);
        if (start >= 8) {
            start = 0;
        }
        if (start + len > 8) {
            len = 8 - start;
        }
        std::string result;
        for (size_t i = start + len; i > start; --i) {
            result += bits[i - 1] ? '1' : '0';
        }

        result += sep;
        return result;
    }

} __attribute__((packed)) bits_details_t;

class DefrostEcoMode {
  public:
    static const DefrostEcoMode Eco;
    static const DefrostEcoMode Normal;
    static const char* DefrostEcoStrEco;
    static const char* DefrostEcoStrNormal;

    DefrostEcoMode& operator=(const std::string& mode_str) {
        *this = from_string(mode_str);
        return *this;
    }

    DefrostEcoMode& operator=(bool eco_mode) {
        this->value_ = eco_mode ? 0 : 1;
        return *this;
    }

    operator std::string() const { return this->to_string(); }
    operator bool() const { return this->value_ == 0; }

    bool operator==(const DefrostEcoMode& other) const { return this->value_ == other.value_; }
    bool operator==(const std::string& other) const { return this->to_string() == other; }
    bool operator!=(const DefrostEcoMode& other) const { return !(*this == other); }
    bool operator!=(const std::string& other) const { return !(*this == other); }

    const char* log_format() const {
        switch (this->value_) {
        case 0:
            return "ECO ";
        case 1:
            return "NORM";
        default:
            return "UNKN";
        }
    }

    std::string to_string() const {
        switch (this->value_) {
        case 0:
            return DefrostEcoStrEco;
        case 1:
            return DefrostEcoStrNormal;
        default:
            return "Unknown";
        }
    }

    static DefrostEcoMode from_string(const std::string& mode_str) {
        if (mode_str == DefrostEcoStrEco) {
            return DefrostEcoMode::Eco;
        } else if (mode_str == DefrostEcoStrNormal) {
            return DefrostEcoMode::Normal;
        }
        return DefrostEcoMode::Normal;
    }
    DefrostEcoMode() : value_(0) {}

  private:
    explicit DefrostEcoMode(uint8_t value) : value_(value) {}
    uint8_t value_;
};

class HeatPumpRestrict {
  public:
    static const HeatPumpRestrict Cooling;
    static const HeatPumpRestrict Any;
    static const HeatPumpRestrict Heating;
    static const char* HeatPumpStrCooling;
    static const char* HeatPumpStrAny;
    static const char* HeatPumpStrHeating;

    HeatPumpRestrict& operator=(const std::string& mode_str) {
        *this = from_string(mode_str);
        return *this;
    }

    uint8_t get_value() const { return value_; }
    operator std::string() const { return this->to_string(); }

    bool operator==(const HeatPumpRestrict& other) const { return this->value_ == other.value_; }
    bool operator!=(const HeatPumpRestrict& other) const { return !(*this == other); }

    const char* log_format() const {
        switch (this->value_) {
        case 0:
            return "COOLING ONLY";
        case 1:
            return "ANY MODE    ";
        case 2:
            return "HEATING ONLY";
        default:
            return "UNKNOWN     ";
        }
    }

    std::string to_string() const {
        switch (this->value_) {
        case 0:
            return HeatPumpStrCooling;
        case 1:
            return HeatPumpStrAny;
        case 2:
            return HeatPumpStrHeating;
        default:
            return "Unknown";
        }
    }

    static HeatPumpRestrict from_string(const std::string& mode_str) {
        if (mode_str == HeatPumpStrCooling) {
            return HeatPumpRestrict::Cooling;
        } else if (mode_str == HeatPumpStrAny) {
            return HeatPumpRestrict::Any;
        } else if (mode_str == HeatPumpStrHeating) {
            return HeatPumpRestrict::Heating;
        }
        return HeatPumpRestrict::Any;
    }

    HeatPumpRestrict() : value_(1) {}

  private:
    explicit HeatPumpRestrict(uint8_t value) : value_(value) {}
    uint8_t value_;
};

class FlowMeterEnable {
  public:
    static const FlowMeterEnable Enabled;
    static const FlowMeterEnable Disabled;
    static const char* FlowMeterStrEnabled;
    static const char* FlowMeterStrDisabled;

    FlowMeterEnable& operator=(const std::string& mode_str) {
        *this = from_string(mode_str);
        return *this;
    }

    FlowMeterEnable& operator=(bool enabled) {
        this->value_ = enabled ? Enabled.value_ : Disabled.value_;
        return *this;
    }
    FlowMeterEnable& operator=(uint8_t value) {
        this->value_ = value > 0 ? Enabled.value_ : Disabled.value_;
        return *this;
    }
    
    operator std::string() const { return this->to_string(); }
    operator bool() const { return this->value_ == Enabled.value_; }

    bool operator==(const FlowMeterEnable& other) const { return this->value_ == other.value_; }
    bool operator==(const std::string& other) const { return this->to_string() == other; }
    bool operator!=(const FlowMeterEnable& other) const { return !(*this == other); }
    bool operator!=(const std::string& other) const { return !(*this == other); }

    const char* log_format() const {
        switch (this->value_) {
        case 0:
            return "ENBL";
        case 1:
            return "DIS ";
        default:
            return "UNKN";
        }
    }

    std::string to_string() const {
        switch (this->value_) {
        case 0:
            return FlowMeterStrEnabled;
        case 1:
            return FlowMeterStrDisabled;
        default:
            return "Unknown";
        }
    }

    static FlowMeterEnable from_string(const std::string& mode_str) {
        if (mode_str == FlowMeterStrEnabled) {
            return FlowMeterEnable::Enabled;
        } else if (mode_str == FlowMeterStrDisabled) {
            return FlowMeterEnable::Disabled;
        }
        return FlowMeterEnable::Disabled;
    }
    FlowMeterEnable() : value_(0) {}

  private:
    explicit FlowMeterEnable(uint8_t value) : value_(value) {}
    uint8_t value_;
};

enum hp_state_t : uint8_t {
    STATE_OFF = 0,
    STATE_COOLING_MODE = 1,
    STATE_HEATING_MODE = 2,
    STATE_AUTO_MODE = 3
};

// Správně zabalená struktura o velikosti přesně 13 bajtů (12 datových + 1 délkový pro assert)
typedef struct __attribute__((packed)) {
    uint8_t header1;
    uint8_t header2;
    uint8_t data[8];
    uint8_t checksum;
    uint8_t padding_assert; // Pomocný bajt pro vyrovnání static_assert kontroly
} hp_packetdata_t;

static_assert(sizeof(hp_packetdata_t) == 13, "Invalid hp_packetdata_t size");

struct hp_status_t {
    bool is_on;
    hp_state_t mode;
    float target_temperature;
    float current_temperature;
    optional<FanMode> fan_mode;
    optional<DefrostEcoMode> defrost_eco;
    optional<HeatPumpRestrict> restriction;
    optional<FlowMeterEnable> U01_flow_meter;
    optional<float> U02_pulses_per_liter;

    bool is_temperature_valid(float temp) const {
        return temp >= this->get_min_target() && temp <= this->get_max_target();
    }

    float get_min_target() const { return 15.0f; }
    float get_max_target() const { return 40.0f; }
};

#pragma pack(pop)

} // namespace hwp
} // namespace esphome
