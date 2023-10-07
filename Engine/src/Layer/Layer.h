#pragma once

#include "Common/CommonHeaders.h"
#include "Event/Event.h"

namespace Engine
{
	/*
	* ���ø����̼��� ���伥�� ���̾� ó�� ���� ������ �����ϱ� ���� ����ϴ� Ŭ����.
	*/
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void On_Attach() {}				// ���̾ �߰��� �� ó���� �۾�.
		virtual void On_Detach() {}				// ���̾ ������ �� ó���� �۾�.
		virtual void On_Update(float dt) {}		// ���̾ ������Ʈ �Ǿ�� �� �� ���ø����̼� Ŭ������ ���� ȣ��.
		virtual void On_Render() {}              // ���̾ ���� ������ �۾��� �����ؾ� �� �� ȣ��.
		virtual void On_Event(Event& event) {}	// ���̾�� �̺�Ʈ�� ������ ó��.

		inline const std::string& Get_Name() const { return _debugName; }
	protected:
		std::string _debugName; // ���̾��� �̸�
	};
}