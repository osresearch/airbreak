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
typedef struct {
	float high_pressure; // in mm H20
	float low_pressure; //  in mm H20
	unsigned high_time; // in microseconds
	unsigned low_time; // in microseconds

	unsigned magic;
	unsigned last_change;
	unsigned state;
	unsigned yoff;
	unsigned char flow[64];
} breath_state_t;

int breath(float target, float * base)
{
	// therapy manager variable dictionaries
	float * const fvars	= (void*) 0x2000e948;
	int * const ivars	= (void*) 0x2000e750;

	// our state is is at the top of SRAM
	breath_state_t * const state = (void*) 0x20001f00;

	// read current time in microseconds from tim5
	const volatile unsigned * const tim5 = (volatile unsigned*) 0x40000c24;
	const unsigned now = *tim5;

	static const unsigned MAGIC = 0xDECAFBAD;

	if (state->magic != MAGIC)
	{
		state->state = 0;
		state->last_change = now;
		state->high_pressure = 0;
		state->low_pressure = 0;
		state->high_time = 0; // microseconds
		state->low_time = 0;
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
		fvars[0x2d] = high_pressure;
	else
		fvars[0x2d] = low_pressure;

	// every time through update the max pressure difference
	fvars[0x0d] = 30.0;

	return 1;
}
