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
// Version: 22.09.22
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

#pragma region Game_Objects
	enum class BallColor { None = Ball, Red = RedBall, Green = GreenBall };
	enum class ObjDirection { North, South, East, West };
	class _GameObject; typedef shared_ptr<_GameObject> GameObject;
	typedef void (*ObjectMove)(_GameObject*);
	void RegMove(_GameObject*); void GirlMove(_GameObject*);
	class _GameObject {
	private:
		static map<string,map<ObjTypes, TQSG_AutoImage>> ImgReg;
		static GameObject _NEW() { return make_shared<_GameObject>(); }
		ObjectMove _Move{ nullptr };
	public:
		static vector<GameObject> List;
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
		BallColor Col{ BallColor::None }; // Only used for exit scans
		ObjDirection Direction{ ObjDirection::South };
		TQSG_AutoImage Img() { return ImgReg[PlayPuzzle->PackName()][Type]; }

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
			}
			return ret;
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
							case GreenBall:
								NewBall(ix, iy, d, BallColor::Green);
								break;
							case RedBall:
								NewBall(ix, iy, d, BallColor::Red);
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
			for (auto o : List) if (!o->Removed) o->Draw();
		}
	};

	map<string,map<ObjTypes, TQSG_AutoImage>> _GameObject::ImgReg{};
	vector<GameObject> _GameObject::List{};

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
	bool Break(int x, int y) { 
		if (x < 0 || y < 0 || x >= PlayPuzzle->W() || y >= PlayPuzzle->H()) return true;
		auto ret{ PlayPuzzle->PuzR()->Layers["BREAK"]->Field->Value(x, y) > 0 };  
		if (ret) {
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
		_Explosion::NEW(o->x, o->y);
		Destroy(o);
	}

	void RegMove(_GameObject* o) {
		// Conflict prevention on slow CPUs
		o->modx = 0;
		o->mody = 0;
		if (o->Removed) return;
		// Where to go?
		switch (o->Direction) {
		case ObjDirection::North:
			if (Break(o->x, o->y - 1) || Block(o->x, o->y - 1))
				o->Direction = ObjDirection::South;
			else {
				o->y--;
				o->mody = PlayPuzzle->GridH();
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
		case ExitGreen: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Green) Finished(o); else if (o->Col != BallColor::None) Destroy(o); break;
		case ExitRed: if (o->Type == ObjTypes::Ball) if (o->Col == BallColor::Red) Finished(o); else if (o->Col != BallColor::None) Destroy(o); break;
		}

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

	void GirlMove(_GameObject* o) { Crash("Moving girls not yet supported"); }

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
		if (success) {
			auto u{ _User::Get() };
			SetGameStage(GameStages::Success);
			PlayPuzzle->Solved();
			PlayPuzzle->BestTime(PlayPuzzle->Time);
			PlayPuzzle->BestMoves(PlayPuzzle->Moves);
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
		if (TQSE_MouseHit(1) && SpotIsClear(TX, TY) && GameTool::Tools[GameTool::Chosen].left) {
			GameTool::Tools[GameTool::Chosen].left--;
			PlayPuzzle->PuzR()->LayVal("DIRECTIONS",TX, TY, GameTool::Tools[GameTool::Chosen].PlaceTileID);
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
			for (auto o : _GameObject::List) {
				o->Move();
				if (PlayPuzzle->PuzR()->LayVal("BOMBS", o->x, o->y)) {
					Bomb(o.get());
					PlayPuzzle->PuzR()->LayVal("BOMBS", o->x, o->y, 0);
				}
			}
			OTicks = PTicks;
		}
		_Explosion::Run();
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
			if (strcmp(dlay, "WALL") == 0) _GameObject::DrawAll();
			PlayPuzzle->DrawLayer(dlay);
		}
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


		TQSG_Color(0, 0, 0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth()-2, 4,1,0);
		TQSG_Color(180,255,0);
		Fnt->Draw(PlayPuzzle->MissionName(), TQSG_ScreenWidth() - 4, 2, 1, 0);
		TStage::Stage[TStage::Current].Flow(TStage::Current);
		TStage::Stage[TStage::Current].HandleButtons();
		Flip();
		return true;
	}
#pragma endregion
}