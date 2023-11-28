
#include "Gbu.hpp"

#define GBU_WAVE(f)	this->wave[((((int)(f * GBU_RESOLUTION) % GBU_RESOLUTION) \
						+ GBU_RESOLUTION) % GBU_RESOLUTION)]

//#define GBU_WAVE(f)	(std::sin(2.0 * M_PI * f))

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Gbu::Gbu() {
	int		i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configInput(INPUT_PITCH, "V/Oct");
	configInput(INPUT_LEVEL_1, "Level Good");
	configInput(INPUT_LEVEL_2, "Level Bad");
	configInput(INPUT_LEVEL_3, "Level Ugly");
	configInput(INPUT_FEEDBACK_1, "Feedback Good");
	configInput(INPUT_FEEDBACK_2, "Feedback Bad");
	configInput(INPUT_FEEDBACK_3, "Feedback Ugly");
	configInput(INPUT_PM_1_2, "PM Good -> Bad");
	configInput(INPUT_PM_2_1, "PM Bad -> Good");
	configInput(INPUT_PM_3_1, "PM Ugly -> Good");
	configInput(INPUT_PM_3_2, "PM Ugly -> Bad");
	configInput(INPUT_RM_1_2_1, "AM Good <-> Bad");
	configInput(INPUT_RM_3_1, "AM Ugly -> Good");
	configInput(INPUT_RM_3_2, "AM Ugly -> Bad");
	configInput(INPUT_RM_MODE, "RM / AM");

	configOutput(OUTPUT_MIX, "Mix");
	configOutput(OUTPUT_EXTRA, "Extra");

	configParam(PARAM_FREQ_GLOBAL, -9, +9, 0, "Freq");
	configParam(PARAM_FREQ_1, -9, +9, 0, "Freq Good");
	configParam(PARAM_FREQ_2, -9, +9, 0, "Freq Bad");
	configParam(PARAM_FREQ_3, -9, +9, 0, "Freq Ugly");

	configParam(PARAM_LEVEL_1, 0, 1, 1, "Level Good");
	configParam(PARAM_LEVEL_2, 0, 1, 0, "Level Bad");
	configParam(PARAM_LEVEL_3, 0, 1, 0, "Level Ugly");

	configParam(PARAM_FEEDBACK_1, 0, 1, 0, "Feedback Good");
	configParam(PARAM_FEEDBACK_2, 0, 1, 0, "Feedback Bad");
	configParam(PARAM_FEEDBACK_3, 0, 1, 0, "Feedback Ugly");

	configParam(PARAM_RM_MODE, 0, 1, 1, "RM / AM");
	configParam(PARAM_PM_1_2, 0, 1, 0, "PM Good -> Bad");
	configParam(PARAM_PM_2_1, 0, 1, 0, "PM Bad -> Good");
	configParam(PARAM_PM_3_1, 0, 1, 0, "PM Ugly -> Good");
	configParam(PARAM_PM_3_2, 0, 1, 0, "PM Ugly -> Bad");
	configParam(PARAM_RM_1_2_1, -1, +1, 0, "RM Good <-> Bad");
	configParam(PARAM_RM_3_1, 0, 1, 0, "RM Ugly -> Good");
	configParam(PARAM_RM_3_2, 0, 1, 0, "RM Ugly -> Bad");

	configParam(PARAM_NOISE_SPEED, 0, 1, 0.3, "Ugly noise speed");
	configParam(PARAM_NOISE_AMP, 0, 1, 0.5, "Ugly noise amplitude");
	configParam(PARAM_FOLLOW_ATTRACTION, 0, 1, 0.5, "Ugly attraction");
	configParam(PARAM_FOLLOW_FRICTION, 0, 1, 0.5, "Ugly friction");

	for (i = 0; i < 16; ++i) {
		this->phase_1[i] = 0.0;
		this->phase_2[i] = 0.0;
		this->phase_3[i] = 0.0;
		this->out_1[i] = 0.0;
		this->out_2[i] = 0.0;
		this->out_3[i] = 0.0;
		this->pitch_3[i] = 0.0;
		this->pitch_3_acc[i] = 0.0;
		this->pitch_3_noise_phase[i] = (float)i * 12.345;
	}

	for (i = 0; i < GBU_RESOLUTION; ++i)
		this->wave[i] = std::sin(2.0 * M_PI * (i / (float)GBU_RESOLUTION));
}

void Gbu::process(const ProcessArgs& args) {
	// p_x	value on knob
	// x	value on knob * input
	float	p_mod_pm_1_2;
	float	p_mod_pm_2_1;
	float	p_mod_pm_3_1;
	float	p_mod_pm_3_2;
	float	mod_pm_1_2;
	float	mod_pm_2_1;
	float	mod_pm_3_1;
	float	mod_pm_3_2;
	float	mod_rm;
	float	p_mod_rm_mode;
	float	mod_rm_mode;
	float	mod_rm_mul;
	float	mod_rm_add;
	float	p_mod_rm_1_2;
	float	p_mod_rm_2_1;
	float	p_mod_rm_3_1;
	float	p_mod_rm_3_2;
	float	mod_rm_1_2;
	float	mod_rm_2_1;
	float	mod_rm_3_1;
	float	mod_rm_3_2;
	float	p_mod_feedback_1;
	float	p_mod_feedback_2;
	float	p_mod_feedback_3;
	float	mod_feedback_1;
	float	mod_feedback_2;
	float	mod_feedback_3;
	float	noise_speed;
	float	noise_amp;
	float	follow_attraction;
	float	follow_friction;
	float	pitch_global;
	float	pitch_main;
	float	p_pitch_1;
	float	p_pitch_2;
	float	p_pitch_3;
	float	pitch_1;
	float	pitch_2;
	float	pitch_3;
	float	pitch_3_aim;
	float	p_level_1;
	float	p_level_2;
	float	p_level_3;
	float	level_1;
	float	level_2;
	float	level_3;
	float	freq;
	float	phase;
	float	out;
	float	out_level;
	int		channel_count;
	int		i;

	/// [1] GET PARAMETERS
	//// MODULATION RING
	mod_rm = this->params[PARAM_RM_1_2_1].getValue();
	if (mod_rm >= 0) {
		p_mod_rm_1_2 = mod_rm;
		p_mod_rm_2_1 = 0.0;
	} else {
		p_mod_rm_1_2 = 0.0;
		p_mod_rm_2_1 = -mod_rm;
	}
	p_mod_rm_3_1 = this->params[PARAM_RM_3_1].getValue();
	p_mod_rm_3_2 = this->params[PARAM_RM_3_2].getValue();
	p_mod_rm_mode = this->params[PARAM_RM_MODE].getValue();
	//// MODULATION FREQUENCY
	p_mod_pm_2_1 = this->params[PARAM_PM_2_1].getValue();
	p_mod_pm_3_1 = this->params[PARAM_PM_3_1].getValue();
	p_mod_pm_1_2 = this->params[PARAM_PM_1_2].getValue();
	p_mod_pm_3_2 = this->params[PARAM_PM_3_2].getValue();
	//// MODULATION FEEDBACK
	p_mod_feedback_1 = this->params[PARAM_FEEDBACK_1].getValue();
	p_mod_feedback_2 = this->params[PARAM_FEEDBACK_2].getValue();
	p_mod_feedback_3 = this->params[PARAM_FEEDBACK_3].getValue();
	//// LEVEL
	p_level_1 = this->params[PARAM_LEVEL_1].getValue();
	p_level_2 = this->params[PARAM_LEVEL_2].getValue();
	p_level_3 = this->params[PARAM_LEVEL_3].getValue();
	//// PITCH
	pitch_global = this->params[PARAM_FREQ_GLOBAL].getValue();
	p_pitch_1 = pitch_global + this->params[PARAM_FREQ_1].getValue();
	p_pitch_2 = pitch_global + this->params[PARAM_FREQ_2].getValue();
	p_pitch_3 = pitch_global + this->params[PARAM_FREQ_3].getValue();
	//// NOISE
	noise_speed = this->params[PARAM_NOISE_SPEED].getValue();
	noise_amp = this->params[PARAM_NOISE_AMP].getValue();
	//// PITCH FOLLOWING
	follow_attraction = this->params[PARAM_FOLLOW_ATTRACTION].getValue()
	/**/ * (0.01 - 0.0005) + 0.0005;
	follow_friction = (1.0 - this->params[PARAM_FOLLOW_FRICTION].getValue())
	/**/ * (0.9999 - 0.999) + 0.999;

	channel_count = this->inputs[INPUT_PITCH].getChannels();
	if (channel_count == 0)
		channel_count = 1;
	this->outputs[OUTPUT_MIX].setChannels(channel_count);
	this->outputs[OUTPUT_EXTRA].setChannels(channel_count);
	out_level = p_level_1 + p_level_2 + p_level_3;

	/// FOR EACH CHANNEL
	for (i = 0; i < channel_count; ++i) {

		out = 0.0;

		/// [2] COMPUTE MODULATION
		//// PITCH
		pitch_main = this->inputs[INPUT_PITCH].getVoltage(i);
		pitch_1 = p_pitch_1 + pitch_main;
		pitch_2 = p_pitch_2 + pitch_main;
		pitch_3 = p_pitch_3 + pitch_main;
		//// MODULATION RING
		mod_rm = this->inputs[INPUT_RM_1_2_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_1_2 = p_mod_rm_1_2 * mod_rm;
		mod_rm_2_1 = p_mod_rm_2_1 * mod_rm;
		mod_rm_3_1 = p_mod_rm_3_1
		/**/ * this->inputs[INPUT_RM_3_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_3_2 = p_mod_rm_3_2
		/**/ * this->inputs[INPUT_RM_3_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_mode = p_mod_rm_mode
		/**/ * this->inputs[INPUT_RM_MODE].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_rm_mul = (1.0 - mod_rm_mode * 0.5);
		mod_rm_add = mod_rm_mode * 0.5;
		//// MODULATION PHASE
		mod_pm_1_2 = p_mod_pm_1_2
		/**/ * this->inputs[INPUT_PM_1_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_2_1 = p_mod_pm_2_1
		/**/ * this->inputs[INPUT_PM_2_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_3_1 = p_mod_pm_3_1
		/**/ * this->inputs[INPUT_PM_3_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_pm_3_2 = p_mod_pm_3_2
		/**/ * this->inputs[INPUT_PM_3_2].getNormalPolyVoltage(10.0, i) / 10.0;
		//// MODULATION FEEDBACK
		mod_feedback_1 = p_mod_feedback_1
		/**/ * this->inputs[INPUT_FEEDBACK_1].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_2 = p_mod_feedback_2
		/**/ * this->inputs[INPUT_FEEDBACK_2].getNormalPolyVoltage(10.0, i) / 10.0;
		mod_feedback_3 = p_mod_feedback_3
		/**/ * this->inputs[INPUT_FEEDBACK_3].getNormalPolyVoltage(10.0, i) / 10.0;
		//// MODULATION LEVEL
		level_1 = p_level_1
		/**/ * this->inputs[INPUT_LEVEL_1].getNormalPolyVoltage(10.0, i) / 10.0;
		level_2 = p_level_2
		/**/ * this->inputs[INPUT_LEVEL_2].getNormalPolyVoltage(10.0, i) / 10.0;
		level_3 = p_level_3
		/**/ * this->inputs[INPUT_LEVEL_3].getNormalPolyVoltage(10.0, i) / 10.0;

		/// [3] COMPUTE GOOD
		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * std::pow(2.f, pitch_1);
		//// COMPUTE PHASE
		this->phase_1[i] += freq * args.sampleTime;
		while (this->phase_1[i] >= 1.0)
			this->phase_1[i] -= 1.0;
		//// COMPUTE PHASE MODULATION
		phase = this->phase_1[i]
		///// MODULATION FREQUENCY (FROM BAD)
		/**/ + (this->out_2[i] * 0.5 + 0.5) * mod_pm_2_1 * 5.0
		///// MODULATION FREQUENCY (FROM UGLY)
		/**/ + (this->out_3[i] * 0.5 + 0.5) * mod_pm_3_1 * 5.0
		///// MODULATION FEEDBACK
		/**/ + (this->out_1[i] * 0.5 + 0.5) * mod_feedback_1;
		//// COMPUTE OUTPUT
		this->out_1[i] = GBU_WAVE(phase);
		//// COMPUTE AMPLITUDE MODULATION
		this->out_1[i] *=
		///// MODULATION AMPLITUDE / RING (FROM BAD)
		/**/ ((1.0 - mod_rm_2_1)
		/**/ + ((this->out_2[i] * mod_rm_mul + mod_rm_add) * mod_rm_2_1))
		///// MODULATION AMPLITUDE / RING (FROM UGLY)
		/**/ * ((1.0 - mod_rm_3_1)
		/**/ + ((this->out_3[i] * mod_rm_mul + mod_rm_add) * mod_rm_3_1));
		//// SEND OUTPUT
		out += this->out_1[i] * level_1;

		/// [4] COMPUTE BAD
		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * std::pow(2.f, pitch_2);
		//// COMPUTE PHASE
		this->phase_2[i] += freq * args.sampleTime;
		while (this->phase_2[i] >= 1.0)
			this->phase_2[i] -= 1.0;
		//// COMPUTE PHASE MODULATION
		phase = this->phase_2[i]
		///// MODULATION FREQUENCY (FROM GOOD)
		/**/ + (this->out_1[i] * 0.5 + 0.5) * mod_pm_1_2 * 5.0
		///// MODULATION FREQUENCY (FROM UGLY)
		/**/ + (this->out_3[i] * 0.5 + 0.5) * mod_pm_3_2 * 5.0
		///// MODULATION FEEDBACK
		/**/ + (this->out_2[i] * 0.5 + 0.5) * mod_feedback_2;
		//// COMPUTE OUTPUT
		this->out_2[i] = GBU_WAVE(phase);
		//// COMPUTE AMPLITUDE MODULATION
		this->out_2[i] *=
		///// MODULATION RING (FROM BAD)
		/**/ ((1.0 - mod_rm_1_2)
		/**/ + ((this->out_1[i] * mod_rm_mul + mod_rm_add) * mod_rm_1_2))
		///// MODULATION RING (FROM UGLY)
		/**/ * ((1.0 - mod_rm_3_2)
		/**/ + ((this->out_3[i] * mod_rm_mul + mod_rm_add) * mod_rm_3_2));
		//// SEND OUTPUT
		out += this->out_2[i] * level_2;

		/// [5] COMPUTE UGLY
		//// COMPUTE PITCH MOVEMENT
		pitch_3_aim = pitch_3;
		if (this->pitch_3[i] > pitch_3)
			this->pitch_3_acc[i] -= args.sampleTime * follow_attraction;
		else
			this->pitch_3_acc[i] += args.sampleTime * follow_attraction;
		this->pitch_3_acc[i] *= follow_friction;
		this->pitch_3[i] += this->pitch_3_acc[i];
		pitch_3 = this->pitch_3[i];
		//// COMPUTE PITCH RANDOM MOVEMENT
		pitch_3 += noise_amp * 0.3 *
		/**/ ((GBU_WAVE(this->pitch_3_noise_phase[i])
		/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 3.152)
		/**/ * GBU_WAVE(this->pitch_3_noise_phase[i] * 4.936)) * 0.5
		/**/ + GBU_WAVE(this->pitch_3_noise_phase[i] * 24.967 + 0.5) * 0.05);
		this->pitch_3_noise_phase[i] += args.sampleTime * 5.0 * noise_speed;
		//// COMPUTE FREQUENCY (Hz)
		freq = dsp::FREQ_C4 * std::pow(2.f, pitch_3);
		//// COMPUTE PHASE
		this->phase_3[i] += freq * args.sampleTime;
		while (this->phase_3[i] >= 1.0)
			this->phase_3[i] -= 1.0;
		//// COMPUTE PHASE MODULATION
		phase = this->phase_3[i]
		///// MODULATION FEEDBACK
		/**/ + (this->out_3[i] * 0.5 + 0.5) * mod_feedback_3;
		//// COMPUTE OUTPUT
		this->out_3[i] = GBU_WAVE(phase);
		//// SEND OUTPUT
		out += this->out_3[i] * level_3;
		this->outputs[OUTPUT_EXTRA].setVoltage(pitch_3_aim - pitch_3, i);

		if (out_level > 1.0)
			out /= out_level;
		this->outputs[OUTPUT_MIX].setVoltage(out * 5.0, i);
	}
}

Model* modelGbu = createModel<Gbu, GbuWidget>("Biset-Gbu");