#include "GUIFrame.h"
#include "Terminal.h"

#include <wx/icon.h>
#include <wx/string.h>

#include <iostream>
#include <map>

#define numGUIrows					
#define numGUIcolumns
#define vgapGUI
#define hgapGUI

wxIMPLEMENT_APP(Terminal); //Macro initialization of terminal. 

GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, -1, title, pos, size), Input(1), Inter(1), Output(1)
{
	wxInitAllImageHandlers();//Starts up the image loading (May be done by default already?)
	CreateStatusBar(1); //little discription bar at the bottom

	SetIcon(wxIcon(wxICON(MAINICON))); //Adjust this in the rc file if you need to. VS's rc file, not wxwidgets
	SetBackgroundColour(wxColour(*wxWHITE));

	MenuSetup();

	GlobalGUISizer = new wxBoxSizer(wxHORIZONTAL); //Seems to be FCFS when aligning
	RobotSerialSetup();
	NetworkGridSetup();

	//Frame Defaults
	SetSizer(GlobalGUISizer);
	ResetNetworkToDefaults();
	Centre(); //puts the display window in the center of the screen 
}

GUIFrame::~GUIFrame()
{
	// tell all the threads to terminate: note that they can't terminate while
	// we're deleting them because they will block in their OnExit() -- this is
	// important as otherwise we might access invalid array elements

	{
		wxCriticalSectionLocker locker(wxGetApp().m_critsect);

		// check if we have any threads running first
		const wxArrayThread& threads = wxGetApp().m_threads;
		size_t count = threads.GetCount();

		if (!count)
			return;

		// set the flag indicating that all threads should exit
		wxGetApp().m_shuttingDown = true;
	}

	// now wait for them to really terminate
	wxGetApp().m_semAllDone.Wait();

	CloseHandle(serialComm);
}

/*
	GUI functions, attributed to some GUI element
	All functions are attatched in some way or perform some action for the main GUI elements
*/

void GUIFrame::ButtonSmushed(wxCommandEvent& event)
{
	std::cout << "test";
}