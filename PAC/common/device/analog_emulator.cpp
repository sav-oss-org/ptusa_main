#include "analog_emulator.h"
std::random_device analog_emulator::rd = {};

analog_emulator::analog_emulator( float math_expec, float st_deviation ) :
    d( math_expec, st_deviation )
    {
    }

float analog_emulator::get_st_deviation() const
    {
    return d.stddev();
    }

float analog_emulator::get_m_expec() const
    {
    return d.mean();
    }

void analog_emulator::param( float math_expec, float stddev )
    {
    d.param(
        std::normal_distribution<float>::param_type{ math_expec, stddev } );
    }

float analog_emulator::get_value() const
    {
    return d( rd );
    }
