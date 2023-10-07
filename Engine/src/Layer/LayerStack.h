#pragma once

#include "Layer.h"

namespace Engine
{
	/*
	* 레이어들을 관리하기 위한 클래스. 이름은 Stack이지만, 레이어 중간 삽입, 삭제를 위해 내부적으로 vector로 레이어들을 참조함.
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
		std::vector<Layer*> _layers;                   // 레이어들을 가지는 벡터.
		unsigned int _layer_insert_index = 0;          // Layer를 삽입할 위치를 가리키는 index.
	};
	// 레이어 목록을 앞에서부터 순차적으로 훑어 가면서 렌더링이나 업데이트 작업을 수행하고, 끝에서부터 반대로 훑어가면서 이벤트를 처리함.
	// 최상위 레이어에 버튼이 있다면 해당 버튼에 대한 이벤트를 먼저 처리해야 하기 때문.
}