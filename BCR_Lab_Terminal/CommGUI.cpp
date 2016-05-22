/*
	CommGUI.cpp
	===========
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher
	Functions related to the serial communication output window (the two text controllers on the left hand side) are listed here

	Last revised May 22, 2016
	Notes:
	Any function that requires wxIMPLEMENT_APP(Terminal); to be present in the file(which cannot be defined twice without crashing) are in GUIFrame.cpp
*/


#include "GUIFrame.h"

#define RobotOutputWidth			450 //Only define the Width. Height is based on frame size, so no need for default definition
WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

/*
	RobotSerialSetup
	=====================
	Creates a vertical wxBoxSizer that will arrange elements along the vertical plane.
	RobotOutput will stretch based on the vertical dimension of the GUI, but not along the horizontal.
	RobotInput will not stretch at all.
	The RobotSerialPromptSizer has been flagged to expand along it's sizer axis (vertical), but not to expand it's width to account for extra space.
		This is done allow the Neural Network grid to be placed adjacent with no empty space.
	The GlobalGUISizer was setup to control the arrangement of GUI elements along the horizontal plane 

	The communication port is also opened using the flags and baud rate for mode 3 of the Khepera II robot
*/
void GUIFrame::RobotSerialSetup()
{
	//Robot Serial Prompt
	RobotSerialPromptSizer = new wxBoxSizer(wxVERTICAL);

	RobotOutput = new wxTextCtrl(this, ROBOT_OUTPUT,
		wxT(""), wxPoint(-1, -1), wxSize(RobotOutputWidth, -1),
		wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);

	RobotSerialPromptSizer->Add(
		RobotOutput,//adds in the edit box
		1,			// make vertically stretchable
		wxALL,
		5);			// set border width to 5

	RobotInput = new wxTextCtrl(this, ROBOT_INPUT,
		wxT(""), wxPoint(-1, -1), wxSize(RobotOutputWidth, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);

	RobotSerialPromptSizer->Add(
		RobotInput,//adds in the edit box
		0,			// Makes it not vertically stretchable. If both input and output where, wxWidgets would fill the space with both
		wxALL,
		5);			// set border width to 5

	GlobalGUISizer->Add(
		RobotSerialPromptSizer,
		0,
		wxALIGN_LEFT| wxEXPAND); //expand area to fit allocated space

	RobotInput->SetFocus();


	//Serial Port Setup
	//=================
	serialComm = CreateFileA("\\\\.\\COM1",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	//Should have COM1, 38400, 8 bit, no parity, 2 bit stop, no flow contrl
	/*serialComm = CreateFile(LPCWSTR("COM1"), //port name
	GENERIC_READ | GENERIC_WRITE, //Read/Write
	0,                            // No Sharing
	NULL,                         // No Security
	OPEN_EXISTING,// Open existing port only
	0,            // Non Overlapped I/O
	NULL);        // Null for Comm Devices
	*/
	if (serialComm == INVALID_HANDLE_VALUE)
	{
		wxLogMessage(wxT("Could not connect to Robot. Please check the serial line and/or ensure the robot is in the correct mode. \n"));
	}

	else
	{
		DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		serialStatus = GetCommState(serialComm, &dcbSerialParams);     //retreives  the current settings

		if (serialStatus == FALSE){
			wxLogMessage(wxT("\n   Error! in GetCommState()"));
		}
		dcbSerialParams.BaudRate = CBR_38400;      // Setting BaudRate = 38400
		dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
		dcbSerialParams.StopBits = TWOSTOPBITS;    // Setting StopBits = 2
		dcbSerialParams.Parity = NOPARITY;      // Setting Parity = None 

		serialStatus = SetCommState(serialComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

		if (serialStatus == FALSE)//Note: serialStatus is just a boolean for checking, not anything to do with the serial port itself. Set functions return booleans
		{
			wxLogMessage(wxT("\n   Error! in Setting DCB Structure"));
		}

		serialStatus = SetCommMask(serialComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

		if (serialStatus == FALSE)
		{
			wxLogMessage(wxT("\n   Error! in Setting DCB Structure"));
		}

		COMMTIMEOUTS timeouts = { 0 };

		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (SetCommTimeouts(serialComm, &timeouts) == FALSE)
		{
			wxLogMessage(wxT("\n   Error! in Setting Time Outs"));
		}
	}
	//=================

	ThreadHandle = CreateThread();
}

/*
	SendCommand
	=====================
	Event function (refer to Terminal.cpp for macro declaration) for sending some input across the serial connection
	Uses the standard Windows serial communication (NOT THE .NET SERIALPORT CLASS)
	The robot does not seem to respond to it as of 5/22/2016, even on a successful write
*/
void GUIFrame::SendCommand(wxCommandEvent& event)
{

	std::string outputTemp = RobotInput->GetLineText(0);
	unsigned int strnLen = outputTemp.length();
	char *writeBuffer = new char[strnLen];

	for (unsigned int i = 0; i < strnLen; i++)
		writeBuffer[i] = outputTemp[i];
	RobotInput->Clear();

	DWORD  dNoOFBytestoWrite;              // Num of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;          // Num of bytes written to the port

	dNoOFBytestoWrite = sizeof(writeBuffer); // Calculating the no of bytes to write into the port

	serialStatus = WriteFile(serialComm,               // Handle to the Serialport
		writeBuffer,            // Data to be written to the port 
		dNoOFBytestoWrite,   // Num of bytes to write into the port
		&dNoOfBytesWritten,  // Num of bytes written to the port
		NULL);

	if (serialStatus == TRUE)
	{
		RobotOutput->AppendText("\nWritten " + std::string(writeBuffer)); //May wish to change the formatting of this to ignore or just the command
	}
	else
	{
		wxLogMessage(wxT("\n\n   Error %d in Writing to Serial Port" + GetLastError()));
	}

}