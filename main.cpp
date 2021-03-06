#pragma once
#include "application.h"

int main()
{
	collider* cld = new collider(2560, 1440);
	emitter* emt = new emitter(2560, 1440);
	application app(cld, emt, "Particle Collider");

	app.load(1200);
	app.run();

	delete cld;
	delete emt;
}
