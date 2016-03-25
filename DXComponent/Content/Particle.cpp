#include "pch.h"
#include "Sample2DSceneRenderer.h"
#include <stdio.h>
#include "Common\DirectXHelper.h"
#include "Particle.h"

using namespace App7;

using namespace DirectX;
using namespace Windows::Foundation;

Particle::Particle(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_currentParticles(0),
	m_maxParticles(MAX_PARTICLES),
	m_accomulatedTime(0),
	m_particlesPerSecond(PARTICLES_PER_SECOND),
	m_lifetime(PARTICLE_LIFETIME),
	m_particleSize(PARTICLE_SIZE),
	m_loaded(false)
{
	m_particleList.resize(m_maxParticles);
	KillAllParticles();
}

void Particle::CreateDeviceDependentResources()
{
}

void Particle::CreateWindowSizeDependentResources()
{
}

void Particle::ReleaseDeviceDependentResources()
{
	m_image.Reset();
}

void Particle::Update(DX::StepTimer const & timer, float posx, float posy)
{
	// Release old particles.
	KillParticles(timer);

	// Emit new particles.
	EmitParticles(timer, posx, posy);
}

void Particle::Render()
{
	if (!m_loaded || m_image.Get() == nullptr)
		return;
	auto context = m_deviceResources->GetD2DDeviceContext();
	double h2 = m_particleSize * height / 2, w2 = width / 2 * m_particleSize;
	for (int i = 0; i < m_maxParticles; ++i)
	{
		if (m_particleList[i].active)
		{
			context->DrawImage(m_image.Get(), D2D1::Point2F(m_particleList[i].x - h2, m_particleList[i].y - w2));
		}
	}
}

void Particle::LoadTexture(LPCWSTR path)
{
	m_loaded = false;
	auto t = Concurrency::create_task([this, path]()->void {
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder;
	m_image.Reset();
	DX::ThrowIfFailed(m_deviceResources->GetWicImagingFactory()->CreateDecoderFromFilename(
		path,                      // Image to be decoded
		NULL,                            // Do not prefer a particular vendor
		GENERIC_READ,                    // Desired read access to the file
		WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
		pDecoder.GetAddressOf()          // Pointer to the decoder
		)
		);
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame;
	DX::ThrowIfFailed(pDecoder->GetFrame(0, pFrame.GetAddressOf()));
	Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
	DX::ThrowIfFailed(m_deviceResources->GetWicImagingFactory()->CreateFormatConverter(converter.GetAddressOf()));
	DX::ThrowIfFailed(converter->Initialize(pFrame.Get(),                          // Input bitmap to convert
		GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
		WICBitmapDitherTypeNone,         // Specified dither pattern
		NULL,                            // Specify a particular palette 
		0.f,                             // Alpha threshold
		WICBitmapPaletteTypeCustom       // Palette translation type
		)
		);
	DX::ThrowIfFailed(m_deviceResources->GetD2DDeviceContext()->CreateBitmapFromWicBitmap(converter.Get(), NULL, m_image.GetAddressOf()));
	converter.Reset();
		pFrame.Reset();
	pDecoder.Reset();
	});

	t.then([this]() {
		m_loaded = true;
	});
}

void App7::Particle::KillAllParticles()
{
	for (int i = 0; i < m_maxParticles; ++i)
	{
		m_particleList[i].active = false;
	}
}

void App7::Particle::EmitParticles(DX::StepTimer const & timer, float x, float y)
{
	if (m_currentParticles == m_maxParticles)
	{
		return;
	}
	m_accomulatedTime += timer.GetElapsedSeconds();
	if (m_accomulatedTime * m_particlesPerSecond > 1.)
	{
		m_accomulatedTime = 0.;
		int index = 0;
		for (; index < m_maxParticles && m_particleList[index].active; ++index);
		if (index == m_maxParticles) return;
		m_particleList[index].active = true;
		m_particleList[index].x = x;
		m_particleList[index].y = y;
		m_particleList[index].time = timer.GetTotalSeconds();
		m_currentParticles++;

	}
}

void App7::Particle::KillParticles(DX::StepTimer const & timer)
{
	for (int i = 0; i < m_maxParticles; ++i)
	{
		if (m_particleList[i].active && (timer.GetTotalSeconds() - m_particleList[i].time > m_lifetime))
		{
			m_particleList[i].active = false;
			m_currentParticles--;
		}
	}
}
