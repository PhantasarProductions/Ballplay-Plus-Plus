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
// Version: 22.10.15
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
#include <TrickyTime.hpp>
#include <TrickyMath.hpp>
#include <TrickyLinkedList.hpp>
#pragma endregion

#pragma region BallPlay_Includes
#include <dir_obj.h>
#include <Sinus.hpp>
#include <Game.hpp>
#include <Fonts.hpp>
#include <MainMenu.hpp>
#include <Chain.hpp>
#include <Users.hpp>
#include <Audio.hpp>
#pragma endregion


#pragma region UsingNamespace
using namespace std;
using namespace TrickyUnits;
#pragma endregion

namespace BallPlay {

	static const int OWaitTicks{ 500 };
	static int OTicks{ 0 }, PTicks{ 0 };

	static const char LayOrder[5][25]{
		"FLOOR",
		"DIRECTIONS",
		"BOMBS",
		"WALL",
		"BREAK"
	};

	static TQSG_AutoImageFont Fnt{ nullptr };
	Puzzle PlayPuzzle{ nullptr };


	enum class BallColor { None = Ball, Red = RedBall, Green = GreenBall, Blue = BlueBall, Ember = EmberBall };
	enum class ObjDirection { North, South, East, West };


#pragma region Kaartkleuren
	static int CountBalls();
	static bool __Always(Puzzle P) { return true; }
	static bool __ShowHearts(Puzzle P) { return PlayPuzzle->EMission() == Mission::Normal || PlayPuzzle->EMission()==Mission::ColorSplit || PlayPuzzle->EMission() == Mission::BreakFree; }
	static int __Hearts(Puzzle P) { return P->BallsIn; }
	static int __Clubs(Puzzle P) { return P->Required(); }
	static int __Diamonds(Puzzle P) { return CountBalls(); }
	static int __Spades(Puzzle P) { return P->BallsDestroyed; }

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

#pragma region Lasers
	class _LaserPoint;
	typedef shared_ptr<_LaserPoint> LaserPoint;
	class _LaserPoint {
	private:
	public:
		static BallColor Cols[4];
		static map<BallColor, shared_ptr<vector<LaserPoint>>> Register;
		static map<BallColor, bool> _Activated;
		int StartX, StartY;
		BallColor Color{ BallColor::None };
		byte r{ 0 }, g{ 0 }, b{ 0 };
		ObjDirection Dir;
		bool Activated() { return _Activated[Color]; }
		_LaserPoint(int x, int y, BallColor C, ObjDirection D) {
			StartX = x;
			StartY = y;
			Dir = D;
			Color = C;
			switch (C) {
			case BallColor::Red: r = 255; break;
			case BallColor::Green: g = 255; break;
			case BallColor::Blue: b = 255; break;
			case BallColor::Ember: r = 255; g = 180; break;
			default: Crash("Unknown laser color"); break;
			}
			printf("Laser created (%02d,%02d) (Col code: %04x) Color #%02x%02x%02x  Dir: %d\n", x, y, (int)C, r, g, b, (int)D);
		}
		void Shoot();
		static void Make(int x, int y, BallColor C, ObjDirection D) {
			if (!Register.count(C)) Register[C] = make_shared<vector<LaserPoint>>();
			Register[C]->push_back(make_shared<_LaserPoint>(x, y, C, D));
		}
		static void ClearActivated() {

			for (auto c : Cols) _Activated[c] = false;
		}
	};
	BallColor _LaserPoint::Cols[4]{ BallColor::Red,BallColor::Green,BallColor::Ember,BallColor::Blue };
	map<BallColor, shared_ptr<vector<LaserPoint>>> _LaserPoint::Register{};
	map<BallColor, bool> _LaserPoint::_Activated;
#pragma endregion


#pragma region Game_buttons_and_stages

	enum class GameStages {
		PreStart, // Just show the puzzle before move
		Game, // Game itself
		Pause, // Game paused
		Success,
		Fail
	};
	
	// Choices
	void SetGameStage(GameStages);
	void StartPuzzle(GameStages) { SetGameStage(GameStages::Game); TQSE_Flush(); }
	void Other(GameStages) { SetGameStage(GameStages::PreStart); SetChain(PuzzleSelector); TQSE_Flush(); }
	void BackToMain(GameStages) { SetGameStage(GameStages::PreStart); SetChain(MainMenu); TQSE_Flush();	}
	void Pause(GameStages) { SetGameStage(GameStages::Pause); }
	void Resume(GameStages) { SetGameStage(GameStages::Game); }
	void Next(GameStages) {  
		_Puzzle::Load(PlayPuzzle->PackName(),PlayPuzzle->Next());
		SetGameStage(GameStages::PreStart);
	}
	void Again(GameStages) { 
		PlayPuzzle->Reload();
		SetGameStage(GameStages::PreStart);
	}
	void Forfeit(GameStages) { if(TQSE_Yes("Do you really want to forfeit this puzzle?")) SetGameStage(GameStages::Fail); }

	// Flows
	void EmptyFlow(GameStages s) {
		static int MdX{ TQSG_ScreenWidth() / 2 };
		TQSG_AutoImage
			PZ_Success{ nullptr },
			PZ_Fail{ nullptr },
			PZ_Pause{ nullptr };
		TQSG_Color(255, 255, 255);
		switch (s) {
		case GameStages::PreStart:
			break;
		case GameStages::Fail:
			if (!PZ_Fail) PZ_Fail = TQSG_LoadAutoImage(Resource(), "GFX/Game/Result/Failed.png");
			PZ_Fail->Draw(MdX - (PZ_Fail->W() / 2), 50);
			break;
		case GameStages::Success:
			if (!PZ_Success) PZ_Success = TQSG_LoadAutoImage(Resource(), "GFX/Game/Result/Success.png");
			PZ_Success->Draw(MdX - (PZ_Success->W() / 2), 50);
			break;
		case GameStages::Pause:
			if (!PZ_Pause) PZ_Pause = TQSG_LoadAutoImage(Resource(), "GFX/Game/Result/Paused.png");
			PZ_Pause->Draw(MdX - (PZ_Pause->W() / 2), 50);
			break;
		}
	}
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
			make_shared<_TStageButton>("Give Up",Forfeit)	,
			make_shared<_TStageButton>("Pause",Pause)}
			} },
		{GameStages::Success,{GameStages::Success,EmptyFlow,{
			make_shared<_TStageButton>("Main Menu",BackToMain),
			make_shared<_TStageButton>("Other",Other),
			make_shared<_TStageButton>("Again",Again),
			make_shared<_TStageButton>("Next",Next)}
			}},
		{GameStages::Fail,{GameStages::Fail,EmptyFlow,{
			make_shared<_TStageButton>("Main Menu",BackToMain),
			make_shared<_TStageButton>("Other",Other),
			make_shared<_TStageButton>("Again",Again)}
			}}		
	};
	void SetGameStage(GameStages N) { TStage::Current = N; /* TQSE_Flush(); */ }
	
#pragma endregion

#pragma region Explosions
	class _Explosion; typedef shared_ptr<_Explosion> Explosion;
	class _Explosion {
		static TQSG_AutoImage Pics;
		static int Teller;
		int ID{ Teller++ };
	public:
		
		const int MaxFrame{ 16 };
		const int MaxSkip{ 2 };
		int 
			x{ 0 }, y{ 0 },
			Skip{ 0 },
			Frame{ 0 };
		static TList<Explosion> Lijst;
		static Explosion NEW(int x, int y,bool pure=false) {
			int dx, dy;
			if (pure) { dx = x; dy = y; }
			else {
				dx = (x * PlayPuzzle->GridW()) + (PlayPuzzle->GridW() / 2) + PlayPuzzle->gPX();
				dy = (y * PlayPuzzle->GridH()) + (PlayPuzzle->GridH() / 2) + PlayPuzzle->gPY();
			}
			auto ret{ make_shared <_Explosion>() };
			ret->x = dx;
			ret->y = dy;
			if (!Pics) {
				cout << "Loading explosions picture!\n";
				Pics = TQSG_LoadAutoImage(Resource(), "GFX/Game/Effect/Explosion.jpbf");
				Pics->HotCenter();
			}
			cout << "Explosion #" << ret->ID << " ("<<dx<<","<<dy<<") initiated!\n";
			Lijst += ret;		
			return ret;
		}

		static void Run() {
			//cout << "Start!\n";
			TLFORA(Lijst, EL) {
				//cout << "Explosion loop!\n"; // DEBUG! Making sure no illegal loops come here!
				auto E{ EL->Obj };
				if (E->Frame >= E->MaxFrame)
					EL->UnLink();
				else {
					TQSG_Color(255, 255, 255);
					Pics->Draw(E->x, E->y, E->Frame);
					E->Skip = (++E->Skip) % E->MaxSkip;
					if (E->Skip == 0) E->Frame++;
				}
			}
		}
		~_Explosion() { cout << "Disposed explosion #" << ID << endl; }
	};
	TList<Explosion> _Explosion::Lijst;
	TQSG_AutoImage _Explosion::Pics{ nullptr };
	int _Explosion::Teller{ 0 };
#pragma endregion

#pragma region DeathZones
	class _DeathAnim;
	typedef shared_ptr<_DeathAnim> DeathAnim;

	class _DeathAnim {
	private:
		const int maxskip{ 3 };
		int skip{ 0 };
		static int Teller;
		int ID{ Teller++ };
	public:
		static TList<DeathAnim> List;
		int
			x{ 0 },
			y{ 0 },
			modx{ 0 },
			mody{ 0 },
			scalex{ 0 },
			scaley{ 0 };
		TQSG_AutoImage img;
		static void Create(TQSG_AutoImage img, int x, int y, int modx=0, int mody=0) {
			auto ret{ make_shared<_DeathAnim>() };
			ret->x = x;
			ret->y = y;
			ret->modx = modx;
			ret->mody = mody;
			ret->img = img;
			cout << "Death anim #" << ret->ID << " created\n";
			List += ret;
		}
		~_DeathAnim() { cout << "Death anim #" << ID << " disposed!\n"; }
		int DW() { return max(0, img->W() - (scalex * 2)); }
		int DH() { return max(0, img->H() - (scaley * 2)); }
		void Draw(bool dontprocess = false) {
			if (DW() && DH()) img->Stretch(x + scalex + modx, y + scaley + mody, DW(), DH());
			if (dontprocess) return;
			skip = (++skip) % maxskip;
			if (!skip) { 
				if (abs(modx) || abs(mody)) {
					if (modx < 0) modx = min(0, modx + 4); else if (modx > 0) modx = max(0, modx - 4);
					if (mody < 0) mody = min(0, mody + 4); else if (mody > 0) mody = max(0, mody - 4);
				} else {
					scalex++; scaley++;
				}
			}
		}
		static void DrawAll(bool dontprocess = false, bool dontremove = false) {
			TLFORA(List, Lnk) {
				Lnk->Obj->Draw(dontprocess);
				if (!dontremove) {
					if (Lnk->Obj->DW() <= 0 || Lnk->Obj->DH() <= 0) Lnk->UnLink();
				}
			}
		}
	};
	int _DeathAnim::Teller{ 0 };
	TList<DeathAnim> _DeathAnim::List{};

#pragma endregion

#pragma region Game_Objects
	class _GameObject; typedef shared_ptr<_GameObject> GameObject;
	void Destroy(_GameObject* o);
	void Bomb(_GameObject* o);

	typedef void (*ObjectMove)(_GameObject*);
	void RegMove(_GameObject*); void GirlMove(_GameObject*);
	class _GameObject {
	private:
		static uint64 Teller;
		static map<string,map<ObjTypes, TQSG_AutoImage>> ImgReg;
		static GameObject _NEW() { return make_shared<_GameObject>(); }
		uint64 _ID{ Teller++ };
		ObjectMove _Move{ nullptr };
	public:
		static vector<GameObject> List;
		static vector<GameObject> Girls;
		bool Removed{ false };
		ObjTypes Type{ Ball };
		int
			x{ 0 },
			y{ 0 },
			modx{ 0 },
			mody{ 0 },
			spdx{ 4 },
			spdy{ 4 };
		byte
			r{ 255 },
			g{ 255 },
			b{ 255 },
			a{ 255 };
		bool JustTransported{ false };
		BallColor Col{ BallColor::None }; // Only used for exit scans
		ObjDirection Direction{ ObjDirection::South };
		TQSG_AutoImage Img() { return ImgReg[PlayPuzzle->PackName()][Type]; }

		int ID() { return _ID; }
		static GameObject NewGhost(int x, int y, ObjDirection D, byte r, byte g, byte b, byte alpha, ObjectMove _MoveFunction = nullptr) {
			auto ret{ _NEW() };
			auto Pack{ PlayPuzzle->PackName() };
			ret->Type = Ghost;
			ret->Direction = D;
			ret->x = x;
			ret->y = y;
			ret->_Move = _MoveFunction;
			ret->r = r;
			ret->g = g;
			ret->b = b;
			ret->a = alpha;
			if (!ImgReg[Pack].count(Ghost)) {
				ImgReg[Pack][Ghost] = TQSG_LoadAutoImage(Resource(), "Packages/" + PlayPuzzle->PackName() + "/Objects/Ghost.png");
				Assert(ImgReg[Pack][Ghost] && ImgReg[Pack][Ghost]->Frames(), "No ghost image loaded");
			}
			List.push_back(ret);
			cout << "Ghost created! Object #" << List.size() << endl;
			return ret;
		}

		static GameObject NewDroid(int x, int y, ObjDirection D, ObjectMove _MoveFunction = nullptr) {
			auto ret{ _NEW() };
			auto Pack{ PlayPuzzle->PackName() };
			ret->Type = Droid;
			ret->Direction = D;
			ret->x = x;
			ret->y = y;
			if (!ImgReg[Pack].count(Droid)) {
				ImgReg[Pack][Droid] = TQSG_LoadAutoImage(Resource(), "Packages/" + PlayPuzzle->PackName() + "/Objects/Droid.png");
				Assert(ImgReg[Pack][Droid] && ImgReg[Pack][Droid]->Frames(), "No Droid image loaded");
			}
			List.push_back(ret);
			cout << "Droid created! Object #" << List.size() << endl;
			return ret;
		}

		static GameObject NewGirl(int x, int y) {
			auto ret{ _NEW() };
			auto Pack{ PlayPuzzle->PackName() };
			ret->Type = Girl;
			ret->x = x;
			ret->y = y;
			ret->_Move = GirlMove;
			if (!ImgReg[Pack].count(Girl)) {
				cout << "Loading girl\n";
				// I must keep in mind to update this, if more packs are going to support this, but for now only Cupid does.
				ImgReg[Pack][Girl] = TQSG_LoadAutoImage(Resource(), "Packages/BallPlay Cupid/Textures/pz_objects_woman.png");
				Assert(ImgReg[Pack][Girl] && ImgReg[Pack][Girl]->Frames(), "No Girl image loaded");
				ImgReg[Pack][Girl]->Hot(0, PlayPuzzle->GridH());
			}
			List.push_back(ret);
			Girls.push_back(ret);
		}

		static GameObject NewBall(int x, int y, ObjDirection D=ObjDirection::South,BallColor Col = { BallColor::None },ObjectMove _MoveFunction=nullptr) {
			auto ret{ _NEW() };
			auto Pack{ PlayPuzzle->PackName() };
			//ret->Type = (ObjTypes)Col;
			ret->Type = Ball;
			ret->Col = Col;
			ret->Direction = D;
			ret->x = x;
			ret->y = y;
			ret->_Move = _MoveFunction;
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
			case BallColor::Ember:
				ret->r = 255;
				ret->g = 180;
				ret->b = 0;
				break;
			case BallColor::Blue:
				ret->r = 0;
				ret->g = 0;
				ret->b = 255;
				break;
			default:
				Crash("Unknown ball color\nCan only be the result of a bug. Please report!");
				break;
			}
			List.push_back(ret);
			cout << "Ball created! Object #" << List.size() << endl;
			return ret;
		}
		static void Scan(bool clean=true) {
			if (clean) {
				List.clear();
				Girls.clear();
			}
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
							case GreenBall:
								NewBall(ix, iy, d, BallColor::Green);
								break;
							case RedBall:
								NewBall(ix, iy, d, BallColor::Red);
								break;
							case EmberBall:
								NewBall(ix, iy, d, BallColor::Ember);
								break;
							case BlueBall:
								NewBall(ix, iy, d, BallColor::Blue);
								break;
							case Ghost:
								NewGhost(ix, iy, d, ToInt(o->Data["Red"]), ToInt(o->Data["Green"]), ToInt(o->Data["Blue"]), max(ToInt(o->Data["Alpha"]), 150));
								break;
							case Droid:
								NewDroid(ix, iy, d);
								break;
							case LaserRedEast:
								_LaserPoint::Make(ix, iy, BallColor::Red, ObjDirection::East);
								break;
							case LaserRedWest:
								_LaserPoint::Make(ix, iy, BallColor::Red, ObjDirection::West);
								break;
							case LaserRedNorth:
								_LaserPoint::Make(ix, iy, BallColor::Red, ObjDirection::North);
								break;
							case LaserRedSouth:
								_LaserPoint::Make(ix, iy, BallColor::Red, ObjDirection::South);
								break;
							case LaserBlueEast:
								_LaserPoint::Make(ix, iy, BallColor::Blue, ObjDirection::East);
								break;
							case LaserBlueWest:
								_LaserPoint::Make(ix, iy, BallColor::Blue, ObjDirection::West);
								break;
							case LaserBlueNorth:
								_LaserPoint::Make(ix, iy, BallColor::Blue, ObjDirection::North);
								break;
							case LaserBlueSouth:
								_LaserPoint::Make(ix, iy, BallColor::Blue, ObjDirection::South);
								break;
							case LaserEmberEast:
								_LaserPoint::Make(ix, iy, BallColor::Ember, ObjDirection::East);
								break;
							case LaserEmberWest:
								_LaserPoint::Make(ix, iy, BallColor::Ember, ObjDirection::West);
								break;
							case LaserEmberNorth:
								_LaserPoint::Make(ix, iy, BallColor::Ember, ObjDirection::North);
								break;
							case LaserEmberSouth:
								_LaserPoint::Make(ix, iy, BallColor::Ember, ObjDirection::South);
								break;
							case LaserGreenEast:
								_LaserPoint::Make(ix, iy, BallColor::Green, ObjDirection::East);
								break;
							case LaserGreenWest:
								_LaserPoint::Make(ix, iy, BallColor::Green, ObjDirection::West);
								break;
							case LaserGreenNorth:
								_LaserPoint::Make(ix, iy, BallColor::Green, ObjDirection::North);
								break;
							case LaserGreenSouth:
								_LaserPoint::Make(ix, iy, BallColor::Green, ObjDirection::South);
								break;
							case Girl:
								NewGirl(ix, iy);
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

		void Move() {
			if (!_Move) _Move = RegMove;
			_Move(this);
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
			for (auto o : List) if (!o->Removed) {
				o->Draw();
				for (auto o2 : List) if (!o2->Removed) {
					if (o2->x == o->x && o2->y == o->y && o2->ID() != o->ID()) {
						switch (o->Type) {
						case ObjTypes::Ball:
							if (o2->Type == ObjTypes::Girl) Crash("Killing girls by balls not yet supported");
							break;
						case ObjTypes::Ghost:
							switch(o2->Type){
							case ObjTypes::Girl: Crash("Killing girls by ghosts not yet supported"); break;
							case ObjTypes::Ball: Destroy(o2.get()); SFX("Ghost"); break;
							}
							break;
						case ObjTypes::Droid:
							switch (o2->Type) {
							case ObjTypes::Girl: Crash("Killing girls by droids not yet supported"); break;
							case ObjTypes::Ball: Bomb(o2.get()); break;
							}
						}
					}
				}
			}
		}
	};
	uint64 _GameObject::Teller{ 0 };
	map<string,map<ObjTypes, TQSG_AutoImage>> _GameObject::ImgReg{};
	vector<GameObject> _GameObject::List{};
	vector<GameObject> _GameObject::Girls{};

	void ScanObjects(bool clean) { _GameObject::Scan(clean); }
	static int CountBalls() {
		int ret{ 0 };
		for (auto o : _GameObject::List) {
			if (!o->Removed) {
				switch (o->Type) {
				case ObjTypes::Ball:
				case ObjTypes::BallGreen:
				case ObjTypes::BallRed:
					ret++;
					break;
				}
			}
		}
		return ret;
	}
	static int CountBreakBlocks() {
		int ret{ 0 };
		for (uint32 y = 0; y < PlayPuzzle->H(); y++)
			for (uint32 x = 0; x < PlayPuzzle->W(); x++)
				if (PlayPuzzle->PuzR()->LayVal("BREAK", x, y))
					ret++;
		return ret;
	}

	void _LaserPoint::Shoot() {
		int
			beamx{ StartX },
			beamy{ StartY },
			lbx{ StartX * PlayPuzzle->GridW() },
			lby{ StartY * PlayPuzzle->GridH() },
			lsx{ 0 },
			lsy{ 0 },
			lex{ 0 },
			ley{ 0 };
		//cout << "Shoot! (" << StartX << "," << StartY << ") D:" << (int)Dir << "C:" << (int)r << "," << (int)g << "," << (int)b << endl; // Debug Only
		//cout << "Check " << (beamx >= 0) << (beamy >= 0) << (beamx < PlayPuzzle->W()) << (beamy < PlayPuzzle->H()) << (PlayPuzzle->PuzR()->LayVal("WALL", beamx, beamy) == 0) << (PlayPuzzle->PuzR()->LayVal("BREAK", beamx, beamy) == 0) << " Break:" << PlayPuzzle->PuzR()->LayVal("BREAK", beamx, beamy) << endl;
		do {
			switch (Dir) {
			case ObjDirection::North:
				lsy = lby;
				lsx = lbx + (PlayPuzzle->GridW() / 2);
				beamy--;
				lex = lsx;
				ley = beamy * PlayPuzzle->GridH();
				break;
			case ObjDirection::South:
				lsy = lby + PlayPuzzle->GridH();
				lsx = lbx + (PlayPuzzle->GridW() / 2);
				beamy++;
				lex = lsx;
				ley = (beamy+1) * PlayPuzzle->GridH();
				break;
			case ObjDirection::West:
				lsx = lbx;
				lsy = lby + (PlayPuzzle->GridH() / 2);
				beamx--;
				ley = lsy;
				lex = beamx * PlayPuzzle->GridW();
				break;
			case ObjDirection::East:
				lsx = lbx + PlayPuzzle->GridW();
				lsy = lby + (PlayPuzzle->GridH() / 2);
				beamx++;
				ley = lsy;
				lex = (beamx+1) * PlayPuzzle->GridW();
				break;
			default:
				Crash("Unknown laser direction");
			}
			for (auto o : _GameObject::List) {
				if (beamx == o->x && beamy == o->y && (!o->Removed)) Bomb(o.get());
			}
		} while (beamx >= 0 && beamy >= 0 && beamx < PlayPuzzle->W() && beamy < PlayPuzzle->H() && PlayPuzzle->PuzR()->LayVal("WALL", beamx, beamy) == 0 && PlayPuzzle->PuzR()->LayVal("BREAK", beamx, beamy) == 0);
		TQSG_ACol(r, g, b, 255);
		//printf("Laser Line(%04d,%04d) -> (%04d,%04d)  :: GridLine(%02d,%02d) -> (%02d,%02d)\n", lsx + PlayPuzzle->gPX(), lsy + PlayPuzzle->gPY(), lex + PlayPuzzle->gPX(), ley + PlayPuzzle->gPY(),StartX,StartY,beamx,beamy); // debug only
		TrickyUnits::TQSG_Line(lsx + PlayPuzzle->gPX(), lsy + PlayPuzzle->gPY(), lex + PlayPuzzle->gPX(), ley + PlayPuzzle->gPY());
	}
#pragma endregion

#pragma region Dots
	int CountDots() {
		int ret{ 0 };
		for (uint32 y = 0; y < PlayPuzzle->H(); y++)
			for (uint32 x = 0; x < PlayPuzzle->W(); x++)
				if (PlayPuzzle->PuzR()->LayVal("DIRECTIONS", x, y)==dot)
					ret++;
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
			int
				MX{ TQSE_MouseX() },
				MY{ TQSE_MouseY() };
			bool ML{ TQSE_MouseHit(1) };
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
					if (MX > x && MY > y && MX < x + Tools[i].Img[PlayPuzzle->PackName()]->W() && MY < y + Tools[i].Img[PlayPuzzle->PackName()]->W() && ML) Chosen = i;
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
		{"Barrier",1,"Barrier","WALL"}, 
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
	bool Break(int x, int y) { 
		if (x < 0 || y < 0 || x >= PlayPuzzle->W() || y >= PlayPuzzle->H()) return true;
		auto ret{ PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y) > 0 };  
		if (ret) {
			SFX("Break");
			PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y, 0);
			if (PlayPuzzle->EMission() == Mission::BreakFree && CountBreakBlocks() == 0) PlayPuzzle->PuzR()->LayVal("DIRECTIONS", x, y, normalexit);
		}
		return ret; 
	}
	bool Block(int x, int y) { 
		if (x < 0 || y < 0 || x >= PlayPuzzle->W() || y >= PlayPuzzle->H()) return true;
		/*		
		TQSG_Color(255, 255, 255); Fnt->Draw(to_string(PlayPuzzle->PuzR()->Layers["WALL"]->Field->Value(x, y)), x * 40, y * 40);
		TQSG_Color(255, 180,0); Fnt->Draw(to_string(PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y)), x * 40, (y * 40)+20);
		//*/
		return PlayPuzzle->PuzR()->Layers["WALL"]->Field->Value(x, y) > 0;//&& PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y) > 0;
	}
	void Finished(_GameObject* o) {
		o->Removed = true;
		PlayPuzzle->BallsIn++;
	}

	void Destroy(_GameObject* o) {
		if (o->Type == Ball) PlayPuzzle->BallsDestroyed++;
		o->Removed = true;
	}

	void Bomb(_GameObject* o) {
		SFX("Explosion");
		_Explosion::NEW(o->x, o->y);
		Destroy(o);
	}

	void DeathCheck(_GameObject* o) {
		auto R{ PlayPuzzle->PuzR() };
		auto D{ R->LayVal("DEATH", o->x, o->y) };
		if ((!o->Removed) && D) {
			PlayPuzzle->Pack()->DeathSound(D);
			_DeathAnim::Create(o->Img(), PlayPuzzle->gPX() + (o->x * PlayPuzzle->GridW()), PlayPuzzle->gPY() + (o->y * PlayPuzzle->GridH()), o->modx, o->mody);
			Destroy(o);
		}
	}

	void RegMove(_GameObject* o) {
		// Conflict prevention on slow CPUs
		o->modx = 0;
		o->mody = 0;
		if (o->Removed) return; // cleaning up vectors can be a disaster. Should have used my Linked List in stead.
		// Transporters
		auto tr1{ PlayPuzzle->PuzR()->LayVal("TRANS", o->x, o->y) };
		if (tr1) {
			switch (o->Direction) {
			case ObjDirection::North:
			case ObjDirection::South:
				o->JustTransported = o->JustTransported && (!(Block(o->x, o->y + 1) && Block(o->x, o->y - 1)));
				break;
			case ObjDirection::West:
			case ObjDirection::East:
				o->JustTransported = o->JustTransported && (!(Block(o->x + 1, o->y) && Block(o->x - 1, o->y)));
				break;
			}
			if (!o->JustTransported) {
				cout << "\x1b[32mF: Transporter check: " << tr1 << "(" << o->x << "," << o->y << ")\x1b[0m\n"; // Debug only!
				for (uint32 y2 = 0; y2 < PlayPuzzle->H(); y2++) for (uint32 x2 = 0; x2 < PlayPuzzle->W(); x2++) {
					auto tr2{ PlayPuzzle->PuzR()->LayVal("TRANS", x2, y2) };
					if (tr2) cout << "\x1b[33mT: Transporter check: " << tr2 << "(" << x2 << "," << y2 << ") --> " << (tr1 == tr2) << "\x1b[0m\n";
					if ((x2 != o->x || y2 != o->y) && tr1 == tr2) {
						o->x = x2;
						o->y = y2;
						o->JustTransported = true;
						SFX("TRANS");
						goto einde_trans; // Make sure no conflicts with other transporters can arise!
					}
				}
			}
		}
		einde_trans:
		// Where to go?
		switch (o->Direction) {
		case ObjDirection::North:
			if (Break(o->x, o->y - 1) || Block(o->x, o->y - 1))
				o->Direction = ObjDirection::South;
			else {
				o->y--;
				o->mody = PlayPuzzle->GridH();
				o->JustTransported = false;
				//if (Break(o->x, o->y - 1) || Block(o->x, o->y - 1))
				//	o->Direction = ObjDirection::South;
			}
			break;
		case ObjDirection::South:
			if (Break(o->x, o->y + 1) || Block(o->x, o->y + 1))
				o->Direction = ObjDirection::North;
			else {
				o->y++;
				o->mody = -PlayPuzzle->GridH();
				o->JustTransported = false;
				//if (Break(o->x, o->y + 1) || Block(o->x, o->y + 1))
				//	o->Direction = ObjDirection::North;
			}
			break;
		case ObjDirection::West:
			if (Break(o->x - 1, o->y) || Block(o->x - 1, o->y))
				o->Direction = ObjDirection::East;
			else {
				o->x--;
				o->modx = PlayPuzzle->GridW();
				o->JustTransported = false;
				//if (Break(o->x - 1, o->y) || Block(o->x - 1, o->y - 1))
				//	o->Direction = ObjDirection::East;
			}
			break;
		case ObjDirection::East:
			if (Break(o->x + 1, o->y) || Block(o->x + 1, o->y))
				o->Direction = ObjDirection::West;
			else {
				o->x++;
				o->modx = -PlayPuzzle->GridW();
				o->JustTransported = false;
				//if (Break(o->x + 1, o->y) || Block(o->x + 1, o->y))
				//	o->Direction = ObjDirection::West;
			}
			break;
		default:
			Crash("(M)Uknown direction!");
		}
		// Direction changes
		switch (PlayPuzzle->PuzR()->Layers["DIRECTIONS"]->Field->Value(o->x, o->y)) {
		case levelplate1:
		case irreplacableplate1:
		case userplate1:
			if (o->Type != ObjTypes::Droid) {
				switch (o->Direction) { // Plate /
				case ObjDirection::North: if (!Block(o->x + 1, o->y)) o->Direction = ObjDirection::East; break;
				case ObjDirection::East: if (!Block(o->x, o->y - 1)) o->Direction = ObjDirection::North; break;
				case ObjDirection::South: if (!Block(o->x - 1, o->y)) o->Direction = ObjDirection::West; break;
				case ObjDirection::West: if (!Block(o->x, o->y + 1)) o->Direction = ObjDirection::South; break;
				}
			}
			break;
		case levelplate2:
		case irreplacableplate2:
		case userplate2:
			if (o->Type != ObjTypes::Droid) {
				switch (o->Direction) { // Plate /
				case ObjDirection::North: if (!Block(o->x - 1, o->y)) o->Direction = ObjDirection::West; break;
				case ObjDirection::East: if (!Block(o->x, o->y + 1)) o->Direction = ObjDirection::South; break;
				case ObjDirection::South: if (!Block(o->x + 1, o->y)) o->Direction = ObjDirection::East; break;
				case ObjDirection::West: if (!Block(o->x, o->y - 1)) o->Direction = ObjDirection::North; break;
				}
			}
			break;
		case arrowup: o->Direction = ObjDirection::North; return;
		case arrowdown: o->Direction = ObjDirection::South; return;
		case arrowleft: o->Direction = ObjDirection::West; return;
		case arrowright: o->Direction = ObjDirection::East; return;
		case droidarrowup: if (o->Type == ObjTypes::Droid) o->Direction = ObjDirection::North; return;
		case droidarrowdown: if (o->Type == ObjTypes::Droid) o->Direction = ObjDirection::South; return;
		case droidarrowleft: if (o->Type == ObjTypes::Droid) o->Direction = ObjDirection::West; return;
		case droidarrowright: if (o->Type == ObjTypes::Droid) o->Direction = ObjDirection::East; return;

		case Exit: if (o->Type == ObjTypes::Ball && o->Col == BallColor::None)  Finished(o); break;
		case ExitGreen: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Green) Finished(o); else if (o->Col != BallColor::None) { SFX("Buzz"); Destroy(o); } break;
		case ExitRed: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Red) Finished(o); else if (o->Col != BallColor::None) { SFX("Buzz"); Destroy(o); } break;
		case ExitEmber: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Ember) Finished(o); else if (o->Col != BallColor::None) { SFX("Buzz"); Destroy(o); } break;
		case ExitBlue: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Blue) Finished(o); else if (o->Col != BallColor::None) { SFX("Buzz"); Destroy(o); } break;

		case LaserPlateBlue: _LaserPoint::_Activated[BallColor::Blue] = true; break;
		case LaserPlateEmber:_LaserPoint::_Activated[BallColor::Ember] = true; break;
		case LaserPlateGreen:_LaserPoint::_Activated[BallColor::Green] = true; break;
		case LaserPlateRed: _LaserPoint::_Activated[BallColor::Red] = true; break;
		}

		if ((!o->JustTransported) && (!PlayPuzzle->PuzR()->LayVal("TRANS", o->x, o->y))) {
			switch (o->Direction) {
			case ObjDirection::North:
				if (Break(o->x, o->y - 1) || Block(o->x, o->y - 1))
					o->Direction = ObjDirection::South;
				break;
			case ObjDirection::South:
				if (Break(o->x, o->y + 1) || Block(o->x, o->y + 1))
					o->Direction = ObjDirection::North;
				break;
			case ObjDirection::West:
				if (Break(o->x - 1, o->y) || Block(o->x - 1, o->y))
					o->Direction = ObjDirection::East;
				break;
			case ObjDirection::East:
				if (Break(o->x + 1, o->y) || Block(o->x + 1, o->y))
					o->Direction = ObjDirection::West;
				break;
			default: 
				Crash("Uknown direction! (B)");
				break;
			}
		}


	}

	void GirlMove(_GameObject* o) { 
		//Crash("Moving girls not yet supported"); 
	}

	void EndPuzzle() {
		bool success{ false };
		switch (PlayPuzzle->EMission()) {
		case Mission::Normal:
		case Mission::BreakFree:
		case Mission::ColorSplit:
			success = PlayPuzzle->BallsIn >= PlayPuzzle->Required();
			break;
		default:
			success = CountBalls() >= PlayPuzzle->Required();
			break;
		}
		if (success){
			if (TStage::Current != GameStages::Success) {
				auto u{ _User::Get() };
				SetGameStage(GameStages::Success);
				PlayPuzzle->Solved();
				PlayPuzzle->BestTime(PlayPuzzle->Time);
				PlayPuzzle->BestMoves(PlayPuzzle->Moves);
			}
		} else
			SetGameStage(GameStages::Fail);
	}

	bool SpotIsClear(int x, int y,bool fullclear=false) {
		auto PFV{ PlayPuzzle->PuzR() };
		auto DV{ PFV->LayVal("DIRECTIONS",x,y) };
		/*if*/ return !(
			PFV->Layers["WALL"]->Field->Value(x, y) ||
			PFV->Layers["BREAK"]->Field->Value(x, y) ||
			PFV->Layers["BREAK"]->Field->Value(x, y) ||
			PFV->Layers["TRANS"]->Field->Value(x, y) ||
			PFV->Layers["DEATH"]->Field->Value(x, y) ||
			(PFV->LayVal("DIRECTIONS", x, y) && fullclear) ||
			(DV != 0 && DV != userplate1 && DV != userplate2 && DV != levelplate1 && DV != levelplate2)
			); //return false;
		//return true;
	}

	void TileCheck(GameStages s) {
		int
			MX{ TQSE_MouseX() - PlayPuzzle->gPX() },
			MY{ TQSE_MouseY() - PlayPuzzle->gPY() },
			TX{ MX / PlayPuzzle->GridW() },
			TY{ MY / PlayPuzzle->GridH() };
		if (TX < 0 || TY < 0 || TX >= PlayPuzzle->W() || TY >= PlayPuzzle->H()) return;
		int
			r{ (int)(127 + (DegSin((double)SDL_GetTicks() / 5) * 127)) },
			g{ (int)(127 + (DegCos((double)SDL_GetTicks() / 5) * 127)) },
			b{ 0 };
		TQSG_Color(r, g, b);
		TQSG_Rect(TX * PlayPuzzle->GridW() + PlayPuzzle->gPX(), TY * PlayPuzzle->GridH() + PlayPuzzle->gPY(), PlayPuzzle->GridW(), PlayPuzzle->GridH(), true);
		if (s != GameStages::Game) return;
		// Tool Placements
		if (TQSE_MouseHit(1) && SpotIsClear(TX, TY) && GameTool::Tools[GameTool::Chosen].left) {
			GameTool::Tools[GameTool::Chosen].left--;
			PlayPuzzle->PuzR()->LayVal(GameTool::Tools[GameTool::Chosen].Layer,TX, TY, GameTool::Tools[GameTool::Chosen].PlaceTileID);
			PlayPuzzle->Moves++;
		}
	}

	void GameFlow(GameStages s){
		// Init Ticks
		if (!OTicks) OTicks = SDL_GetTicks();
		// Process Ticks
		PTicks = SDL_GetTicks();
		// Move when minimal amount of ticks have passed
		if (abs(PTicks - OTicks > OWaitTicks)) {
			_LaserPoint::ClearActivated();
			for (auto o : _GameObject::List) {
				if (o->Type!=Girl) o->Move();
				if (PlayPuzzle->PuzR()->LayVal("BOMBS", o->x, o->y)) {
					Bomb(o.get());
					PlayPuzzle->PuzR()->LayVal("BOMBS", o->x, o->y, 0);
				}
				DeathCheck(o.get());
			}
			OTicks = PTicks;
			//static BallColor lcols[4]{ BallColor::Red,BallColor::Green,BallColor::Ember,BallColor::Blue };
		}
		for (auto girl : _GameObject::Girls) girl->Move(); // Result of not really thinking the timer though.
		for (auto c : _LaserPoint::Register) {
			if (_LaserPoint::_Activated[c.first]) {
				for (auto l : *c.second) {
					l->Shoot();
				}
			}
		}
	
		/* DEBUG ONLY
		for (int y = 0; y < PlayPuzzle->H(); y++) for (int x = 0; x < PlayPuzzle->W(); x++) {
			TQSG_Color(255, 255, 255); Fnt->Draw(to_string(PlayPuzzle->PuzR()->Layers["WALL"]->Field->Value(x, y)), x * 40, y * 40);
			TQSG_Color(255, 180, 0); Fnt->Draw(to_string(PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y)), x * 40, (y * 40) + 20);
		}
		//*/
		// Has the goal been reached?
		if (CountBalls() <= 0) { EndPuzzle(); return; }
		// TODO: Missions with other objectives like Break-Away and Dot Collector
		// Time
		static auto ot{ CurrentTime() };
		auto ct{ CurrentTime() };
		//cout << "ot" << ot << "\tct" << ct << endl; // debug only
		if (ct != ot) {
			ot = ct;
			PlayPuzzle->Time++;
		}
	}
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
			PlayPuzzle->DrawLayer(dlay);
			if (strcmp(dlay, "WALL") == 0) _GameObject::DrawAll();
		}
		if (PlayPuzzle->Pack()->GlassLayer()) PlayPuzzle->DrawLayer("GLASS");
		TQSG_SetAlpha(255);
		TileCheck(TStage::Current);
		SClass::ShowSuits();
		GameTool::Draw();
		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->Title(),4,4);
		TQSG_Color(255, 255, 255);
		Fnt->Draw(PlayPuzzle->Title(), 2, 2);

		auto STime{ _User::Sec2Time(PlayPuzzle->Time) };
		TQSG_Color(0, 0, 0);
		Fnt->Draw(STime, (TQSG_ScreenWidth()/2)+2, 4,2);
		TQSG_Color(0, 180, 255);
		Fnt->Draw(STime, (TQSG_ScreenWidth() / 2), 2,2);

		if (PlayPuzzle->EMission() == Mission::BreakAway || PlayPuzzle->EMission() == Mission::BreakFree) {
			auto
				x{ (TQSG_ScreenWidth() / 4) * 3 },
				t{ CountBreakBlocks() };
			char show[300];
			switch (t) {
			case 0: show[0] = 0; break;
			case 1: strcpy_s(show, "1 tile"); break;
			default: sprintf_s(show, "%d tiles", t); break;
			}
			TQSG_Color(0, 0, 0);
			Fnt->Draw(show, x + 2, 4, 2);
			TQSG_Color(180, 0, 255);
			Fnt->Draw(show, x, 2, 2);
			if (t <= 0 && PlayPuzzle->EMission() == Mission::BreakAway) EndPuzzle();
		}

		if (PlayPuzzle->EMission() == Mission::DotCollector) {
			if (PlayPuzzle->EMission() == Mission::BreakAway || PlayPuzzle->EMission() == Mission::BreakFree) {
				auto
					x{ (TQSG_ScreenWidth() / 4)  },
					t{ CountDots() };
				char show[300];
				switch (t) {
				case 0: show[0] = 0; break;
				case 1: strcpy_s(show, "1 dots"); break;
				default: sprintf_s(show, "%d dots", t); break;
				}
				TQSG_Color(0, 0, 0);
				Fnt->Draw(show, x + 2, 4, 2);
				TQSG_Color(180, 255, 0);
				Fnt->Draw(show, x, 2, 2);
				if (t <= 0 && PlayPuzzle->EMission() == Mission::DotCollector) EndPuzzle();
			}
		}


		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth()-2, 4,1,0);
		TQSG_Color(180,255,0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth() - 4, 2, 1, 0);
		TStage::Stage[TStage::Current].Flow(TStage::Current);
		TStage::Stage[TStage::Current].HandleButtons();
		_Explosion::Run();
		_DeathAnim::DrawAll();
		Flip();
		return true;
	}
#pragma endregion
}