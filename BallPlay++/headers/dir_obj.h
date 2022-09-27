// Lic:
// BallPlay++
// Enums
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
#pragma once
#ifdef __cplusplus
namespace BallPlay {
	extern "C" {
#endif

		/// <summary>
		/// Directional types
		/// </summary>
		enum DirTypes {
			redplate1 = 0x20,
			userplate1 = 0x20,
			redplate2 = 0x21,
			userplate2 = 0x21,
			irreplacableplate1 = 0x22,
			irreplacableplate2 = 0x23,
			nobuild = 0x24,
			arrowwest = 0x25,
			arrowleft = 0x25,
			arroweast = 0x26,
			arrowright = 0x26,
			arrowdown = 0x27,
			arrowsouth = 0x27,
			arrowup = 0x28,
			arrownorth = 0x28,
			levelplate1 = 0x29, // Same as user plate, but in the puzzle by default, and user plate was placed by the player
			levelplate2 = 0x2a,
			droidarrowwest = 0x2b,
			droidarrowleft = 0x2b,
			droidarroweast = 0x2c,
			droidarrowright = 0x2c,
			droidarrowdown = 0x2d,
			droidarrowsouth = 0x2d,
			droidarrowup = 0x2e,
			droidarrownorth = 0x2e,
			dot = 0x31,
			normalexit = 0x70,
			greenexit = 0x71,
			redexit = 0x72,
			blueexit = 0x73,
			emberexit = 0x74,
			laser = 0xf0,
			lasertrigger = 0xf1,
			Exit = 0x70,
			ExitGreen = 0x71,
			GreenExit = 0x71,
			ExitRed = 0x72,
			RedExit = 0x72,
			BlueExit = 0x73,
			EmberExit = 0x74,
			ExitBlue = 0x73,
			ExitEmber = 0x74,

			GirlHome = 2, girlhome = 2
		};

		enum ObjTypes {
			Ball = 0x01,
			NormalBall = 0x01,
			BallNormal = 0x01,
			BallGreen = 0x02,
			GreenBall = 0x02,
			BallRed = 0x03,
			RedBall = 0x03,
			Ghost = 0x04, //0x12,
			Droid = 0x05, //0x11		
			Girl = 0x06
		};

#ifdef __cplusplus
	}
}
#endif