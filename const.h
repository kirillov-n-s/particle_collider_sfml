#pragma once

const float FRAMETIME = 1.f / 60.f;

//particles
const float MAX_VELOCITY = 4.f;
const float MAX_CHARGE = 32.f;

const float MIN_RADIUS = 8.f;
const float MID_RADIUS = 16.f;
const float MAX_RADIUS = 64.f;

const float MASS_SCALAR = 1.f;

//particle interactions
const float GRAVITY_SCALAR = 16.f;
const float ELECTRICITY_SCALAR = 256.f;
const float DRAG_SCALAR = FRAMETIME;
const float FORCE_SCALAR = 32.f;
const float LOSS = 0.1f;

//constant forces
const float G_FIELD_SCALAR = FRAMETIME;
const float EM_FIELD_SCALAR = FRAMETIME;
