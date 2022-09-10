// Lic:
// BallPlay++
// Sinus
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

#include <QuickTypes.hpp>
#include <TQSG.hpp>
#include <jcr6_core.hpp>

#include <Sinus.hpp>
#include <Resource.hpp>
#include <Chain.hpp>

using namespace TrickyUnits;

namespace BallPlay {

	static byte
		sin_r{ 0 },
		sin_g{ 0 },
		sin_b{ 255 };
	static int
		sin_sw{ 0 },
		sin_sh{ 0 },
		sin_iw{ 0 },
		sin_ih{ 0 },
		sin_dw{ 0 };
	static TQSG_AutoImage sin_image[2]{ nullptr,nullptr };
	

	void Sinus() {
		if (!sin_sh) sin_sh = TQSG_ScreenHeight();
		Sinus(0, sin_sh);
	}

	void Sinus(int y, int h) {
		static byte
			r{ 0 },
			g{ 0 },
			b{ 0 };
		static uint32 offset{ 0 };
		if (!sin_sw) sin_sw = TQSG_ScreenWidth();
		if (!sin_image[0]) { sin_image[0] = TQSG_LoadAutoImage(Resource(), "GFX/Sinus/Sinus.png"); Assert(sin_image[0] != nullptr, JCR_Error); Assert(sin_image[0]->Frames() > 0, std::string("Sinus 0 Load Error\n")+SDL_GetError()); }
		if (!sin_image[1]) { sin_image[1] = TQSG_LoadAutoImage(Resource(), "GFX/Sinus/Sinus2.png"); Assert(sin_image[1] != nullptr, JCR_Error);  Assert(sin_image[1]->Frames() > 0, std::string("Sinus 1 Load Error\n")+SDL_GetError()); }
		if (!sin_iw) sin_iw = sin_image[0]->W();
		if (!sin_ih) sin_ih = sin_image[0]->H();
		if (!sin_dw) sin_dw = floor((sin_sw / 2) - (sin_iw / 2));
		if (r < sin_r) r++; else if (r > sin_r) r--;
		if (g < sin_g) g++; else if (g > sin_g) g--;
		if (b < sin_b) b++; else if (b > sin_b) b--;
		//printf("Sinus(%d,%d)  Color:%02x%02x%02x -> %02x%02x%02x   Tile(%d,%d,%d,%d)\n", y, h, r, g, b, sin_r, sin_g, sin_b, sin_dw, y, sin_iw, sin_sh-y);
		TQSG_Color(r, g, b);
		TQSG_SetBlend(TQSG_Blend::ADDITIVE);
		for(byte i=0;i<2;i++) sin_image[i]->Tile(sin_dw, y, sin_iw, sin_sh-y,0,y+offset);
		offset = (offset + 1) % sin_ih;
		TQSG_SetBlend(TQSG_Blend::ALPHA);
		TQSG_Color(255,255,255);
		
	}

}