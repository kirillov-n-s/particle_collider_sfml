#pragma once
#include "application.h"

int main()
{
	collider* cld = new collider(2560, 1440);
	application app(cld, "Particle Collider");

	app.load(400);
	app.run();

	delete cld;
}