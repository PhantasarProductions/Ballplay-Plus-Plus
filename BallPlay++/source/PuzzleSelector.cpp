// Lic:
// BallPlay++
// Puzzle Selector
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
// Version: 22.09.14
// EndLic
// C++
#include <map>

// Tricky's Uints
#include <TrickySTOI.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>

// BallPlay
#include <MainMenu.hpp>
#include <Sinus.hpp>
#include <PuzzleSelector.hpp>
#include <Fonts.hpp>
#include <Users.hpp>

using namespace std;
using namespace TrickyUnits;

 

namespace BallPlay {

	std::map<std::string, PuzPack> _PuzPack::Pack{};
	std::string _PuzPack::_Selected{ "" };

	
	void _PuzPack::SetPack(string pack) {
		_Selected = pack;
		if (!Pack.count(pack)) Pack[pack] = make_shared<_PuzPack>(pack);
	}

	PuzPack _PuzPack::GetPack(std::string pack, bool autoset) {
		if (autoset) SetPack(pack);		
		return Pack[pack];
	}

	PuzPack _PuzPack::GetPack() {
		return GetPack(_Selected);
	}

	int _PuzPack::MaxPuzzles() {
		return ToInt(Meta.Value("Puzzles", "Max"));
	}

	std::string _PuzPack::Tag(int num) {
		char ret[20]{0};
		if (MaxPuzzles() < 10)
			sprintf_s(ret, "Puz%d", num);
		else if (MaxPuzzles() < 100)
			sprintf_s(ret, "Puz%02d", num);
		else if (MaxPuzzles() < 1000)
			sprintf_s(ret, "Puz%03d", num);
		else
			Crash("Cannot create TAG for packs with more than 1000 puzzles in this this version of the game");
		return ret;
	}

	std::string _PuzPack::Name(int num) {
		//cout << "Puzzle name from " << Tag(num) << endl;
		return Meta.Value("Puzzles",Tag(num));
	}

	int _PuzPack::Solved(int num) {
		return _User::Get()->Solved(_Selected, Tag(num));
	}
	

	_PuzPack::_PuzPack(std::string pack) {
		cout << "Loading pack data: " << pack << endl;
		Meta.Parse(Resource()->String("Packages/" + pack + "/Meta.ini"));
	}

	bool PuzzleSelector() {
		int
			mdx{ TQSG_ScreenWidth() / 2 },
			MX{ TQSE_MouseX() },
			MY{ TQSE_MouseY() },
			startcollumn{ 0 }; // NOT expected to be needed, but if larger packs do appear I got this as a backup
		const int cols{ 2 };
		static auto KzF{ GetFont("conthrax-sb") };
		static auto Vink{ TQSG_LoadAutoImage("GFX/PuzzleSelector/Vink.png") };
		bool ML{ TQSE_MouseHit(1) };
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		SinusColor(0, 180, 255);
		Sinus();
		Logo()->Draw(mdx, 50);	
		auto pck{ _PuzPack::GetPack() };
		auto use{ _User::Get() };
		//cout << pck->MaxPuzzles() << endl; // debug only
		for (int i = 0; i < pck->MaxPuzzles(); ++i) {
			int
				ti{ i + 1 },
				puz{ ti + (startcollumn * 25) },
				col{ i / 25 },
				row{ i % 25 },
				W{ TQSG_ScreenWidth() / cols },
				X{( W * col)+30 },
				Y{ (row * 30)+150 };
			TQSG_Color(255, 255, 255);
			if (pck->Solved(ti)) Vink->Draw(X - 30, Y);
			//printf("(%04d,%04d) - #%02d - %s: %s\n", X, Y, ti, pck->Tag(ti).c_str(), pck->Name(ti).c_str());
			//KzF->Draw(pck->Tag(ti), X, Y); // Debug only!
			TQSG_Color(255, 255, 255);
			if (MX > X - 30 && MY > Y && MX < X + W && MY < Y + 30) {
				TQSG_Color(255, 180, 0);
			}
			KzF->Draw(pck->Name(ti), X, Y); // When not using tags
		}
		Flip();
		return true;
	}
}