// ***********************************************************
// Referenced from Frank Luna's DirectX12 Book. 
// https://github.com/d3dcoder/d3d12book/tree/master
// ***********************************************************

#pragma once

#include "Common/CommonHeaders.h"

namespace Engine
{
	/*
	* ���ø����̼ǿ��� ����� Ÿ�̸� Ŭ����.
	* ���� �����쿡�� �����ϴ� ���� Ÿ�̸Ӹ� ���� �����ϰ� �����Ƿ�, �ٸ� �÷��� ��� �� �۵����� ���� �� ����.
	*/
	class Game_Timer
	{
	public:
		Game_Timer();

		float Total_Time()const; // ��ü ��� �ð�(�� ����).
		float Delta_Time()const; // ������ ��� �ð�(�� ����).

		void Reset(); // App Ŭ������ loop ���� ���� ȣ���Ͽ� Ÿ�̸Ӹ� �ʱ�ȭ. 
		void Start(); // ���ø����̼� ���� �簳 �� ȣ��.
		void Stop();  // ���ø����̼� ���� �� ȣ��.
		void Tick();  // �� �����Ӹ��� ȣ��.

	private:
		double _seconds_per_count; // �� Tick count�� �� ��(�� Tick ������ �� ���ΰ�). 
		double _delta_time;        // ������ ��� �ð�.

		int64 _base_time;   // ���ø����̼� ���� �ð�.
		int64 _paused_time; // ���ø����̼��� ������ ������ �ð�.
		int64 _stop_time;   // ���ø����̼��� ������ �ð�.
		int64 _prev_time;   // ���� ������ �ð�.
		int64 _cur_time;    // ���� ������ �ð�.

		bool _stopped;      // Ÿ�̸Ӱ� �����Ǿ����� ����.
	};
}