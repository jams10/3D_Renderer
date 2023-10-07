// ***********************************************************
// Referenced from Frank Luna's DirectX12 Book. 
// https://github.com/d3dcoder/d3d12book/tree/master
// ***********************************************************

#pragma once

#include "Common/CommonHeaders.h"

namespace Engine
{
	/*
	* 애플리케이션에서 사용할 타이머 클래스.
	* 현재 윈도우에서 제공하는 성능 타이머를 통해 측정하고 있으므로, 다른 플랫폼 사용 시 작동하지 않을 수 있음.
	*/
	class Game_Timer
	{
	public:
		Game_Timer();

		float Total_Time()const; // 전체 경과 시간(초 단위).
		float Delta_Time()const; // 프레임 경과 시간(초 단위).

		void Reset(); // App 클래스의 loop 실행 전에 호출하여 타이머를 초기화. 
		void Start(); // 애플리케이션 실행 재개 시 호출.
		void Stop();  // 애플리케이션 정지 시 호출.
		void Tick();  // 매 프레임마다 호출.

	private:
		double _seconds_per_count; // 한 Tick count당 초 수(한 Tick 동안이 몇 초인가). 
		double _delta_time;        // 프레임 경과 시간.

		int64 _base_time;   // 애플리케이션 시작 시간.
		int64 _paused_time; // 애플리케이션이 정지된 동안의 시간.
		int64 _stop_time;   // 애플리케이션이 정지된 시간.
		int64 _prev_time;   // 이전 프레임 시간.
		int64 _cur_time;    // 현재 프레임 시간.

		bool _stopped;      // 타이머가 정지되었는지 여부.
	};
}