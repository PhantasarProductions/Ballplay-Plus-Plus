// Lic:
// BallPlay++
// Pack Selector
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
#include <vector>
#include <string>
#include <iostream>

// TQSG
#include <TQSG.hpp>
#include <TQSE.hpp>

// Tricky's Units
#include <GINIE.hpp>

// BallPlay
#include <Resource.hpp>
#include <MainMenu.hpp>
#include <Sinus.hpp>
#include <Chain.hpp>
#include <Fonts.hpp>
#include <PuzzleSelector.hpp>

using namespace std;
using namespace TrickyUnits;
namespace BallPlay {
	static vector<string> PackageList;
	static TQSG_AutoImageFont KzFnt{nullptr};
	static TQSG_AutoImageFont Mini{nullptr};


	static void PSInit() {
		static bool Done{ false }; if (Done) return;
		auto BT{ Resource()->B("Packages/List") };
		auto aantal{ BT->ReadInt() };
		PackageList.clear();
		for (int i=0; i < aantal; i++) PackageList.push_back(BT->ReadString());
		cout << "Loaded package list. Game contains " << aantal << " packages\n";
		Done = true;
		KzFnt = GetFont("conthrax-sb");
		Mini = GetFont("Mini");
	}

	static void Info(int x, int& y, string txt) {
		Mini->Draw(txt,x, y, 1, 1);
		y -= 20;
	}
	bool PackSelector() {
		int
			mdx{ TQSG_ScreenWidth() / 2 },
			MX{ TQSE_MouseX() },
			MY{ TQSE_MouseY() };
		bool ML{ TQSE_MouseHit(1) };
		PSInit();
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		SinusColor(255, 0, 255);
		Sinus();
		Logo()->Draw(mdx, 50);
		bool Hover{ false };
		for (uint32 i = 0; i < PackageList.size(); ++i) {
			uint32
				X{ 10 },
				Y{ 100 + (i * 30) };
			if (MY > Y && MY < Y + 30) {
				static map<string,GINIE> Meta;
				TQSG_Color(255, 255, 255);
				if (!Meta.count(PackageList[i])) Meta[PackageList[i]].Parse(Resource()->String("Packages/" + PackageList[i] + "/Meta.ini"));
				static int MetX{ TQSG_ScreenWidth() - 10 };
				int MetY{ TQSG_ScreenHeight() - 10 };
				Info(MetX,MetY,Meta[PackageList[i]].Value("Copyright", "License"));
				Info(MetX, MetY, Meta[PackageList[i]].Value("Copyright", "Copyright"));
				Info(MetX, MetY, "Original game was created in " + Meta[PackageList[i]].Value("Meta", "Created")+" with "+ Meta[PackageList[i]].Value("Tech", "Original_Programming_Language"));
				Info(MetX, MetY, "Author: " + Meta[PackageList[i]].Value("Meta", "Author"));
				TQSG_Color(0, 180, 255);
				Info(MetX, MetY, Meta[PackageList[i]].Value("Meta", "Name"));				
				TQSG_Color(255, 180, 0);
				if (ML) {
					_PuzPack::SetPack(PackageList[i]);
					SetChain(PuzzleSelector);
					return true;
				}
				X += 10;
				Hover = true;
			} else {
				TQSG_Color(255, 255, 255);
			}
			KzFnt->Draw(PackageList[i], X, Y);
		}
		if (!Hover) {
			static int SH{ TQSG_ScreenHeight() };
			static int BTMMW{ 20 + BackToMainMenu()->W() };
			TQSG_Color(255, 255, 255);
			if (MX<BTMMW && MY>SH - 100) {
				TQSG_Color(255, 0, 255);
				if (ML) {
					SetChain(MainMenu);
					return true;
				}
			}			
			BackToMainMenu()->Draw(10, SH - 100);
		}
		Flip();
		return true;
	}
}