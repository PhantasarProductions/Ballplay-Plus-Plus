// Lic:
// BallPlay++
// Game Itself (header)
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
// Version: 22.09.15
// EndLic
#pragma once

// Tricky's Units
#include <TQSG.hpp>

// BallPlay
#include <Chain.hpp>
#include <PuzzleSelector.hpp>



namespace BallPlay {
	typedef int(*SuitGetNum)(Puzzle Pz);
	typedef bool(*SuitAllow)(Puzzle Pz);
	class SClass {
	private:
		static SClass Suits[4];
		SuitGetNum GetNum;
		SuitAllow Allow;
		TrickyUnits::TQSG_AutoImage Img{ nullptr };
		std::string Picture;
	public:
		SClass(std::string Pic, SuitGetNum _GetNum,SuitAllow _Allow);
	};
}