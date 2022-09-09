function private:builddate{
	$date = Get-Date
	$cpp = '
	
	#pragma once
	#define BPP_BuildDate "' + $date + '"
	' 
	
	echo $cpp > headers/builddate.h 
}


function private:imp($want) {
	echo "Importing: $want"
	cp "$want" Build
}

echo "Pre-build script - (c) Jeroen P. Broks 2022"

builddate
../../BuildJCR/bin/Release/net6.0/BuildJCR.exe

imp "E:\projects\applications\visualstudio\vc\zlib-msvc14-x64.1.2.11.7795\build\native\bin_release\zlib.dll"
