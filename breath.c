/*
 * This replaces the normal pressure target code
 * in the ROM.  Instead of ramping to
 * new values, it commands instantaneous jumps
 * and alternates between 4.0 and the actual target.
 *
 * Put the system in AutoSet mode and use the high-low values
 *
 * mon stm32f2x options_write 0 0x2c ; reset halt ;  flash write_image erase ../stm32-breath.bin 0x8000000

 */
#include "stubs.h"

typedef struct {
	float high_pressure; // in mm H20
	float low_pressure; //  in mm H20
	unsigned high_time; // in microseconds
	unsigned low_time; // in microseconds
	float smoothing;

	unsigned magic;
	unsigned last_change;
	unsigned state;
	unsigned sample;
	float target;
	int last_value;
} breath_state_t;

int start(float target, float * base)
{
	// therapy manager variable dictionaries
	float * const fvars	= (void*) 0x2000e948;
	int * const ivars	= (void*) 0x2000e750;

	// don't do anything if we are not in an active therapy mode
	if (ivars[0x6f] == 0)
		return 0;

	// our state is is at the top of SRAM
	breath_state_t * const state = (void*) 0x20001f00;

	// read current time in microseconds from tim5
	//const volatile unsigned * const tim5 = (volatile unsigned*) 0x40000c24;
	const unsigned now = tim_read_tim5();

	static const unsigned MAGIC = 0xDECAFBAD;

	if (state->magic != MAGIC)
	{
		state->state = 0;
		state->last_change = now;
		state->high_pressure = 0;
		state->low_pressure = 0;
		state->high_time = 0; // microseconds
		state->low_time = 0;
		state->target = 0.0;
		state->smoothing = 3;
		state->sample = 0;
		state->last_value = 0;
		state->magic = MAGIC;
	}

/*
	if ((state->state && now - state->last_change > state->high_time)
	|| (!state->state && now - state->last_change > state->low_time))
*/
	// Use the AutoSet mode to set the "High" and "Low" pressure modes
	unsigned microseconds_per_breath = fvars[0xa] * 250000; // AutoSet low
	float high_pressure = fvars[0x09]; // AutoSet high
	float low_pressure = 4.0; // Default

	// allow the state structure to override the gui defaults
	if (state->state && state->high_time != 0)
		microseconds_per_breath = state->high_time;
	else
	if (!state->state && state->low_time != 0)
		microseconds_per_breath = state->low_time;

	if (state->high_pressure != 0)
		high_pressure = state->high_pressure;
	if (state->low_pressure != 0)
		low_pressure = state->low_pressure;
	
	// check for time to change state
	if (now - state->last_change > microseconds_per_breath)
	{
		state->state = !state->state;
		state->last_change = now;
	}

	// fvars[0x09] == target high
	// fvars[0x0a] == target low
	
	if (state->state)
		state->target = high_pressure;
	else
		state->target = low_pressure;
		
	// smoothly ramp to the new value
	float smooth_target = (state->smoothing * fvars[0x2d] + state->target) / (state->smoothing + 1);
	fvars[0x2d] = smooth_target;

	// every time through update the max pressure difference
	fvars[0x0d] = 30.0;

	// turn off the motor overshoot detector to prevent it from updating fvar[0x34]
	//unsigned char * const motor_overshoot_obj = (void*) 0x200177bc;
	//motor_overshoot_obj[0x10] = 0;

	// draw something
#if 1
	// break out of the current clipping so we can drawon the entire screen
	unsigned * const color_ptr = (unsigned*)(gui_context + 60);
	short * const clip = (short*)(gui_context + 8);
	short * const xOff = (short*)(gui_context + 76);
	short * const yOff = (short*)(gui_context + 78);
	const short old_x0 = clip[0];
	const short old_y0 = clip[1];
	const short old_x1 = clip[2];
	const short old_y1 = clip[3];
	const short old_xOff = *xOff;
	const short old_yOff = *yOff;
	const unsigned old_color = *color_ptr;
	clip[0] = 0;
	clip[1] = 0;
	clip[2] = 0x1000;
	clip[3] = 0x1000;
	*xOff = 0;
	*yOff = 0;

#if 0
	GUI_SetColor(0x8);
	GUI_FillRect(0, 130, 200, 160);
	
	GUI_SetColor(0xFF0000);
	GUI_SetFont(font_16);
	//static const char __attribute__((__section__(".text"))) msg[] = "Hello, world!";
	//static const char __attribute__((__section__(".text"))) fmt[] = "%d.%02d";
	GUI_DispStringAt("Hello, world", 10, 130);
	char buf[16];
	int flow = fvars[1] * 100;
	snprintf(buf, sizeof(buf), "%d.%02d", flow / 100, flow % 100);
	GUI_SetColor(0x00FF00);
	GUI_DispStringAt(buf, 40, 150);
#endif

	// Draw a strip chart
	const int width = 240;
	const int center = 190;
	const int height = 80;
#if 0
	// flow is roughly 
	int value = fvars[3];
#else
	// actual pressure 0 - 32, scaled to -30 to 30
	int value = 2 * fvars[2] - height/2;
	int command = 2 * fvars[0x2d] - height/2;
#endif
	if (value < -height/2)
		value = -height/2;
	else
	if (value > +height/2)
		value = +height/2;

	GUI_SetColor(0x0);
	LCD_FillRect(state->sample, center - height/2, state->sample + 8, center + height/2);
	// bgr?

	// draw 5, 10, 15, 20 very faintly
	GUI_SetColor(0x202020);
	LCD_DrawPixel(state->sample, center - (2 * 10 - height/2));
	LCD_DrawPixel(state->sample, center - (2 * 20 - height/2));
	LCD_DrawPixel(state->sample, center - (2 * 30 - height/2));

	// draw the current commanded pressure faintly
	GUI_SetColor(0x0000F0);
	LCD_DrawPixel(state->sample, center - command);

	// draw thje strip chart in bright green
	GUI_SetColor(0x00FF00);

	if (state->last_value < value)
	{
		for(int y = state->last_value ; y <= value ; y++)
			LCD_DrawPixel(state->sample, center - y);
	} else {
		for(int y = value ; y <= state->last_value ; y++)
			LCD_DrawPixel(state->sample, center - y);
	}


	state->last_value = value;
	state->sample = (state->sample + 1) % width;

	// restore the old clipping rectangle
	clip[0] = old_x0;
	clip[1] = old_y0;
	clip[2] = old_x1;
	clip[3] = old_y1;
	*xOff = old_xOff;
	*yOff = old_yOff;
	*color_ptr = old_color;
#endif

	return 1;
}
