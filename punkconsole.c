/*
    punkconsole LV2 plugin

    Copyright (C) 2017 Stéphane Witryk <s.witryk@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include "lv2.h"
#include "punkconsole.h"

typedef struct {
	/* ports */
	struct {
		const float* gain;
		const float* pot1;
		const float* pot2;
		const float* soundOn;
		float*       out;
	} ports;

	/* variables */
	float rate;
	float output;

	int  run_time_astable;
	int  run_time_monostable;
} PunkConsole;

static LV2_Handle
instantiate(const LV2_Descriptor* descriptor, double rate,
	const char* bundle_path, const LV2_Feature* const* features)
{
	PunkConsole* c = (PunkConsole*)calloc(1, sizeof(*c));

	c->rate = rate;
	c->output = 0.0f;

	return (LV2_Handle)c;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	PunkConsole* c = (PunkConsole*)instance;

	switch ((PortIndex)port) {
		case PUNK_CONSOLE_GAIN:
			c->ports.gain = (const float*)data;
			break;
		case PUNK_CONSOLE_POT1:
			c->ports.pot1 = (const float*)data;
			break;
		case PUNK_CONSOLE_POT2:
			c->ports.pot2 = (const float*)data;
			break;
		case PUNK_CONSOLE_SOUND_ON:
			c->ports.soundOn = (const float*)data;
			break;
		case PUNK_CONSOLE_OUTPUT:
			c->ports.out = (float*)data;
			break;
	}
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	PunkConsole* c = (PunkConsole*)instance;
	float* const       out = c->ports.out;

	int  high_time_astable = 0.693 * ((*c->ports.pot1) + 1000.0) * .01E-6 * c->rate;
	int  low_time_astable = 0.693 * (*c->ports.pot1) * .01E-6 * c->rate;
	int  high_time_monostable = 0.693 * (*c->ports.pot2) * .1E-6 * c->rate;

	uint32_t pos;
	for (pos = 0; pos < n_samples; pos++) {
        if (c->run_time_astable >= high_time_astable + low_time_astable)
            c->run_time_astable = 0;

        if (c->run_time_monostable >= high_time_monostable) {
            c->output = 0.0f;
            if (c->run_time_astable == high_time_astable) {
                c->run_time_monostable = 0;
                c->output = 1.0f;
            }
        }

        (c->run_time_astable) ++;
        (c->run_time_monostable) ++;

        out[pos] = ((*c->ports.soundOn) > 0.5f)
            ? (c->output) * (*c->ports.gain)
            : 0.0f;
    }
}

static void
cleanup(LV2_Handle instance)
{
	free(instance);
}

static void
activate(LV2_Handle instance)
{
}

static void
deactivate(LV2_Handle instance)
{
}

static const void*
extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	PUNK_CONSOLE_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	  case 0:  return &descriptor;
	  default: return NULL;
	}
}
