#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

enum Element { aether, fire };
struct Particle
{
	float x;
	float y;
	Element element;
	int ticks;
	int lifetime;

	Particle(float x, float y, Element element, int lifetime)
	{
		this->x = x;
		this->y = y;
		this->element = element;
		this->lifetime = lifetime;

		ticks = 0;
	}
};

class ParticleEngine
{
public:
	static ParticleEngine main;
	float lastTick;
	float tickDelay;
	std::vector<Particle*> particles;

	void Init(float tickDelay)
	{
		// Do absolutely fucking nothing.
		lastTick = 0.0f;
		this->tickDelay = tickDelay;
	}

	void AddParticle(float x, float y, Element element, int lifetime)
	{
		Particle* p = new Particle(x, y, element, lifetime);
		particles.push_back(p);
	}

	void AddParticles(int number, float x, float y, Element element, int lifetime)
	{
		for (int i = 0; i < number; i++)
		{
			Particle* p = new Particle(x, y, element, lifetime);
			particles.push_back(p);
		}
	}

	void RemoveParticle(Particle* p)
	{
		particles.erase(std::remove(particles.begin(), particles.end(), p), particles.end());
		delete p;
	}

	void Update(float deltaTime)
	{
		if (lastTick > tickDelay)
		{
			lastTick = 0.0f;

			for (int p = 0; p < particles.size(); p++)
			{
				Particle* particle = particles[p];
				Texture2D* s = Game::main.textureMap["blank_pixel"];
				glm::vec4 color;

				int r = rand() % 100 + 1;
				float cr = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

				if (particle->element == fire)
				{
					color = glm::vec4(cr, 0.0f, 0.0f, 1.0f);

					particle->y += 5.0f;

					if (r > 75)
					{
						particle->x += 5.0f;
					}
					else if (r > 50)
					{
						particle->x -= 5.0f;
					}
					else if (r < 10)
					{
						particle->y -= 5.0f;
					}
				}
				else
				{
					color = glm::vec4(0.0f, 0.8f, cr, 1.0f);

					if (r > 90)
					{
						particle->x += 5.0f;
					}
					else if (r > 80)
					{
						particle->x -= 5.0f;
					}
					else if (r < 70)
					{
						particle->y += 5.0f;
					}
				}

				if (particle->ticks < particle->lifetime)
				{
					particle->ticks += 1;
					// Game::main.renderer->prepareQuad(glm::vec2(particle->x, particle->y), 1.0f, 1.0f, color, s->ID);
					Game::main.renderer->prepareQuad(glm::vec2(particle->x, particle->y), s->width / 3.0f, s->height / 3.0f, color, s->ID);
				}
				else
				{
					RemoveParticle(particle);
				}
			}
		}
		else
		{
			lastTick += deltaTime;

			for (int p = 0; p < particles.size(); p++)
			{
				Particle* particle = particles[p];
				Texture2D* s = Game::main.textureMap["blank_pixel"];
				glm::vec4 color;

				float cr = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

				if (particle->element == fire)
				{
					color = glm::vec4(cr, 0.0f, 0.0f, 1.0f);
				}
				else
				{
					color = glm::vec4(0.0f, 0.8f, cr, 1.0f);
				}

				Game::main.renderer->prepareQuad(glm::vec2(particle->x, particle->y), s->width / 3.0f, s->height / 3.0f, color, s->ID);
			}
		}
	}
};


#endif
