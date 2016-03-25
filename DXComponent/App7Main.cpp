#include "pch.h"
#include "App7Main.h"
#include "Common\DirectXHelper.h"

using namespace App7;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
App7Main::App7Main()
{
	// Register to be notified if the Device is lost or recreated
	//m_deviceResources->RegisterDeviceNotify(this);

}

App7Main::~App7Main()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void App7Main::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

void App7Main::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(m_criticalSection);
			Update();
			if (Render())
			{
				m_deviceResources->Present();
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void App7Main::StopRenderLoop()
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->Trim();
	m_renderLoopWorker->Cancel();
}

void App7::App7Main::CreateDX()
{
	m_deviceResources = std::make_shared<DX::DeviceResources>();
}

void App7::App7Main::CreateComponent()
{
	// TODO: Replace this with your app's content initialization.
	m_sceneRenderer = std::unique_ptr<Sample2DSceneRenderer>(new Sample2DSceneRenderer(m_deviceResources));


	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);

	m_sceneRenderer->LoadLeaf();
}

// Updates the application state once per frame.
void App7Main::Update() 
{
	//ProcessInput();

	// Update scene objects.
	m_timer.Tick([&]()
	{
		m_sceneRenderer->Update(m_timer, m_pointerLocationX, m_pointerLocationY);
	});
}

// Process all input from the user before updating game state
void App7Main::ProcessInput()
{
	// TODO: Add per frame input handling here.
	//m_sceneRenderer->TrackingUpdate(m_pointerLocationX);
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool App7Main::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	m_sceneRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void App7Main::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void App7Main::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void App7::App7Main::OnDpiChanged(DisplayInformation ^ sender, Object ^ args)
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->SetDpi(sender->LogicalDpi);
	CreateWindowSizeDependentResources();
}

void App7::App7Main::OnOrientationChanged(DisplayInformation ^ sender, Object ^ args)
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
	CreateWindowSizeDependentResources();
}

void App7::App7Main::OnDisplayContentsInvalidated(DisplayInformation ^ sender, Object ^ args)
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->ValidateDevice();
}

void App7::App7Main::OnCompositionScaleChanged(SwapChainPanel ^ sender, Object ^ args)
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->SetCompositionScale(sender->CompositionScaleX, sender->CompositionScaleY);
	CreateWindowSizeDependentResources();
}

void App7::App7Main::OnSwapChainPanelSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e)
{
	critical_section::scoped_lock lock(GetCriticalSection());
	m_deviceResources->SetLogicalSize(e->NewSize);
	CreateWindowSizeDependentResources();
}

void App7::App7Main::OnPointerMoved(Platform::Object ^ sender, Windows::UI::Core::PointerEventArgs ^ e)
{
	m_pointerLocationX = e->CurrentPoint->Position.X;
	m_pointerLocationY = e->CurrentPoint->Position.Y;
}

void App7::App7Main::SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel ^ panel)
{
	m_deviceResources->SetSwapChainPanel(panel);
}
