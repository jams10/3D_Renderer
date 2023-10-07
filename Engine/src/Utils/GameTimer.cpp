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
		// Tick �� �� ���� �����.
		int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		_seconds_per_count = 1.0 / (double)countsPerSec;
	}

	// ���ø����̼��� ���� ���� ���� ������ ��� �ð��� ������.
	// �߰��� �ߴ��� �ð��� ������.
	float Game_Timer::Total_Time()const
	{
		// Ÿ�̸Ӱ� ���� ���, ���� ���ķ��� �ð��� ������. ����, Ÿ�̸Ӱ� ���߱� ���� ���� �־��� �ð��� ��������.
		if (_stopped)
		{
			return (float)(((_stop_time - _paused_time) - _base_time) * _seconds_per_count);
		}
		// Ÿ�̸Ӱ� ������ ���� ���, ���� �ð����� Ÿ�̸Ӱ� ���� �־��� �ð��� ������ �ð��� ������.
		else
		{
			return (float)(((_cur_time - _paused_time) - _base_time) * _seconds_per_count);
		}
	}

	// ������ ��� �ð��� �����ϴ� �Լ�.
	float Game_Timer::Delta_Time()const
	{
		return (float)_delta_time;
	}

	// Ÿ�̸Ӹ� �ʱ�ȭ �ϴ� �Լ�.
	void Game_Timer::Reset()
	{
		// Reset() ȣ�� ����� �ð����� ���� �ð��� �ʱ�ȭ.
		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_base_time = currTime;
		_prev_time = currTime;
		_stop_time = 0;
		_stopped = false;
	}

	// Ÿ�̸Ӹ� �簳�ϴ� �Լ�.
	void Game_Timer::Start()
	{
		int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		// Ÿ�̸Ӱ� ���� �־�����, �����ִ� �ð��� ���� ���� ��.
		if (_stopped)
		{
			_paused_time += (startTime - _stop_time);

			_prev_time = startTime;
			_stop_time = 0;
			_stopped = false;
		}
	}

	// Ÿ�̸Ӹ� �����ϴ� �Լ�.
	void Game_Timer::Stop()
	{
		// Ÿ�̸Ӹ� �簳�� �� ���� �ִ� �ð��� ���ϱ� ���� ���� ����� �ð��� StopTime�� �����ص�.
		if (!_stopped)
		{
			int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			_stop_time = currTime;
			_stopped = true;
		}
	}

	// ������ ��� �ð��� �����ϱ� ���� �� �����Ӹ��� ȣ���ϴ� �Լ�.
	void Game_Timer::Tick()
	{
		// Ÿ�̸Ӱ� ���� �ִ� ��� ������ ��� �ð��� 0.
		if (_stopped)
		{
			_delta_time = 0.0;
			return;
		}

		int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		_cur_time = currTime;

		// ���� �����Ӱ� ���� ������ ������ Tick ���� ����ϰ�, Tick�� �ʸ� ���� ������ ��� �ð��� �����.
		_delta_time = (_cur_time - _prev_time) * _seconds_per_count;

		_prev_time = _cur_time;

		// DXSDK ������ CDXUTTimer �׸񿡼� ���μ����� ���� ���� ���ų� ������ �ٸ� ���μ����� ��Ű�� ���, deltaTime�� ������ �� �� �ִٰ� ��.
		if (_delta_time < 0.0)
		{
			_delta_time = 0.0;
		}
	}
}
