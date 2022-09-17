// Lic:
// BallPlay++
// Game Itself
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
// Version: 22.09.17
// EndLic
#include <string>

#include <TQSE.hpp>

#include <Sinus.hpp>
#include <Game.hpp>
#include <Fonts.hpp>



using namespace std;
using namespace TrickyUnits;
namespace BallPlay {


	static const char LayOrder[5][25]{
		"FLOOR",
		"DIRECTIONS",
		"BOMBS",
		"WALL",
		"BREAK"
	};
	static TQSG_AutoImageFont Fnt{ nullptr };
	Puzzle PlayPuzzle{ nullptr };

	static bool __Always(Puzzle P) { return true; }
	static bool __ShowHearts(Puzzle P) { return PlayPuzzle->EMission() == Mission::Normal || PlayPuzzle->EMission()==Mission::ColorSplit || PlayPuzzle->EMission() == Mission::BreakFree; }
	static int __Hearts(Puzzle P) { return 0; }
	static int __Clubs(Puzzle P) { return P->Required(); }
	static int __Diamonds(Puzzle P) { return 0; }
	static int __Spades(Puzzle P) { return 0; }

	SClass::SClass(std::string Pic, SuitGetNum _GetNum, SuitAllow _Allow) {
		Picture = Pic;
		GetNum = _GetNum;
		Allow = _Allow;
		cout << "Initized Suit Class: " << Pic << endl;
	}

	void SClass::_Show(int X) {
		if (!Allow(PlayPuzzle)) return;
		if (!Img) {
			cout << "Loading suit: " << Picture << endl;
			Img = TQSG_LoadAutoImage(Resource(), "GFX/Game/Suit/" + Picture + ".png");
			Img->HotBottomCenter();
		}
		//TQSG_Color(25, 25, 25);
		//Img->Draw(X+2, TQSG_ScreenHeight() - 2);
		TQSG_Color(255, 255, 255);
		Img->Draw(X, TQSG_ScreenHeight() - 10);
		if (!Fnt) Fnt = GetFont("conthrax-sb");
		TQSG_Color(10, 10, 10);
		Fnt->Draw(to_string(GetNum(PlayPuzzle)),X, TQSG_ScreenHeight() - 10,1,1);
		TQSG_Color(255,255,255);
		Fnt->Draw(to_string(GetNum(PlayPuzzle)), X-2, TQSG_ScreenHeight() - 12,1,1);

	}

	void SClass::ShowSuits() {
		int X{ TQSG_ScreenWidth() / 2 };
		for (int i = 0; i < 4; i++) {
			Suits[i]._Show(X);
			X += 80;
		}
	}

	SClass SClass::Suits[4]{
		SClass("Hearts",__Hearts,__ShowHearts),
		SClass("Clubs",__Clubs,__Always),
		SClass("Diamonds",__Diamonds,__Always),
		SClass("Spades",__Diamonds,__Always)
	};

	bool Game() {
		TQSE_Poll();
		static int
			sw{ TQSG_ScreenWidth() },
			sh{ TQSG_ScreenHeight() },
			mdx{ TQSG_ScreenWidth() / 2 };
		int
			MX{ TQSE_MouseX() },
			MY{ TQSE_MouseY() };
		bool
			ML{ TQSE_MouseHit(1) };
		TQSG_Cls();
		Sinus();
		DoCheckQuit();
		PlayPuzzle->DBack();
		for (auto dlay : LayOrder) PlayPuzzle->DrawLayer(dlay);
		SClass::ShowSuits();
		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->Title(),4,4);
		TQSG_Color(255, 255, 255);
		Fnt->Draw(PlayPuzzle->Title(), 2, 2);
		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth()-2, 4,1,0);
		TQSG_Color(180,255,0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth() - 4, 2, 1, 0);

		Flip();
		return true;
	}

}