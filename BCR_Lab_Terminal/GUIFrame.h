#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_THREADS
#error "This program requires thread support!"
#endif // wxUSE_THREADS

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/valnum.h>

#include <wx/thread.h>
#include <wx/dynarray.h>
#include <wx/numdlg.h>
#include <wx/progdlg.h>

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <map>

#include "Resources.h"
#include "SerialThread.h"

#ifndef WX_PRECOMP // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wx.h>
#endif
#pragma once

class GUIFrame : public wxFrame
{
public:
	GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~GUIFrame();
private:

	//GUI Element Handles and Variables
	//=================================
	wxBoxSizer *GlobalGUISizer;

	wxBoxSizer *RobotSerialPromptSizer;
	wxTextCtrl *RobotOutput;
	wxTextCtrl *RobotInput;

	wxBoxSizer *NetworkSizer;
	wxBoxSizer *NeuronSettingsSizer;
	wxBoxSizer *WeightsSizer;

	wxMenuBar *MainMenu;
	wxMenu *m_pFileMenu;
	wxString CurrentDocPath;

	wxTextCtrl *InputsSet;
	wxTextCtrl *OutputsSet;
	wxTextCtrl *InterSet;
	wxTextCtrl *ThresholdSet;
	wxGrid *Weights;
	double gridNums;

	HANDLE serialComm;
	BOOL   serialStatus;

	size_t m_nRunning, // remember the number of running threads and total number of threads
		m_nCount;
	//=================================

	//Network storage
	//===============
	std::vector <double> networkWeights;
	//===============

	double inDouble;
	unsigned int inNum; //used for the validator.
	unsigned int Input, Inter, Output;
	double networkThreshold;
	
	//Menu Items
	//==========
	void NewNetwork(wxCommandEvent& event);
	void OpenNetwork(wxCommandEvent& event);
	void SaveNetwork(wxCommandEvent& event);
	void SaveNetworkAs(wxCommandEvent& event);
	void Quit(wxCommandEvent& event);

	void NetworkToTextFile();

	void TerminalSettings(wxCommandEvent& event);
	void SendS37(wxCommandEvent& event);
	//==========


	//Setup Functions
	//===============
	void MenuSetup();
	void RobotSerialSetup();
	void NetworkGridSetup();
	void ResetNetworkToDefaults();
	//===============

	//GUI Specific Functions
	//================
	void InputEnter(wxCommandEvent& event);
	void InterEnter(wxCommandEvent& event);
	void OutputEnter(wxCommandEvent& event);
	void ThresholdEnter(wxCommandEvent& event);

	void ResizeWeightsGrid(unsigned int rows, unsigned int cols);
	//=================

	//Serial Thread Related Functions
	//===============================
	void SendCommand(wxCommandEvent& event);

	void OnStartThread(wxCommandEvent& event);
	void OnStartThreads(wxCommandEvent& event);
	void OnStopThread(wxCommandEvent& event);
	void OnPauseThread(wxCommandEvent& event);
	void OnResumeThread(wxCommandEvent& event);

	void OnStartWorker(wxCommandEvent& event);
	void OnExecMain(wxCommandEvent& event);
	void OnStartGUIThread(wxCommandEvent& event);

	void OnIdle(wxIdleEvent &event);
	void OnWorkerEvent(wxThreadEvent& event);
	void OnUpdateWorker(wxUpdateUIEvent& event);

	// accessors for Worker thread (called in its context!)
	bool Cancelled();

	// helper function - creates a new thread (but doesn't run it)
	SerialThread *CreateThread();

	// update display in our status bar: called during idle handling
	void UpdateThreadStatus();


	//================================
	void ButtonSmushed(wxCommandEvent& event); //test function

	DECLARE_EVENT_TABLE()
};
enum
{
	ROBOT_OUTPUT = wxID_HIGHEST + 1, // declares an id for main text
	ROBOT_INPUT,
	NETWORK_Neuron_Input,
	NETWORK_Neuron_Inter,
	NETWORK_Neuron_Output,
	NETWORK_Neuron_Threshold,
	MENU_New,
	MENU_Open,
	MENU_Save,
	MENU_SaveAs,
	MENU_Quit,
	MENU_TerminalSettings,
	MENU_SendS37,
	
	THREAD_QUIT = wxID_EXIT,
	THREAD_ABOUT = wxID_ABOUT,
	THREAD_TEXT = 101,
	THREAD_CLEAR,
	THREAD_START_THREAD = 201,
	THREAD_START_THREADS,
	THREAD_STOP_THREAD,
	THREAD_PAUSE_THREAD,
	THREAD_RESUME_THREAD,

	THREAD_START_WORKER,
	THREAD_EXEC_MAIN,
	THREAD_START_GUI_THREAD,

	WORKER_EVENT = wxID_HIGHEST + 2,   // this one gets sent from MyWorkerThread
	GUITHREAD_EVENT                  // this one gets sent from MyGUIThread
};
