
#include "Tracker.hpp"

char	modes_note[3][8] = {
	"gate",
	"trigger",
	"drum"
};
char	modes_cv[2][4] = {
	"cv",
	"bpm"
};

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void field_num(const Widget::DrawArgs &args, Rect rect, Vec p,
	char *title, int value_src, int value_new, int color, int y) {
	static char			string[16];

	/// SET COLOR
	nvgFillColor(args.vg, colors[color]);
	/// DRAW TITLE
	nvgText(args.vg,
	/**/ p.x + 3.5,
	/**/ p.y + 3.5 + CHAR_H * (y * 2.7) - 2.0,
	/**/ title, NULL);
	/// DRAW VALUE
	//// VALUE UNCHANGED
	nvgFillColor(args.vg, colors[13]);
	if (value_src == value_new
	|| g_editor.pattern_row != g_editor.pattern_row_prev) {
		itoa(value_src, string, 10);
		nvgText(args.vg,
		/**/ p.x + 3.5,
		///**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 1),
		/**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 0.7),
		/**/ string, NULL);
	//// VALUE CHANGED
	} else {
		itoa(value_new, string, 10);
		nvgFillColor(args.vg, colors[2]);
		nvgText(args.vg,
		/**/ p.x + 3.5,
		///**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 1),
		/**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 0.7),
		/**/ string, NULL);
	}
}

static void field_str(const Widget::DrawArgs &args, Rect rect, Vec p,
	char *title, char *value_src, char *value_new, int color, int y) {
	/// SET COLOR
	nvgFillColor(args.vg, colors[color]);
	/// DRAW TITLE
	nvgText(args.vg,
	/**/ p.x + 3.5,
	/**/ p.y + 3.5 + CHAR_H * (y * 2.7) - 2.0,
	/**/ title, NULL);
	/// DRAW VALUE
	//// VALUE UNCHANGED
	nvgFillColor(args.vg, colors[13]);
	if (value_src == value_new
	|| g_editor.pattern_row != g_editor.pattern_row_prev) {
		nvgText(args.vg,
		/**/ p.x + 3.5,
		/**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 0.7),
		/**/ value_src, NULL);
	//// VALUE CHANGED
	} else {
		nvgFillColor(args.vg, colors[2]);
		nvgText(args.vg,
		/**/ p.x + 3.5,
		/**/ p.y + 3.5 + CHAR_H * (y * 2.7 + 0.7),
		/**/ value_new, NULL);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerEditDisplay::TrackerEditDisplay() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerEditDisplay::drawLayer(const DrawArgs& args, int layer) {
	std::shared_ptr<Font>	font;
	PatternSource			*pattern;
	PatternNoteRow			*col_note;
	PatternCVRow			*col_cv;
	Rect					rect;
	Vec						p;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	//nvgFontSize(args.vg, 9);
	nvgFontSize(args.vg, 7);
	nvgFontFaceId(args.vg, font->handle);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	p = rect.getTopLeft();
	/// BACKGROUND
	//nvgBeginPath(args.vg);
	//nvgFillColor(args.vg, colors[0]);
	//nvgRect(args.vg, RECT_ARGS(rect));
	//nvgFill(args.vg);

	int	i;
	for (i = 0; i < 9; ++i) {
		nvgBeginPath(args.vg);
		nvgFillColor(args.vg, colors[0]);
		nvgRect(args.vg,
		/**/ rect.pos.x, rect.pos.y + (CHAR_H * 2.7 * i),
		/**/ rect.size.x, CHAR_H * 2.0);
		nvgFill(args.vg);
	}


	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

	/// SONG LENGTH
	field_num(args, rect, p, (char*)"Song length",
	/**/ g_timeline.beat_count,
	/**/ module->params[Tracker::PARAM_EDIT + 0].getValue(),
	/**/ 6, 0);
	/// SYNTH CHANNELS
	field_num(args, rect, p, (char*)"Synth channel",
	/**/ g_timeline.synths[g_editor.synth_id].channel_count,
	/**/ module->params[Tracker::PARAM_EDIT + 1].getValue(),
	/**/ 5, 1);
	/// PATTERN LENGTH
	field_num(args, rect, p, (char*)"Pattern length",
	/**/ g_editor.pattern->beat_count,
	/**/ module->params[Tracker::PARAM_EDIT + 2].getValue(),
	/**/ 10, 2);
	/// PATTERN LINES PER BEAT
	field_num(args, rect, p, (char*)"Pattern lpb",
	/**/ g_editor.pattern->lpb,
	/**/ module->params[Tracker::PARAM_EDIT + 3].getValue(),
	/**/ 10, 3);
	/// PATTERN NOTE ROW
	field_num(args, rect, p, (char*)"Pattern notes",
	/**/ g_editor.pattern->note_count,
	/**/ module->params[Tracker::PARAM_EDIT + 4].getValue(),
	/**/ 10, 4);
	/// PATTERN CV ROW
	field_num(args, rect, p, (char*)"Pattern cv",
	/**/ g_editor.pattern->cv_count,
	/**/ module->params[Tracker::PARAM_EDIT + 5].getValue(),
	/**/ 10, 5);
	/// PATTERN NOTE / CV
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		pattern = g_editor.pattern;
		/// NOTE ROW
		if (g_editor.pattern_row < g_editor.pattern->note_count) {
			col_note = pattern->notes[g_editor.pattern_row];
			field_str(args, rect, p, (char*)"Note mode",
			/**/ modes_note[col_note->mode],
			/**/ modes_note[(int)module->params[Tracker::PARAM_EDIT + 6].getValue()],
			/**/ 9, 6);
			field_num(args, rect, p, (char*)"Note effects",
			/**/ col_note->effect_count,
			/**/ module->params[Tracker::PARAM_EDIT + 7].getValue(),
			/**/ 9, 7);
		/// CV ROW
		} else {
			col_cv = pattern->cvs[g_editor.pattern_row - g_editor.pattern->note_count];
			field_str(args, rect, p, (char*)"CV mode",
			/**/ modes_cv[col_cv->mode],
			/**/ modes_cv[(int)module->params[Tracker::PARAM_EDIT + 6].getValue()],
			/**/ 9, 6);
			field_num(args, rect, p, (char*)"CV synth",
			/**/ col_cv->synth,
			/**/ module->params[Tracker::PARAM_EDIT + 7].getValue(),
			/**/ 9, 7);
			field_num(args, rect, p, (char*)"CV channel",
			/**/ col_cv->channel,
			/**/ module->params[Tracker::PARAM_EDIT + 8].getValue(),
			/**/ 9, 8);
		}
	}
	
	LedDisplay::drawLayer(args, layer);
}