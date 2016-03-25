#include "pch.h"
#include "Sample2DSceneRenderer.h"

#include "Common\DirectXHelper.h"

using namespace App7;




Sample2DSceneRenderer::Sample2DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	m_particleSystem = std::unique_ptr<Particle>(new Particle(deviceResources));
	m_particleSystem->CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
}

void Sample2DSceneRenderer::CreateDeviceDependentResources()
{
	m_particleSystem->CreateDeviceDependentResources();
}

void Sample2DSceneRenderer::CreateWindowSizeDependentResources()
{
	Windows::Foundation::Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

}

void Sample2DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_particleSystem->ReleaseDeviceDependentResources();
}

void Sample2DSceneRenderer::Update(DX::StepTimer const & timer, float posx, float posy)
{
	m_particleSystem->Update(timer, posx, posy);
}

void Sample2DSceneRenderer::Render()
{
	if (!m_particleSystem->m_loaded)
	{
		return;
	}
	auto context = m_deviceResources->GetD2DDeviceContext();
	m_deviceResources->GetD2DDeviceContext()->BeginDraw();
	context->Clear(D2D1::ColorF(0., 1., 0., 0.));
	m_particleSystem->Render();
	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->EndDraw()
		);


}
void App7::Sample2DSceneRenderer::LoadLeaf()
{
	m_particleSystem->LoadTexture(L"leaf.png");
}

void App7::Sample2DSceneRenderer::LoadSnowflake()
{
	m_particleSystem->LoadTexture(L"Snowflake.png");
}

void App7::Sample2DSceneRenderer::LoadFlare()
{
	m_particleSystem->LoadTexture(L"flare.png");
}
