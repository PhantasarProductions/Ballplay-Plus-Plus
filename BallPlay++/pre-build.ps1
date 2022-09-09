function private:builddate{
	$date = Get-Date
	$cpp = '
	
	#pragma once
	#define BPP_BuildDate "' + $date + '"
	' 
	
	echo $cpp > headers/builddate.h 
}

echo "Pre-build script - (c) Jeroen P. Broks 2022"

builddate