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
#include <BaseButtons.hpp>

#include <Dirry.hpp>
#include <FileList.hpp>
#include <QuickStream.hpp>
#include <QuickString.hpp>
#include <TQSE.hpp>
#include <TQSG.hpp>

using namespace TrickyUnits;

namespace BallPlay {

	std::string _User::UserDir{ Dirry("$AppSupport$/BallPlay++/Users") };
	std::string _User::_CurrentUserName{ "" };
	User _User::_CurrentUser{ nullptr };
	

	std::string _User::NameToFile(string name) { return UserDir + "/" + name + ".ini"; }
	bool _User::Exists(std::string username) { return FileExists(NameToFile(username)); }

	void _User::Set(std::string username) {
		_CurrentUser = make_shared<_User>(username);
		_CurrentUserName = username;
	}
	User _User::Get() { return _CurrentUser; }

	_User::_User(std::string UserName) {
		std::string fname{ NameToFile(UserName) };
		MakeDir(UserDir);
		_UserName = UserName;
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
		static TQSG_AutoImage BackSpace{ TQSG_LoadAutoImage(Resource(),"GFX/User/BackSpace.png") }; Assert(BackSpace->W(), "BackSpace not properly loaded!\n\n" + JCR_Error);
		static auto Fnt{ GetFont("Spaced") };
		int
			mdx = TQSG_ScreenWidth() / 2,
			mx{ TQSE_MouseX() },
			my{ TQSE_MouseY() };

		static string EName{ "" };
		TQSG_Cls();
		TQSE_Poll();
		DoCheckQuit();
		SinusColor(255, 0, 0);
		Sinus();
		Logo()->Draw(mdx, 50);
		EnterName->Draw(mdx, 150);
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
				if (mx > dx && my > dy && mx < dx + Fnt->W(st) && my < dy + Fnt->H(st)) {
					TQSG_Color(255, 180, 0);
					if (EName.size() < 10 && TQSE_MouseHit(1)) EName += c;
				}
				Fnt->Draw(st, dx, dy, 2);
			}
		}
		static int topy = TQSG_ScreenHeight() - 90;
		if (EName.size()) {
			TQSG_Color(255, 255, 255);
			
			//printf("Backspace(0,%d)  %dx%d\n", y,BackSpace->W(),BackSpace->H());
			if (my > topy && mx < BackSpace->W()) {
				TQSG_Color(255, 0, 0);
				if (TQSE_MouseHit(1)) EName = TrickyUnits::left(EName, EName.size() - 1);
			}
			BackSpace->Draw(0, topy);
			TQSG_Color(255, 255, 255);
			static int cx = (TQSG_ScreenWidth() / 2) - (ImgOk()->W() / 2);
			if (my > topy && mx > cx && mx < cx + ImgOk()->W()) {
				TQSG_Color(0, 255, 0);
				if(TQSE_MouseHit(1)) {
					if (_User::Exists(EName))
						SDL_ShowSimpleMessageBox(0, "Sorry!", "That username already exists!\nPick another please!",NULL);
					else {
						cout << "Creating user: " << EName << "\n";
						// actual creation comes later!
					}
				}
			}
				ImgOk()->Draw(cx, topy);
		}
		//ShowMouse();
		Flip();
		return true;
	}

}