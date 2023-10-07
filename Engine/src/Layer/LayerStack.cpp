
#include "LayerStack.h"

namespace Engine
{
	Layer_Stack::Layer_Stack()
	{
	}

	Layer_Stack::~Layer_Stack()
	{
		for (Layer* layer : _layers)
		{
			layer->On_Detach();
			delete layer;
		}
	}

	// 레이어를 추가하는 함수. 앞에서부터 추가됨.
	void Layer_Stack::Push_Layer(Layer* layer) // 앞에서 부터 넣음.
	{
		_layers.emplace(_layers.begin() + _layer_insert_index, layer);
		_layer_insert_index++;
		layer->On_Attach();
	}

	// Overlay 레이어를 추가하는 함수. Overlay의 경우 맨 뒤에서부터 추가됨.
	void Layer_Stack::Push_Overlay(Layer* overlay) // 뒤에서 부터 넣음.
	{
		_layers.emplace_back(overlay);
		overlay->On_Attach();
	}

	// 레이어를 제거하는 함수. 앞에서 부터 제거함.
	void Layer_Stack::Pop_Layer(Layer* layer) // 앞에서 부터 뺌.
	{
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end())
		{
			_layers.erase(it);
			_layer_insert_index--;
			layer->On_Detach();
		}
	}

	// Overlay 레이어를 제거하는 함수. Overlay의 경우 맨 뒤에서부터 제거됨.
	void Layer_Stack::Pop_Overlay(Layer* overlay) // 뒤에서 부터 뺌.
	{
		auto it = std::find(_layers.begin(), _layers.end(), overlay);
		if (it != _layers.end())
		{
			_layers.erase(it);
			overlay->On_Detach();
		}
	}
}

