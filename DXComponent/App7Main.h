#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"
#include "Content\SampleFpsTextRenderer.h"
#include "Sample2DSceneRenderer.h"

using namespace D2D1;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml;
using namespace Platform;

// Renders Direct2D and 3D content on the screen.
namespace App7
{
	public ref class App7Main sealed
	{
	public:
		App7Main();
		virtual ~App7Main();
		void CreateWindowSizeDependentResources();
		void StartRenderLoop();
		void StopRenderLoop();
		void CreateDX();
		void CreateComponent();

		// IDeviceNotify
		void OnDeviceLost();
		void OnDeviceRestored();

		void OnDpiChanged(DisplayInformation^ sender, Object^ args);
		void OnOrientationChanged(DisplayInformation^ sender, Object^ args);
		void OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args);
		void OnCompositionScaleChanged(SwapChainPanel^ sender, Object^ args);
		void OnSwapChainPanelSizeChanged(Object^ sender, SizeChangedEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Core::PointerEventArgs^ e);
		void SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel^ panel);


	private:
		void ProcessInput();
		void Update();
		bool Render();

		Concurrency::critical_section& GetCriticalSection() { return m_criticalSection; }
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		std::unique_ptr<Sample2DSceneRenderer> m_sceneRenderer;

		Windows::Foundation::IAsyncAction^ m_renderLoopWorker;
		Concurrency::critical_section m_criticalSection;

		// Rendering loop timer.
		DX::StepTimer m_timer;

		// Track current input pointer position.
		float m_pointerLocationX;
		float m_pointerLocationY;
	};
}