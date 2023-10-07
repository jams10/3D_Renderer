// ***********************************************************
// Referenced from Frank Luna's DirectX12 Book. 
// https://github.com/d3dcoder/d3d12book/tree/master
// ***********************************************************

#include "Platform/Win32/WindowsHeaders.h"
#include "GameTimer.h"

namespace Engine
{
	Game_Timer::Game_Timer()
		: _seconds_per_count(0.0), _delta_time(-1.0), _base_time(0),
		_paused_time(0), _prev_time(0), _cur_time(0), _stopped(false)
	{
		// Tick 당 초 수를 계산함.
		int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		_seconds_per_count = 1.0 / (double)countsPerSec;
	}

	// 애플리케이션의 시작 부터 현재 까지의 경과 시간을 리턴함.
	// 중간에 중단한 시간은 제외함.
	float Game_Timer::Total_Time()const
	{
		// 타이머가 멈춘 경우, 멈춘 이후로의 시간은 제외함. 또한, 타이머가 멈추기 전에 멈춰 있었던 시간도 제외해줌.
		if (_stopped)
		{
			return (float)(((_stop_time - _paused_time) - _base_time) * _seconds_per_count);
		}
		// 타이머가 멈추지 않은 경우, 현재 시간에서 타이머가 멈춰 있었던 시간을 제외한 시간을 리턴함.
		else
		{
			return (float)(((_cur_time - _paused_time) - _base_time) * _seconds_per_count);
		}
	}

	// 프레임 경과 시간을 리턴하는 함수.
	float Game_Timer::Delta_Time()const
	{
		return (float)_delta_time;
	}

	// 타이머를 초기화 하는 함수.
	void Game_Timer::Reset()
	{
		// Reset() 호출 당시의 시간으로 기준 시간을 초기화.
		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_base_time = currTime;
		_prev_time = currTime;
		_stop_time = 0;
		_stopped = false;
	}

	// 타이머를 재개하는 함수.
	void Game_Timer::Start()
	{
		int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		// 타이머가 멈춰 있었으면, 멈춰있던 시간을 누적 시켜 줌.
		if (_stopped)
		{
			_paused_time += (startTime - _stop_time);

			_prev_time = startTime;
			_stop_time = 0;
			_stopped = false;
		}
	}

	// 타이머를 중지하는 함수.
	void Game_Timer::Stop()
	{
		// 타이머를 재개할 때 멈춰 있던 시간을 구하기 위해 멈춘 당시의 시간을 StopTime에 저장해둠.
		if (!_stopped)
		{
			int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			_stop_time = currTime;
			_stopped = true;
		}
	}

	// 프레임 경과 시간을 측정하기 위해 매 프레임마다 호출하는 함수.
	void Game_Timer::Tick()
	{
		// 타이머가 멈춰 있는 경우 프레임 경과 시간은 0.
		if (_stopped)
		{
			_delta_time = 0.0;
			return;
		}

		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		_cur_time = currTime;

		// 이전 프레임과 현재 프레임 사이의 Tick 수를 계산하고, Tick당 초를 곱해 프레임 경과 시간을 계산함.
		_delta_time = (_cur_time - _prev_time) * _seconds_per_count;

		_prev_time = _cur_time;

		// DXSDK 문서의 CDXUTTimer 항목에서 프로세서가 절전 모드로 들어가거나 실행이 다른 프로세서와 엉키는 경우, deltaTime이 음수가 될 수 있다고 함.
		if (_delta_time < 0.0)
		{
			_delta_time = 0.0;
		}
	}
}
