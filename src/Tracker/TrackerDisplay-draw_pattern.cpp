
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/// PATTERN TEXT
static void ptext(const Widget::DrawArgs& args, Vec p, float x, float y,
	char *str, int color, bool background) {
	float	sx, sy;

	// TODO: handle left under flow ?
	// TODO: handle right over flow ?

	/// [1] OFFSET COORD WITH CAMERA
	x -= g_editor.pattern_cam_x;
	if (x < 0)
		return;
	if (x > CHAR_COUNT_X - 1)
		return;
	/// [2] COMPUTE REAL COORD
	sx = p.x + 2.0 + CHAR_W * (x + 3.0);
	sy = p.y + 11.0 + CHAR_H * y;
	/// [3] DRAW BACKGROUND
	if (background) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[12]);
		if (str[1] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, CHAR_W, CHAR_H);
		else if (str[2] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 2.0 * CHAR_W, CHAR_H);
		else if (str[3] == 0)
			nvgRect(args.vg, sx, sy - CHAR_H + 1.0, 3.0 * CHAR_W, CHAR_H);
		nvgFill(args.vg);
	}
	/// [4] DRAW TEXT
	nvgFillColor(args.vg, colors[color]);
	nvgText(args.vg, sx, sy, str, NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

void TrackerDisplay::draw_pattern(const DrawArgs &args, Rect rect) {
	PatternSource			*pattern;
	PatternNote				*note;
	PatternNoteCol			*note_col;
	PatternCV				*cv;
	PatternCVCol			*cv_col;
	PatternEffect			*effect;
	Vec						p;
	bool					focus_col, focus_line, focus_fx, focus;
	int						i, j, k;
	int						line;
	float					x, y;
	int						tx, tx_col;
	char					str[32];

	pattern = g_editor.pattern;
	if (pattern == NULL)
		return;
	p = rect.getTopLeft();

	/// [1] LAYER 1 (MARKERS + NOTES + CURVES)
	/// DRAW BEAT CURSOR
	if (g_timeline.play != TIMELINE_MODE_STOP) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[15]);
		nvgRect(args.vg,
		/**/ p.x,
		/**/ p.y + 3.5 + CHAR_H * (pattern->line_play - g_editor.pattern_cam_y),
		/**/ rect.getWidth() + 0.5, CHAR_H);
		nvgFill(args.vg);
	}
	/// DRAW PATTERN CURSOR LINE
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[15]);
	nvgRect(args.vg,
	/**/ p.x,
	/**/ p.y + 3.5 + CHAR_H * (g_editor.pattern_line - g_editor.pattern_cam_y),
	/**/ rect.getWidth() + 0.5, CHAR_H);
	nvgFill(args.vg);
	/// DRAW LINE / BEAT COUNT
	x = p.x + 2;
	for (i = 0; i < CHAR_COUNT_Y; ++i) {
		line = g_editor.pattern_cam_y + i;
		y = p.y + 11.0 + i * CHAR_H;
		/// BEAT COUNT
		if (line % pattern->lpb == 0) {
			itoaw(str, line / pattern->lpb, 3);
			nvgFillColor(args.vg, colors[13]);
		/// LINE COUNT
		} else {
			itoaw(str, line % pattern->lpb, 2);
			nvgFillColor(args.vg, colors[15]);
		}
		nvgText(args.vg, x, y, str, NULL);
	}

	/// [2] LAYER 2 (TRACKER)
	/// FOR EACH NOTE COL	
	tx = 0;
	for (i = 0; i < pattern->note_count; ++i) {
		note_col = pattern->notes[i];
		focus_col = (g_editor.pattern_col == i);
		/// FOR EACH NOTE COL LINE
		for (j = 0; j < CHAR_COUNT_Y; ++j) {
			line = g_editor.pattern_cam_y + j;
			if (line >= pattern->line_count)
				break;
			focus_line = focus_col & (g_editor.pattern_line == line);
			tx_col = tx;
			note = &(note_col->lines[line]);
			/// PITCH
			focus = focus_line & (g_editor.pattern_cell == 0);
			nvgFillColor(args.vg, colors[3]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
				ptext(args, p, tx_col, j, str, 3, focus);
			} else if (note->mode == PATTERN_NOTE_STOP) {
				str[0] = '-';
				str[1] = '-';
				str[2] = 0;
				ptext(args, p, tx_col, j, str, 3, focus);
			} else {
				ptext(args, p, tx_col, j,
				/**/ table_pitch[note->pitch % 12], 3, focus);
			}
			tx_col += 2;
			/// OCTAVE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, colors[2]);
			if (note->mode == PATTERN_NOTE_KEEP) {
				str[0] = '.';
				str[1] = 0;
			} else if (note->mode == PATTERN_NOTE_STOP) {
				str[0] = '-';
				str[1] = 0;
			} else {
				itoa(note->pitch / 12, str, 10);
			}
			ptext(args, p, tx_col, j, str, 2, focus);
			tx_col += 1;
			/// VELOCITY
			if (g_editor.switch_view[0].state) {
				focus = focus_line & (g_editor.pattern_cell == 2);
				if (note->mode == PATTERN_NOTE_NEW) {
					itoaw(str, note->velocity, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_col, j, str, 5, focus);
				tx_col += 2;
			}
			/// PANNING
			if (g_editor.switch_view[1].state) {
				focus = focus_line & (g_editor.pattern_cell == 3);
				if (note->mode == PATTERN_NOTE_NEW) {
					itoaw(str, note->panning, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_col, j, str, 6, focus);
				tx_col += 2;
			}
			/// SYNTH
			focus = focus_line & (g_editor.pattern_cell == 4);
			if (note->mode == PATTERN_NOTE_NEW) {
				itoaw(str, note->synth, 2);
			} else {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			}
			ptext(args, p, tx_col, j, str, 4, focus);
			tx_col += 2;
			/// DELAY
			if (g_editor.switch_view[2].state) {
				focus = focus_line & (g_editor.pattern_cell == 5);
				if (note->mode != PATTERN_NOTE_KEEP) {
					itoaw(str, note->delay, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_col, j, str, 10, focus);
				tx_col += 2;
			}
			/// GLIDE
			if (g_editor.switch_view[3].state) {
				focus = focus_line & (g_editor.pattern_cell == 6);
				if (note->mode == PATTERN_NOTE_GLIDE) {
					itoaw(str, note->glide, 2);
				} else {
					str[0] = '.';
					str[1] = '.';
					str[2] = 0;
				}
				ptext(args, p, tx_col, j, str, 11, focus);
				tx_col += 2;
			}
			/// EFFECTS
			if (g_editor.switch_view[4].state) {
				for (k = 0; k < note_col->effect_count; ++k) {
					effect = &(note->effects[k]);
					focus_fx = focus_line & ((g_editor.pattern_cell - 7) / 2 == k);
					/// COMPUTE STRINGS
					if (note->mode == PATTERN_NOTE_KEEP
					|| note->mode == PATTERN_NOTE_STOP
					|| effect->type == PATTERN_EFFECT_NONE) {
						str[0] = '.';
						str[1] = 0;
						str[2] = '.';
						str[3] = '.';
						str[4] = 0;
					} else {
						str[0] = table_effect[effect->type - 1];
						str[1] = 0;
						itoaw(str + 2, note->effects[k].value, 2);
					}
					/// EFFECT TYPE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 0);
					ptext(args, p, tx_col, j, str, 13, focus);
					tx_col += 1;
					/// EFFECT VALUE
					focus = focus_fx & ((g_editor.pattern_cell - 7) % 2 == 1);
					ptext(args, p, tx_col, j, str + 2, 14, focus);
					tx_col += 2;
				}
			}
		}
		/// OFFSET X
		tx += (2 + 1														// PITCH
		/**/ + g_editor.switch_view[0].state * 2							// VELOCITY
		/**/ + g_editor.switch_view[1].state * 2							// PANNING
		/**/ + 2					  										// SYNTH
		/**/ + g_editor.switch_view[2].state * 2							// DELAY
		/**/ + g_editor.switch_view[3].state * 2							// GLIDE
		/**/ + g_editor.switch_view[4].state * 3 * note_col->effect_count	// EFFECTS
		/**/ + 1);
	}
	/// FOR EACH CV COL
	for (i = 0; i < pattern->cv_count; ++i) {
		cv_col = pattern->cvs[i];
		focus_col = (g_editor.pattern_col == pattern->note_count + i);
		/// FOR EACH CV COL LINE
		for (j = 0; j < CHAR_COUNT_Y; ++j) {
			line = g_editor.pattern_cam_y + j;
			if (line >= pattern->line_count)
				break;
			focus_line = focus_col & (g_editor.pattern_line == line);
			tx_col = tx;
			cv = &(cv_col->lines[line]);
			/// VALUE
			focus = focus_line & (g_editor.pattern_cell == 0);
			nvgFillColor(args.vg, colors[3]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = '.';
				str[3] = 0;
			} else {
				itoaw(str, cv->value, 3);
			}
			ptext(args, p, tx_col, j, str, 3, focus);
			tx_col += 3;
			/// GLIDE
			focus = focus_line & (g_editor.pattern_cell == 1);
			nvgFillColor(args.vg, colors[5]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				itoaw(str, cv->glide, 2);
			}
			ptext(args, p, tx_col, j, str, 5, focus);
			tx_col += 2;
			/// DELAY
			focus = focus_line & (g_editor.pattern_cell == 2);
			nvgFillColor(args.vg, colors[10]);
			if (cv->mode == PATTERN_CV_KEEP) {
				str[0] = '.';
				str[1] = '.';
				str[2] = 0;
			} else {
				itoaw(str, cv->delay, 2);
			}
			ptext(args, p, tx_col, j, str, 10, focus);
			tx_col += 2;
		}
		tx += (3 + 2 + 2 + 1);
	}
}