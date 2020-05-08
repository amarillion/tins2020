#include "isogrid.h"
#include <allegro5/allegro_primitives.h>

void IsoGrid::drawSurfaceWire (const GraphicsContext &gc, int mx, int my, int mz, int mxs, int mys, ALLEGRO_COLOR color) const
{
	float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;

	canvasFromIso_f (tilex * mx,
					tiley * my,
					tilez * mz,
					rx1, ry1);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my),
					tilez * mz,
					rx3, ry3);

	canvasFromIso_f (tilex * (mx),
					tiley * (my + mys),
					tilez * mz,
					rx4, ry4);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my + mys),
					tilez * mz,
					rx2, ry2);

	al_draw_line (rx1, ry1, 		rx3, ry3,	color, 1.0);
	al_draw_line (rx3, ry3, 		rx2, ry2,	color, 1.0);
	al_draw_line (rx2, ry2,			rx4, ry4,	color, 1.0);
	al_draw_line (rx4, ry4,			rx1, ry1,	color, 1.0);
}

void IsoGrid::drawRightWire (const GraphicsContext &gc, int mx, int my, int mz, int mxs, int mzs, ALLEGRO_COLOR color) const {

	float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;

	canvasFromIso_f (tilex * mx,
					tiley * my,
					tilez * mz,
					rx1, ry1);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my),
					tilez * mz,
					rx3, ry3);

	canvasFromIso_f (tilex * (mx),
					tiley * (my),
					tilez * (mz + mzs),
					rx4, ry4);

	canvasFromIso_f (tilex * (mx + mxs),
					tiley * (my),
					tilez * (mz + mzs),
					rx2, ry2);

	al_draw_line (rx1, ry1, 		rx3, ry3,	color, 1.0);
	al_draw_line (rx3, ry3, 		rx2, ry2,	color, 1.0);
	al_draw_line (rx2, ry2,			rx4, ry4,	color, 1.0);
	al_draw_line (rx4, ry4,			rx1, ry1,	color, 1.0);

}

void IsoGrid::drawLeftWire (const GraphicsContext &gc, int mx, int my, int mz, int mys, int mzs, ALLEGRO_COLOR color) const {

	float rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4;

	canvasFromIso_f (tilex * mx,
					tiley * my,
					tilez * mz,
					rx1, ry1);

	canvasFromIso_f (tilex * (mx),
					tiley * (my + mys),
					tilez * mz,
					rx3, ry3);

	canvasFromIso_f (tilex * (mx),
					tiley * (my),
					tilez * (mz + mzs),
					rx4, ry4);

	canvasFromIso_f (tilex * (mx),
					tiley * (my + mys),
					tilez * (mz + mzs),
					rx2, ry2);

	al_draw_line (rx1, ry1, 		rx3, ry3,	color, 1.0);
	al_draw_line (rx3, ry3, 		rx2, ry2,	color, 1.0);
	al_draw_line (rx2, ry2,			rx4, ry4,	color, 1.0);
	al_draw_line (rx4, ry4,			rx1, ry1,	color, 1.0);
}

void IsoGrid::drawWireFrame (const GraphicsContext &gc, int mx, int my, int mz, int msx, int msy, int msz, ALLEGRO_COLOR color) const
{
	float drx[7];
	float dry[7];

	int x2 = mx + msx;
	int y2 = my + msy;
	int z2 = mz + msz;

	canvasFromIso_f(mx * tilex, my * tiley, z2 * tilez,   drx[0], dry[0]);
	canvasFromIso_f(x2 * tilex, my * tiley, z2 * tilez,  drx[1], dry[1]);
	canvasFromIso_f(mx * tilex, y2 * tiley, z2 * tilez,  drx[2], dry[2]);
	canvasFromIso_f(x2 * tilex, y2 * tiley, z2 * tilez, drx[3], dry[3]);
	canvasFromIso_f(x2 * tilex, my * tiley, mz * tilez,   drx[4], dry[4]);
	canvasFromIso_f(mx * tilex, y2 * tiley, mz * tilez,   drx[5], dry[5]);
	canvasFromIso_f(x2 * tilex, y2 * tiley, mz * tilez,  drx[6], dry[6]);

	al_draw_line (gc.xofst + drx[0], gc.yofst + dry[0], gc.xofst + drx[1], gc.yofst + dry[1], color, 1.0);
	al_draw_line (gc.xofst + drx[0], gc.yofst + dry[0], gc.xofst + drx[2], gc.yofst + dry[2], color, 1.0);
	al_draw_line (gc.xofst + drx[3], gc.yofst + dry[3], gc.xofst + drx[1], gc.yofst + dry[1], color, 1.0);
	al_draw_line (gc.xofst + drx[3], gc.yofst + dry[3], gc.xofst + drx[2], gc.yofst + dry[2], color, 1.0);
	al_draw_line (gc.xofst + drx[3], gc.yofst + dry[3], gc.xofst + drx[6], gc.yofst + dry[6], color, 1.0);
	al_draw_line (gc.xofst + drx[1], gc.yofst + dry[1], gc.xofst + drx[4], gc.yofst + dry[4], color, 1.0);
	al_draw_line (gc.xofst + drx[2], gc.yofst + dry[2], gc.xofst + drx[5], gc.yofst + dry[5], color, 1.0);
	al_draw_line (gc.xofst + drx[5], gc.yofst + dry[5], gc.xofst + drx[6], gc.yofst + dry[6], color, 1.0);
	al_draw_line (gc.xofst + drx[4], gc.yofst + dry[4], gc.xofst + drx[6], gc.yofst + dry[6], color, 1.0);
}

//TODO: implement
void drawIsoPoly (GraphicsContext *gc, int num, int x[], int y[], int z[], int color);
