
#include "Tree.hpp"

////////////////////////////////////////////////////////////////////////////////
/// PRIVATE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

TreeDisplay::TreeDisplay() {
}

void TreeDisplay::draw(const DrawArgs &args) {
	Rect		rect;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	/// RECT BACKGROUND
	nvgBeginPath(args.vg);
	nvgFillColor(args.vg, colors[0]);
	nvgRect(args.vg, RECT_ARGS(rect));
	nvgFill(args.vg);
}

void TreeDisplay::drawLayer(const DrawArgs &args, int layer) {
	TreeBranch	*branch;
	Rect		rect;
	Vec			center;
	float		width;
	int			i;

	if (this->module == NULL || layer != 1)
		return;

	/// GET CANVAS FORMAT
	rect = box.zeroPos();
	center.x = rect.pos.x + rect.size.x * 0.5;
	center.y = rect.pos.y + rect.size.y;

	char	str[128];
	sprintf(str, "count [%d]", this->module->branch_count);
	nvgFillColor(args.vg, colors[3]);
	nvgText(args.vg, rect.pos.x + 10, rect.pos.y + 10.0, str, NULL);

	nvgTranslate(args.vg, center.x, center.y);
	nvgScale(args.vg, 2.0, 2.0);
	nvgRotate(args.vg, -M_PI / 2.0);
	for (i = 0; i < this->module->branch_count; ++i) {
		branch = &(this->module->branches[i]);
		width = branch->length * 0.1;
		nvgStrokeColor(args.vg, colors[4]);
		nvgStrokeWidth(args.vg, width);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, branch->pos_root.x, branch->pos_root.y);
		nvgLineTo(args.vg, branch->pos_tail.x, branch->pos_tail.y);
		nvgStroke(args.vg);
		nvgClosePath(args.vg);
	}
	nvgResetTransform(args.vg);
}
