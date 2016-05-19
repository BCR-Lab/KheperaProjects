#include "GUIFrame.h"

#include <string>

#include <wx/textfile.h>
#include <wx/choicdlg.h>
#include <wx/wfstream.h>

void GUIFrame::MenuSetup()
{
	CurrentDocPath = "nofile"; //Just a default setup since I can't seem to just check a wxString for NULL

	//Menu Items
	//==========
	MainMenu = new wxMenuBar();
	wxMenu *FileMenu = new wxMenu();

	FileMenu->Append(MENU_New, wxT("&New"),
		wxT("Create a new network"));
	FileMenu->Append(MENU_Open, wxT("&Open"),
		wxT("Open an existing network text file"));
	FileMenu->Append(MENU_Save, wxT("&Save"),
		wxT("Save the current network to a text file"));
	FileMenu->Append(MENU_SaveAs, wxT("Save &As"),
		wxT("Save the current network under a new file name"));
	FileMenu->Append(MENU_Quit, wxT("&Quit"),
		wxT("Quit the BCR Lab Terminal"));

	MainMenu->Append(FileMenu, wxT("File"));

	wxMenu *Terminal = new wxMenu();

	Terminal->Append(MENU_TerminalSettings, wxT("&Baud Rate"),
		wxT("Change the current baud rate for Robot Communication"));

	MainMenu->Append(Terminal, wxT("Terminal"));

	SetMenuBar(MainMenu);
	//==========

}

//File Menu
void GUIFrame::NewNetwork(wxCommandEvent& WXUNUSED(event))
{
	wxMessageDialog *ClearPrompt = new wxMessageDialog(NULL,
		wxT("This will reset your entire network back to the default settings. If you have unsaved work, it will be lost.\nContinue?"), wxT("Question"),
		wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
	if (ClearPrompt->ShowModal() == wxID_YES)
	{
		ResetNetworkToDefaults();
	}

}

void GUIFrame::OpenNetwork(wxCommandEvent& WXUNUSED(event)) //Unfinished: Validation issues could not be resolved.
{
	wxFileDialog *OpenDialog = new wxFileDialog(
		this, _("Choose a text file containing your network."), wxEmptyString, wxEmptyString,
		_("Text File (*.txt)|*.txt"),
		wxFD_OPEN, wxDefaultPosition);

	if (OpenDialog->ShowModal() == wxID_OK)
	{
		CurrentDocPath = OpenDialog->GetPath(); // Sets our current document to the file the user selected
		wxTextFile tfile;
		tfile.Open(CurrentDocPath);

		wxString fileLineStr;
		fileLineStr = tfile.GetFirstLine();
		while (!tfile.Eof()) //Until the end of the file
		{
			
			//If statement list could use replacement. Function pointer map too clunky/marginally less wasteful
			if (fileLineStr.find("numberOfInputs") != wxNOT_FOUND)
			{
				try
				{
					wxString inputStr = fileLineStr.Mid(fileLineStr.find(" "));
					Input = std::stoi(std::string(inputStr));
					InputsSet->SetLabelText(inputStr);
				}
				catch (const std::invalid_argument& ia)
				{
					wxStreamToTextRedirector redirect(RobotOutput);
					std::cout << "Invalid numberOfInputs setup: " << ia.what() << "\n"; //test output
				}
			}
			else if (fileLineStr.find("numberOfOutputs") != wxNOT_FOUND)
			{
				try
				{
					wxString OutputStr = fileLineStr.Mid(fileLineStr.find(" "));
					Output = std::stoi(std::string(OutputStr));
					OutputsSet->SetLabelText(OutputStr);
				}
				catch (const std::invalid_argument& ia)
				{
					wxStreamToTextRedirector redirect(RobotOutput);
					std::cout << "Invalid numberOfOutputs setup: " << ia.what() << "\n"; //test output
				}
			}
			else if (fileLineStr.find("numberOfInterNeurons") != wxNOT_FOUND)
			{
				try
				{
					wxString InterStr = fileLineStr.Mid(fileLineStr.find(" "));
					Inter = std::stoi(std::string(InterStr));
					InterSet->SetLabelText(InterStr);
				}
				catch (const std::invalid_argument& ia)
				{
					wxStreamToTextRedirector redirect(RobotOutput);
					std::cout << "Invalid numberOfInterNeurons setup: " << ia.what() << "\n"; //test output
				}

			}
			else if (fileLineStr.find("networkThresholds") != wxNOT_FOUND)
			{
				fileLineStr = tfile.GetNextLine();
				wxString remainingString = fileLineStr.Mid(fileLineStr.find(" ")+1);
				
				try
				{
					networkThreshold = std::stod(std::string(remainingString.SubString(0, remainingString.find(" "))));
				}
				catch (const std::invalid_argument& ia)
				{
					wxStreamToTextRedirector redirect(RobotOutput);
					std::cout << "Invalid networkThresholds setup: " << ia.what() <<"\n"; //test output
				}
			}
			else if (fileLineStr.find("networkweights") != wxNOT_FOUND)
			{
				unsigned int combinedNeurons = Input + Inter + Output; //Should have values by the time this comes around
				ResizeWeightsGrid(combinedNeurons, combinedNeurons);

				fileLineStr = tfile.GetNextLine();
				networkWeights.clear();

				wxString remainingString;
				int row = 0;
				int col;

				while (fileLineStr.find("networkinputs") == wxNOT_FOUND)
				{
					remainingString = fileLineStr;
					col = 0;

					while (!remainingString.empty())
					{
						try
						{
							//Weights->SetValueAsDouble();
							//Weights->SetCellValue(row, col++, remainingString.SubString(0, remainingString.find(" ")) )
							Weights->SetCellValue(row, col++, std::string(remainingString.SubString(0, remainingString.find(" "))) );
							//Weights->SaveEditControlValue();
							networkWeights.push_back(std::stod(std::string(remainingString.SubString(0, remainingString.find(" ")))));
						}
						catch (const std::invalid_argument& ia)
						{
							wxStreamToTextRedirector redirect(RobotOutput);
							std::cout << "Invalid networkweights setup: " << ia.what() << "\n"; //test output
							break;
						}

						remainingString = remainingString.Mid(remainingString.find(" ") + 1);
					}
					fileLineStr = tfile.GetNextLine();
					row++;
				}

			}

			fileLineStr = tfile.GetNextLine();
		}

		SetTitle(this->GetTitle() << wxString(" - Edit - ") << OpenDialog->GetFilename()); // Set the Title to reflect the file open
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}

void GUIFrame::SaveNetwork(wxCommandEvent& WXUNUSED(event))
{
	wxTextFile tfile;
	if (CurrentDocPath == "nofile")
	{
		wxFileDialog *saveFileDialog = new wxFileDialog(
			this, _("Save Neural Network As"), wxEmptyString, wxEmptyString,
			_("Text File (*.txt)|*.txt"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

		if (saveFileDialog->ShowModal() == wxID_OK)
		{
			CurrentDocPath = saveFileDialog->GetPath(); // Sets our current document to the file the user selected
			tfile.Create(CurrentDocPath);
		}

		saveFileDialog->Destroy();
	}

	//SetTitle(wxString("Edit - ") << CurrentDocPath);
	
	NetworkToTextFile();
}

void GUIFrame::SaveNetworkAs(wxCommandEvent& WXUNUSED(event))//UNFINISHED
{
	wxTextFile tfile;
	wxFileDialog *saveFileDialog = new wxFileDialog(
		this, _("Save Neural Network As"), wxEmptyString, wxEmptyString,
		_("Text File (*.txt)|*.txt"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	if (saveFileDialog->ShowModal() == wxID_OK)
	{
		CurrentDocPath = saveFileDialog->GetPath(); // Sets our current document to the file the user selected
		tfile.Create(CurrentDocPath);
	}

	//SetTitle(wxString("Edit - ") << saveFileDialog->GetFilename());
	saveFileDialog->Destroy();
	
	NetworkToTextFile();
}

void GUIFrame::NetworkToTextFile()
{
	wxTextFile tfile;
	tfile.Open(CurrentDocPath);
	if (tfile.IsOpened()) //FIXED to match standard network format
	{
		tfile.Clear();

		std::string input = "numberOfInputs " + std::to_string(Input);
		tfile.AddLine(input);

		input = "numberOfOutputs " + std::to_string(Output);
		tfile.AddLine(input);

		input = "numberOfInterNeurons " + std::to_string(Inter);
		tfile.AddLine(input);

		unsigned int networkDimension = Inter + Input + Output;
		input = "networkDimension " + std::to_string(networkDimension);
		tfile.AddLine(input);

		input = "networkActivations";
		tfile.AddLine(input);
		input = "";
		for (unsigned int i = 0; i < networkDimension - 1; i++)
		{
			input += "0.000000 ";
		}
		input += "0.000000"; //just to keep off that extra spacing
		tfile.AddLine(input);

		input = "networkOutputs";
		tfile.AddLine(input);
		input = "";
		for (unsigned int i = 0; i < networkDimension - 1; i++)
		{
			input += "0.000000 ";
		}
		input += "0.000000"; //just to keep off that extra spacing
		tfile.AddLine(input);

		input = "networkThresholds";
		tfile.AddLine(input);
		input = "";
		for (unsigned int i = 0; i < networkDimension - 1; i++)
		{
			input += "0.000000 ";
		}
		input += "0.000000"; //just to keep off that extra spacing
		tfile.AddLine(input);

		input = "networkweights";
		tfile.AddLine(input);
		for (unsigned int j = 0; j < networkDimension; j++)
		{
			input = "";
			for (unsigned int i = 0; i < networkDimension - 1; i++)
			{
				input += Weights->GetCellValue(i, j) + " ";
			}
			input += Weights->GetCellValue(networkDimension - 1, j); //just to keep off that extra spacing
			tfile.AddLine(input);
		}

		input = "";
		tfile.AddLine(input);

		input = "networkinputs";
		tfile.AddLine(input);
		input = "";
		for (unsigned int i = 0; i < Input - 1; i++)
		{
			input += "0.000000 ";
		}
		input += "0.000000"; //just to keep off that extra spacing
		tfile.AddLine(input);

		input = "networkoutputs";
		tfile.AddLine(input);
		input = "";
		for (unsigned int i = 0; i < Output - 1; i++)
		{
			input += "0.000000 ";
		}
		input += "0.000000"; //just to keep off that extra spacing
		tfile.AddLine(input);

		input = "networkplasticweightsmask";
		tfile.AddLine(input);
		for (unsigned int j = 0; j < networkDimension; j++)
		{
			input = "";
			for (unsigned int i = 0; i < networkDimension - 1; i++)
			{
				input += input += "0.000000 ";
			}
			input += "0.000000 "; //just to keep off that extra spacing
			tfile.AddLine(input);
		}

		tfile.Write();
		tfile.Close();
	}
}

void GUIFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE); // Tells the OS to quit running this process
}

void GUIFrame::SendS37(wxCommandEvent& event) //Massively unfinished, just opens a dialog
{
	wxFileDialog *OpenDialog = new wxFileDialog(
		this, _("Choose a file to send to the Robot."), wxEmptyString, wxEmptyString,
		_("SREC (*.s37)|*.s37"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		CurrentDocPath = OpenDialog->GetPath();
		// Sets our current document to the file the user selected
		RobotOutput->LoadFile(CurrentDocPath); //Opens that file
		SetTitle(wxString("Edit - ") <<
			OpenDialog->GetFilename()); // Set the Title to reflect the file open
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}

//Terminal Menu
void GUIFrame::TerminalSettings(wxCommandEvent& WXUNUSED(event))
{

}
