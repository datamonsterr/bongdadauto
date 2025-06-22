#ifndef __JOYSTICK_DATA_H__
#define __JOYSTICK_DATA_H__

#include <stdint.h>

typedef struct
{
    uint16_t j1_x;
    uint16_t j1_y;
    uint16_t j2_x;
    uint16_t j2_y;
    uint16_t j1_sw;
    uint16_t j2_sw;
} JoystickData_t;

#endif