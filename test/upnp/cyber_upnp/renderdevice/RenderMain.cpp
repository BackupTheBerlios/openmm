/******************************************************************
*
*
*	File: RenderMain.cpp
*
*	Revision;
*
*	2008-03-26
*		- first revision
*
******************************************************************/


#include "RenderDevice.h"

#include <string>
#include <iostream>

using namespace std;
using namespace CyberLink;
using namespace CyberUtil;

RenderDevice *renderDev;

////////////////////////////////////////////////////////// 
//  InitApp
////////////////////////////////////////////////////////// 

void InitApp()
{
    try {
	renderDev = new RenderDevice();
    }
    catch (InvalidDescriptionException e){
        const char *errMsg = e.getMessage();
        cout << "InvalidDescriptionException = " << errMsg << endl;
    }
	renderDev->start();
}

void ExitApp()
{
	renderDev->stop();
	delete renderDev;
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	InitApp();
    
//	gtk_main ();
	int ch = 0;
	do
	{
		ch = getchar();
		ch = toupper( ch );
// 		Wait(1000);
	} while( ch != 'Q');
	
	ExitApp();
	
	return(0);
}
