#pragma once

#include "Graphics/Renderer.h"
#include "D3D12Headers.h"
#include "D3D12Command.h"
#include "D3D12Resources.h"

namespace Engine::Graphics::D3D12
{
	using Microsoft::WRL::ComPtr;

	class D3D12_Renderer : public Renderer
	{
	public:
		D3D12_Renderer();
		~D3D12_Renderer();

		DISABLE_COPY_AND_MOVE(D3D12_Renderer)

		virtual bool Initialize() override;
		virtual void Shutdown() override;
		virtual void Render() override;

		static D3D12_Renderer* Get_Instance() { return _instance; }
		static ID3D12Device8* Get_Device() { return _instance->_main_device; }
		static uint32 Get_Current_Frame_Index() { return _instance->_gfx_command->Frame_Index(); }

		void Set_Deferred_Release_Flag();
		void Deferred_Release(IUnknown* resource);
	
	private:
		bool Failed_Initialize();
		IDXGIAdapter4* Determine_MainAdapter();
		D3D_FEATURE_LEVEL Get_Maximum_Feature_Level(IDXGIAdapter4* adapter);

		void Process_Deferred_Release(uint32 frame_idx);

	private:
		static D3D12_Renderer* _instance;
		ID3D12Device8* _main_device{ nullptr };
		IDXGIFactory7* _dxgi_factory{ nullptr };
		std::unique_ptr<D3D12_Command> _gfx_command{ nullptr };

		D3D_FEATURE_LEVEL _minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 }; // �������� ������ ���� ���� feature level.

		Descriptor_Heap _rtv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV };           // RTV�� ���� heap.
		Descriptor_Heap _dsv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV };           // DSV�� ���� heap.
		Descriptor_Heap _srv_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };   // SRV�� ���� heap.
		Descriptor_Heap _uav_desc_heap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV };   // UAV�� ���� heap.

		uint32					_deferred_release_flag[frame_buffer_count]{};       // Ư�� �����ӿ� ������ �ڿ��� �ִ��� ǥ���� flag ������ �迭.
		std::vector<IUnknown*>  _deferred_release[frame_buffer_count]{};            // Ư�� �����ӿ� ������ COM ��ü�� ������ vector �迭.
		std::mutex              _deferred_release_mutex;                            // �ڿ� ���� �� race condition ������ ���� mutex.
	};
}