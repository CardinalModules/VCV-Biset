
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

Synth::Synth() {
	strcpy(this->name, "synth");
	this->color = 1;
	this->index = 0;
	this->channel_cur = 0;
	this->channel_count = 1;
}

void Synth::init(void) {
	int			index;
	int			i, j;

	/// TODO: INIT NAME & COLOR ?
	/// INIT INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline.synths) / sizeof(Synth);
	this->index = index;
	/// INIT NAME
	this->rename((char*)"synth");
	/// INIT COLOR
	this->color = 1;
	/// INIT VOICES
	for (i = 0; i < 16; ++i)
		this->voices[i].init(i);
	/// INIT OUTPUTS
	for (i = 0; i < 16; ++i)
		for (j = 0; j < 4; ++j)
			this->out_synth[i * 4 + j] = 0.0;
	for (i = 0; i < 8; ++i)
			this->out_cv[i] = 0.0;
	/// SET CHANNEL COUNT
	this->channel_count = 1;
}

void Synth::process(float dt_sec, float dt_beat) {
	int			i;

	// -> ! ! ! BOTTLENECK ! ! !
	// -> Call only if active ? On de-activation -> set output to 0
	/// COMPUTE VOICES
	for (i = 0; i < this->channel_count; ++i)
		this->voices[i].process(dt_sec, dt_beat, this->out_synth);
}

void Synth::rename(void) {
	int		index;

	/// UPDATE SYNTH INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline.synths) / sizeof(Synth);
	itoaw(this->name, index, 2);
	this->name[2] = ' ';
}

void Synth::rename(char *name) {
	int		index;

	/// UPDATE SYNTH INDEX
	index = ((intptr_t)this - (intptr_t)g_timeline.synths) / sizeof(Synth);
	itoaw(this->name, index, 2);
	strcpy(this->name + 2, " - ");
	strncpy(this->name + 5, name, 255);
}

SynthVoice* Synth::add(PatternNoteCol *col, PatternNote *note, int lpb) {
	Synth			*synth;
	SynthVoice		*voice;

	synth = &(g_timeline.synths[note->synth]);
	/// MODE DRUM
	if (synth->mode == SYNTH_MODE_DRUM) {
		voice = &(this->voices[note->pitch % 12]);
		if (voice->start(synth, col, note, lpb) == true)
			return voice;
	/// MODE GATE + TRIGGER
	} else {
		voice = &(this->voices[this->channel_cur]);
		if (voice->start(synth, col, note, lpb) == true) {
			this->channel_cur = (this->channel_cur + 1) % this->channel_count;
			return voice;
		}
	}
	return NULL;
}