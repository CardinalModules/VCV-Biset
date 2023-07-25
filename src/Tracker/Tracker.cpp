
#include "Tracker.hpp"

Timeline		*g_timeline = NULL;
Editor			*g_editor = NULL;
Tracker			*g_module = NULL;

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Tracker::Tracker() {
	int	i;

	config(PARAM_COUNT, INPUT_COUNT, OUTPUT_COUNT, LIGHT_COUNT);

	configButton(PARAM_PLAY_SONG, "Play song");
	configButton(PARAM_PLAY_PATTERN, "Play pattern");
	configButton(PARAM_PLAY, "Play");
	configButton(PARAM_STOP, "Stop");

	configParam(PARAM_BPM, 30.0f, 300.0f, 120.f, "BPM");
	configParam(PARAM_SYNTH, 0.0f, 63.0f, 0.f, "Synth")->snapEnabled = true;
	configParam(PARAM_PATTERN, 0.0f, 255.0f, 0.f, "Pattern")->snapEnabled = true;

	configSwitch(PARAM_VIEW, 0, 1, 0, "View Velocity");
	configSwitch(PARAM_VIEW + 1, 0, 1, 0, "View Panning");
	configSwitch(PARAM_VIEW + 2, 0, 1, 0, "View Delay");
	configSwitch(PARAM_VIEW + 3, 0, 1, 0, "View Glide");
	configSwitch(PARAM_VIEW + 4, 0, 1, 0, "View Effects");

	configParam(PARAM_SONG_LENGTH, 1.0f, 9999.0f, 0.0f, "Song length", " beats")->snapEnabled = true;
	configParam(PARAM_SYNTH_CHANNEL_COUNT, 1.0f, 16.0f, 0.0f, "Synth channels")->snapEnabled = true;
	configParam(PARAM_SYNTH_MODE, 0.0f, 2.0f, 0.0f, "Synth mode")->snapEnabled = true;
	configParam(PARAM_PATTERN_LENGTH, 1.0f, 999.0f, 0.0f, "Pattern length", " beats")->snapEnabled = true;
	configParam(PARAM_PATTERN_LPB, 1.0f, 32.0f, 0.0f, "Pattern lpb", " lines / beat")->snapEnabled = true;
	configParam(PARAM_PATTERN_NOTE_COUNT, 0.0f, 32.0f, 0.0f, "Pattern notes", " columns")->snapEnabled = true;
	configParam(PARAM_PATTERN_CV_COUNT, 0.0f, 32.0f, 0.0f, "Pattern cv", " columns")->snapEnabled = true;
	configParam(PARAM_COLUMN_NOTE_EFFECT_COUNT, 0.0f, 16.0f, 0.0f, "Column effects", "")->snapEnabled = true;
	configParam(PARAM_COLUMN_CV_MODE, 0.0f, 3.0f, 0.0f, "Column mode", "")->snapEnabled = true;
	configParam(PARAM_COLUMN_CV_SYNTH, 0.0f, 63.0f, 0.0f, "Column synth", "")->snapEnabled = true;
	configParam(PARAM_COLUMN_CV_CHANNEL, 0.0f, 7.0f, 0.0f, "Column channel", "")->snapEnabled = true;

	configParam(PARAM_COLUMN_FX_VELOCITY, 0.0f, 100.0f, 0.0f, "Random velocity", "%");
	configParam(PARAM_COLUMN_FX_PANNING, 0.0f, 100.0f, 0.0f, "Random panning", "%");
	configParam(PARAM_COLUMN_FX_OCTAVE_MODE, 0.0f, 2.0f, 0.0f, "Random octave mode", "")->snapEnabled = true;
	configParam(PARAM_COLUMN_FX_OCTAVE, 0.0f, 4.0f, 0.0f, "Random octave", "")->snapEnabled = true;
	configParam(PARAM_COLUMN_FX_PANNING, 0.0f, 100.0f, 0.0f, "Random pitch", "%");
	configParam(PARAM_COLUMN_FX_DELAY, 0.0f, 100.0f, 0.0f, "Random delay", "%");
	configParam(PARAM_COLUMN_FX_CHANCE, 0.0f, 100.0f, 0.0f, "Random chance", "%");

	configButton(PARAM_MODE + 0, "Mode pattern");
	configButton(PARAM_MODE + 1, "Mode timeline");
	configButton(PARAM_MODE + 2, "Mode parameters");

	configButton(PARAM_OCTAVE_UP, "Octave +");
	configButton(PARAM_OCTAVE_DOWN, "Octave -");
	configButton(PARAM_JUMP_UP, "Jump +");
	configButton(PARAM_JUMP_DOWN, "Jump -");

	configParam(PARAM_PITCH_BASE, 400.0f, 500.0f, 440.0f, "Base Pitch");
	configParam(PARAM_RATE, 1, 512, 64, "Rate");
	for (i = 0; i < 12; ++i)
		configParam(PARAM_TEMPERAMENT + i, 0.0f, 12.0f, i * 1.0f, table_pitch[i]);

	configLight(LIGHT_FOCUS, "Focus");
	configLight(LIGHT_PLAY, "Play");

	configOutput(OUTPUT_CLOCK, "Clock");

	/// DEFINE GLOBAL KEYBOARD
	for (i = 0; i < 128; ++i)
		table_keyboard[i] = -1;
	//// OCTAVE LOW 1
	table_keyboard['Z'] = 0;		// C
		table_keyboard['S'] = 1;	// C#
	table_keyboard['X'] = 2;		// D
		table_keyboard['D'] = 3;	// D#
	table_keyboard['C'] = 4;		// E
	table_keyboard['V'] = 5;		// F
		table_keyboard['G'] = 6;	// F#
	table_keyboard['B'] = 7;		// G
		table_keyboard['H'] = 8;	// G#
	table_keyboard['N'] = 9;		// A
		table_keyboard['J'] = 10;	// A#
	table_keyboard['M'] = 11;		// B
	//// OCTAVE LOW 2
	table_keyboard[','] = 12;		// C
		table_keyboard['L'] = 13;	// C#
	table_keyboard['.'] = 14;		// D
		table_keyboard[';'] = 15;	// D#
	table_keyboard['/'] = 16;		// E
	//// OCTAVE HIGH 2
	table_keyboard['Q'] = 12;		// C
		table_keyboard['2'] = 13;	// C#
	table_keyboard['W'] = 14;		// D
		table_keyboard['3'] = 15;	// D#
	table_keyboard['E'] = 16;		// E
	table_keyboard['R'] = 17;		// F
		table_keyboard['5'] = 18;	// F#
	table_keyboard['T'] = 19;		// G
		table_keyboard['6'] = 20;	// G#
	table_keyboard['Y'] = 21;		// A
		table_keyboard['7'] = 22;	// A#
	table_keyboard['U'] = 23;		// B
	//// OCTAVE HIGH 3
	table_keyboard['I'] = 24;		// C
		table_keyboard['9'] = 25;	// C#
	table_keyboard['O'] = 26;		// D
		table_keyboard['0'] = 27;	// D#
	table_keyboard['P'] = 28;		// E
	table_keyboard['['] = 29;		// F
		table_keyboard['='] = 30;	// F#
	table_keyboard[']'] = 31;		// G

	/// SET GLOBAL STRUCTURES
	if (g_module == NULL)
		g_module = this;
	if (g_timeline == NULL) {
		g_timeline = new Timeline();
		g_editor = new Editor();
	}
}

Tracker::~Tracker() {
	if (g_module == this)
		g_module = NULL;
}

void Tracker::process(const ProcessArgs& args) {
	float	dt_sec, dt_beat;
	float	bpm;

	/// PROCESS EDITOR
	g_editor->process(args.frame);

	/// COMPUTE CLOCK
	bpm = params[PARAM_BPM].getValue();
	dt_sec = args.sampleTime;
	dt_beat = (bpm * dt_sec) / 60.0f;

	/// PROCESS TIMELINE
	g_timeline->process(args.frame, dt_sec, dt_beat);
}

Model* modelTracker = createModel<Tracker, TrackerWidget>("Biset-Tracker");
