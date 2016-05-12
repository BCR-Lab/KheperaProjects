/*
Author: Stephan Kritikos
Description: BCR GUI for serial communications with Khepera II robot. Written to allow user to quickly change network configuration without recompilation of Khepera II robot.
NOTE: No neural network code is included in this program. For an understanding of the implementation of Neural Networks, reference the Network class that is included in the Khepera II robot code.
Makes use of: wxWidgets v3.02, an open source GUI library and SerialPort, an open source Windows serial communication wrapper libraray
*/


#include "Terminal.h"
#include "GUIFrame.h"

wxBEGIN_EVENT_TABLE(GUIFrame, wxFrame)
	EVT_MENU(MENU_New, GUIFrame::NewNetwork)
	EVT_MENU(MENU_Open, GUIFrame::OpenNetwork)
	EVT_MENU(MENU_Save, GUIFrame::SaveNetwork)
	EVT_MENU(MENU_SaveAs, GUIFrame::SaveNetworkAs)
	EVT_MENU(MENU_Quit, GUIFrame::Quit)

	EVT_MENU(MENU_TerminalSettings, GUIFrame::TerminalSettings)
	EVT_MENU(MENU_SendS37, GUIFrame::SendS37)

	EVT_TEXT_ENTER(ROBOT_INPUT, GUIFrame::SendCommand)

	//EVT_COMMAND(THREAD_UPDATE, wxEVT_GUITHREAD_UPDATED, GUIFrame::OnCommThreadUpdate)
	//EVT_COMMAND(THREAD_COMPLETE, wxEVT_GUITHREAD_COMPLETED, GUIFrame::OnCommThreadCompletion)
	//EVT_THREAD(WORKER_EVENT, GUIFrame::OnWorkerEvent)
	//EVT_IDLE(GUIFrame::OnIdle)

	EVT_TEXT_ENTER(NETWORK_Neuron_Input, GUIFrame::InputEnter) //UNFINISHED: Give them all their respective enter functions
	EVT_TEXT_ENTER(NETWORK_Neuron_Inter, GUIFrame::InterEnter)
	EVT_TEXT_ENTER(NETWORK_Neuron_Output, GUIFrame::OutputEnter)
	EVT_TEXT_ENTER(NETWORK_Neuron_Threshold, GUIFrame::ThresholdEnter)

wxEND_EVENT_TABLE()

//NOTE: INITIALIZATION MOVED TO GUIFRAME.cpp, DECLARATION NEEDED THERE FOR DECONSTRUCTOR

bool Terminal::OnInit()
{
	GUIFrame *frame = new GUIFrame("Biometric and Cognitive Lab Khepera II Terminal", wxPoint(50, 50), wxSize(1200, 600));
	frame->Show(true);
	SetTopWindow(frame);// and finally, set it as the main window

	return true;
}
