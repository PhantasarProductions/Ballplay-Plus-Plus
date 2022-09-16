// Lic:
// BallPlay++
// Puzzle Selector (header)
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
// Version: 22.09.16
// EndLic
#pragma once
// C++
#include <string>
#include <memory>
// Tricky's Units
#include <GINIE.hpp>
// BallPlay
#include <Chain.hpp>
#include <Resource.hpp>
namespace BallPlay {

	enum class Mission { Unknown, Normal, BreakAway, BreakFree, DotCollector, SaveTheGirl };

	class _PuzPack;
	class _Puzzle;
	typedef std::shared_ptr<_PuzPack> PuzPack;
	typedef std::shared_ptr<_Puzzle> Puzzle;

	class _PuzPack {
	private:
		static std::map<std::string, PuzPack> Pack;
		static std::string _Selected;
		TrickyUnits::GINIE Meta;
	public:
		_PuzPack(std::string pack);
		static void SetPack(std::string pack);
		static PuzPack GetPack(std::string pack, bool autoset = false);
		static PuzPack GetPack();
		static std::string Selected();
		int MaxPuzzles();
		std::string Tag(int num);
		std::string Name(int num);
		int Next(int n);
		int Solved(int num); // Solved by the current user that is
		int BestMoves(int num);
		std::string BestTime(int num);
	};

	class _Puzzle{
	private:
		std::string
			_Pack{ "" },
			_Tag{ "" };
		int
			num{ 0 };
		Mission _Mission{ Mission::Unknown };

	public:
		PuzPack Pack();
		static Puzzle Load(std::string Pck, int PuzNum);		
	};

	bool PuzzleSelector();

}