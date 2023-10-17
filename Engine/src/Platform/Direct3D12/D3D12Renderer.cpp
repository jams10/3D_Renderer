
#include "D3D12Renderer.h"
#include "D3D12RenderSurface.h"
#include "Utils/CustomLog.h"

namespace Engine::Graphics::D3D12
{
	D3D12_Renderer* D3D12_Renderer::_instance = new D3D12_Renderer();

	D3D12_Renderer::D3D12_Renderer()
		:Renderer()
	{
	}

	D3D12_Renderer::~D3D12_Renderer()
	{
	}

	// Direct3D12 API ����� ���� �ʱ�ȭ �Լ�.
	bool D3D12_Renderer::Initialize(Window* wnd)
	{
		// �̹� MainDevice ��ü�� ���� �Ǿ��ٸ� �׳� �ݾ���.
		if (_main_device != nullptr)
			Shutdown();

		uint32 dxgiFactoryFlags{ 0 };
#ifdef _DEBUG
		{
			// ����� ���� Ȱ��ȭ.
			ComPtr<ID3D12Debug3> debug_interface;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface))))
			{
				debug_interface->EnableDebugLayer();
			}
			else
			{
				OutputDebugStringA("Warning : D3D12 Debug interface is not available. Verify that Graphics Tools optional feature is installed in this system.\n");
			}
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // _DEBUG

		HRESULT hr{ S_OK };
		// 1. DXGIFactor ��ü ����.
		DXCHECK(hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_dxgi_factory)));
		if (FAILED(hr)) return Failed_Initialize();

		// 2. � �����(���� �׷���ī��)�� �� ������ ������.
		ComPtr<IDXGIAdapter4> main_adapter;
		main_adapter.Attach(Determine_MainAdapter());
		if (!main_adapter) return Failed_Initialize();

		D3D_FEATURE_LEVEL max_feature_level{ Get_Maximum_Feature_Level(main_adapter.Get()) }; // ������ adapter�� �ִ� feature level�� ����.
		assert(max_feature_level >= minimum_feature_level);
		if (max_feature_level < minimum_feature_level) return Failed_Initialize();

		// 3. ����̽� ����.
		DXCHECK(hr = D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&_main_device)));
		if (FAILED(hr)) return Failed_Initialize();

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> info_queue;
			DXCHECK(_main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

			// CORRUPTION, WARNING, ERROR D3D12 �޽��� �߻� ��, ���ø����̼ǿ��� break point�� �������� ��.
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}
#endif // _DEBUG

		// 4. Command Ŭ���� �ν��Ͻ� ����.
		_gfx_command = std::make_unique<D3D12_Command>(_main_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (!_gfx_command->Command_Queue()) return Failed_Initialize();

		// 5. descriptor heap ����.
		bool result{ true };
		result &= _rtv_desc_heap.Initialize(512, false);
		result &= _dsv_desc_heap.Initialize(512, false);
		result &= _srv_desc_heap.Initialize(4096, false); // ���̴��� ���ø��� ���Ǵ� �ؽ��� �ڿ��� ���� srv heap�� ��� ũ�⸦ ũ�� �����. 
		result &= _uav_desc_heap.Initialize(512, false);
		if (!result) return Failed_Initialize();

		// ��ũ�θ� �̿��ؼ� device ��ü�� �̸��� �ٿ���.
		NAME_D3D12_OBJECT(_main_device, L"Main D3D12 Device");
		NAME_D3D12_OBJECT(_rtv_desc_heap.Heap(), L"RTV Descriptor Heap");
		NAME_D3D12_OBJECT(_dsv_desc_heap.Heap(), L"DSV Descriptor Heap");
		NAME_D3D12_OBJECT(_srv_desc_heap.Heap(), L"SRV Descriptor Heap");
		NAME_D3D12_OBJECT(_uav_desc_heap.Heap(), L"UAV Descriptor Heap");

		// 6. Render Surface ����.(���� ü��, ����Ʈ, scissor rect ����.)
		_render_surface = std::make_unique<D3D12_RenderSurface>(wnd);
		_render_surface->Create_Swap_Chain(_dxgi_factory, _gfx_command->Command_Queue(), render_target_format);

		LOG_WARN("Initialized : D3D12Renderer class!");

		return true;
	}

	// Direct3D12 ���� �� ���� �Լ�.
	void D3D12_Renderer::Shutdown()
	{
		_gfx_command->Release();
		_render_surface->Release();

		// swap chain ��� ���� ���ҽ����� ���� ���ҽ����� �����Ǳ� �� ���� ���� �� �� �����Ƿ� ���� Process_Deferred_Release()�� ȣ��, ����ϴ� �ڿ����� ���� ����.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Process_Deferred_Release(i);
		}

		Release(_dxgi_factory);

		_rtv_desc_heap.Release();
		_dsv_desc_heap.Release();
		_srv_desc_heap.Release();
		_uav_desc_heap.Release();

		// ������ �� heap�� �ִ� descriptor���� ��� �����ϰ�, �� heap�� Release() �Լ��� ȣ���� ���� ��� �迭�� heap COM ��ü���� �־� �־���.
		// ���� ��� �迭�� �ִ� COM ��ü���� ������ �����ϱ� ���� �ٽ� �� ���� ���� ������ �����ϴ� Process_Deferred_Release() ȣ��, heap���� ���� ����.
		Process_Deferred_Release(0);

#ifdef _DEBUG
		{
			{
				ComPtr<ID3D12InfoQueue> info_queue;
				DXCHECK(_main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
			}

			// ���α׷��� ������ �� �ؾ������ release ���� ���� COM ��ü�� ������ üũ.
			ComPtr<ID3D12DebugDevice2> debug_device;
			DXCHECK(_main_device->QueryInterface(IID_PPV_ARGS(&debug_device)));
			Release(_main_device); // debug_device �����ϰ� ���� �ִ� COM ��ü�� ������ main_device ������.
			DXCHECK(debug_device->ReportLiveDeviceObjects(
				D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
#endif // _DEBUG

		Release(_main_device);
	}

	//// Direct3D12 API�� ����� ������ �۾��� �����ϴ� �Լ�.
	void D3D12_Renderer::Render()
	{
		// ���� �����ӿ� ����� ��ɵ��� �������� üũ�ϰ� �������� Ŀ�ǵ� �Ҵ��ڸ� �ʱ�ȭ �Ͽ� ��ɵ��� ��ϵ� �޸𸮸� ������.
		// Ŀ�ǵ� �Ҵ��� �ʱ�ȭ �Ŀ� Ŀ�ǵ� ����Ʈ�� �ʱ�ȭ �Ͽ� �� ��ɵ��� ����ϱ� ���� ������.
		_gfx_command->Begin_Frame();

		ID3D12GraphicsCommandList6* cmd_list{ _gfx_command->Command_List() };
		// ��� ���.

		// ��� ���� �ʴ� �ڿ� ����.
		const uint32 frame_idx{ _gfx_command->Frame_Index() };
		if (_deferred_release_flag[frame_idx]) // ���� ������ �ε����� ���� release flag�� true�� ���.
		{
			Process_Deferred_Release(frame_idx);
		}

		// ��� ����� ������ ��ɵ��� ť�� �����ϰ�, ��ɵ��� �������� ���θ� üũ�ϱ� ���� fence value�� 1 ���� ��Ű��, Signal() �Լ��� ȣ��.
		_gfx_command->End_Frame();
	}

	void D3D12_Renderer::Set_Deferred_Release_Flag()
	{
		// lock�� ������� �ʰ� flag ���� �ٲ��ְ� ����.
		// x86, x64 ��Ű���Ŀ��� ���� ������ ���� �־��ִ� ���� atomic�ϰ� ó����. ���� ���� �ٸ� thread���� ������ �� lock�� �ɾ��� �ʿ䰡 ����.
		_instance->_deferred_release_flag[_instance->_gfx_command->Frame_Index()] = 1;
	}

	void D3D12_Renderer::Deferred_Release(IUnknown* resource)
	{
		if (_instance->_gfx_command == nullptr) return;
		const uint32 frame_idx{ _instance->_gfx_command->Frame_Index() };
		std::lock_guard lock{ _instance->_deferred_release_mutex };
		_instance->_deferred_release[frame_idx].push_back(resource);
		Set_Deferred_Release_Flag();
	}

	// Direct3D12 �ʱ�ȭ�� ���� ���� �� ȣ������ �Լ�.
	bool D3D12_Renderer::Failed_Initialize()
	{
		Shutdown();
		return false;
	}

	// ���������� ����� �����(�׷��� ī��)�� �����ϴ� �Լ�.
	// ������ �ּ� feature level�� �����ϴ� �� ���θ� üũ�� �ְ� ������, ��� ��ġ�� �ִ���, �����ϴ� �ػ� �� ���� ���ǵ��� �߰��� �� ����.
	IDXGIAdapter4* D3D12_Renderer::Determine_MainAdapter()
	{
		IDXGIAdapter4* adapter{ nullptr };
		// ���� ���� ���� ������ ����͵��� �����ϰ�, �츮�� ������ �ּ� feature level�� �����ϴ� ù ��° ����͸� ������.
		for (uint32 i{ 0 };
			_dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			// ����͸� ������ �ּ� feature level�� �����ϴ� device�� ������ �� �ִ��� �õ�, ���� �� ������ �ش� ����͸� ������.
			if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}
			Release(adapter);
		}
		return nullptr;
	}

	// ���ڷ� �־��� adapter�� �����ϴ� �ִ� feature level�� �����ϴ� �Լ�.
	D3D_FEATURE_LEVEL D3D12_Renderer::Get_Maximum_Feature_Level(IDXGIAdapter4* adapter)
	{
		constexpr D3D_FEATURE_LEVEL feature_levels[4]{
						D3D_FEATURE_LEVEL_11_0,
						D3D_FEATURE_LEVEL_11_1,
						D3D_FEATURE_LEVEL_12_0,
						D3D_FEATURE_LEVEL_12_1,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS feature_level_info{};
		feature_level_info.NumFeatureLevels = _countof(feature_levels);
		feature_level_info.pFeatureLevelsRequested = feature_levels;

		// device�� �����غ���, CheckFeatureSupport() �Լ��� ���� �����ϴ� �ִ� feature level�� ����.
		ComPtr<ID3D12Device> device;
		DXCHECK(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)));
		DXCHECK(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_level_info, sizeof(feature_level_info)));
		return feature_level_info.MaxSupportedFeatureLevel;
	}

	void D3D12_Renderer::Process_Deferred_Release(uint32 frame_idx)
	{
		std::lock_guard lock{ _deferred_release_mutex };

		// ���� ���� �ڿ� ���� �۾��� �ռ� frame_idx �ε����� �ش��ϴ� �ڿ� ���� ǥ�� flag ���� 0���� ��������.
		_deferred_release_flag[frame_idx] = 0;

		// heap�� �ִ� descriptor ����.
		_rtv_desc_heap.Process_Deferred_Free(frame_idx);
		_dsv_desc_heap.Process_Deferred_Free(frame_idx);
		_srv_desc_heap.Process_Deferred_Free(frame_idx);
		_uav_desc_heap.Process_Deferred_Free(frame_idx);

		// heap�� �Ҵ��� descriptor�� �ƴ� �ٸ� �ڿ����� ������. (heap ��)
		// ���� ������ �ε����� �ش��ϴ� ���� ��� �ڿ��鿡 ���� �����͸� ���� ���͸� ������.
		std::vector<IUnknown*>& resources{ _deferred_release[frame_idx] };
		if (!resources.empty())
		{
			for (auto& resource : resources) Release(resource);
			resources.clear();
		}
	}
}