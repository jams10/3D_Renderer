
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

	// ���̾ �߰��ϴ� �Լ�. �տ������� �߰���.
	void Layer_Stack::Push_Layer(Layer* layer) // �տ��� ���� ����.
	{
		_layers.emplace(_layers.begin() + _layer_insert_index, layer);
		_layer_insert_index++;
		layer->On_Attach();
	}

	// Overlay ���̾ �߰��ϴ� �Լ�. Overlay�� ��� �� �ڿ������� �߰���.
	void Layer_Stack::Push_Overlay(Layer* overlay) // �ڿ��� ���� ����.
	{
		_layers.emplace_back(overlay);
		overlay->On_Attach();
	}

	// ���̾ �����ϴ� �Լ�. �տ��� ���� ������.
	void Layer_Stack::Pop_Layer(Layer* layer) // �տ��� ���� ��.
	{
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end())
		{
			_layers.erase(it);
			_layer_insert_index--;
			layer->On_Detach();
		}
	}

	// Overlay ���̾ �����ϴ� �Լ�. Overlay�� ��� �� �ڿ������� ���ŵ�.
	void Layer_Stack::Pop_Overlay(Layer* overlay) // �ڿ��� ���� ��.
	{
		auto it = std::find(_layers.begin(), _layers.end(), overlay);
		if (it != _layers.end())
		{
			_layers.erase(it);
			overlay->On_Detach();
		}
	}
}

