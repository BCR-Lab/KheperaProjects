/*
	Terminal.cpp
	===========
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher

	Last revised May 22, 2016

	BCR GUI for serial communications with Khepera II robot. Written to allow user to quickly change network configuration without recompilation of Khepera II robot.
	NOTE: No neural network code is included in this program. For an understanding of the implementation of Neural Networks, reference the Network class that is included in the Khepera II robot code.
	Makes use of: wxWidgets v3.02, an open source GUI library and the Windows serial communication libraray (NOT THE .NET LIBRARY)

	Contains all macro definitons that will be run to handle events in the GUI
*/


#include "Terminal.h"
#include "GUIFrame.h"

#define DefaultGUIWidth		900
#define DefaultGUIHeight	600

wxDECLARE_EVENT(wxEVT_COMMAND_SERIALTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_SERIALTHREAD_UPDATE, wxThreadEvent);

wxBEGIN_EVENT_TABLE(GUIFrame, wxFrame)
	EVT_MENU(MENU_New, GUIFrame::NewNetwork)
	EVT_MENU(MENU_Open, GUIFrame::OpenNetwork)
	EVT_MENU(MENU_Save, GUIFrame::SaveNetwork)
	EVT_MENU(MENU_SaveAs, GUIFrame::SaveNetworkAs)
	EVT_MENU(MENU_Quit, GUIFrame::Quit)

	EVT_MENU(MENU_TerminalSettings, GUIFrame::TerminalSettings)
	EVT_MENU(MENU_SendS37, GUIFrame::SendS37)

	EVT_TEXT_ENTER(ROBOT_INPUT, GUIFrame::SendCommand)

	EVT_TEXT_ENTER(NETWORK_Neuron_Input, GUIFrame::InputEnter) 
	EVT_TEXT_ENTER(NETWORK_Neuron_Inter, GUIFrame::InterEnter)
	EVT_TEXT_ENTER(NETWORK_Neuron_Output, GUIFrame::OutputEnter)
	EVT_TEXT_ENTER(NETWORK_Neuron_Threshold, GUIFrame::ThresholdEnter)

	EVT_IDLE(GUIFrame::OnIdle)

wxEND_EVENT_TABLE()

//NOTE: INITIALIZATION MOVED TO GUIFRAME.cpp, DECLARATION NEEDED THERE FOR DECONSTRUCTOR AND THREAD RELATED FUNCTIONS

/*
	Terminal Constructor
	=====================
	Sets the flag that alerts threads whether or not the program is closing to false
*/
Terminal::Terminal()
{
	m_shuttingDown = false;
}

/*
	OnInit
	=====================
	The true "main" function of the program. Note that main() is used in the wxWidgets library and initialization is done by macro definition
	Creates an instance of the GUIFrame class (everything related to the GUI)
*/
bool Terminal::OnInit()
{
	GUIFrame *frame = new GUIFrame("Biometric and Cognitive Lab Khepera II Terminal", wxPoint(50, 50), wxSize(DefaultGUIWidth, DefaultGUIHeight));
	frame->Show(true);
	SetTopWindow(frame);// and finally, set it as the main window

	return true;
}

