#pragma once
#include <string>

typedef bool(*ChainState)();

namespace BallPlay {
	void Crash(std::string err);
	void Assert(bool condition, std::string err);
	void SetChain(ChainState State);
	void ChainRun();

}