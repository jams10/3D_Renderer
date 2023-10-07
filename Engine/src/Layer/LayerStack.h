#pragma once

#include "Layer.h"

namespace Engine
{
	/*
	* ���̾���� �����ϱ� ���� Ŭ����. �̸��� Stack������, ���̾� �߰� ����, ������ ���� ���������� vector�� ���̾���� ������.
	*/
	class Layer_Stack
	{
	public:
		Layer_Stack();
		~Layer_Stack();

		void Push_Layer(Layer* layer);
		void Push_Overlay(Layer* overlay);
		void Pop_Layer(Layer* layer);
		void Pop_Overlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return _layers.begin(); }
		std::vector<Layer*>::iterator end() { return _layers.end(); }
	private:
		std::vector<Layer*> _layers;                   // ���̾���� ������ ����.
		unsigned int _layer_insert_index = 0;          // Layer�� ������ ��ġ�� ����Ű�� index.
	};
	// ���̾� ����� �տ������� ���������� �Ⱦ� ���鼭 �������̳� ������Ʈ �۾��� �����ϰ�, ���������� �ݴ�� �Ⱦ�鼭 �̺�Ʈ�� ó����.
	// �ֻ��� ���̾ ��ư�� �ִٸ� �ش� ��ư�� ���� �̺�Ʈ�� ���� ó���ؾ� �ϱ� ����.
}