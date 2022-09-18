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
// Version: 22.09.19
// EndLic

#pragma region Include_C++
#include <string>
#pragma endregion

#pragma region TQSG
#include <TQSE.hpp>
#pragma endregion

#pragma region TrickyUnits
#include <TrickySTOI.hpp>
#include <TRandom.hpp>
#pragma endregion

#pragma region BallPlay_Includes
#include <dir_obj.h>
#include <Sinus.hpp>
#include <Game.hpp>
#include <Fonts.hpp>
#include <MainMenu.hpp>
#include <Chain.hpp>
#pragma endregion


#pragma region UsingNamespace
using namespace std;
using namespace TrickyUnits;
#pragma endregion

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

#pragma region Kaartkleuren
	static int CountBalls();
	static bool __Always(Puzzle P) { return true; }
	static bool __ShowHearts(Puzzle P) { return PlayPuzzle->EMission() == Mission::Normal || PlayPuzzle->EMission()==Mission::ColorSplit || PlayPuzzle->EMission() == Mission::BreakFree; }
	static int __Hearts(Puzzle P) { return 0; }
	static int __Clubs(Puzzle P) { return P->Required(); }
	static int __Diamonds(Puzzle P) { return CountBalls(); }
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
		SClass("Spades",__Spades,__Always)
	};
#pragma endregion


#pragma region Game_buttons_and_stages

	enum class GameStages {
		PreStart, // Just show the puzzle before move
		Game, // Game itself
		Pause, // Game paused
		Success,
		Fail
	};
	
	// Choises
	void SetGameStage(GameStages);
	void StartPuzzle(GameStages) { Crash("Start puzzle NOT yet implemented"); }
	void Other(GameStages) { SetGameStage(GameStages::PreStart); SetChain(PuzzleSelector); TQSE_Flush(); }
	void BackToMain(GameStages) { SetGameStage(GameStages::PreStart); SetChain(MainMenu); TQSE_Flush();	}
	void Pause(GameStages) { SetGameStage(GameStages::Pause); }
	void Resume(GameStages) { SetGameStage(GameStages::Game); }
	void Next(GameStages) { Crash("Next puzzle yet to be implemented"); }
	void Again(GameStages) { Crash("Again same puzzle yet to be implented"); }
	void Forfeit(GameStages) { if(TQSE_Yes("Do you really want to forfeit this puzzle?")) SetGameStage(GameStages::Fail); }

	// Flows
	void EmptyFlow(GameStages) {}
	void GameFlow(GameStages);
	

	typedef void(*StageFunction)(GameStages);
	class _TStageButton; typedef shared_ptr<_TStageButton> TStageButton;
	class _TStageButton {
	public:
		TQSG_AutoImage ButImg{nullptr};
		StageFunction Reaction;
		string ButFile{ "" };
		_TStageButton(string ImgFile, StageFunction _Reaction) {
			Reaction = _Reaction;
			ButFile = ImgFile; // Can't load immediately as TQSG is not initized when this constructor is called.			
		}
	};
	class TStage {
	public:
		static GameStages Current;		
		static map<GameStages, TStage> Stage;
		GameStages Me{GameStages::PreStart};
		vector<TStageButton> Button;
		StageFunction Flow{ nullptr };
		TStage() {} // Compiler wants it for some silly reason
		TStage(GameStages id, StageFunction _Flow, vector<TStageButton> _Buttons) {
			Me = id;
			Flow = _Flow;
			Button = _Buttons;
		}

		void HandleButtons() {
			int
				ey{ TQSG_ScreenHeight() },
				y{ 0 },
				mx{ TQSE_MouseX() },
				my{ TQSE_MouseY() };
			for (auto B : Button) {
				if (!B->ButImg) {
					B->ButImg = TQSG_LoadAutoImage(Resource(), "GFX/Game/Buttons/" + B->ButFile + ".png");
					cout << "Loaded image for game button: " << B->ButFile << endl;
				}
				y = ey - B->ButImg->H();
				TQSG_Color(255, 255, 255); 
				if (my > y && my < ey && mx < 5 + B->ButImg->W()) {
					TQSG_Color(180, 0, 255);
					if (TQSE_MouseHit(1)) B->Reaction(Me);
				}
				B->ButImg->Draw(5, y);
				ey = y;
			}

		}
	};
	GameStages TStage::Current{ GameStages::PreStart };
	map<GameStages, TStage> TStage::Stage{
		{ GameStages::PreStart , { GameStages::PreStart, EmptyFlow, {
			make_shared<_TStageButton>("Main Menu", BackToMain),
			make_shared<_TStageButton>("Other", Other),
			make_shared<_TStageButton>("Start", StartPuzzle) }
			} } ,
		{ GameStages::Pause,{GameStages::Pause,EmptyFlow,{
			make_shared<_TStageButton>("Main Menu",BackToMain),
			make_shared<_TStageButton>("Resume",Resume) }
			} },
		{GameStages::Game,{GameStages::Game,GameFlow,{
			make_shared<_TStageButton>("Forfeit",Forfeit)	,
			make_shared<_TStageButton>("Pause",Pause)}
			} },
		{GameStages::Success,{GameStages::Success,EmptyFlow,{
			make_shared<_TStageButton>("MainMenu",BackToMain),
			make_shared<_TStageButton>("Other",Other),
			make_shared<_TStageButton>("Again",Again),
			make_shared<_TStageButton>("Next",Next)}
			}},
		{GameStages::Fail,{GameStages::Fail,EmptyFlow,{
			make_shared<_TStageButton>("MainMenu",BackToMain),
			make_shared<_TStageButton>("Other",Other),
			make_shared<_TStageButton>("Again",Again)}
			}}		
	};
	void SetGameStage(GameStages N) { TStage::Current = N; }

#pragma endregion


#pragma region Game_Objects
	enum class BallColor { None = Ball, Red = RedBall, Green = GreenBall };
	enum class ObjDirection { North, South, East, West };
	class _GameObject; typedef shared_ptr<_GameObject> GameObject;
	class _GameObject {
	private:
		static map<string,map<ObjTypes, TQSG_AutoImage>> ImgReg;
		static GameObject _NEW() { return make_shared<_GameObject>(); }
	public:
		static vector<GameObject> List;
		ObjTypes Type{ Ball };
		int
			x{ 0 },
			y{ 0 },
			modx{ 0 },
			mody{ 0 },
			spdx{ 2 },
			spdy{ 2 };
		byte
			r{ 255 },
			g{ 255 },
			b{ 255 },
			a{ 255 };
		ObjDirection Direction{ ObjDirection::South };
		TQSG_AutoImage Img() { return ImgReg[PlayPuzzle->PackName()][Type]; }

		static GameObject NewBall(int x, int y, ObjDirection D=ObjDirection::South,BallColor Col = { BallColor::None }) {
			auto ret{ _NEW() };
			auto Pack{ PlayPuzzle->PackName() };
			ret->Type = (ObjTypes)Col;
			ret->Direction = D;
			ret->x = x;
			ret->y = y;
			if (!ImgReg[Pack].count(Ball)) {
				ImgReg[Pack][Ball] = TQSG_LoadAutoImage(Resource(), "Packages/" + PlayPuzzle->PackName() + "/Objects/Ball.png");
				ImgReg[Pack][RedBall] = ImgReg[Pack][Ball];
				ImgReg[Pack][GreenBall] = ImgReg[Pack][Ball];
			}
			switch (Col) {
			case BallColor::None:
				break;
			case BallColor::Red:
				ret->r = 255;
				ret->g = 0;
				ret->b = 0;
				break;
			case BallColor::Green:
				ret->r = 0;
				ret->g = 255;
				ret->b = 0;
				break;
			default:
				Crash("Unknown ball color\nCan only be the result of a bug. Please report!");
				break;
			}
			List.push_back(ret);
			return ret;
		}
		static void Scan(bool clean=true) {
			if (clean) List.clear();
			uint32 obj0{ 0 };
			for (auto iy = 0; iy < PlayPuzzle->H(); ++iy) {
				for (auto ix = 0; ix < PlayPuzzle->W(); ++ix) {
					if (PlayPuzzle->PuzR()->MapObjects->Value(ix, iy)) {
						for (auto o : *PlayPuzzle->PuzR()->MapObjects->Value(ix, iy)) {
							auto d{ ObjDirection::South };
							if (o->Data["Direction"] == "North") d = ObjDirection::North;
							else if (o->Data["Direction"] == "East") d = ObjDirection::East;
							else if (o->Data["Direction"] == "West") d = ObjDirection::West;
							switch (o->kind) {
							case 0:
								cout << "\x1b[31mWARNING!\x1b[0m\tObject kind 0 encountered on position (" << ix << "," << iy << ") (" << ++obj0 << ")\n";
								break;
							case Ball:
								NewBall(ix, iy, d);
								break;
							default: {
								char E[400];
								sprintf_s(E, "Unknown object type: '%d'(%x) on position (%02d,%02d)!", o->kind, o->kind, ix, iy);
								Crash(E);
							} break;								
							}
						}
					}
					cout << "  Scanning for objects: " << List.size() << "\r";
				}
			}
			cout << endl;
		}

		void Draw() {
			int dx{ PlayPuzzle->gPX() + (x * PlayPuzzle->GridW()) + modx };
			int dy{ PlayPuzzle->gPY() + (y * PlayPuzzle->GridH()) + mody };
			TQSG_ACol(r, g, b, a);
			Img()->Draw(dx, dy);
			if (modx > 0) modx = max(0, modx - spdx); else if (modx < 0) modx = min(0, modx + spdx);
			if (mody > 0) mody = max(0, mody - spdy); else if (mody < 0) mody = min(0, mody + spdy);
		}
		static void DrawAll() {
			for (auto o : List) o->Draw();
		}
	};

	map<string,map<ObjTypes, TQSG_AutoImage>> _GameObject::ImgReg{};
	vector<GameObject> _GameObject::List{};

	void ScanObjects(bool clean) { _GameObject::Scan(clean); }
	static int CountBalls() {
		int ret{ 0 };
		for (auto o : _GameObject::List) {
			switch (o->Type) {
			case ObjTypes::Ball:
			case ObjTypes::BallGreen:
			case ObjTypes::BallRed:
				ret++;
				break;
			}
		}
		return ret;
	}
#pragma endregion


#pragma region Game_Tools
	class GameTool {
	public:
		static byte Chosen;
		static GameTool Tools[4];
		string ImgTag{ "" };
		map<string, TQSG_AutoImage> Img{}; // String = Pack. The picks can be different depending on the pack
		string Layer{ "DIRECTIONS" };
		int PlaceTileID{ 0 };
		string FromPuzData{ "" };
		int left{ 0 };
		bool show{ false };
		GameTool(string _ImgTag, int _PlaceTileID, string _FromPuzData, string _Layer = "DIRECTIONS") {
			ImgTag = _ImgTag;
			PlaceTileID = _PlaceTileID;
			FromPuzData = _FromPuzData;
			Layer = _Layer;
		}

		static void Draw() {
			static int y{ TQSG_ScreenHeight() - 80 };
			if (Chosen >= 4) Chosen %= 4;
			for (int i = 0; i < 4; ++i) {
				if (Tools[i].show) {
					if (!Tools[Chosen].show) Chosen = i;
					int x = 250 + (i * 65);
					if (!Tools[i].Img.count(PlayPuzzle->PackName())) {
						cout << "No " << Tools[i].ImgTag << " picture on pack " << PlayPuzzle->PackName() << "? Let's load it then\n";
						Tools[i].Img[PlayPuzzle->PackName()] = TQSG_LoadAutoImage(Resource(), "Packages/" + PlayPuzzle->PackName() + "/Tools/" + Tools[i].ImgTag + ".png");
					}
					if (TQSE_KeyHit((SDL_KeyCode)(i + 49))) Chosen = i;
					if (i == Chosen) {
						TQSG_Color(TRand(0, 255), TRand(0, 255), TRand(0, 255));
						TQSG_Rect(x - 1, y - 1, Tools[i].Img[PlayPuzzle->PackName()]->W() + 2, Tools[i].Img[PlayPuzzle->PackName()]->H() + 2, true);
					}
					TQSG_Color(255, 255, 255);
					Tools[i].Img[PlayPuzzle->PackName()]->Draw(x, y);
					TQSG_Color(0, 0, 0);
					Fnt->Draw(to_string(Tools[i].left), x - 3, y + Tools[i].Img[PlayPuzzle->PackName()]->H() - 2, 2, 1);
					TQSG_Color(255, 255, 255);
					Fnt->Draw(to_string(Tools[i].left), x - 5, y + Tools[i].Img[PlayPuzzle->PackName()]->H() - 4, 2, 1);
				}
			}
		}

	};
	byte GameTool::Chosen{ 0 };
	GameTool GameTool::Tools[4]{
		{"Plate1",redplate1, "Plate/"},
		{"Plate2",redplate2, "Plate\\"},
		{"Barrier",1,"Wall","WALL"},
		{"Remove",0,"Remove"}
	};
	void Scan4Tools(){
		for (int i = 0; i < 4;++i) {
			GameTool::Tools[i].left = PlayPuzzle->iDat(GameTool::Tools[i].FromPuzData);
			GameTool::Tools[i].show = GameTool::Tools[i].left > 0;
		}
	}
#pragma endregion

#pragma region Game_Flow
	void GameFlow(GameStages s){}
#pragma endregion


#pragma region Chain callback
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
		for (auto dlay : LayOrder) {
			if (strcmp(dlay, "WALL") == 0) _GameObject::DrawAll();
			PlayPuzzle->DrawLayer(dlay);
		}
		SClass::ShowSuits();
		GameTool::Draw();
		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->Title(),4,4);
		TQSG_Color(255, 255, 255);
		Fnt->Draw(PlayPuzzle->Title(), 2, 2);
		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth()-2, 4,1,0);
		TQSG_Color(180,255,0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth() - 4, 2, 1, 0);
		TStage::Stage[TStage::Current].HandleButtons();
		Flip();
		return true;
	}
#pragma endregion
}