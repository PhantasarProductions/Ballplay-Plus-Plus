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
#include <MainMenu.hpp>
#include <Chain.hpp>



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

#pragma region Kaartkleuren
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
#pragma endregion


#pragma region Game Buttons and stages

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



#pragma region Game Flow
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
		TStage::Stage[TStage::Current].HandleButtons();
		Flip();
		return true;
	}
#pragma endregion
}