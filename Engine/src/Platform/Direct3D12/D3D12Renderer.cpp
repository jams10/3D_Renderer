
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

	// Direct3D12 API 사용을 위한 초기화 함수.
	bool D3D12_Renderer::Initialize(Window* wnd)
	{
		// 이미 MainDevice 객체가 생성 되었다면 그냥 닫아줌.
		if (_main_device != nullptr)
			Shutdown();

		uint32 dxgiFactoryFlags{ 0 };
#ifdef _DEBUG
		{
			// 디버그 계층 활성화.
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
		// 1. DXGIFactor 객체 생성.
		DXCHECK(hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&_dxgi_factory)));
		if (FAILED(hr)) return Failed_Initialize();

		// 2. 어떤 어댑터(보통 그래픽카드)를 쓸 것인지 결정함.
		ComPtr<IDXGIAdapter4> main_adapter;
		main_adapter.Attach(Determine_MainAdapter());
		if (!main_adapter) return Failed_Initialize();

		D3D_FEATURE_LEVEL max_feature_level{ Get_Maximum_Feature_Level(main_adapter.Get()) }; // 선택한 adapter의 최대 feature level을 구함.
		assert(max_feature_level >= minimum_feature_level);
		if (max_feature_level < minimum_feature_level) return Failed_Initialize();

		// 3. 디바이스 생성.
		DXCHECK(hr = D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&_main_device)));
		if (FAILED(hr)) return Failed_Initialize();

#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> info_queue;
			DXCHECK(_main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

			// CORRUPTION, WARNING, ERROR D3D12 메시지 발생 시, 애플리케이션에서 break point가 잡히도록 함.
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}
#endif // _DEBUG

		// 4. Command 클래스 인스턴스 생성.
		_gfx_command = std::make_unique<D3D12_Command>(_main_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		if (!_gfx_command->Command_Queue()) return Failed_Initialize();

		// 5. descriptor heap 생성.
		bool result{ true };
		result &= _rtv_desc_heap.Initialize(512, false);
		result &= _dsv_desc_heap.Initialize(512, false);
		result &= _srv_desc_heap.Initialize(4096, false); // 쉐이더에 샘플링해 사용되는 텍스쳐 자원을 위한 srv heap의 경우 크기를 크게 잡아줌. 
		result &= _uav_desc_heap.Initialize(512, false);
		if (!result) return Failed_Initialize();

		// 매크로를 이용해서 device 객체에 이름을 붙여줌.
		NAME_D3D12_OBJECT(_main_device, L"Main D3D12 Device");
		NAME_D3D12_OBJECT(_rtv_desc_heap.Heap(), L"RTV Descriptor Heap");
		NAME_D3D12_OBJECT(_dsv_desc_heap.Heap(), L"DSV Descriptor Heap");
		NAME_D3D12_OBJECT(_srv_desc_heap.Heap(), L"SRV Descriptor Heap");
		NAME_D3D12_OBJECT(_uav_desc_heap.Heap(), L"UAV Descriptor Heap");

		// 6. Render Surface 생성.(스왑 체인, 뷰포트, scissor rect 생성.)
		_render_surface = std::make_unique<D3D12_RenderSurface>(wnd);
		_render_surface->Create_Swap_Chain(_dxgi_factory, _gfx_command->Command_Queue(), render_target_format);

		LOG_WARN("Initialized : D3D12Renderer class!");

		return true;
	}

	// Direct3D12 정리 및 종료 함수.
	void D3D12_Renderer::Shutdown()
	{
		_gfx_command->Release();
		_render_surface->Release();

		// swap chain 등과 같은 리소스들은 종속 리소스들이 해제되기 전 까지 해제 할 수 없으므로 먼저 Process_Deferred_Release()를 호출, 사용하는 자원들을 해제 해줌.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Process_Deferred_Release(i);
		}

		Release(_dxgi_factory);

		_rtv_desc_heap.Release();
		_dsv_desc_heap.Release();
		_srv_desc_heap.Release();
		_uav_desc_heap.Release();

		// 위에서 각 heap에 있는 descriptor들을 모두 해제하고, 각 heap의 Release() 함수를 호출해 삭제 대기 배열에 heap COM 객체들을 넣어 주었음.
		// 따라서 대기 배열에 있는 COM 객체들을 완전히 해제하기 위해 다시 한 번만 지연 해제를 수행하는 Process_Deferred_Release() 호출, heap들을 해제 해줌.
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

			// 프로그램을 종료할 때 잊어버리고 release 하지 않은 COM 객체가 없는지 체크.
			ComPtr<ID3D12DebugDevice2> debug_device;
			DXCHECK(_main_device->QueryInterface(IID_PPV_ARGS(&debug_device)));
			Release(_main_device); // debug_device 제외하고 남아 있는 COM 객체가 없도록 main_device 릴리즈.
			DXCHECK(debug_device->ReportLiveDeviceObjects(
				D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
#endif // _DEBUG

		Release(_main_device);
	}

	//// Direct3D12 API를 사용해 렌더링 작업을 수행하는 함수.
	void D3D12_Renderer::Render()
	{
		// 이전 프레임에 기록한 명령들이 끝났는지 체크하고 끝났으면 커맨드 할당자를 초기화 하여 명령들이 기록된 메모리를 날려줌.
		// 커맨드 할당자 초기화 후에 커맨드 리스트도 초기화 하여 새 명령들을 기록하기 위해 열어줌.
		_gfx_command->Begin_Frame();

		ID3D12GraphicsCommandList6* cmd_list{ _gfx_command->Command_List() };
		// 명령 기록.

		// 사용 하지 않는 자원 해제.
		const uint32 frame_idx{ _gfx_command->Frame_Index() };
		if (_deferred_release_flag[frame_idx]) // 현재 프레임 인덱스에 대한 release flag가 true인 경우.
		{
			Process_Deferred_Release(frame_idx);
		}

		// 명령 기록이 끝나면 명령들을 큐에 제출하고, 명령들이 끝났는지 여부를 체크하기 위해 fence value를 1 증가 시키고, Signal() 함수를 호출.
		_gfx_command->End_Frame();
	}

	void D3D12_Renderer::Set_Deferred_Release_Flag()
	{
		// lock을 사용하지 않고 flag 값을 바꿔주고 있음.
		// x86, x64 아키텍쳐에서 정수 변수에 값을 넣어주는 것은 atomic하게 처리됨. 따라서 서로 다른 thread들이 접근할 때 lock을 걸어줄 필요가 없음.
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

	// Direct3D12 초기화에 실패 했을 때 호출해줄 함수.
	bool D3D12_Renderer::Failed_Initialize()
	{
		Shutdown();
		return false;
	}

	// 렌더러에서 사용할 어댑터(그래픽 카드)를 선택하는 함수.
	// 지금은 최소 feature level을 충족하는 지 여부만 체크해 주고 있지만, 출력 장치가 있는지, 지원하는 해상도 등 여러 조건들을 추가할 수 있음.
	IDXGIAdapter4* D3D12_Renderer::Determine_MainAdapter()
	{
		IDXGIAdapter4* adapter{ nullptr };
		// 가장 좋은 성능 순으로 어댑터들을 열거하고, 우리가 설정한 최소 feature level을 충족하는 첫 번째 어댑터를 리턴함.
		for (uint32 i{ 0 };
			_dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			// 어댑터를 가지고 최소 feature level을 만족하는 device를 생성할 수 있는지 시도, 만들 수 있으면 해당 어댑터를 리턴함.
			if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr)))
			{
				return adapter;
			}
			Release(adapter);
		}
		return nullptr;
	}

	// 인자로 주어진 adapter가 지원하는 최대 feature level을 리턴하는 함수.
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

		// device를 생성해보고, CheckFeatureSupport() 함수를 통해 지원하는 최대 feature level을 얻어옴.
		ComPtr<ID3D12Device> device;
		DXCHECK(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)));
		DXCHECK(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_level_info, sizeof(feature_level_info)));
		return feature_level_info.MaxSupportedFeatureLevel;
	}

	void D3D12_Renderer::Process_Deferred_Release(uint32 frame_idx)
	{
		std::lock_guard lock{ _deferred_release_mutex };

		// 본격 적인 자원 해제 작업에 앞서 frame_idx 인덱스에 해당하는 자원 해제 표시 flag 값을 0으로 설정해줌.
		_deferred_release_flag[frame_idx] = 0;

		// heap에 있는 descriptor 해제.
		_rtv_desc_heap.Process_Deferred_Free(frame_idx);
		_dsv_desc_heap.Process_Deferred_Free(frame_idx);
		_srv_desc_heap.Process_Deferred_Free(frame_idx);
		_uav_desc_heap.Process_Deferred_Free(frame_idx);

		// heap에 할당한 descriptor가 아닌 다른 자원들을 해제함. (heap 등)
		// 현재 프레임 인덱스에 해당하는 삭제 대기 자원들에 대한 포인터를 가진 벡터를 가져옴.
		std::vector<IUnknown*>& resources{ _deferred_release[frame_idx] };
		if (!resources.empty())
		{
			for (auto& resource : resources) Release(resource);
			resources.clear();
		}
	}
}