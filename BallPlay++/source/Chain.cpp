#include "Chain.hpp"
#include <TQSE.hpp>
namespace BallPlay {

	ChainState _State{ nullptr };
	void Crash(std::string err) {
		SDL_ShowSimpleMessageBox(0, "FATAL ERROR", err.c_str(), NULL);
		exit(1);
	}

	void Assert(bool condition, std::string err) { if (!condition) Crash(err); }
		
	void SetChain(ChainState State) {
		_State = State;
	}

	void ChainRun() {
		do {
			Assert(_State, "No state given!");
		} while (_State());
	}

}