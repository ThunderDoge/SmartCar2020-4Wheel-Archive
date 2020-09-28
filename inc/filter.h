#include "include.h"
#include "stdint.h"
#include "math.h"

float filter_slide_window_mean(int16_t* input, uint16_t window_wide);
float filter_slide_window_exp_weight(int16_t* input, uint16_t window_wide, float exp_argument);
float filter_finite_impulse_response(void);
