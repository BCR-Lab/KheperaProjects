/*
	GUIFrame.h
	============
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher
	
	Class declartion of the GUIFrame and SerialThread (which is listed here due to it's heavy ties to GUIFrame and it's tiny size)
	Function impelenetation is spread by type into:
		GUIFrame.cpp (general functionality functions and those dependent on wxIMPLEMENT_APP(Terminal))
		Menu.cpp (functions related to the dropdown menu of the GUI)
		NeuralNetGrid.cpp (functions related to the right hand side neural network editor)
		CommGUI.cpp (functions related to the left hand side serial communication)

	Last revised May 22, 2016
*/

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

#include "resource1.h"

#ifndef WX_PRECOMP // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wx.h>
#endif
#pragma once


class SerialThread;

class GUIFrame : public wxFrame, private wxLog
{
public:
	GUIFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~GUIFrame();
	
	bool Cancelled();
private:

	//GUI Element Handles and Variables
	//=================================
	wxBoxSizer *GlobalGUISizer;

	wxBoxSizer *RobotSerialPromptSizer;
	wxTextCtrl *RobotOutput;
	wxTextCtrl *RobotInput;

	wxBoxSizer *NetworkSizer;
	wxStaticBoxSizer *NeuronSettingsSizer;
	wxStaticBoxSizer *WeightsSizer;

	wxMenuBar *MainMenu;
	wxMenu *m_pFileMenu;
	wxString CurrentDocPath;

	wxTextCtrl *InputsSet;
	wxTextCtrl *OutputsSet;
	wxTextCtrl *InterSet;
	wxTextCtrl *ThresholdSet;
	wxGrid *Weights;
	double gridNums;

	HANDLE serialComm; //receives a copy of the pointer as to not set up the same serial port twice, should two threads ever be introduced. I have not tested 2 threads
	BOOL   serialStatus;

	size_t m_nRunning, // remember the number of running threads and total number of threads
		m_nCount;

	wxLog *Logger;
	SerialThread *ThreadHandle;
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
	void SendWriteToThread(char *input);

	SerialThread *CreateThread(); // creates a new thread (but doesn't run it)
	void StopThread();
	void OnIdle(wxIdleEvent &event);

	void UpdateThreadStatus(); // update display in our status bar: called during idle handling

	void DoLogLine(wxTextCtrl *text, const wxString& timestr, const wxString& threadstr, const wxString& msg);
	void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info);
	//================================

	void ButtonSmushed(wxCommandEvent& event); //test button function. Not attatched to any button right now

protected:
	DECLARE_EVENT_TABLE()

};

enum
{
	ROBOT_OUTPUT = wxID_HIGHEST + 1,
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
	MENU_SendS37
};


class SerialThread : public wxThread
{
public:
	SerialThread(HANDLE serialComm, BOOL serialStatus);
	virtual ~SerialThread();

	virtual void *Entry();

private:
	HANDLE serialComm; //receives a copy of the pointer as to not set up the same serial port twice, should two threads ever be introduced. I have not tested 2 threads
	BOOL   serialStatus;
};
