/*
	Terminal.h
	============
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher
	
	Class declartion of the Terminal class
	This is the app declaration that wxWidgets will use to start the executable.
	Almost all changes to the GUI itself should be done in GUIFrame

	Last revised May 22, 2016
*/

#include <wx/wxprec.h>

#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/numdlg.h>
#include <wx/progdlg.h>

#ifndef WX_PRECOMP // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wx.h>
#endif

#pragma once

WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);
class Terminal : public wxApp //wxWidgets based classes should be derived from wxApp
{
public:
	Terminal();
	virtual ~Terminal(){};

	virtual bool OnInit();

	// critical section protects access to all of the fields below
	wxCriticalSection m_critsect;

	// all the threads currently alive - as soon as the thread terminates, it's
	// removed from the array
	wxArrayThread m_threads;

	// semaphore used to wait for the threads to exit, see SerialFrame::OnQuit()
	wxSemaphore m_semAllDone;

	// indicates that we're shutting down and all threads should exit
	bool m_shuttingDown;

};
