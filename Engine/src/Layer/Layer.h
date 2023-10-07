#pragma once

#include "Common/CommonHeaders.h"
#include "Event/Event.h"

namespace Engine
{
	/*
	* 애플리케이션을 포토샵의 레이어 처럼 계층 구조로 관리하기 위해 사용하는 클래스.
	*/
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void On_Attach() {}				// 레이어를 추가할 때 처리할 작업.
		virtual void On_Detach() {}				// 레이어를 제거할 때 처리할 작업.
		virtual void On_Update(float dt) {}		// 레이어가 업데이트 되어야 할 때 애플리케이션 클래스에 의해 호출.
		virtual void On_Render() {}              // 레이어에 대한 렌더링 작업을 수행해야 할 때 호출.
		virtual void On_Event(Event& event) {}	// 레이어로 이벤트가 들어오면 처리.

		inline const std::string& Get_Name() const { return _debugName; }
	protected:
		std::string _debugName; // 레이어의 이름
	};
}