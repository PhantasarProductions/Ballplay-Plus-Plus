// Lic:
// BallPlay++
// User Management
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
#include <Users.hpp>
#include <Config.hpp>
#include <Chain.hpp>
#include <Sinus.hpp>
#include <MainMenu.hpp>
#include <Resource.hpp>
#include <Fonts.hpp>
#include <Mouse.hpp>

#include <Dirry.hpp>
#include <FileList.hpp>
#include <QuickStream.hpp>
#include <TQSE.hpp>
#include <TQSG.hpp>

using namespace TrickyUnits;

namespace BallPlay {

	std::string _User::UserDir{ Dirry("$AppSupport$/BallPlay++/Users") };
	std::string _User::CurrentUser{ "" };

	_User::_User(std::string UserName) {
		std::string fname{ UserDir + "/" + UserName + ".ini" };
		MakeDir(UserDir);
		Data.FromFile(fname,true);
		Data.AutoSave = fname;
	}

	void CheckUserStartUp(){
		auto chku{ Config("Users","Last") };
		if (chku.size() && FileExists(_User::UserDir + "/" + chku + ".ini")) {
			Crash("No chain to main menu possible yet!"); 
			return;
		}
		if (FileList(_User::UserDir).size()) {
			Crash("No chain yet possible to user list");
			return;
		}
		SetChain(NewUser);
		return;
	}

	bool NewUser() {
		static TQSG_AutoImage EnterName{ TQSG_LoadAutoImage(Resource(),"GFX/User/Enter your name.png") }; EnterName->HotCenter();
		static auto Fnt{ GetFont("Spaced") };
		int mx = TQSG_ScreenWidth() / 2;
		static string EName{ "" };
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		SinusColor(255, 0, 0);
		Sinus();
		Logo()->Draw(mx, 50);
		EnterName->Draw(mx, 150);
		TQSG_Color(0, 180, 255); Fnt->Draw(EName, TQSG_ScreenWidth() / 2, 200, 2);
		for (auto c = '0'; c <= 'Z'; c++) {
			int tel = (int)c - (int)'0';
			int y = tel / 10;
			int x = tel % 10;
			char st[3] = { c,0 };			
			if (c <= '9' || c >= 'A') {
				int dx = (TQSG_ScreenWidth() / 12) * (x + 1);
				int dy = 300 + (y * 70);
				//printf("char %c/%d (%d,%d) tel(%d)\n", c, c, dx, dy,tel); // debug only!!!
				TQSG_Color(255, 255, 255);
				int
					mx{ TQSE_MouseX() },
					my{ TQSE_MouseY() };
				if (mx > dx && my > dy && mx < dx + Fnt->W(st) && my < dy + Fnt->H(st)) {
					TQSG_Color(255, 180, 0);
					if (EName.size() < 20 && TQSE_MouseHit(1)) EName += c;
				}
				Fnt->Draw(st, dx, dy, 2);
			}
		}
		//ShowMouse();
		Flip();
		return true;
	}

}