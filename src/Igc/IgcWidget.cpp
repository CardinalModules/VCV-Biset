
#include "Igc.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

IgcWidget::IgcWidget(Igc* _module) {
	IgcDisplay	*display;

	this->module = _module;
	setModule(this->module);
	setPanel(createPanel(asset::plugin(pluginInstance, "res/Pkm.svg")));

	display = createWidget<IgcDisplay>(mm2px(Vec(0.0, 0.0)));
	display->box.size = mm2px(Vec(40.64, 60.0));
	display->module = module;
	display->moduleWidget = this;
	addChild(display);
}

//void IgcWidget::appendContextMenu(Menu *menu) {
//}
