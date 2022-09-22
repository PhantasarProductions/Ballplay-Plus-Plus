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
// Version: 22.09.20
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
#include <SuperTed_Core.hpp>

namespace BallPlay {

	enum class Mission { Unknown, Normal, BreakAway, BreakFree, ColorSplit, DotCollector, SaveTheGirl };

	class _PuzPack;
	class _Puzzle;
	typedef std::shared_ptr<_PuzPack> PuzPack;
	typedef std::shared_ptr<_Puzzle> Puzzle;

	class _PuzPack {
	private:
		static std::map<std::string, PuzPack> Pack;
		static std::map<int, std::string> Sounds;
		static std::string _Selected;
		TrickyUnits::GINIE Meta;
	public:
		_PuzPack(std::string pack);
		static void SetPack(std::string pack);
		static PuzPack GetPack(std::string pack, bool autoset = false);
		static PuzPack GetPack();
		static std::string Selected();
		int MaxPuzzles();
		int MaxDeathSounds();
		void DeathSound(int i);
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
			num{ 0 },
			PX{ 0 },
			PY{ 0 };

		Mission _Mission{ Mission::Unknown };
		SuperTed::Teddy PuzMap;
		TrickyUnits::TQSG_AutoImage BackImg{ nullptr };

	public:
		PuzPack Pack();
		SuperTed::TeddyRoom PuzR();
		void DrawLayer(std::string Lay);
		int PixW();
		int PixH();
		int W();
		int H();
		int GridW();
		int GridH();
		std::string Title();
		int Required();
		void DBack();
		std::string MissionName();
		std::string PackName();
		Mission EMission();
		int gPX();
		int gPY();
		int iDat(std::string k);
		int BallsIn{ 0 };
		int BallsDestroyed{ 0 };
		int Next();		
		TrickyUnits::uint32 Time{ 0 };
		TrickyUnits::uint32 Moves{ 0 };
		void Solved(bool val = true);
		void BestTime(TrickyUnits::uint32 i);
		void BestMoves(TrickyUnits::uint32 i);

		void Reload();
		static Puzzle Load(std::string Pck, int PuzNum);		
		static std::map<Mission, std::string>MissionNames;
	};

	bool PuzzleSelector();

}