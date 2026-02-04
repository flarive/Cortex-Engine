#include "../../include/particles/global.h"

drawType engine::Global::drawtype=Basic;
float engine::Global::particleSize;

void engine::Global::setParticleSize(float _particleSize) {
	particleSize = _particleSize;
}
void engine::Global::setDrawType(int code) {
	if (code == 0)
		drawtype = Basic;
	else if (code == 1)
		drawtype = Geometry;
	else if (code == 2)
		drawtype = Instanced;
}





