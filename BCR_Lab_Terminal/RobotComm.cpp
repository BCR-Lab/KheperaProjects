#include "GUIFrame.h"

#define RobotOutputWidth			450 //Height is based on frame size, so no need for default definition

void GUIFrame::RobotSerialSetup()
{
	//Robot Serial Prompt
	RobotSerialPromptSizer = new wxBoxSizer(wxVERTICAL);

	RobotOutput = new wxTextCtrl(this, ROBOT_OUTPUT,
		wxT(""), wxPoint(-1, -1), wxSize(RobotOutputWidth, -1),
		wxTE_MULTILINE | wxTE_RICH | wxTE_READONLY, wxDefaultValidator, wxTextCtrlNameStr);

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
		wxStreamToTextRedirector redirect(RobotOutput);
		std::cout << "Could not connect to Robot. Please check the serial line and/or ensure the robot is in the correct mode. \n";
	}

	else
	{
		DCB dcbSerialParams = { 0 };                        // Initializing DCB structure
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		serialStatus = GetCommState(serialComm, &dcbSerialParams);     //retreives  the current settings

		if (serialStatus == FALSE){
			wxStreamToTextRedirector redirect(RobotOutput);
			std::cout << ("\n   Error! in GetCommState()");
		}
		dcbSerialParams.BaudRate = CBR_38400;      // Setting BaudRate = 38400
		dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
		dcbSerialParams.StopBits = TWOSTOPBITS;    // Setting StopBits = 2
		dcbSerialParams.Parity = NOPARITY;      // Setting Parity = None 

		serialStatus = SetCommState(serialComm, &dcbSerialParams);  //Configuring the port according to settings in DCB 

		if (serialStatus == FALSE)//Note: serialStatus is just a boolean for checking, not anything to do with the serial port itself. Set functions return booleans
		{
			wxStreamToTextRedirector redirect(RobotOutput);
			std::cout << "\n   Error! in Setting DCB Structure";
		}

		serialStatus = SetCommMask(serialComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

		if (serialStatus == FALSE)
		{
			wxStreamToTextRedirector redirect(RobotOutput);
			std::cout << "\n   Error! in Setting DCB Structure";
		}

		COMMTIMEOUTS timeouts = { 0 };

		timeouts.ReadIntervalTimeout = 50;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 10;

		if (SetCommTimeouts(serialComm, &timeouts) == FALSE)
		{
			wxStreamToTextRedirector redirect(RobotOutput);
			std::cout << "\n   Error! in Setting Time Outs";
		}
	}

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
		1,
		wxALIGN_LEFT | wxEXPAND); //expand area to fit allocated space
}

void GUIFrame::SendCommand(wxCommandEvent& event)
{
	wxStreamToTextRedirector redirect(RobotOutput);

	std::string outputTemp = RobotInput->GetLineText(0);
	unsigned int strnLen = outputTemp.length();
	char *writeBuffer = new char[strnLen];

	for (unsigned int i = 0; i < strnLen; i++)
		writeBuffer[i] = outputTemp[i];
	RobotInput->Clear();

	char   lpBuffer[] = "run user-flash";		       // lpBuffer should be  char or byte array, otherwise write wil fail
	DWORD  dNoOFBytestoWrite;              // Num of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;          // Num of bytes written to the port

	dNoOFBytestoWrite = sizeof(lpBuffer); // Calculating the no of bytes to write into the port

	serialStatus = WriteFile(serialComm,               // Handle to the Serialport
		lpBuffer,            // Data to be written to the port 
		dNoOFBytestoWrite,   // Num of bytes to write into the port
		&dNoOfBytesWritten,  // Num of bytes written to the port
		NULL);

	if (serialStatus == TRUE)
	{
		std::cout << "\nWritten " << lpBuffer;
	}
	else
	{
		std::cout << "\n\n   Error %d in Writing to Serial Port", GetLastError();
	}



	/*
	DWORD dwEventMask;// Event mask to trigger
	serialStatus = WaitCommEvent(serialComm, &dwEventMask, NULL); //Wait for the character to be received

	char  TempChar;                        // Temperory Character
	char  SerialBuffer[256];               // Buffer Containing Rxed Data
	DWORD NoBytesRead;                     // Bytes read by ReadFile()
	int i = 0;
	if (serialStatus == FALSE)
	{
		std::cout <<"Error! in Setting WaitCommEvent()";
	}
	else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
	{
		std::cout << "\n\n  Characters Received";
		do
		{
			serialStatus = ReadFile(serialComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
			SerialBuffer[i] = TempChar;
			i++;
		} while (NoBytesRead > 0);
	}

	int j = 0;
	for (j = 0; j < i - 1; j++)		// j < i-1 to remove the dupliated last character
		std::cout << SerialBuffer[j];
		*/
}
