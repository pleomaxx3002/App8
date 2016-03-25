#pragma once


#include "Common\DeviceResources.h"
#include "Content\ShaderStructures.h"
#include "Common\StepTimer.h"
#include "Particle.h"


namespace App7
{
	class Sample2DSceneRenderer sealed
	{
	public:
		Sample2DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer, float posx, float posy);
		void Render();
		void LoadLeaf();
		void LoadSnowflake();
		void LoadFlare();

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		std::unique_ptr<Particle> m_particleSystem;
	};
};
