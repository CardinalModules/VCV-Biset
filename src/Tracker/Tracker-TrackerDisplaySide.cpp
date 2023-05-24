
#include "Tracker.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static bool draw_list_synth(int index, char **name, int *color, bool *state) {
	Synth			*synth;

	if (index < (int)g_timeline.synth_count) {
		synth = &(g_timeline.synths[index]);
		*name = synth->name;
		*color = synth->color;
		*state = (g_editor.synth == synth);
		return true;
	}
	return false;
}

static bool draw_list_pattern(int index, char **name, int *color, bool *state) {
	PatternSource	*pattern;

	if (index < (int)g_timeline.pattern_count) {
		pattern = &(g_timeline.patterns[index]);
		*name = pattern->name;
		*color = pattern->color;
		*state = (g_editor.pattern == pattern);
		return true;
	}
	return false;
}

static void menu_pattern(PatternSource *pattern) {
	Menu		*menu;
	MenuLabel	*label;

	menu = createMenu();
	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit pattern";
	menu->addChild(label);
	/// ADD COLOR SUB-MENU
	menu->addChild(rack::createSubmenuItem("Color", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Purple", "",
				[=]() { return pattern->color == 0; },
				[=]() { pattern->color = 0; }
			));
			menu->addChild(new MenuCheckItem("Red", "",
				[=]() { return pattern->color == 1; },
				[=]() { pattern->color = 1; }
			));
			menu->addChild(new MenuCheckItem("Orange", "",
				[=]() { return pattern->color == 2; },
				[=]() { pattern->color = 2; }
			));
			menu->addChild(new MenuCheckItem("Yellow", "",
				[=]() { return pattern->color == 3; },
				[=]() { pattern->color = 3; }
			));
			menu->addChild(new MenuCheckItem("Light green", "",
				[=]() { return pattern->color == 4; },
				[=]() { pattern->color = 4; }
			));
			menu->addChild(new MenuCheckItem("Dark green", "",
				[=]() { return pattern->color == 5; },
				[=]() { pattern->color = 5; }
			));
			menu->addChild(new MenuCheckItem("Light blue", "",
				[=]() { return pattern->color == 6; },
				[=]() { pattern->color = 6; }
			));
			menu->addChild(new MenuCheckItem("Dark blue", "",
				[=]() { return pattern->color == 7; },
				[=]() { pattern->color = 7; }
			));
		}
	));
	/// ADD RENAME BUTTON
	menu->addChild(rack::createMenuItem("Rename", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;
			MenuTextField			*text_field;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Rename pattern";
			menu->addChild(label);
			/// ADD TEXT FIELD
			text_field = new MenuTextField(pattern->name,
				[=](char *name) {
					if (name[0] != 0)
						strcpy(pattern->name, name);
				}, true
			);
			menu->addChild(text_field);
			APP->event->setSelectedWidget(text_field);
			text_field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Rename", "",
				[=](void) {
					std::string	name;

					name = text_field->getText();
					if (name.length() > 0)
						strcpy(pattern->name, name.c_str());
				}
			));

		}
	));
	/// ADD DELETE BUTTON
	menu->addChild(rack::createMenuItem("Delete", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Confirm deletion";
			menu->addChild(label);
			/// ADD DELETE BUTTON
			menu->addChild(rack::createMenuItem("Delete", "",
				[=](void) {
					g_timeline.pattern_del(pattern);
				}
			));
		}
	));
}

static void menu_synth(Synth *synth) {
	Menu		*menu;
	MenuLabel	*label;

	menu = createMenu();
	/// ADD LABEL
	label = new MenuLabel();
	label->text = "Edit synth";
	menu->addChild(label);
	/// ADD COLOR SUB-MENU
	menu->addChild(rack::createSubmenuItem("Color", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Purple", "",
				[=]() { return synth->color == 0; },
				[=]() { synth->color = 0; }
			));
			menu->addChild(new MenuCheckItem("Red", "",
				[=]() { return synth->color == 1; },
				[=]() { synth->color = 1; }
			));
			menu->addChild(new MenuCheckItem("Orange", "",
				[=]() { return synth->color == 2; },
				[=]() { synth->color = 2; }
			));
			menu->addChild(new MenuCheckItem("Yellow", "",
				[=]() { return synth->color == 3; },
				[=]() { synth->color = 3; }
			));
			menu->addChild(new MenuCheckItem("Light green", "",
				[=]() { return synth->color == 4; },
				[=]() { synth->color = 4; }
			));
			menu->addChild(new MenuCheckItem("Dark green", "",
				[=]() { return synth->color == 5; },
				[=]() { synth->color = 5; }
			));
			menu->addChild(new MenuCheckItem("Light blue", "",
				[=]() { return synth->color == 6; },
				[=]() { synth->color = 6; }
			));
			menu->addChild(new MenuCheckItem("Dark blue", "",
				[=]() { return synth->color == 7; },
				[=]() { synth->color = 7; }
			));
		}
	));
	/// ADD RENAME BUTTON
	menu->addChild(rack::createMenuItem("Rename", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;
			MenuTextField			*text_field;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Rename synth";
			menu->addChild(label);
			/// ADD TEXT FIELD
			text_field = new MenuTextField(synth->name,
				[=](char *name) {
					if (name[0] != 0)
						strcpy(synth->name, name);
				}, true
			);
			menu->addChild(text_field);
			APP->event->setSelectedWidget(text_field);
			text_field->selectAll();
			/// ADD CONFIRM BUTTON
			menu->addChild(rack::createMenuItem("Rename", "",
				[=](void) {
					std::string	name;

					name = text_field->getText();
					if (name.length() > 0)
						strcpy(synth->name, name.c_str());
				}
			));

		}
	));
	/// ADD DELETE BUTTON
	menu->addChild(rack::createMenuItem("Delete", "",
		[=](void) {
			Menu					*menu;
			MenuLabel				*label;

			menu = createMenu();
			/// ADD LABEL
			label = new MenuLabel();
			label->text = "Confirm deletion";
			menu->addChild(label);
			/// ADD DELETE BUTTON
			menu->addChild(rack::createMenuItem("Delete", "",
				[=](void) {
					g_timeline.synth_del(synth);
				}
			));
		}
	));
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerDisplaySide::TrackerDisplaySide() {
	font_path = std::string(asset::plugin(pluginInstance, "res/FT88-Regular.ttf"));
}

void TrackerDisplaySide::draw(const DrawArgs &args) {
	std::shared_ptr<Font>	font;
	Rect					rect;
	float					off_x, off_y;

	LedDisplay::draw(args);

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y + 1.0);
	nvgFill(args.vg);
}

void TrackerDisplaySide::draw_list(const DrawArgs &args, Rect rect,
	int camera_y, std::function<bool(int,char**,int*,bool*)> func) {
	char		*name;
	int			color;
	bool		state;
	float		x, y, w, h;
	Vec			p;
	int			i;

	p = rect.getTopLeft();
	x = p.x;
	w = CHAR_W * 26;
	h = CHAR_H * 3 - 4;
	/// GET ITEM NAME & COLOR & STATUS ?
	for (i = 0; i < 13; ++i) {
		y = p.y + CHAR_H * 3 * i + 6;
		/// DRAW ITEM
		if (func(i + camera_y, &name, &color, &state) == true) {
			/// FILL
			//// MAIN ROUND RECT
			nvgBeginPath(args.vg);
			if (state == true
			|| (g_editor.side_mouse_pos.y > y
			&& g_editor.side_mouse_pos.y < y + h))
				nvgFillColor(args.vg, colors[13]);
			else
				nvgFillColor(args.vg, colors[14]);
			nvgRoundedRect(args.vg, x, y, w - 10, h, 5);
			nvgFill(args.vg);
			//// COLORED ROUND RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRoundedRect(args.vg, x + w - 15, y, 15, h, 5);
			nvgFill(args.vg);
			//// COLORED SQUARE RECT
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, colors_user[color]);
			nvgRect(args.vg, x + w - 15, y, 5, h);
			nvgFill(args.vg);
			/// TEXT
			nvgFillColor(args.vg, colors[12]);
			nvgTextBox(args.vg,
			/**/ x + 3.0, y + 3.0,
			/**/ CHAR_W * 23,
			/**/ name, NULL);
		/// DRAW ADD ITEM
		} else {
			/// FILL
			nvgBeginPath(args.vg);
			if (g_editor.side_mouse_pos.y > y && g_editor.side_mouse_pos.y < y + h)
				nvgFillColor(args.vg, colors[3]);
			else
				nvgFillColor(args.vg, colors[2]);
			nvgRoundedRect(args.vg, x, y, w, h, 5);
			nvgFill(args.vg);
			/// TEXT
			nvgFillColor(args.vg, colors[0]);
			nvgTextBox(args.vg,
			/**/ x + w / 2.0 - CHAR_W / 2.0, y + 6.0,
			/**/ CHAR_W * 26,
			/**/ "+", NULL);
			return;
		}
	}
}

void TrackerDisplaySide::drawLayer(const DrawArgs &args, int layer) {
	std::shared_ptr<Font>	font;
	Rect					rect;

	if (module == NULL || layer != 1)
		return;
	/// GET FONT
	font = APP->window->loadFont(font_path);
	if (font == NULL)
		return;
	/// SET FONT
	nvgFontSize(args.vg, 9);
	nvgFontFaceId(args.vg, font->handle);
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	/// GET CANVAS FORMAT
	rect = box.zeroPos();

	nvgScissor(args.vg, RECT_ARGS(rect));
	/// DRAW
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		this->draw_list(args, rect,
		/**/ g_editor.side_synth_cam_y, draw_list_synth);
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		this->draw_list(args, rect,
		/**/ g_editor.side_pattern_cam_y, draw_list_pattern);
	}
	nvgResetScissor(args.vg);
	LedDisplay::drawLayer(args, layer);
}

void TrackerDisplaySide::onHover(const HoverEvent &e) {
	g_editor.side_mouse_pos = e.pos;
}

void TrackerDisplaySide::onButton(const ButtonEvent &e) {
	int		index;

	g_editor.side_mouse_button = e.button;
	g_editor.side_mouse_action = e.action;
	if (e.action != GLFW_PRESS)
		return;
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		index = (e.pos.y - 6) / (CHAR_H * 3) + g_editor.side_synth_cam_y;
		/// CLICK ON SYNTH
		if (index < (int)g_timeline.synth_count) {
			/// SELECT SYNTH
			g_editor.synth_id = index;
			g_editor.synth = &(g_timeline.synths[index]);
			/// CLICK RIGHT
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
				e.consume(this);
				menu_synth(&(g_timeline.synths[index]));
			}
		/// CLICK ON ADD SYNTH
		} else if (index == (int)g_timeline.synth_count) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (g_editor.side_synth_cam_y == g_timeline.synth_count - 12)
					g_editor.side_synth_cam_y += 1;
				g_timeline.synth_new();
			}
		/// CLICK NOTHING
		} else {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				g_editor.synth_id = -1;
				g_editor.synth = NULL;
			}
		}
	} else if (g_editor.mode == EDITOR_MODE_TIMELINE) {
		index = (e.pos.y - 6) / (CHAR_H * 3) + g_editor.side_pattern_cam_y;
		/// CLICK ON PATTERN
		if (index < (int)g_timeline.pattern_count) {
			/// SELECT PATTERN
			g_editor.pattern_id = index;
			g_editor.pattern = &(g_timeline.patterns[index]);
			/// CLICK RIGHT
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT) {
				e.consume(this);
				menu_pattern(&(g_timeline.patterns[index]));
			}
		/// CLICK ON ADD PATTERN
		} else if (index == (int)g_timeline.pattern_count) {
			/// CLICK LEFT
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				if (g_editor.side_pattern_cam_y == g_timeline.pattern_count - 12)
					g_editor.side_pattern_cam_y += 1;
				g_timeline.pattern_new();
			}
		/// CLICK NOTHING
		} else {
			/// CLICK LEFT
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				g_editor.pattern_id = -1;
				g_editor.pattern = NULL;
			}
		}
	}
}

void TrackerDisplaySide::onLeave(const LeaveEvent &e) {
	g_editor.side_mouse_pos = {0, 0};
}

void TrackerDisplaySide::onHoverScroll(const HoverScrollEvent &e) {
	int		*count;
	int		*scroll;

	/// CONSUME EVENT
	e.consume(this);
	/// POINT CORRESPONDING SCROLL
	if (g_editor.mode == EDITOR_MODE_PATTERN) {
		count = &(g_timeline.synth_count);
		scroll = &(g_editor.side_synth_cam_y);
	} else {
		count = &(g_timeline.pattern_count);
		scroll = &(g_editor.side_pattern_cam_y);
	}
	/// UPDATE CORRESPONDING SCROLL
	*scroll += (e.scrollDelta.y > 0) ? -1 : +1;
	if (*scroll > *count - 12)
		*scroll = *count - 12;
	if (*scroll < 0)
		*scroll = 0;
}