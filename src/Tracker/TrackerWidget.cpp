
#include "Tracker.hpp"

#define BTN_PLAY_X			(5.2 + 1.0)
#define BTN_PLAY_Y			6.0
#define BTN_PLAY_STEP		6.0
#define BTN_VIEW_X			BTN_PLAY_X
#define BTN_VIEW_Y			108
#define BTN_VIEW_STEP		BTN_PLAY_STEP
#define KNOB_X				(10.0 + 1.0)
#define KNOB_Y				18.0
#define BTN_JUMP_X			(4.75 + 1.0)
#define BTN_JUMP_Y			40.0
#define BTN_OCTAVE_X		(BTN_JUMP_X + 12.25)
#define BTN_OCTAVE_Y		BTN_JUMP_Y
#define DISPLAY_X			31.25				// 51.5
#define DISPLAY_Y			5.0
#define DISPLAY_SIDE_X		(DISPLAY_X + 192.5)	// 244
#define DISPLAY_SIDE_Y		5.0
#define DISPLAY_INFO_X		3.0
#define DISPLAY_INFO_Y		75.0

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

static void set_temperament(float *table) {
	float	value;
	float	note;
	int		note_round;
	int		i;

	for (i = 0; i < 12; ++i) {
		note = g_module->params[Tracker::PARAM_TUNING + i].getValue() / 100.0;
		note_round = (int)note;
		if (note - (float)note_round >= 0.5) {
			if (note_round < 11)
				value = table[note_round + 1];
			else
				value = table[0];
		} else {
			value = table[note_round];
		}
		g_module->params[Tracker::PARAM_TUNING + i].setValue(value);
	}
}

static void set_scale(float *table) {
	float	value;
	int		i;

	for (i = 0; i < 12; ++i) {
		value = table[i];
		g_module->params[Tracker::PARAM_TUNING + i].setValue(value);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TrackerWidget::TrackerWidget(Tracker* _module) {
	TrackerDisplay			*display;
	TrackerDisplaySide		*display_side;
	TrackerDisplayInfo		*display_info;
	LedDisplayDigit			*display_bpm;
	LedDisplayDigit			*display_jump;
	LedDisplayDigit			*display_octave;
	int						i;

	//
	// BUTTONS:
	// - TL1105
	//
	// KNOBS:
	// - RoundBlackKnob
	// - RoundKnob
	// - Round(Big / Huge / Large / Small)BlackKnob
	// - Trimpot (very small)
	// - Rogan(1 / 2 / 3 / 5 / 6)(P / PS)(Blue / Green / Red / White / Gray)
	// - Befaco(Big / Tiny)Knob
	// - Davies1900h(Large)(Black / Red / White)Knob
	//
	// INPUTS:
	// - PJ301MPort
	//
	// OUTPUTS:
	// - PJ301MPort
	//
	// LIGHTS:
	// - LargeLight<YellowLight>
	// - SmallLight<YellowLight>
	//

	module = _module;
	setModule(module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Tracker.svg")));

	/// [1] ADD LIGHTS
	addChild(createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(10.0, 12.0)),
	module, Tracker::LIGHT_FOCUS));
	addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(15.5, 12.0)),
	module, Tracker::LIGHT_PLAY));
	addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(21.0, 12.0)),
	module, Tracker::LIGHT_RECORD));

	/// [2] ADD PARAMS
	//// PLAY BUTTONS
	addParam(
	/**/ createParamCentered<ButtonPlaySong>(mm2px(Vec(BTN_PLAY_X, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_SONG));
	addParam(
	/**/ createParamCentered<ButtonPlayPattern>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY_PATTERN));
	addParam(
	/**/ createParamCentered<ButtonPlaySimple>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 2, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_PLAY));
	addParam(
	/**/ createParamCentered<ButtonStop>(mm2px(Vec(BTN_PLAY_X + BTN_PLAY_STEP * 3, BTN_PLAY_Y)),
	/**/ module,
	/**/ Tracker::PARAM_STOP));

	//// EDITOR MODE SWITCHES
	addParam(
	/**/ createParamCentered<ButtonViewPattern>(mm2px(Vec(BTN_VIEW_X, BTN_VIEW_Y)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_PATTERN));
	addParam(
	/**/ createParamCentered<ButtonViewTimeline>(mm2px(Vec(BTN_VIEW_X + BTN_VIEW_STEP, BTN_VIEW_Y)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_TIMELINE));
	addParam(
	/**/ createParamCentered<ButtonViewMatrix>(mm2px(Vec(BTN_VIEW_X + BTN_VIEW_STEP * 2, BTN_VIEW_Y)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_MATRIX));
	addParam(
	/**/ createParamCentered<ButtonViewTuning>(mm2px(Vec(BTN_VIEW_X + BTN_VIEW_STEP * 3, BTN_VIEW_Y)),
	/**/ module,
	/**/ Tracker::PARAM_MODE_TUNING));

	//// PATTERN VIEW MODE SWITCHES
	for (i = 0; i < 5; ++i) {
		addParam(
		/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(90.0 + 7.0 * i, 123.7)),
		/**/ module,
		/**/ Tracker::PARAM_VIEW + i));
	}

	//// RECORD SWITCH
	addParam(
	/**/ createParamCentered<ButtonSwitch>(mm2px(Vec(15.5, 67.5)),
	/**/ module,
	/**/ Tracker::PARAM_RECORD));

	/// [3] ADD DISPLAYS
	//// MAIN LED DISPLAY
	display = createWidget<TrackerDisplay>(mm2px(Vec(DISPLAY_X, DISPLAY_Y)));
	//display->box.size = mm2px(Vec(173.5 + 14.0, 94.5 + 15.0));
	display->box.size = Vec(CHAR_W * (CHAR_COUNT_X + 3) + 4, CHAR_H * CHAR_COUNT_Y + 5.5);
	display->module = module;
	display->moduleWidget = this;
	this->display = display;
	addChild(display);

	//// SIDE LED DISPLAY
	display_side = createWidget<TrackerDisplaySide>(mm2px(Vec(DISPLAY_SIDE_X, DISPLAY_SIDE_Y)));
	display_side->box.size = Vec(CHAR_W * 26, CHAR_H * CHAR_COUNT_Y + 5.5);
	display_side->module = module;
	display_side->moduleWidget = this;
	this->display_side = display_side;
	addChild(display_side);

	//// INFO LED DISPLAY
	display_info = createWidget<TrackerDisplayInfo>(mm2px(Vec(DISPLAY_INFO_X, DISPLAY_INFO_Y)));
	display_info->box.size = Vec(CHAR_W * 12 + 4, CHAR_H * 5 + 4);
	display_info->module = module;
	display_info->moduleWidget = this;
	this->display_info = display_info;
	addChild(display_info);

	/// BPM SELECTOR
	//// DISPLAY
	display_bpm = createWidget<LedDisplayDigit>(mm2px(Vec(KNOB_X, KNOB_Y)));
	display_bpm->box.size = mm2px(Vec(8.25, 3.5));
	display_bpm->module = module;
	if (module)
		display_bpm->value_quant = module->paramQuantities[Tracker::PARAM_BPM];
	display_bpm->value_length = 3;
	display_bpm->color_back = colors[15];
	display_bpm->color_font = colors[4];
	addChild(display_bpm);
	//// KNOB
	addParam(
	/**/ createParamCentered<KnobMedium>(mm2px(Vec(KNOB_X + 4.25, KNOB_Y + 10.0)),
	/**/ module,
	/**/ Tracker::PARAM_BPM));

	/// SELECT JUMP
	//// DISPLAY
	display_jump = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_JUMP_X, BTN_JUMP_Y)));
	display_jump->box.size = mm2px(Vec(8.25, 3.5));
	display_jump->module = module;
	display_jump->value_link = &(g_editor->pattern_jump);
	display_jump->value_length = 2;
	display_jump->color_back = colors[15];
	display_jump->color_font = colors[4];
	addChild(display_jump);
	//// BUTTONS
	addParam(
	/**/ createParamCentered<ButtonPlus>(mm2px(Vec(BTN_JUMP_X + 4.0, BTN_JUMP_Y + 8.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_UP));
	addParam(
	/**/ createParamCentered<ButtonMinus>(mm2px(Vec(BTN_JUMP_X + 4.0, BTN_JUMP_Y + 14.0)),
	/**/ module,
	/**/ Tracker::PARAM_JUMP_DOWN));
	/// SELECT OCTAVE
	//// DISPLAY
	display_octave = createWidget<LedDisplayDigit>(mm2px(Vec(BTN_OCTAVE_X, BTN_OCTAVE_Y)));
	display_octave->box.size = mm2px(Vec(8.25, 3.5));
	display_octave->module = module;
	display_octave->value_link = &(g_editor->pattern_octave);
	display_octave->value_length = 2;
	display_octave->color_back = colors[15];
	display_octave->color_font = colors[4];
	addChild(display_octave);
	//// BUTTONS
	addParam(
	/**/ createParamCentered<ButtonPlus>(mm2px(Vec(BTN_OCTAVE_X + 4.0, BTN_OCTAVE_Y + 8.0)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_UP));
	addParam(
	/**/ createParamCentered<ButtonMinus>(mm2px(Vec(BTN_OCTAVE_X + 4.0, BTN_OCTAVE_Y + 14.0)),
	/**/ module,
	/**/ Tracker::PARAM_OCTAVE_DOWN));
}

void TrackerWidget::onSelectKey(const SelectKeyEvent &e) {
	if (g_module == NULL || g_editor == NULL)
		return;

	if (e.action == GLFW_PRESS && (e.mods & GLFW_MOD_CONTROL))
		return;

	/// [1] EDIT PATTERN & TIMELINE

	if (g_editor->mode == EDITOR_MODE_PATTERN)
		this->display->on_key_pattern(e);
	else if (g_editor->mode == EDITOR_MODE_TIMELINE)
		this->display->on_key_timeline(e);
}

void TrackerWidget::onHoverScroll(const HoverScrollEvent &e) {

	if (g_module == NULL)
		return;

	ModuleWidget::onHoverScroll(e);
	//if (g_editor->selected == false)
	//	return;
	//if (g_editor->pattern) {
	//	/// SCROLL X
	//	if (APP->window->getMods() & GLFW_MOD_SHIFT) {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor->pattern_move_cursor_x(-1);
	//		else
	//			g_editor->pattern_move_cursor_x(+1);
	//	/// SCROLL Y
	//	} else {
	//		/// MOVE CURSOR
	//		if (e.scrollDelta.y > 0)
	//			g_editor->pattern_move_cursor_y(-1);
	//		else
	//			g_editor->pattern_move_cursor_y(+1);
	//	}
	//}
	//e.consume(this);
}

void TrackerWidget::onSelect(const SelectEvent &e) {
	g_editor->selected = true;
	//this->module->lights[0].setBrightness(1.0);
}

void TrackerWidget::onDeselect(const DeselectEvent &e) {
	g_editor->selected = false;
	//ParamWidget	*pw;
	//
	//pw = APP->scene->rack->touchedParam;
	//if (pw && pw->module == this->module) {
	//	APP->event->setSelectedWidget(this);
	//	return;
	//}
	//this->module->lights[0].setBrightness(0.0);
}

//void TrackerWidget::onDragStart(const DragStartEvent& e) {
//	APP->window->cursorLock();
//}
//void TrackerWidget::onDragMove(const DragMoveEvent& e) {
//	/// NOT CALLED = NOT MOVED
//	ModuleWidget::onDragMove(e);
//}
//void TrackerWidget::onDragEnd(const DragEndEvent& e) {
//	APP->window->cursorUnlock();
//}

void TrackerWidget::appendContextMenu(Menu *menu) {
	MenuLabel		*label;
	MenuSliderEdit	*slider;
	Param			*param_rate;


	/// [1] MENU HELP

	menu->addChild(new MenuSeparator);
	label = new MenuLabel();
	label->text = "Help";
	menu->addChild(label);

	menu->addChild(rack::createSubmenuItem("Note effects", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("Axx", "Rand amplitude (x: range)", [=](){}));
			menu->addChild(new MenuItemStay("Pxx", "Rand panning (x: range)", [=](){}));
			menu->addChild(new MenuItemStay("Dxx", "Rand delay (x: range)", [=](){}));
			menu->addChild(new MenuItemStay("Oxy", "Rand octave (x: 0(-+) 1(+) 2(-), y: range)", [=](){}));
			menu->addChild(new MenuItemStay("Nxy", "Pick note (between note, note + x and note + y)", [=](){}));
			menu->addChild(new MenuItemStay("Vxy", "Vibrato (x: speed, y: amplitude)", [=](){}));
			menu->addChild(new MenuItemStay("Txy", "Tremolo (x: speed, y: amplitude)", [=](){}));
			menu->addChild(new MenuItemStay("vxy", "Rand vibrato (x: max speed, y: max amplitude)", [=](){}));
			menu->addChild(new MenuItemStay("txy", "Rand tremolo (x: max speed, y: max amplitude)", [=](){}));
			menu->addChild(new MenuItemStay("Cxx", "Chance to play (x: chance)", [=](){}));
			menu->addChild(new MenuItemStay("cxx", "Chance to play or stops (x: chance))", [=](){}));
		}
	));
	menu->addChild(rack::createSubmenuItem("Shortcuts pattern", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("Arrows", "Move cursor", [=](){}));
			menu->addChild(new MenuItemStay("Shift + ↓/↑", "Change value", [=](){}));
			menu->addChild(new MenuItemStay("Shift + ←/→", "Change value (faster)", [=](){}));
			menu->addChild(new MenuItemStay("Backspace", "Clear line", [=](){}));
			menu->addChild(new MenuItemStay("Delete", "Delete line", [=](){}));
			menu->addChild(new MenuItemStay("Insert", "Insert line", [=](){}));
			menu->addChild(new MenuItemStay("Space", "Insert note stop", [=](){}));
		}
	));
	menu->addChild(rack::createSubmenuItem("Shortcuts timeline", "",
		[=](Menu *menu) {
			menu->addChild(new MenuItemStay("Arrows", "Move camera", [=](){}));
			menu->addChild(new MenuItemStay("Backspace / Delete", "Delete pattern instance", [=](){}));
		}
	));

	/// [2] MENU SETTINGS

	menu->addChild(new MenuSeparator());
	label = new MenuLabel();
	label->text = "Settings";
	menu->addChild(label);

	/// MIDI / KEYBOARD CHOICE
	menu->addChild(rack::createSubmenuItem("Midi", "",
		[=](Menu *menu) {
			appendMidiMenu(menu, &(this->module->midi_input));
		}
	));

	/// RATE
	param_rate = &(g_module->params[Tracker::PARAM_RATE]);
	menu->addChild(rack::createSubmenuItem("Rate", "",
		[=](Menu *menu) {
			menu->addChild(new MenuCheckItem("Sample rate / 1", "",
				[=]() { return param_rate->getValue() == 1; },
				[=]() { param_rate->setValue(1); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 2", "",
				[=]() { return param_rate->getValue() == 2; },
				[=]() { param_rate->setValue(2); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 4", "",
				[=]() { return param_rate->getValue() == 4; },
				[=]() { param_rate->setValue(4); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 8", "",
				[=]() { return param_rate->getValue() == 8; },
				[=]() { param_rate->setValue(8); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 16", "",
				[=]() { return param_rate->getValue() == 16; },
				[=]() { param_rate->setValue(16); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 32", "",
				[=]() { return param_rate->getValue() == 32; },
				[=]() { param_rate->setValue(32); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 64", "default",
				[=]() { return param_rate->getValue() == 64; },
				[=]() { param_rate->setValue(64); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 128", "",
				[=]() { return param_rate->getValue() == 128; },
				[=]() { param_rate->setValue(128); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 256", "",
				[=]() { return param_rate->getValue() == 256; },
				[=]() { param_rate->setValue(256); }
			));
			menu->addChild(new MenuCheckItem("Sample rate / 512", "",
				[=]() { return param_rate->getValue() == 512; },
				[=]() { param_rate->setValue(512); }
			));
		}
	));

	/// [3] MENU TUNING

	menu->addChild(new MenuSeparator());

	label = new MenuLabel();
	label->text = "Tuning";
	menu->addChild(label);

	/// BASE PITCH
	slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_PITCH_BASE]);
	slider->box.size.x = 200.f;
	menu->addChild(slider);
	/// TUNING
	menu->addChild(rack::createSubmenuItem("Tuning", "",
		[=](Menu *menu) {
			MenuSliderEdit	*slider;
			int				i;

			/// PRESETS
			menu->addChild(new MenuItemStay("Reset scale", "",
				[=]() { set_scale(table_temp_equal); }
			));
			//// SCALE PRESETS
			menu->addChild(rack::createSubmenuItem("Presets scale", "",
				[=](Menu *menu) {
					/// FAMILY 1 - MAJOR
					menu->addChild(rack::createSubmenuItem("Major modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian", "Major",
								[=]() { set_scale(table_scale_ionian); }
							));
							menu->addChild(new MenuItemStay("Dorian", "",
								[=]() { set_scale(table_scale_dorian); }
							));
							menu->addChild(new MenuItemStay("Phrygian", "",
								[=]() { set_scale(table_scale_phrygian); }
							));
							menu->addChild(new MenuItemStay("Lydian", "",
								[=]() { set_scale(table_scale_lydian); }
							));
							menu->addChild(new MenuItemStay("Mixolydian", "",
								[=]() { set_scale(table_scale_mixolydian); }
							));
							menu->addChild(new MenuItemStay("Aeolian", "",
								[=]() { set_scale(table_scale_aeolian); }
							));
							menu->addChild(new MenuItemStay("Locrian", "",
								[=]() { set_scale(table_scale_locrian); }
							));
						}
					));

					/// FAMILY 2 - MELODIC MINOR
					menu->addChild(rack::createSubmenuItem("Melodic minor modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian #1", "",
								[=]() { set_scale(table_scale_ionian_s1); }
							));
							menu->addChild(new MenuItemStay("Dorian #7", "",
								[=]() { set_scale(table_scale_dorian_s7); }
							));
							menu->addChild(new MenuItemStay("Phrygian #6", "",
								[=]() { set_scale(table_scale_phrygian_s6); }
							));
							menu->addChild(new MenuItemStay("Lydian #5", "",
								[=]() { set_scale(table_scale_lydian_s5); }
							));
							menu->addChild(new MenuItemStay("Mixolydian #4", "",
								[=]() { set_scale(table_scale_mixolydian_s4); }
							));
							menu->addChild(new MenuItemStay("Aeolian #3", "",
								[=]() { set_scale(table_scale_aeolian_s3); }
							));
							menu->addChild(new MenuItemStay("Locrian #2", "",
								[=]() { set_scale(table_scale_locrian_s2); }
							));
						}
					));

					/// FAMILY 3 - HARMONIC MINOR
					menu->addChild(rack::createSubmenuItem("Harmonic minor modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian #5", "",
								[=]() { set_scale(table_scale_ionian_s5); }
							));
							menu->addChild(new MenuItemStay("Dorian #4", "",
								[=]() { set_scale(table_scale_dorian_s4); }
							));
							menu->addChild(new MenuItemStay("Phrygian #3", "",
								[=]() { set_scale(table_scale_phrygian_s3); }
							));
							menu->addChild(new MenuItemStay("Lydian #2", "",
								[=]() { set_scale(table_scale_lydian_s2); }
							));
							menu->addChild(new MenuItemStay("Mixolydian #1", "",
								[=]() { set_scale(table_scale_mixolydian_s1); }
							));
							menu->addChild(new MenuItemStay("Aeolian #7", "",
								[=]() { set_scale(table_scale_aeolian_s7); }
							));
							menu->addChild(new MenuItemStay("Locrian #6", "",
								[=]() { set_scale(table_scale_locrian_s6); }
							));
						}
					));

					/// FAMILY 4 - HARMONIC MAJOR
					menu->addChild(rack::createSubmenuItem("Harmonic major modes", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("Ionian b6", "",
								[=]() { set_scale(table_scale_ionian_b6); }
							));
							menu->addChild(new MenuItemStay("Dorian b5", "",
								[=]() { set_scale(table_scale_dorian_b5); }
							));
							menu->addChild(new MenuItemStay("Phrygian b4", "",
								[=]() { set_scale(table_scale_phrygian_b4); }
							));
							menu->addChild(new MenuItemStay("Lydian b3", "",
								[=]() { set_scale(table_scale_lydian_b3); }
							));
							menu->addChild(new MenuItemStay("Mixolydian b2", "",
								[=]() { set_scale(table_scale_mixolydian_b2); }
							));
							menu->addChild(new MenuItemStay("Aeolian b1", "",
								[=]() { set_scale(table_scale_aeolian_b1); }
							));
							menu->addChild(new MenuItemStay("Locrian b7", "",
								[=]() { set_scale(table_scale_locrian_b7); }
							));
						}
					));

					/// FAMILY 5 / 6 / 7
					menu->addChild(rack::createSubmenuItem("Other scales", "",
						[=](Menu *menu) {
							/// FAMILY 5 - DIMINISHED MODES
							menu->addChild(new MenuItemStay("Diminished", "",
								[=]() { set_scale(table_scale_diminished); }
							));
							menu->addChild(new MenuItemStay("Diminished inverted", "",
								[=]() { set_scale(table_scale_diminished_inverted); }
							));

							/// FAMILY 6 - AUGMENTED MODES
							menu->addChild(new MenuItemStay("Augmented", "",
								[=]() { set_scale(table_scale_augmented); }
							));
							menu->addChild(new MenuItemStay("Augmented inverted", "",
								[=]() { set_scale(table_scale_augmented_inverted); }
							));

							/// FAMILY 7 - WHOLE TONE
							menu->addChild(new MenuItemStay("Whole tone", "",
								[=]() { set_scale(table_scale_whole); }
							));
						}
					));

					/// <12 EDO
					menu->addChild(rack::createSubmenuItem("EDO", "",
						[=](Menu *menu) {
							menu->addChild(new MenuItemStay("2EDO", "",
								[=]() { set_scale(table_scale_2edo); }
							));
							menu->addChild(new MenuItemStay("3EDO", "",
								[=]() { set_scale(table_scale_3edo); }
							));
							menu->addChild(new MenuItemStay("4EDO", "",
								[=]() { set_scale(table_scale_4edo); }
							));
							menu->addChild(new MenuItemStay("5EDO", "",
								[=]() { set_scale(table_scale_5edo); }
							));
							menu->addChild(new MenuItemStay("6EDO", "",
								[=]() { set_scale(table_scale_6edo); }
							));
							menu->addChild(new MenuItemStay("7EDO", "",
								[=]() { set_scale(table_scale_7edo); }
							));
							menu->addChild(new MenuItemStay("8EDO", "",
								[=]() { set_scale(table_scale_8edo); }
							));
							menu->addChild(new MenuItemStay("9EDO", "",
								[=]() { set_scale(table_scale_9edo); }
							));
							menu->addChild(new MenuItemStay("10EDO", "",
								[=]() { set_scale(table_scale_10edo); }
							));
							menu->addChild(new MenuItemStay("11EDO", "",
								[=]() { set_scale(table_scale_11edo); }
							));
						}
					));
				}
			));
			//// TEMPERAMENT PRESETS
			menu->addChild(rack::createSubmenuItem("Presets temperament", "",
				[=](Menu *menu) {
					menu->addChild(new MenuItemStay("Equal", "default",
						[=]() { set_temperament(table_temp_equal); }
					));
					menu->addChild(new MenuItemStay("Just", "",
						[=]() { set_temperament(table_temp_just); }
					));
					menu->addChild(new MenuItemStay("Pythagorean", "",
						[=]() { set_temperament(table_temp_pyth); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Super Just", "",
						[=]() { set_temperament(table_temp_carlos_super_just); }
					));
					menu->addChild(new MenuItemStay("Wendy Carlos Harmonic", "",
						[=]() { set_temperament(table_temp_carlos_harmonic); }
					));
					menu->addChild(new MenuItemStay("Kirnberger", "",
						[=]() { set_temperament(table_temp_kirnberger); }
					));
					menu->addChild(new MenuItemStay("Vallotti Young", "",
						[=]() { set_temperament(table_temp_vallotti_young); }
					));
					menu->addChild(new MenuItemStay("Werckmeister III", "",
						[=]() { set_temperament(table_temp_werckmeister_3); }
					));
				}
			));
			/// MANUAL
			for (i = 0; i < 12; ++i) {
				slider = new MenuSliderEdit(g_module->paramQuantities[Tracker::PARAM_TUNING + i]);
				slider->box.size.x = 200.f;
				menu->addChild(slider);
			}
		}
	));
}
