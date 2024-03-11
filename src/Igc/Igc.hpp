
#ifndef IGC_HPP
#define IGC_HPP

#include "../plugin.hpp"

#define IGC_BUFFER				480000	// 10s at 48000Hz sample rate
#define IGC_BUFFER_PRECISION	96		// Display precision

#define IGC_MODE_POS_REL		0
#define IGC_MODE_POS_ABS		1
#define IGC_MODE_SPEED			2
#define IGC_MODE_GRAIN			3

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct IgcPlayhead {
	float						phase;
	float						level;
	float						env_time;
};

struct Igc : Module {
	enum	ParamIds {
		PARAM_DELAY_TIME,
		PARAM_MODE,
		PARAM_POS,
		PARAM_POS_MOD_1,
		PARAM_POS_MOD_2,
		PARAM_SPEED,
		PARAM_SPEED_MOD_1,
		PARAM_SPEED_MOD_2,
		PARAM_GRAIN,
		PARAM_GRAIN_MOD_1,
		PARAM_GRAIN_MOD_2,
		PARAM_LVL,
		PARAM_LVL_MOD_1,
		PARAM_LVL_MOD_2,
		//PARAM_ENV_TIME,
		//PARAM_ENV_SPEED,
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_L,
		INPUT_R,
		INPUT_POS_1,
		INPUT_POS_2,
		INPUT_SPEED_1,
		INPUT_SPEED_2,
		INPUT_GRAIN_1,
		INPUT_GRAIN_2,
		INPUT_LVL_1,
		INPUT_LVL_2,
		//INPUT_ENV_TRIGGER,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_L,
		OUTPUT_R,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_MODE_POS_REL,
		LIGHT_MODE_POS_ABS,
		LIGHT_MODE_SPEED,
		LIGHT_MODE_GRAIN,
		LIGHT_COUNT
	};

	dsp::TSchmittTrigger<float>	trigger_env[16];
	IgcPlayhead					playheads[16];
	int							playhead_count;

	float						audio[2][IGC_BUFFER];
	int							audio_index;

	Igc();

	void	process(const ProcessArgs& args) override;
};

struct IgcWidget : ModuleWidget {
	Igc							*module;

	IgcWidget(Igc* _module);
};

struct IgcDisplay : LedDisplay {
	Igc							*module;
	ModuleWidget				*moduleWidget;

	IgcDisplay();
	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs &args, int layer) override;
};

#endif
