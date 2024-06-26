
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "../plugin.hpp"

////////////////////////////////////////////////////////////////////////////////
/// CONSTANTS
////////////////////////////////////////////////////////////////////////////////

#define EDITOR_MODE_PATTERN				0
#define EDITOR_MODE_TIMELINE			1
#define EDITOR_MODE_MATRIX				2
#define EDITOR_MODE_TUNING				3

#define PATTERN_EFFECT_NONE				0
#define PATTERN_EFFECT_RAND_AMP			'A'	// Axx
#define PATTERN_EFFECT_RAND_PAN			'P'	// Pxx
#define PATTERN_EFFECT_RAND_DELAY		'D'	// Dxx
#define PATTERN_EFFECT_RAND_OCT			'O'	// Oxy
#define PATTERN_EFFECT_RAND_NOTE		'N'	// Nxy
#define PATTERN_EFFECT_VIBRATO			'V'	// Vxy
#define PATTERN_EFFECT_TREMOLO			'T'	// Txy
#define PATTERN_EFFECT_VIBRATO_RAND		'v'	// vxy
#define PATTERN_EFFECT_TREMOLO_RAND		't'	// txy
#define PATTERN_EFFECT_CHANCE			'C'	// Cxx
#define PATTERN_EFFECT_CHANCE_STOP		'c'	// cxx
//#define PATTERN_EFFECT_RACHET			'R'	// Rxy

#define PATTERN_NOTE_KEEP				0
#define PATTERN_NOTE_NEW				1
#define PATTERN_NOTE_GLIDE				2
#define PATTERN_NOTE_STOP				3
#define PATTERN_CV_KEEP					0
#define PATTERN_CV_SET					1
#define PATTERN_CV_MODE_CV				0
#define PATTERN_CV_MODE_GATE			1
#define PATTERN_CV_MODE_BPM				2

#define SYNTH_MODE_GATE					0
#define SYNTH_MODE_TRIGGER				1
#define SYNTH_MODE_DRUM					2

#define VOICE_ADD_ADD					0
#define VOICE_ADD_KEEP					1
#define VOICE_ADD_STOP					2

#define TIMELINE_CELL_KEEP				0
#define TIMELINE_CELL_NEW				1
#define TIMELINE_CELL_STOP				2

#define TIMELINE_MODE_STOP				0
#define TIMELINE_MODE_PLAY_SONG			1
#define TIMELINE_MODE_PLAY_PATTERN		2
#define TIMELINE_MODE_PLAY_PATTERN_SOLO	3
#define TIMELINE_MODE_PLAY_MATRIX		4

#define INSTANCE_HANDLE_LEFT			0
#define INSTANCE_HANDLE_MIDDLE			1
#define INSTANCE_HANDLE_RIGHT			2

#define NOTE_STATE_OFF					0
#define NOTE_STATE_START				1
#define NOTE_STATE_ON					2
#define NOTE_STATE_STOP					3

#define CHAR_W							6.302522
#define CHAR_H							8.5
#define CHAR_COUNT_X					84
#define CHAR_COUNT_Y					39

////////////////////////////////////////////////////////////////////////////////
/// DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct TrackerControl;

struct PatternEffect;
struct PatternCV;
struct PatternNote;
struct PatternCVCol;
struct PatternNoteCol;
struct PatternSource;
struct SynthVoice;
struct Synth;
struct PatternReader;
struct Timeline;
struct Editor;

//////////////////////////////////////////////////
/// CLOCK
//////////////////////////////////////////////////

struct Clock {
	u32							beat;
	float						phase;

	Clock();

	void process(float dt);
	void reset();
};

//////////////////////////////////////////////////
/// PATTERN SOURCE
//////////////////////////////////////////////////
/// PatternSource is the object that stores a pattern content (notes, cv...).
///  It does not process anything, it only stores origin pattern data while
///  PatternReader does the process.
/// PatternSource stores its length (beat, line, note, cv).
/// PatternSource stores its notes and cvs as extended 1D arrays. An array being
///  a row containing the lines (and thus notes or cv).
/// PatternSource also stores the current playing line set by a corresponding
///  PatternReader.
//////////////////////////////////////////////////

struct PatternEffect {
	u8							type;
	u8							value;

	PatternEffect();
};

struct PatternCV {
	i8							mode;		// PATTERN_CV_xxx
	u16							value;
	u8							delay;
	u8							curve;

	PatternCV();
};

struct PatternNote {
	i8							mode;		// PATTERN_NOTE_xxx
	u8							glide;
	u8							synth;
	u8							pitch;
	u8							velocity;
	u8							panning;
	u8							delay;
	PatternEffect				effects[8];

	PatternNote();
};

struct PatternCVCol {
	u8							mode;		// CV | BPM
	u8							synth;		// CV synth output
	u8							channel;	// CV synth channel output
	std::vector<PatternCV>		lines;
	//PatternCV					lines[0];	// CVs (memory as struct extension)
};

struct PatternNoteCol {
	u8							fx_count;
	//u8						fx_velocity;
	//u8						fx_panning;
	//u8						fx_delay;
	//u8						fx_chance;
	//u8						fx_chance_mode;
	//u8						fx_octave;
	//u8						fx_octave_mode;
	//u8						fx_pitch;
	//u8						muted;
	std::vector<PatternNote>	lines;
	//PatternNote				lines[0];	// Notes (memory as struct extension)
};

struct PatternSource {
	char						name[256];	// Name
	u8							color;		// Timelime Color
	u16							beat_count;	// Beat per pattern
	u16							line_count;	// Lines per column
	u16							note_count;	// Note columns
	u16							cv_count;	// CV columns
	PatternNoteCol				notes[32];	// Note columns & lines content
	PatternCVCol				cvs[32];	// CV columns & lines content
	u8							lpb;		// Lines per beat
	u16							line_play;	// Playing line
	float						line_phase;	// Playing line phase

	PatternSource();

	void init(void);
	void init(int note_count, int cv_count, int beat_count, int lpb);
	void destroy(void);
	void resize(int note_count, int cv_count, int beat_count, int lpb);
	void rename(char *name);
	void context_menu(Menu *menu);
	int cv_prev(int column, int line);
	int cv_next(int column, int line);
};

struct PatternInstance {
	PatternSource				*source;
	u8							row;
	u16							beat;
	u16							beat_start;
	u16							beat_length;
	bool						muted;

	PatternInstance(PatternSource *source, int row, int beat);
};

//////////////////////////////////////////////////
/// SYNTH
//////////////////////////////////////////////////
/// SynthVoice is the object that compute the synth voice output.
/// SynthVoice store basic playing information (synth, channel, pitch...)
/// SynthVoice can be active (playing) or not.
/// SynthVoice computes the final pitch, gate and velocity.
//////////////////////////////////////////////////
/// Synth stores the basic synth output informations (synth index, channel
///  count, etc.).
/// There are 100 synths stored in the global timeline structure. They can be
///  accessed from anywere.
/// Synth stores 32 SynthVoice to process (play) a voice.
/// Synth stores its outputs (notes and cv) that can be accessed by the
///  TrackerSynth & TrackerDrum modules.
//////////////////////////////////////////////////

struct SynthVoice {
	bool						active;

	u8							channel;
	float						velocity_from;
	float						velocity_to;
	float						panning_from;
	float						panning_to;

	float						glide_len;
	float						glide_cur;
	float						pitch_from;
	float						pitch_to;

	float						delay_start;// Delay before start
	float						delay_stop;	// Delay before stop
	float						delay_gate;	// Delay before gate opens

	float						vibrato_amp;
	float						vibrato_freq;
	float						vibrato_phase;
	float						tremolo_amp;
	float						tremolo_freq;
	float						tremolo_phase;

	SynthVoice();

	void process(float dt_sec, float dt_beat, float *output);
	int start(Synth *synth, PatternNoteCol *row, PatternNote *note, int lpb);
	void stop(void);
	void init(int channel);
	void reset();
	void glide(PatternNote *note);
};

struct Synth {
	char						name[256 + 5];		// Name (with index)
	int							color;
	u8							index;
	u8							channel_cur;
	u8							channel_count;
	i8							mode;				// Gate | Trig | Drum
	SynthVoice					voices[16];
	float						out_synth[16 * 4];	// Out synth (pitch, gate...)
	float						out_cv[8];			// Out CV

	Synth();

	void process(float dt_sec, float dt_beat);
	void init(void);
	void rename(void);
	void rename(char *name);
	void context_menu(Menu *menu);
	SynthVoice* add(PatternNoteCol *row, PatternNote *note, int lpb, int *state);
};

//////////////////////////////////////////////////
/// TIMELINE
//////////////////////////////////////////////////
/// PatternReader represent the playing part of a PatternSource. It does not
///  store the origin pattern data, it only process.
/// PatternReader process (play) a given pattern from the timeline.
/// There are 32 PatternReader stored in the timeline. One PatternReader
///  per timeline row. This allows to have multiple patterns playing at the same
///  time (up to 32), even if they are from the same PatternSource.
//////////////////////////////////////////////////
/// TimelineCell is the basic cell of the timeline.
/// It consists of a pattern index and a pattern starting beat.
/// They are stored as 2D array (1st dimension: timeline row - 2nd dimension:
///  line / beat).
//////////////////////////////////////////////////
/// Timeline is the struct that stores the whole track (timeline, patterns...).
/// It stores the main clock and a PatternReader per timeline row (32).
/// There are 32 tracks / rows and their length is based on the beat count.
/// Each cell / line of the tracks represent one beat.
//////////////////////////////////////////////////

struct PatternReader {
	SynthVoice*					voices[32];	// Synth voices (1 voice : 1 row)
	PatternNote*				notes[32];

	PatternReader();

	void process(Synth *synths, PatternSource* pattern, Clock clock);
	void reset(void);
	void stop(void);
};

struct Timeline {
	//char						debug_str[4096];
	//int						debug;
	//int						debug_2;

	std::atomic_flag			thread_flag;
	u8							play;
	dsp::PulseGenerator			play_trigger;	// Used by other modules
	dsp::PulseGenerator			stop_trigger;	// Used by other modules
	Clock						clock;

	list<PatternInstance>::iterator	pattern_it[32];
	list<PatternInstance>::iterator	pattern_it_end[32];
	PatternReader					pattern_reader[32];
	bool							pattern_state[32];
	PatternInstance					*pattern_instance;

	float						pitch_base_offset;
	float						pitch_scale[12];

	list<PatternInstance>		timeline[32];
	int							timeline_length;
	PatternSource				patterns[1000];	// [0:999]
	int							pattern_count;
	Synth						synths[100];	// [0:99]
	int							synth_count;

	u8							*save_buffer;
	u32							save_length;
	u32							save_cursor;
	u32							save_cursor_save;
	bool						save_endian_reverse;
	bool						save_mode;

	Timeline();

	void			process(i64 frame, float dt_sec, float dt_beat);
	void			stop(void);
	void			clear(void);
	void			compute_length(void);
	PatternSource	*pattern_new(void);
	PatternSource	*pattern_new(int note_count, int cv_count, int beat_count, int lpb);
	void			pattern_del(PatternSource *source);
	void			pattern_swap(PatternSource *source_a, PatternSource *source_b);
	void			pattern_dup(PatternSource *source);
	Synth			*synth_new(void);
	void			synth_del(Synth *synth);
	void			synth_swap(Synth *synth_a, Synth *synth_b);
	PatternInstance	*instance_new(PatternSource *source, int row, int beat);
	void			instance_del(PatternInstance *instance);
	void			instance_move(PatternInstance *instance, int row, int beat);
	PatternInstance	*instance_find(int row, int beat);
};

//////////////////////////////////////////////////
/// EDITOR
//////////////////////////////////////////////////
/// Editor stores the tracker editor information
///  Such as active row, line, cell, etc.
//////////////////////////////////////////////////

struct EditorLiveVoice {
	SynthVoice*					voice;		// Stores live voices
	u8							state;		// Stores live midi or keyboard events
	u8							velocity;	// Stores live event velocity
};

struct EditorSwitch {
	bool						state;
	float						previous_input;

	EditorSwitch();

	bool process(float in);
};

struct EditorTrigger : dsp::BooleanTrigger {
};

struct Editor {

	//
	// mode: Pannel mode (timeline / pattern / matrix / tuning)
	// recording: Recording state (when True, the cursor follows the playhead)
	// selected: Pannel / module selection state
	// pattern_id: Active pattern id (or -1)
	// pattern: Active pattern
	// synth_id: Active synth id (or -1)
	// synth: Active synth
	// instance: Active instance
	// instance_row: Active instance row
	// instance_beat: Active instance beat
	// instance_handle: Active instance moving handle (left / right / middle)
	//
	// pattern_track:
	// pattern_line:
	// pattern_col:
	// pattern_cell:
	// pattern_char:
	// pattern_cam_x:
	// pattern_cam_y:
	// pattern_view_velo:
	// pattern_view_pan:
	// pattern_view_glide:
	// pattern_view_delay:
	// pattern_view_fx:
	// pattern_octave:
	// pattern_jump:
	//
	// timeline_column:
	// timeline_line:
	// timeline_cell:
	// timeline_char:
	// timeline_cam_x:
	// timeline_cam_y:
	//
	// live_voices: Stores live voices and events (played with keyboard or midi)
	//

	int							mode;			// Pattern / Timeline / Param
	bool						recording;
	bool						selected;
	int							pattern_id;		// Active pattern
	PatternSource				*pattern;
	int							synth_id;		// Active synth
	Synth						*synth;
	PatternInstance				*instance;		// Active instance
	int							instance_row;
	int							instance_beat;
	int							instance_handle;

	int							pattern_track;
	int							pattern_line;
	int							pattern_col;
	int							pattern_cell;
	int							pattern_char;
	int							pattern_cam_x;
	int							pattern_cam_y;
	bool						pattern_view_velo;
	bool						pattern_view_pan;
	bool						pattern_view_glide;
	bool						pattern_view_delay;
	bool						pattern_view_fx;
	int							pattern_octave;
	int							pattern_jump;

	int							timeline_column;
	int							timeline_line;
	int							timeline_cell;
	int							timeline_char;
	float						timeline_cam_x;
	float						timeline_cam_y;

	EditorLiveVoice				live_voices[128];	// Stores live voices and events

	Vec							mouse_pos;
	Vec							mouse_pos_drag;
	int							mouse_button;
	bool						mod_shift;
	bool						mod_caps;

	float						side_synth_cam_y;
	float						side_pattern_cam_y;
	Vec							side_mouse_pos;
	int							side_mouse_button;
	int							side_mouse_action;

	EditorTrigger				button_octave[2];
	EditorTrigger				button_jump[2];
	EditorTrigger				button_play[4];

	Editor();

	void process(i64 frame);
	void set_synth(int index);
	void set_pattern(int index);
	void pattern_clamp_cursor(void);
	void pattern_move_cursor_x(int x);
	void pattern_move_cursor_y(int y);
	void pattern_jump_cursor(void);
	void pattern_reset_cursor(void);
	void live_play(int pitch, int velocity);
	void live_stop(int pitch);
};

////////////////////////////////////////////////////////////////////////////////
/// VCV RACK DATA STRUCTURE
////////////////////////////////////////////////////////////////////////////////

struct Tracker : Module {
	enum ParamIds {
								/// PLAY BUTTONS
								PARAM_PLAY_SONG,
								PARAM_PLAY_PATTERN,
								PARAM_PLAY,
								PARAM_STOP,
								/// BPM
								PARAM_BPM,
								/// JUMP / OCTAVE BUTTONS
								PARAM_JUMP_UP,
								PARAM_JUMP_DOWN,
								PARAM_OCTAVE_UP,
								PARAM_OCTAVE_DOWN,
								/// RECORD
								PARAM_RECORD,
								/// EDITOR MODE SWITCHES
								PARAM_MODE_PATTERN,
								PARAM_MODE_TIMELINE,
								PARAM_MODE_MATRIX,
								PARAM_MODE_TUNING,
								/// VIEW MODE SWITCHES
								ENUMS(PARAM_VIEW, 5),
								/// TUNING / PERFORMANCE
								PARAM_RATE,
								PARAM_PITCH_BASE,
								ENUMS(PARAM_TUNING, 12),
								/// CONTEXT MENU
								ENUMS(PARAM_MENU, 8),
								/// .
								PARAM_COUNT
	};
	enum InputIds {
								INPUT_COUNT
	};
	enum OutputIds {
								OUTPUT_COUNT
	};
	enum LightIds {
								LIGHT_FOCUS,
								LIGHT_PLAY,
								LIGHT_RECORD,
								LIGHT_COUNT
	};
	midi::InputQueue			midi_input;
	TrackerControl				*control;

	Tracker();
	~Tracker();

	void	onAdd(const AddEvent &e) override;	// Legacy
	json_t	*dataToJson(void) override;
	void	dataFromJson(json_t *root) override;
	void	process(const ProcessArgs& args) override;
};

struct TrackerDisplay : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;

	TrackerDisplay();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs& args, int layer) override;

	void onButton(const ButtonEvent &e) override;
	void onHover(const HoverEvent &e) override;
	void onSelectKey(const SelectKeyEvent &e) override;
	void onDragStart(const DragStartEvent &e) override;
	void onDragMove(const DragMoveEvent &e) override;
	void onDragEnd(const DragEndEvent &e) override;
	void onHoverScroll(const HoverScrollEvent &e) override;

	void on_key_pattern(const SelectKeyEvent &e);
	void on_key_timeline(const SelectKeyEvent &e);
	void on_button_pattern(const ButtonEvent &e);
	void on_button_timeline(const ButtonEvent &e);
	void draw_pattern(const DrawArgs& args, Rect rect);
	void draw_timeline(const DrawArgs& args, Rect rect);
	void draw_tuning(const DrawArgs& args, Rect rect);
	void on_drag_start_timeline(const DragStartEvent &e);
	void on_drag_move_timeline(const DragMoveEvent &e);
	void on_drag_end_timeline(const DragEndEvent &e);
};

struct TrackerDisplaySide : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;

	TrackerDisplaySide();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
	void onHover(const HoverEvent &e) override;
	void onButton(const ButtonEvent &e) override;
	void onDoubleClick(const DoubleClickEvent &e) override;
	void onLeave(const LeaveEvent &e) override;
	void onHoverScroll(const HoverScrollEvent &e) override;
	//void onSelectKey(const SelectKeyEvent &e) override;
	void draw_list(const DrawArgs &args, Rect rect,
			int cam_y, std::function<bool(int,char**,int*,bool*)>);
};

struct TrackerDisplayInfo : LedDisplay {
	Tracker*					module;
	ModuleWidget*				moduleWidget;
	std::string					font_path;

	TrackerDisplayInfo();

	void draw(const DrawArgs &args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
};

struct TrackerWidget : ModuleWidget {
	Tracker						*module;
	TrackerDisplay				*display;
	TrackerDisplaySide			*display_side;
	TrackerDisplayInfo			*display_info;

	TrackerWidget(Tracker* _module);

	void onSelectKey(const SelectKeyEvent &e) override;
	void onHoverScroll(const HoverScrollEvent &e) override;
	void onSelect(const SelectEvent &e) override;
	void onDeselect(const DeselectEvent &e) override;
	void appendContextMenu(Menu *menu) override;

	//void onDragStart(const DragStartEvent& e) override;
	//void onDragMove(const DragMoveEvent& e) override;
	//void onDragEnd(const DragEndEvent& e) override;
};

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void itoaw(char *str, int number, int width);
bool endian_native(void);

////////////////////////////////////////////////////////////////////////////////
/// GLOBAL STRUCTURES
////////////////////////////////////////////////////////////////////////////////

extern Timeline	*g_timeline;
extern Editor	*g_editor;
extern Tracker	*g_module;

#endif
