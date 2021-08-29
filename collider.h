#pragma once
#include <vector>
#include <algorithm>
#include "particle.h"

class collider
{
	//operate subdivision
	const int CYCLES = 4;

	//forces
	const float NEWTON = 16.f;
	const float COULOMB = 256.f;
	const float DRAG = 1.f / 60.f;
	const float GRAVITY = 0.1f;
	const float ELECTRO = 1.f;
	const float MAGNET = 0.05f;
	const float FORCE = 64.f;

	enum class wall
	{
		none,
		top,
		bottom,
		left,
		right
	};

	using collision = std::pair<particle*, particle*>;
	using wall_collision = std::pair<particle*, wall>;

	std::vector<particle*> _particles;

	uint32_t _width;
	uint32_t _height;

	int _newton = 0;
	bool _coulomb = false;
	bool _drag = false;

	float _max_speed = 0.f;
	float _max_pos_charge = 0.f;
	float _max_neg_charge = 0.f;

	vec2f _Gfield = ZERO2;
	vec2f _Efield = ZERO2;
	vec3f _Bfield = ZERO3;

	bool in_bounds(particle* particle) const;

	vec2f wall_project(particle* particle, wall wall);
	float wall_distance(particle* particle, wall wall);
	vec2f wall_path(particle* particle, wall wall);
	bool wall_collides(particle* particle, wall wall);
	void wall_resolve_static(particle* particle, wall wall);
	void wall_resolve_dynamic(particle* particle, wall wall);

	void advance_particles();
	void process_wall_collisions();
	void process_particle_collisions();
	void accelerate_particles();
	void purge_particles();

	void update_stats();

public:
	collider(uint32_t width, uint32_t height);
	~collider();

	void operate();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t count() const;

	int get_newton() const;
	bool get_coulomb() const;
	bool get_drag() const;

	void toggle_newton();
	void toggle_coulomb();
	void toggle_drag();

	std::vector<particle*> particles() const;
	void launch(particle* particle);
	void erase(particle* particle);
	void erase();
	particle* get(const vec2f& coords) const;
	void clear();

	float mech_stat(particle* particle) const;
	float elec_stat(particle* particle) const;

	void apply_force(const vec2f& point, const vec2f& vec);

	vec2f get_G_field() const;
	vec3f get_EM_field() const;

	void set_G_field(const vec2f& dir);
	void set_E_field(const vec2f& dir);
	void set_B_field(const vec3f& dir);
};
