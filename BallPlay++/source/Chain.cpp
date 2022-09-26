// Lic:
// BallPlay++
// Chain
// 
// 
// 
// (c) Jeroen P. Broks, 2022
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 22.09.10
// EndLic

#include "Chain.hpp"
#include <TQSE.hpp>

using namespace TrickyUnits;

namespace BallPlay {

	ChainState _State{ nullptr };
	void Crash(std::string err) {
		SDL_ShowSimpleMessageBox(0, "FATAL ERROR", err.c_str(), NULL);
		exit(1);
	}

	void Assert(bool condition, std::string err) { if (!condition) Crash(err); }
		
	void SetChain(ChainState State) {
		_State = State;
		TQSE_Flush();
	}

	void ChainRun() {
		do {
			Assert(_State, "No state given!");
		} while (_State());
	}

	bool CheckQuit() {
		using namespace TrickyUnits;
		if (TQSE_Quit()) {
			return TQSE_Yes("Do you really want to quit immediately?");
		}
		return false;
	}

}