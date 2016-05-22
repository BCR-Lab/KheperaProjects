/*
	NeuralNetGrid.cpp
	=================
	BioMimetic and Cogntive Robotics Laboratory
	Stephan Kritikos, Student Researcher
	Functions related to the Neural Network editor are defined here

	Last revised May 22, 2016

*/

#include "GUIFrame.h"
#include <wx/statline.h>

#define NeuronBoxSize				50 //It's width
#define WeightsColSize				60
#define WeightsSettingsSpacing		6
#define WeightsTextSpacing			2

/*
	NetworkGridSetup
	=====================
	Creates and uses several distinct wxStaticBoxSizers to arrange the elements of the Neural Network editor
	NetworkSizer will arrange the elements along the vertical plane
	NeuronSettingsSizer will arrange the network dimension settings along the horizontal plane, which is then arranged along the vertical by NetworkSizer

	Refer to wxBoxSizer and wxStaticBoxSizer(which is just a BoxSizer with the line drawn around it) for more information
	Refer to wxTextCtrl and wxGrid for more information on the elements themselves
	Refer to the visual studio resources for the logo bmp
*/
void GUIFrame::NetworkGridSetup()
{
	//Grid Elements
	NetworkSizer = new wxBoxSizer(wxVERTICAL);
	NeuronSettingsSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Neural Network Settings");

	//BCR logo
	wxStaticBitmap *logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("LOGOTWO"), wxBITMAP_TYPE_BMP_RESOURCE));

	NetworkSizer->Add( 	//Logo Picture
		logo,
		1,           // make horizontally unstretchable
		wxALIGN_CENTER | wxALIGN_TOP | wxBOTTOM,
		5);


	//Input Neurons Text and Box
	wxStaticText *InputText = new wxStaticText(this, wxID_ANY, "Inputs:");
	wxFont font = InputText->GetFont();
	font.SetPointSize(10);
	InputText->SetFont(font);

	NeuronSettingsSizer->Add(
		InputText,
		0,
		wxRIGHT, //It's not alignment, it's where the border spacing should go. See wxAlign flags for alignment
		WeightsTextSpacing);


	wxIntegerValidator<unsigned int> ResizerValidator(&inNum, wxNUM_VAL_DEFAULT);
	ResizerValidator.SetMin(0); //unsigned int can't go below 0, yes, but someone might try anyway. Error avoidance

	InputsSet = new wxTextCtrl(this, NETWORK_Neuron_Input,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		InputsSet,
		0,
		wxRIGHT,
		WeightsSettingsSpacing);

	//InterNeurons Text and Box
	wxStaticText *InterText = new wxStaticText(this, wxID_ANY, "Inter:");
	InterText->SetFont(font);

	NeuronSettingsSizer->Add(
		InterText,
		0,
		wxRIGHT,
		WeightsTextSpacing);

	InterSet = new wxTextCtrl(this, NETWORK_Neuron_Inter,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		InterSet,
		0,
		wxRIGHT,
		WeightsSettingsSpacing);


	//OutputNeurons Text and Box
	wxStaticText *OutputText = new wxStaticText(this, wxID_ANY, "Outputs:");
	OutputText->SetFont(font);

	NeuronSettingsSizer->Add(
		OutputText,
		0,
		wxRIGHT,
		WeightsTextSpacing);

	OutputsSet = new wxTextCtrl(this, NETWORK_Neuron_Output,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		OutputsSet,
		0,
		wxRIGHT,
		WeightsSettingsSpacing);


	//Threshold Text and Box
	wxStaticText *ThresholdText = new wxStaticText(this, wxID_ANY, "Threshold:");
	ThresholdText->SetFont(font);

	wxFloatingPointValidator<double> ThresholdLimit(&inDouble, wxNUM_VAL_NO_TRAILING_ZEROES);

	NeuronSettingsSizer->Add(
		ThresholdText,
		0,
		wxRIGHT,
		WeightsTextSpacing);

	ThresholdSet = new wxTextCtrl(this, NETWORK_Neuron_Threshold,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ThresholdLimit, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		ThresholdSet,
		0,
		wxRIGHT,
		WeightsSettingsSpacing);


	NetworkSizer->Add(
		NeuronSettingsSizer,
		0,
		wxALIGN_LEFT | wxBOTTOM,
		5);

	//Weights Grid
	Weights = new wxGrid(this, -1, wxPoint(0, 0), wxSize(-1, -1));
	
	
	/*wxGridCellFloatEditor *GridNumOnly = new wxGridCellFloatEditor(7, 6, wxGRID_FLOAT_FORMAT_FIXED); //Setting the cell editor to this causes issues with setvalue. Need work around
	wxFloatingPointValidator<double> WeightsInputValidator(&inDouble, wxNUM_VAL_DEFAULT);
	GridNumOnly->SetValidator(WeightsInputValidator);
	Weights->SetDefaultEditor(GridNumOnly); //UNFINISHED, wxGridCellFloatEditor breaks grid when using SetValue
	*/

	wxGridCellFloatRenderer *GridDisplayNumOnly = new wxGridCellFloatRenderer(-1, 6, wxGRID_FLOAT_FORMAT_DEFAULT);
	Weights->SetDefaultRenderer(GridDisplayNumOnly);

	Weights->CreateGrid(1, 1); //A 3x3 matrix is the smallest dimensions of a network (1,1,1)
	

	Weights->DisableDragRowSize();
	Weights->SetColLabelSize(20);
	Weights->SetDefaultColSize(WeightsColSize);
	Weights->SetRowLabelSize(WeightsColSize);

	WeightsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Weights");

	WeightsSizer->Add( //Separation from Network Sizer done from Weights to prevent stretching to full length downwards without the size to match
		Weights,
		1);

	NetworkSizer->Add(
		WeightsSizer,
		0);

	GlobalGUISizer->Add(
		NetworkSizer,
		1,
		wxALIGN_LEFT);


	ResizeWeightsGrid(3, 3); //Done because after changing default col label, it's sizer treats it under it's old size. Better to initalize small and resize for now
}

/*
	InputEnter
	=====================
	Event function that is ran when a user presses enter when the InputsSet text control is focused.
	Refer to Terminal.cpp for macro definiton
*/
void GUIFrame::InputEnter(wxCommandEvent& event)
{
	Input = std::stoi(std::string(InputsSet->GetLineText(0))); //I hate this. Two conversions. No specific ctrls. Validator keeps it an unsigned int, no check needed at least

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);

}

/*
	InterEnter
	=====================
	Event function that is ran when a user presses enter when the InterSet text control is focused.
	Refer to Terminal.cpp for macro definiton
*/
void GUIFrame::InterEnter(wxCommandEvent& event)
{
	Inter = std::stoi(std::string(InterSet->GetLineText(0))); 

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);
}

/*
	OutputEnter
	=====================
	Event function that is ran when a user presses enter when the OutputsSet text control is focused.
	Refer to Terminal.cpp for macro definiton
*/
void GUIFrame::OutputEnter(wxCommandEvent& event)
{
	Output = std::stoi(std::string(OutputsSet->GetLineText(0))); 

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);
}

/*
	ThresholdEnter
	=====================
	Event function that is ran when a user presses enter when the ThresholdSet text control is focused.
	Refer to Terminal.cpp for macro definiton
*/
void GUIFrame::ThresholdEnter(wxCommandEvent& event)
{
	networkThreshold = std::stod(std::string(ThresholdSet->GetLineText(0)));
}


/*
	ResizeWeightsGrid
	=====================
	Resizes the Weights wxGrid by the rows and columns specified by deleting or adding more rows
	The labels on the rows and columns of the grid are rewritten entirely to match the new dimensions
	The sizers are alerted to a change in dimentional size so that they can reallocate space accordingly.
*/
void GUIFrame::ResizeWeightsGrid(unsigned int rows, unsigned int cols)
{

	unsigned int numRows = Weights->GetNumberRows();
	if (numRows > rows)
		Weights->DeleteRows(rows, numRows - 1);
	else if (numRows < rows)
		Weights->AppendRows(rows - numRows);

	for (unsigned int i = 1; i <= Input; i++)
	{
		Weights->SetRowLabelValue(i - 1, "Input " + std::to_string(i));
	}

	for (unsigned int i = 1; i <= Inter; i++)
	{
		Weights->SetRowLabelValue(Input + i - 1, "Inter " + std::to_string(i));
	}

	for (unsigned int i = 1; i <= Output; i++)
	{
		Weights->SetRowLabelValue(Input + Inter + i - 1, "Output " + std::to_string(i));
	}

	unsigned int numCols = Weights->GetNumberCols();
	if (numCols > cols)
		Weights->DeleteCols(cols, numCols - 1);
	else if (numCols < cols)
		Weights->AppendCols(cols - numCols);

	for (unsigned int i = 1; i <= Input; i++)
	{
		Weights->SetColLabelValue(i - 1, "Input " + std::to_string(i));
	}

	for (unsigned int i = 1; i <= Inter; i++)
	{
		Weights->SetColLabelValue(Input + i - 1, "Inter " + std::to_string(i));
	}

	for (unsigned int i = 1; i <= Output; i++)
	{
		Weights->SetColLabelValue(Input + Inter + i - 1, "Output " + std::to_string(i));
	}

	NetworkSizer->Layout(); //Forces network sizer to reallocate space for the now increased WeightSizer which was expanded due to Resizing
	GlobalGUISizer->Layout(); //Forces the global sizer to reallocate space for the expanded NetworkSizer. Otherwise it would still be centered out
}

/*
	ResetNetworkToDefaults
	=====================
	Resets the neural network grid to it's default settings
*/
void GUIFrame::ResetNetworkToDefaults() 
{
	InputsSet->SetLabelText("1");
	Input = 1;
	OutputsSet->SetLabelText("1");
	Output = 1;
	InterSet->SetLabelText("1");
	Inter = 1;
	ThresholdSet->SetLabelText("0");

	ResizeWeightsGrid(3, 3);
	Weights->ClearGrid();

}