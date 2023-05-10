
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

SynthVoice::SynthVoice() {
	this->reset();
}

void SynthVoice::process(
	float					dt_sec,
	float					dt_beat,
	float					*output) {
	float					pitch, mix;
	float					velocity;
	float					panning;
	float					vibrato, tremolo;

	// -> ! ! ! BOTTLENECK ! ! !

	/// ON NOTE PLAY
	if (this->active && this->delay_start <= 0 && this->delay_gate <= 0) {
		/// COMPUTE CV
		velocity = (float)this->velocity / 99.0 * 10.0;
		panning = (float)this->panning / 99.0 * 10.0;
		/// COMPUTE EFFECTS
		//// COMPUTE GLIDE
		if (this->pitch_glide_len > 0) {
			this->pitch_glide_cur += dt_beat;
			/// GLIDE ENDED
			if (this->pitch_glide_cur >= this->pitch_glide_len) {
				this->pitch_glide_len = 0;
				this->pitch_glide_cur = 0;
				this->pitch_from = this->pitch_to;
				pitch = (float)(this->pitch_from - 69.0) / 12.0f;
			/// GLIDE RUNNING
			} else {
				mix = this->pitch_glide_cur / this->pitch_glide_len;
				pitch = ((this->pitch_from * (1.0 - mix)
				/**/ + this->pitch_to * mix) - 69.0) / 12.0f;
			}
		} else {
			pitch = (float)(this->pitch_from - 69) / 12.0f;
		}
		//// COMPUTE VIBRATO
		if (this->vibrato_amp > 0) {
			this->vibrato_phase += this->vibrato_freq * dt_beat;
			vibrato = sinf(this->vibrato_phase) * this->vibrato_amp;
			pitch += vibrato;
		}
		//// COMPUTE TREMOLO
		if (this->tremolo_amp > 0) {
			this->tremolo_phase += this->tremolo_freq * dt_beat;
			tremolo = 0.5f + sinf(this->tremolo_phase) * 0.5f
			/**/ * this->tremolo_amp;
			velocity *= tremolo;
		}
		/// COMPUTE STOPPING DELAY
		if (this->delay_stop > 0) {
			this->delay_stop -= dt_beat;
			if (this->delay_stop < 0)
				this->active = false;
		}
		/// SET OUTPUT (PITCH + GATE + VELOCITY + PANNING)
		output[this->channel * 4 + 0] = pitch + g_timeline.pitch_base_offset;
		output[this->channel * 4 + 1] = 10.0f;
		output[this->channel * 4 + 2] = velocity;
		output[this->channel * 4 + 3] = panning;
	/// ON NOTE DELAY
	} else {
		/// COMPUTE NOTE STARTING DELAY
		if (this->delay_start > 0)
			this->delay_start -= dt_beat;
		/// COMPUTE INTER NOTES GATE DELAY
		if (this->delay_gate > 0)
			this->delay_gate -= dt_sec;
		/// SET OUTPUT (GATE)
		output[this->channel * 4 + 1] = 0.0f;
	}
}

bool SynthVoice::start(
	Synth					*synth,
	PatternNoteRow			*row,
	PatternNote				*note,
	int						lpb) {
	PatternEffect			*effect;
	int						i;
	int						x, y;
	int						int_1, int_2;
	int						pitch;
	float					pitch_real;
	float					float_1;

	pitch = note->pitch;
	/// SET DELAY
	this->delay_gate = 0.001f;
	this->delay_start = 0.0f;
	this->delay_stop = 0;
	if (synth->mode == SYNTH_MODE_TRIGGER
	|| synth->mode == SYNTH_MODE_DRUM)
		this->delay_stop = 0.001f;
	/// SET BASICS
	this->velocity = note->velocity;
	this->panning = note->panning;
	/// SET EFFECTS
	this->vibrato_amp = 0;
	this->tremolo_amp = 0;
	for (i = 0; i < row->effect_count; ++i) {
		effect = &(note->effects[i]);
		switch(effect->type) {
			case PATTERN_EFFECT_NONE:
				break;
			case PATTERN_EFFECT_RAND_AMP:		// Axx
				float_1 = random::uniform() * (effect->value / 99.0);
				this->velocity *= 1.0 - float_1;
				break;
			case PATTERN_EFFECT_RAND_PAN:		// Pxx
				int_1 = (random::uniform() * 2.0 - 1.0) * (effect->value / 2.0);
				int_2 = (int)this->panning + int_1;
				if (int_2 < 0)
					int_2 = 0;
				if (int_2 > 99)
					int_2 = 99;
				this->panning = int_2;
				break;
			case PATTERN_EFFECT_RAND_DELAY:		// Dxx
				float_1 = random::uniform() * (effect->value / 99.0);
				this->delay_start = float_1 / (float)lpb;
				break;
			case PATTERN_EFFECT_RAND_OCT:		// Oxy
				x = effect->value / 10;
				y = effect->value % 10;
				if (y > 0) {
					if (x == 0)					// > -+
						pitch += (random::u32() % (2 * y) - y) * 12;
					else if (x == 1)			// > +
						pitch += (random::u32() % (y + 1)) * 12;
					else						// > -
						pitch += -(random::u32() % (y + 1)) * 12;
				}
				break;
			case PATTERN_EFFECT_RAND_PITCH:		// Mxy
				x = effect->value / 10;
				y = effect->value % 10;
				int_1 = random::u32() % 3;
				if (int_1 == 0)
					;
				else if (int_1 == 1)
					pitch += x;
				else
					pitch += y;
				break;
			case PATTERN_EFFECT_RAND_SCALE:		// Sxy
				break;
			case PATTERN_EFFECT_VIBRATO:		// Vxy
				this->vibrato_freq =
				/**/ (float)(effect->value / 10) * M_PI * 2.0f;
				this->vibrato_amp = (float)(effect->value % 10) / 128.0;
				break;
			case PATTERN_EFFECT_TREMOLO:		// Txy
				this->tremolo_freq =
				/**/ (float)(effect->value / 10) * M_PI * 2.0f;
				this->tremolo_amp = (float)(effect->value % 10) / 32.0;
				break;
			case PATTERN_EFFECT_FADE_IN:		// Fxx
				break;
			case PATTERN_EFFECT_FADE_OUT:		// fxx
				break;
			case PATTERN_EFFECT_CHANCE:			// Cxx
				if (random::uniform() * 99.0 > effect->value)
					return false;
				break;
			case PATTERN_EFFECT_CHANCE_STOP:	// cxx
				break;
			case PATTERN_EFFECT_RACHET:
				break;
		}
	}
	/// SET PITCH
	pitch_real = g_timeline.pitch_scale[pitch % 12] + (pitch / 12) * 12;
	this->pitch_from = pitch_real;
	this->pitch_to = pitch_real;
	this->pitch_glide_len = 0;
	this->pitch_glide_cur = 0;
	/// ACTIVATE VOICE
	this->active = true;
	return true;
}

void SynthVoice::glide(
	PatternNote				*note) {
	float					pitch;
	float					mix;

	if (this->active) {
		/// GLIDE ALREADY RUNNING
		if (this->pitch_glide_len > 0) {
			/// SET GLIDE STARTING PITCH
			mix = this->pitch_glide_cur / this->pitch_glide_len;
			this->pitch_from = this->pitch_from * (1.0 - mix)
			/**/ + this->pitch_to * mix;
		/// GLIDE OFF
		} else {
		}
		/// SET GLIDE ENDING PITCH
		pitch = g_timeline.pitch_scale[note->pitch % 12] 
		/**/ + (note->pitch / 12) * 12;
		this->pitch_to = pitch;
		this->pitch_glide_len = (1.0 - ((float)note->glide / 100.0));
		this->pitch_glide_cur = 0;
	}
}

void SynthVoice::stop(void) {
	this->active = false;
}

void SynthVoice::init(int synth, int channel) {
	this->reset();
	this->synth = synth;
	this->channel = channel;
}

void SynthVoice::reset() {
	this->active = false;
	this->velocity = 99;
	this->pitch_from = 0.0;
	this->pitch_to = 0.0;
	this->pitch_glide_len = 0;
	this->pitch_glide_cur = 0.0;
	this->delay_start = 0;
	this->vibrato_amp = 0;
	this->vibrato_freq = 0;
	this->vibrato_phase = 0.0;
	this->tremolo_amp = 0;
	this->tremolo_freq = 0;
	this->tremolo_phase = 0.0;
}
