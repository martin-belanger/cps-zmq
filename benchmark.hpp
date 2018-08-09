#ifndef BENCHMARK_HPP_
#define BENCHMARK_HPP_

#include "cps.h"
#include "cps_api_operation.h"
#include "benchmark.h"


namespace benchmark {

/******************************************************************************/
class Object : public cps::Object
{
public:
    Object() : cps::Object()
    {
    }

    Object(cps_api_object_t obj) : cps::Object(obj)
    {
    }

    virtual uint32_t seq_no() const = 0;
    virtual uint32_t last_seq_no() const = 0;
    virtual bool set_seq_no(uint32_t u32) = 0;
    virtual bool set_last_seq_no(uint32_t u32) = 0;
    virtual bool set_timestamp_ns(uint64_t u64) = 0;
};

/******************************************************************************/
class Alarm final : public benchmark::Object
{
public:
    Alarm() : benchmark::Object()
    {
        key_from_attr_with_qual(PUBSUB_ALARM_OBJ, cps_api_qualifier_TARGET);
        attr_set(PUBSUB_ALARM_SEQ_NO, (uint32_t)0);
        attr_set(PUBSUB_ALARM_NAME, "Intruder alert");
    }

    Alarm(cps_api_object_t obj) : benchmark::Object(obj)
    {
    }

    // ***********************************************
    // Getters
    uint32_t seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_ALARM_SEQ_NO, seq_no);
        return seq_no;
    }

    uint32_t last_seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_ALARM_LAST_SEQ_NO, seq_no);
        return seq_no;
    }

    std::string name() const
    {
        std::string name;
        attr_get(PUBSUB_ALARM_NAME, name);
        return name;
    }

    // ***********************************************
    // Setters
    bool set_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_ALARM_SEQ_NO, u32);
    }

    bool set_last_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_ALARM_LAST_SEQ_NO, u32);
    }

    bool set_timestamp_ns(uint64_t u64)
    {
        return attr_set(PUBSUB_ALARM_TIMESTAMP_NS, u64);
    }

    bool set_name(const std::string &name)
    {
        return attr_set(PUBSUB_ALARM_NAME, name);
    }
};

/******************************************************************************/
class Connection final : public benchmark::Object
{
public:
    Connection() : benchmark::Object()
    {
        key_from_attr_with_qual(PUBSUB_CONNECTION_OBJ, cps_api_qualifier_TARGET);
        attr_set(PUBSUB_CONNECTION_SEQ_NO, (uint32_t)0);
        attr_set(PUBSUB_CONNECTION_NAME, "");
    }

    Connection(cps_api_object_t obj) : benchmark::Object(obj)
    {
    }

    // ***********************************************
    // Getters
    uint32_t seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_CONNECTION_SEQ_NO, seq_no);
        return seq_no;
    }

    uint32_t last_seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_CONNECTION_LAST_SEQ_NO, seq_no);
        return seq_no;
    }

    std::string name() const
    {
        std::string name;
        attr_get(PUBSUB_CONNECTION_NAME, name);
        return name;
    }

    // ***********************************************
    // Setters
    bool set_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_CONNECTION_SEQ_NO, u32);
    }

    bool set_last_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_CONNECTION_LAST_SEQ_NO, u32);
    }

    bool set_timestamp_ns(uint64_t u64)
    {
        return attr_set(PUBSUB_CONNECTION_TIMESTAMP_NS, u64);
    }

    bool set_name(const std::string &name)
    {
        return attr_set(PUBSUB_CONNECTION_NAME, name);
    }
};

/******************************************************************************/
class Telemetry final : public benchmark::Object
{
public:
    Telemetry() : benchmark::Object()
    {
        key_from_attr_with_qual(PUBSUB_TELEMETRY_OBJ, cps_api_qualifier_TARGET);
        attr_set(PUBSUB_TELEMETRY_SEQ_NO, (uint32_t)0);
        attr_set(PUBSUB_TELEMETRY_TEMPERATURE, "Normal");
        attr_set(PUBSUB_TELEMETRY_VOLTAGE, "Normal");
    }

    Telemetry(cps_api_object_t obj) : benchmark::Object(obj)
    {
    }

    // ***********************************************
    // Getters
    uint32_t seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_TELEMETRY_SEQ_NO, seq_no);
        return seq_no;
    }

    uint32_t last_seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_TELEMETRY_LAST_SEQ_NO, seq_no);
        return seq_no;
    }

    std::string temperature() const
    {
        std::string temperature;
        attr_get(PUBSUB_TELEMETRY_TEMPERATURE, temperature);
        return temperature;
    }

    std::string voltage() const
    {
        std::string voltage;
        attr_get(PUBSUB_TELEMETRY_VOLTAGE, voltage);
        return voltage;
    }

    // ***********************************************
    // Setters
    bool set_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_TELEMETRY_SEQ_NO, u32);
    }

    bool set_last_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_TELEMETRY_LAST_SEQ_NO, u32);
    }

    bool set_timestamp_ns(uint64_t u64)
    {
        return attr_set(PUBSUB_TELEMETRY_TIMESTAMP_NS, u64);
    }

    bool set_temperature(const std::string &temperature)
    {
        return attr_set(PUBSUB_TELEMETRY_TEMPERATURE, temperature);
    }

    bool set_voltage(const std::string &voltage)
    {
        return attr_set(PUBSUB_TELEMETRY_VOLTAGE, voltage);
    }

};

/******************************************************************************/
class Vlan final : public benchmark::Object
{
public:
    Vlan() : benchmark::Object()
    {
        key_from_attr_with_qual(PUBSUB_VLAN_OBJ, cps_api_qualifier_TARGET);
        attr_set(PUBSUB_VLAN_SEQ_NO, (uint32_t)0);
        attr_set(PUBSUB_VLAN_NAME, "");
    }

    Vlan(cps_api_object_t obj) : benchmark::Object(obj)
    {
    }

    // ***********************************************
    // Getters
    uint32_t seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_VLAN_SEQ_NO, seq_no);
        return seq_no;
    }

    uint32_t last_seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_VLAN_LAST_SEQ_NO, seq_no);
        return seq_no;
    }

    std::string name() const
    {
        std::string name;
        attr_get(PUBSUB_VLAN_NAME, name);
        return name;
    }

    // ***********************************************
    // Setters
    bool set_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_VLAN_SEQ_NO, u32);
    }

    bool set_last_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_VLAN_LAST_SEQ_NO, u32);
    }

    bool set_timestamp_ns(uint64_t u64)
    {
        return attr_set(PUBSUB_VLAN_TIMESTAMP_NS, u64);
    }

    bool set_name(const std::string &name)
    {
        return attr_set(PUBSUB_VLAN_NAME, name);
    }
};

/******************************************************************************/
class Weather final : public benchmark::Object
{
public:
    Weather() : benchmark::Object()
    {
        key_from_attr_with_qual(PUBSUB_WEATHER_OBJ, cps_api_qualifier_TARGET);
        attr_set(PUBSUB_WEATHER_SEQ_NO, (uint32_t)0);
        attr_set(PUBSUB_WEATHER_NAME, "Cloudy with a chance of meatballs");
    }

    Weather(cps_api_object_t obj) : benchmark::Object(obj)
    {
    }

    // ***********************************************
    // Getters
    uint32_t seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_WEATHER_SEQ_NO, seq_no);
        return seq_no;
    }

    uint32_t last_seq_no() const
    {
        uint32_t seq_no;
        attr_get(PUBSUB_WEATHER_LAST_SEQ_NO, seq_no);
        return seq_no;
    }

    std::string name() const
    {
        std::string name;
        attr_get(PUBSUB_WEATHER_NAME, name);
        return name;
    }

    // ***********************************************
    // Setters
    bool set_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_WEATHER_SEQ_NO, u32);
    }

    bool set_last_seq_no(uint32_t u32)
    {
        return attr_set(PUBSUB_WEATHER_LAST_SEQ_NO, u32);
    }

    bool set_timestamp_ns(uint64_t u64)
    {
        return attr_set(PUBSUB_WEATHER_TIMESTAMP_NS, u64);
    }

    bool set_name(const std::string &name)
    {
        return attr_set(PUBSUB_WEATHER_NAME, name);
    }
};

}
    #endif // BENCHMARK_HPP_
