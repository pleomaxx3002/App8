#pragma once
#include "Common\DeviceResources.h"
//#include "Common\ShaderStructures.h"
#include "Common\StepTimer.h"
#include <vector>

const int MAX_PARTICLES = 30;
const double PARTICLES_PER_SECOND = 10.;
const double PARTICLE_LIFETIME = 1.;
const double PARTICLE_SIZE = 1.;

namespace App7
{
	class Particle sealed
	{
		class Particles
		{
		public:
			double x, y;
			double time;
			bool active;
		};
	public:
		Particle(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer, float posx, float posy);
		void Render();
		void LoadTexture(LPCWSTR path);
		void KillAllParticles();
		bool m_loaded;


	private:
		void EmitParticles(DX::StepTimer const& timer, float x = 200, float y = 200);
		void KillParticles(DX::StepTimer const& timer);


		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		Microsoft::WRL::ComPtr<ID2D1Bitmap> m_image;
		double height, width;

		std::vector<Particles> m_particleList;

		int m_maxParticles;
		int m_currentParticles;
		double m_accomulatedTime;
		double m_particlesPerSecond;
		double m_particleSize;
		double m_lifetime;
	};
};
