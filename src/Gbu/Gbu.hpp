
#ifndef GBU_HPP
#define GBU_HPP

#include "../plugin.hpp"

#define GBU_RESOLUTION	2048

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Gbu: Module {
	enum	ParamIds {
		PARAM_FREQ_GLOBAL,
		PARAM_FREQ_1,
		PARAM_FREQ_2,
		PARAM_FREQ_3,
		PARAM_LEVEL_1,
		PARAM_LEVEL_2,
		PARAM_LEVEL_3,
		PARAM_FEEDBACK_1,
		PARAM_FEEDBACK_2,
		PARAM_FEEDBACK_3,
		PARAM_PM_1_2,				// PM Good to Bad
		PARAM_PM_2_1,				// PM Bad to Good
		PARAM_PM_3_1,				// PM Ugly to Good
		PARAM_PM_3_2,				// PM Ugly to Bad
		PARAM_RM_1_2_1,				// RM Good and Bad
		PARAM_RM_3_1,				// RM Ugly to Good
		PARAM_RM_3_2,				// RM Ugly to Bad
		PARAM_RM_MODE,				// RM / AM mix
		PARAM_NOISE_SPEED,			// Ugly noise speed
		PARAM_NOISE_AMP,			// Ugly noise amplitude
		PARAM_FOLLOW_ATTRACTION,	// Ugly pitch following attraction force
		PARAM_FOLLOW_FRICTION,		// Ugly pitch following friction
		PARAM_COUNT
	};
	enum	InputIds {
		INPUT_PITCH,
		INPUT_LEVEL_1,
		INPUT_LEVEL_2,
		INPUT_LEVEL_3,
		INPUT_FEEDBACK_1,
		INPUT_FEEDBACK_2,
		INPUT_FEEDBACK_3,
		INPUT_PM_1_2,
		INPUT_PM_2_1,
		INPUT_PM_3_1,
		INPUT_PM_3_2,
		INPUT_RM_1_2_1,
		INPUT_RM_3_1,
		INPUT_RM_3_2,
		INPUT_RM_MODE,
		INPUT_COUNT
	};
	enum	OutputIds {
		OUTPUT_MIX,
		OUTPUT_EXTRA,
		OUTPUT_COUNT
	};
	enum	LightIds {
		LIGHT_COUNT
	};

	float	wave[GBU_RESOLUTION];
	float	phase_1[16];
	float	phase_2[16];
	float	phase_3[16];
	float	out_1[16];
	float	out_2[16];
	float	out_3[16];
	float	pitch_3[16];
	float	pitch_3_acc[16];
	float	pitch_3_noise_phase[16];

	Gbu();

	void	process(const ProcessArgs& args) override;
};

struct GbuWidget : ModuleWidget {
	Gbu			*module;

	GbuWidget(Gbu* _module);
};

#endif