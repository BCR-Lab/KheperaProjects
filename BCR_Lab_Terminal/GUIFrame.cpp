/*
	GUIFrame.cpp
	============
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher
	Functions related to GUI initialization and the SerialThread are listed here. 

	Last revised May 22, 2016
	Notes:
	Any function that requires wxIMPLEMENT_APP(Terminal); to be present in the file(which cannot be defined twice without crashing) are also here.
*/


#include "GUIFrame.h"
#include "Terminal.h"

#include <wx/icon.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/numdlg.h>
#include <wx/progdlg.h>

#include <iostream>
#include <map>

#define GUIBackgroundColor				wxColour(200, 200, 200) //RGB

wxIMPLEMENT_APP(Terminal); //Macro initialization of terminal. 

/*
	GUI Frame Constructor
	=====================
	Requires a wxString for the window name, a default position and a default size for the GUI
	Establishes the frame as one that will read from the wxWidgets logger tool
	Art used for the library is set up in the visual studio resources file.
	The global Sizer (refer to wxWidgets wxSizers) which will arrange the GUI elements is intilized here.
	GUI sections are devided by type and created using specific functions related to those types.
	See:	Menu.cpp for MenuSetup()
			CommGUI.cpp for RobotSerialSetup()
			NeuralNetGrid.cpp for NetworkGridSetup()
*/
GUIFrame::GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, -1, title, pos, size), Input(1), Inter(1), Output(1)
{
	Logger = wxLog::GetActiveTarget();

	wxLog::SetActiveTarget(this);

	wxInitAllImageHandlers();//Starts up the image loading (May be done by default already?)
	CreateStatusBar(1); //little discription bar at the bottom

	SetIcon(wxIcon(wxICON(MAINICON))); //Adjust this in the rc file if you need to. VS's rc file, not wxwidgets
	SetBackgroundColour(GUIBackgroundColor);

	MenuSetup();

	GlobalGUISizer = new wxBoxSizer(wxHORIZONTAL); //Seems to be FCFS when aligning
	RobotSerialSetup();
	NetworkGridSetup();

	//Frame Defaults
	SetSizer(GlobalGUISizer);
	ResetNetworkToDefaults();
	Centre(); //puts the display window in the center of the screen 
}


/*
	GUI Frame Deconstructor
	=====================
	Removes the frame as the active target for the wxWidgets logger functions (by destroying the Logger handle)
	Ends any threads tied to running GUIFrame
	Closes the serial port connection
*/
GUIFrame::~GUIFrame()
{
	wxLog::SetActiveTarget(Logger);
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
	ButtonSmushed
	=====================
	Test function for button interaction. Has no button tied to it currently, but left in as a template.
*/

void GUIFrame::ButtonSmushed(wxCommandEvent& event)
{
	RobotOutput->AppendText( "test");
}


/*
THREAD RELATED FUNCTIONS
========================
*/

/*
	SerialThread Constructor
	=====================
	Constructor that takes a handle(pointer) to a serial connection.
	Also takes in the boolean value that the GUI used during setup (current status of the port essentially)
	NOTE: Constructor for a wxThread related class cannot be left to the compiler. Even if it is emptied, keep the empty function
*/
SerialThread::SerialThread(HANDLE portHandle, BOOL portStatus) : wxThread()
{
	serialComm = portHandle;
	serialStatus = portStatus; 
}

/*
	SerialThread Deconstructor
	=====================
	Declares to other threads that it intends to delete itself
	NOTE: Deconstructor for a wxThread related class cannot be left to the compiler. Even if it is emptied, keep the empty function
*/
SerialThread::~SerialThread()
{
	wxCriticalSectionLocker locker(wxGetApp().m_critsect);

	wxArrayThread& threads = wxGetApp().m_threads;
	threads.Remove(this);

	if (threads.IsEmpty())
	{
		// signal the main thread that there are no more threads left if it is
		// waiting for us
		if (wxGetApp().m_shuttingDown)
		{
			wxGetApp().m_shuttingDown = false;

			wxGetApp().m_semAllDone.Post();
		}
	}

}

/*
	CreateThread
	=====================
	Helper function for the GUIFrame that creates a new instance of SerialThread and returns a pointer to that thread
*/
SerialThread *GUIFrame::CreateThread()
{
	SerialThread *thread = new SerialThread(serialComm, serialStatus);

	if (thread->Create() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Can't create thread!"));
	}

	wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	wxGetApp().m_threads.Add(thread);

	if (thread->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Can't start thread!"));
	}


	return thread;
}

/*
	StopThread
	=====================
	Stops the last thread to be intialized and deletes it.
	NOTE: This function is not currently in use, but left implemented if a future person needs it as of 5/22/2016
*/
void GUIFrame::StopThread()
{
	wxThread* toDelete = NULL;
	{
		wxCriticalSectionLocker enter(wxGetApp().m_critsect);

		// stop the last thread
		if (wxGetApp().m_threads.IsEmpty())
		{
			wxLogError(wxT("No thread to stop!"));
		}
		else
		{
			toDelete = wxGetApp().m_threads.Last();
		}
	}

	if (toDelete)
	{
		// This can still crash if the thread gets to delete itself
		// in the mean time.
		toDelete->Delete();
	}
}

/*
	UpdateThreadStatus
	=====================
	Helper function for updating the amount of threads running when the GUI is Idle.
	NOTE: This function is used, but the values it writes are not currently important, but left implemented if a future person needs it as of 5/22/2016
*/
void GUIFrame::UpdateThreadStatus()
{
	wxCriticalSectionLocker enter(wxGetApp().m_critsect);

	// update the counts of running/total threads
	size_t nRunning = 0,
		nCount = wxGetApp().m_threads.Count();
	for (size_t n = 0; n < nCount; n++)
	{
		if (wxGetApp().m_threads[n]->IsRunning())
			nRunning++;
	}

	if (nCount != m_nCount || nRunning != m_nRunning)
	{
		m_nRunning = nRunning;
		m_nCount = nCount;

		//wxLogStatus(this, wxT("%u threads total, %u running."), unsigned(nCount), unsigned(nRunning));
	}
	//else: avoid flicker - don't print anything
}

/*
	OnIdle
	=====================
	Event function for updating the amount of threads running when the GUI is Idle. Refer to the macro definition of the idle event in Terminal.cpp
	NOTE: This function is used, but the values it writes are not currently important, but left implemented if a future person needs it as of 5/22/2016
*/
void GUIFrame::OnIdle(wxIdleEvent& event)
{
	UpdateThreadStatus();

	event.Skip();
}

/*
	DoLogLine and DoLogRecord
	=====================
	Implementation of logging functions as part of the structure from wxLogger
	Nothing was changed from the example, mess with this at your own discretion
*/
void GUIFrame::DoLogLine(wxTextCtrl *text, const wxString& timestr, const wxString& threadstr, const wxString& msg)
{
	text->AppendText(wxString::Format("%9s %10s %s", timestr, threadstr, msg));
}

void GUIFrame::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
{
	// let the default GUI logger treat warnings and errors as they should be
	// more noticeable than just another line in the log window and also trace
	// messages as there may be too many of them
	if (level <= wxLOG_Warning || level == wxLOG_Trace)
	{
		Logger->LogRecord(level, msg, info);
		return;
	}

	DoLogLine(
		RobotOutput, wxDateTime(info.timestamp).FormatISOTime(), 
		info.threadId == wxThread::GetMainId() ? wxString("main") : wxString::Format("%lx", info.threadId),
	 	msg + "\n"
	);
}


/*
Serial Write Function
=====================
*/

/*
	Entry
	=====================
	Function that will be run during a thread's lifetime. This is the "main" function of a thread.
	It's main purpose for serial communication is to run the WaitCommEvent function. If ran in the main thread, the program would stall
	NOTE: I was never able to test this. Multithreading setup was done at the end of the semester before finals. Done so whoever you are could
		try and get the Serial connection up and running without wasting time implementing the multithreading.
		The multithreading works fine, the serial read might not. Or it may still stall the program.
*/
wxThread::ExitCode SerialThread::Entry()
{
	//wxLogMessage("Thread started (priority = %u).", GetPriority());

	while (true)
	{
		// check if the application is shutting down: in this case all threads
		// should stop a.s.a.p.
		{
			wxCriticalSectionLocker locker(wxGetApp().m_critsect);
			if (wxGetApp().m_shuttingDown)
				return NULL;
		}

		if (TestDestroy()) 	// check if just this thread was asked to exit
			break;

		//wxLogMessage("Thread running");

		DWORD dwEventMask;// Event mask to trigger
		serialStatus = WaitCommEvent(serialComm, &dwEventMask, NULL); //Wait for the character to be received

		char  TempChar;                        // Temperory Character
		char  SerialBuffer[256];               // Buffer Containing Rxed Data
		DWORD NoBytesRead;                     // Bytes read by ReadFile()
		int i = 0;
		if (serialStatus == FALSE)
		{
			wxLogMessage(wxT("Error! in Setting WaitCommEvent()"));
			break;
		}
		else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
		{
			wxLogMessage("\n\n  Characters Received");
			do
			{
				serialStatus = ReadFile(serialComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
				SerialBuffer[i] = TempChar;
				i++;
			} while (NoBytesRead > 0);

			wxLogMessage(SerialBuffer);
		}



		// wxSleep() can't be called from non-GUI thread! Note: wxThread::Sleep will get the job done the same
		wxThread::Sleep(10); //Your milage may vary on adjusting this. Don't want to end up with a freezing program
	}

	wxLogMessage("Thread finished.");

	return NULL;
}