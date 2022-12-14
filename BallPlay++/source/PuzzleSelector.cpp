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
// Version: 22.10.19
// EndLic
// C++
#include <map>

// Tricky's Units
#include <TRandom.hpp>
#include <TrickySTOI.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>

// SuperTed
#include <SuperTed_Core.hpp>

// BallPlay
#include <MainMenu.hpp>
#include <Sinus.hpp>
#include <PuzzleSelector.hpp>
#include <Fonts.hpp>
#include <Users.hpp>
#include <PackSelector.hpp>
#include <Game.hpp>
#include <Audio.hpp>



using namespace std;
using namespace TrickyUnits;

 

namespace BallPlay {

	static TQSG_AutoImage OtherPack;

	std::map<std::string, PuzPack> _PuzPack::Pack{};
	std::string _PuzPack::_Selected{ "" };

	std::map<Mission, std::string> _Puzzle::MissionNames{
		{Mission::Normal,"Normal"},
		{Mission::BreakAway,"Break Away"},
		{Mission::BreakFree,"Break Free"},
		{Mission::ColorSplit,"Color Split"},
		{Mission::DotCollector,"Dot Collector"},
		{Mission::BreakAndCollect,"Break & Collect"},
		{Mission::Walkthrough,"Walkthrough"}
	};
	static string CapOnly(string A) {
		string ret{ "" };
		A = Upper(A);
		for (int i=0; i < A.size(); i++) { if (A[i] >= 'A' && A[i] <= 'Z') ret += A[i]; }
		return ret;
	}
	static Mission MSCMP(std::string A) {
		auto B{ CapOnly(A) };
		for (auto M : _Puzzle::MissionNames) if (CapOnly(M.second) == B) return M.first;
		return Mission::Unknown;
	}
	
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

	std::string _PuzPack::Selected() {
		return _Selected;
	}

	int _PuzPack::MaxPuzzles() {
		return ToInt(Meta.Value("Puzzles", "Max"));
	}

	int _PuzPack::MaxDeathSounds() {
		return ToInt(Meta.Value("Death","Max"));
	}

	void _PuzPack::DeathSound(int i) { if (Sounds.count(i)) SFX(Sounds[i]); }

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

	int _PuzPack::Next(int n) {
		Assert(n > 0 && n <= MaxPuzzles(), "Next start value out of range");
		for (int chk = n % MaxPuzzles(); chk != n - 1; n = (n + 1) % MaxPuzzles())
			if (!Solved(chk + 1)) return chk + 1;
		return (n % MaxPuzzles()) + 1;
	}

	int _PuzPack::Solved(int num) {
		return _User::Get()->Solved(_Selected, Tag(num));
	}

	int _PuzPack::BestMoves(int num) {
		return _User::Get()->BestMoves(_Selected, Tag(num));
	}

	std::string _PuzPack::BestTime(int num) {
		return _User::Get()->BestTimeStr(_Selected, Tag(num));
	}
	

	_PuzPack::_PuzPack(std::string pack) {
		cout << "Loading pack data: " << pack << endl;
		Meta.Parse(Resource()->String("Packages/" + pack + "/Meta.ini"));
		Sounds.clear();
		for (auto i = 0; i < MaxDeathSounds(); i++) {
			char snd[5];
			sprintf_s(snd, "%03d", i);
			cout << "Death Sound: " << snd << ": " << Meta.Value("Death", snd);
			Sounds[i] = Meta.Value("Death", snd);
		}
	}

	bool PuzzleSelector() {
		static int
			sw{ TQSG_ScreenWidth() },
			sh{ TQSG_ScreenHeight() },
			mdx{ TQSG_ScreenWidth() / 2 };
		int
			MX{ TQSE_MouseX() },
			MY{ TQSE_MouseY() },
			startcollumn{ 0 }; // NOT expected to be needed, but if larger packs do appear I got this as a backup
		bool
			hover{ false };
		const int cols{ 2 };
		static auto
			KzF{ GetFont("conthrax-sb") },
			Mini{ GetFont("Mini") };
		static TQSG_AutoImage Vink{ nullptr };
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
			if (pck->Solved(ti)) {
				if (!Vink) {
					cout << "Loading: Vink\n";
					Vink=TQSG_LoadAutoImage(Resource(), "GFX/PuzzleSelector/Vink.png");
				}
				Vink->Draw(X - 30, Y);
				TQSG_Color(200, 255, 200);
				//printf("(%04d,%04d) - #%02d - %s: %s\n", X, Y, ti, pck->Tag(ti).c_str(), pck->Name(ti).c_str());
				//KzF->Draw(pck->Tag(ti), X, Y); // Debug only!
			} else
			TQSG_Color(255, 255, 255);
			if (MX > X - 30 && MY > Y && MX < X + W && MY < Y + 30) {
				hover = true;
				switch (pck->Solved(ti)) {
				case 0:	Mini->Draw("Unsolved", 5, sh - 5, 0, 1); break;
				case 1: Mini->Draw("Solved: Once", 5, sh - 25, 0, 1); break;
				case 2: Mini->Draw("Solved: Twice", 5, sh - 25, 0, 1); break;
				case 3: Mini->Draw("Solved: Thrice", 5, sh - 25, 0, 1); break;
				default: Mini->Draw("Solved: " + to_string(pck->Solved(ti)) + " times", 5, sh - 25, 0, 1); break;
				}
				if (pck->Solved(ti)) {
					Mini->Draw("Least moves: " + to_string(pck->BestMoves(ti)) + "; Best time: " + pck->BestTime(ti), 5, sh - 5, 0, 1);
				}
				TQSG_Color(255, 180, 0);
				if (ML) {
					_Puzzle::Load(_PuzPack::Selected(), ti);
					SetChain(Game);
					return true;
				}
			}
			KzF->Draw(pck->Name(ti), X, Y); // When not using tags
		}
		if (!hover) {
			if (!OtherPack) {
				cout << " Loading graphic: Other Pack\n";
				OtherPack = TQSG_LoadAutoImage(Resource(), "GFX/User/Other Pack.png");
				OtherPack->HotBottomRight();
			}
			TQSG_Color(255, 255, 255);
			if (MX > sw - OtherPack->W() && MY > sh - OtherPack->H()) {
				TQSG_Color(0, 180, 255);
				if (ML) SetChain(PackSelector);
			}
			OtherPack->Draw(sw - 10, sh - 1);
			TQSG_Color(255, 255, 255);
			if (MY > sh - BackToMainMenu()->H() && MX < 10 + BackToMainMenu()->W()) {
				TQSG_Color(0, 180, 255);
				if (ML) SetChain(MainMenu);
			}

			BackToMainMenu()->Draw(10, sh - BackToMainMenu()->H()-1);
		}
		Flip();
		return true;
	}
	PuzPack _Puzzle::Pack() { return _PuzPack::GetPack(_Pack); }

	SuperTed::TeddyRoom _Puzzle::PuzR() { return PuzMap->Rooms["PUZZLE"]; }

	void _Puzzle::DrawLayer(std::string Lay) {
		PuzMap->DrawLayer("PUZZLE", Lay, -PX, -PY);
	}

	int _Puzzle::PixW() { return PuzR()->PixW(); }
	int _Puzzle::PixH() { return PuzR()->PixH(); }

	int _Puzzle::W() { return PuzR()->W(); }
	int _Puzzle::H() { return PuzR()->H(); }

	int _Puzzle::GridW() { return PuzR()->GW(); }
	int _Puzzle::GridH() { return PuzR()->GH(); }

	std::string _Puzzle::Title() { return PuzMap->Data["Title"]; }

	int _Puzzle::Required() {
		return ToInt(PuzMap->Data["Required"]);
	}

	void _Puzzle::DBack() {
		TQSG_SetAlpha(255);
		if (PuzMap->Data["Background"].size()) {
			if (!BackImg) BackImg = TQSG_LoadAutoImage(Resource(), PuzMap->Data["Background"]);
			BackImg->Stretch(PX, PY, PixW(), PixH());
		} else {
			TQSG_Color(0, 0, 0);
			TQSG_Rect(PX, PY, PixW(), PixH());
		}
	}

	std::string _Puzzle::MissionName() { return MissionNames[_Mission]; }

	std::string _Puzzle::PackName() { return _Pack; }

	Mission _Puzzle::EMission() { return _Mission; }

	int _Puzzle::gPX() { return PX; }
	int _Puzzle::gPY() { return PY; }

	int _Puzzle::iDat(std::string k) {
		return ToInt(PuzMap->Data[k]);
	}

	int _Puzzle::Next() {
		return Pack()->Next(num);
	}
	

	static void PZLCrash(std::string err){
		Crash("SuperTed Loading Error!\n\n" + err);

	}

	void _Puzzle::Solved(bool val) {
		auto u{ _User::Get() };
		if (val)
			u->Solved(_Pack, _Tag, u->Solved(_Pack, _Tag)+1);
	}

	void _Puzzle::BestTime(uint32 i) {
		_User::Get()->BestTime(_Pack, _Tag, i);
	}

	void _Puzzle::BestMoves(TrickyUnits::uint32 i) { _User::Get()->BestMoves(_Pack, _Tag, i); }

	void _Puzzle::Reload() { Load(_Pack, num); }

	Puzzle _Puzzle::Load(std::string Pck, int PuzNum) {
		SuperTed::TeddyPanicFunction = PZLCrash;
		cout << "Request to load puzzle #" << PuzNum << " from pack " << Pck << endl;
		auto ret{ make_shared<_Puzzle>() };
		ret->_Pack = Pck;
		ret->num = PuzNum;
		ret->_Tag = ret->Pack()->Tag(PuzNum);
		int r, g, b;
		int TimeOut{ 10000 };
		do {
			Assert(TimeOut--, "Random Sinus Color Timeout"); // Should never happen, but this is a kind of security safe guard.
			r = TRand(255);
			g = TRand(255);
			b = TRand(255);
		} while (r + g + b < 100);
		printf("Sinus color set to #%02x%02x%02x\n", r, g, b);
		SinusColor(r, g, b); 
		cout << "Loading puzzle: " << ret->_Tag<<endl; 
		ret->PuzMap = SuperTed::LoadTeddy(Resource(),"Packages/"+Pck+"/Puzzles/"+ret->_Tag);
		if (!ret->PuzMap) PZLCrash("For unknown reasons loading puzzle " + ret->_Tag + " from package '" + Pck + "' failed");
		cout << "Success. PixelFormat " << ret->PixW() << "x" << ret->PixH() << "; TileFormat:" << ret->W()<< "x"<<ret->H() <<"; Grid: "<<ret->GridW()<<"x"<<ret->GridH() << endl;
		ret->PX = (TQSG_ScreenWidth() / 2) - (ret->PixW() / 2);
		ret->PY = ((TQSG_ScreenHeight() - 100) / 2) - (ret->PixH() / 2);
		ret->_Mission = MSCMP(ret->PuzMap->Data["Mission"]);
		if (ret->_Mission == Mission::Unknown) {
			if (ret->PuzMap->Data["Mission"] == "Regular")
				ret->_Mission = Mission::Normal;
			else if (ret->PuzMap->Data["Mission"] == "Color Splitting") 
				ret->_Mission = Mission::ColorSplit;
			else
				Crash("Failed to parse mission: '" + ret->PuzMap->Data["Mission"]);
		}
		PlayPuzzle = ret;
		ResetMDots();
		ScanObjects();
		Scan4Tools();
		return ret;
	}

}