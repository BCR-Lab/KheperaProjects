#include "GUIFrame.h"

#define NeuronBoxSize				50 //It's width
#define WeightsColSize				60

void GUIFrame::NetworkGridSetup()
{
	//Grid Elements
	NetworkSizer = new wxBoxSizer(wxVERTICAL);
	NeuronSettingsSizer = new wxBoxSizer(wxHORIZONTAL);


	//Input Neurons Text and Box
	wxStaticText *InputText = new wxStaticText(this, wxID_ANY, "Inputs:");
	wxFont font = InputText->GetFont();
	font.SetPointSize(12);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	InputText->SetFont(font);

	NeuronSettingsSizer->Add(
		InputText,
		0,
		wxRIGHT | wxLEFT,
		5);


	wxIntegerValidator<unsigned int> ResizerValidator(&inNum, wxNUM_VAL_DEFAULT);
	ResizerValidator.SetMin(0); //unsigned int can't go below 0, yes, but someone might try anyway. Error avoidance

	InputsSet = new wxTextCtrl(this, NETWORK_Neuron_Input,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		InputsSet,
		0,
		wxRIGHT | wxLEFT,
		5);


	//InterNeurons Text and Box
	wxStaticText *InterText = new wxStaticText(this, wxID_ANY, "Inter:");
	InterText->SetFont(font);

	NeuronSettingsSizer->Add(
		InterText,
		0,
		wxRIGHT,
		5);

	InterSet = new wxTextCtrl(this, NETWORK_Neuron_Inter,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		InterSet,
		0,
		wxRIGHT | wxLEFT,
		5);


	//OutputNeurons Text and Box
	wxStaticText *OutputText = new wxStaticText(this, wxID_ANY, "Outputs:");
	OutputText->SetFont(font);

	NeuronSettingsSizer->Add(
		OutputText,
		0,
		wxRIGHT | wxLEFT,
		5);

	OutputsSet = new wxTextCtrl(this, NETWORK_Neuron_Output,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ResizerValidator, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		OutputsSet,
		0,
		wxRIGHT | wxLEFT,
		5);


	//Threshold Text and Box
	wxStaticText *ThresholdText = new wxStaticText(this, wxID_ANY, "Threshold:");
	ThresholdText->SetFont(font);

	wxFloatingPointValidator<double> ThresholdLimit(&inDouble, wxNUM_VAL_NO_TRAILING_ZEROES);

	NeuronSettingsSizer->Add(
		ThresholdText,
		0,
		wxRIGHT | wxLEFT,
		5);

	ThresholdSet = new wxTextCtrl(this, NETWORK_Neuron_Threshold,
		wxT(""), wxPoint(-1, -1), wxSize(NeuronBoxSize, -1),
		wxTE_RICH | wxTE_PROCESS_ENTER, ThresholdLimit, wxTextCtrlNameStr);

	NeuronSettingsSizer->Add(
		ThresholdSet,
		0,
		wxRIGHT | wxLEFT,
		5);


	//BCR logo
	wxStaticBitmap *logo = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxT("LOGOTWO"), wxBITMAP_TYPE_BMP_RESOURCE));
	NetworkSizer->Add( 	//Logo Picture
		logo,
		0,           // make horizontally unstretchable
		wxALIGN_RIGHT | wxALIGN_TOP);

	NetworkSizer->Add(
		NeuronSettingsSizer,
		0,
		wxALIGN_LEFT | wxBOTTOM);


	//Weights Grid
	wxStaticText *WeightsText = new wxStaticText(this, wxID_ANY, "Weights:");
	WeightsText->SetFont(font);

	NetworkSizer->Add(
		WeightsText,
		0,
		wxALL,
		5);

	Weights = new wxGrid(this, -1, wxPoint(0, 0), wxSize(-1, -1));
	wxGridCellFloatEditor *GridNumOnly = new wxGridCellFloatEditor(7, 6, wxGRID_FLOAT_FORMAT_FIXED);

	Weights->SetDefaultEditor(GridNumOnly); //UNFINISHED, validator breaks grid when using SetValue
	Weights->CreateGrid(1, 1); //A 3x3 matrix is the smallest dimensions of a network (1,1,1)
	Weights->SetColLabelValue(0, "1");

	Weights->DisableDragRowSize();
	Weights->SetColLabelSize(20);
	Weights->SetDefaultColSize(WeightsColSize);
	Weights->SetRowLabelSize(WeightsColSize);

	Weights->SetCellBackgroundColour(1, 1, *wxLIGHT_GREY);
	//Weights->SetCellValue(0, 0, "0.000000");

	WeightsSizer = new wxBoxSizer(wxVERTICAL);

	WeightsSizer->Add( //Separation from Network Sizer done from Weights to prevent stretching to full length downwards. Unseemly
		Weights,
		1,
		wxALL,
		5);

	NetworkSizer->Add(
		WeightsSizer,
		0,
		wxALL,
		5);

	GlobalGUISizer->Add(
		NetworkSizer,
		1,
		wxEXPAND);

	ResizeWeightsGrid(3, 3); //Done because after changing default col label, it's sizer treats it under it's old size. Better to initalize small and resize for now
}

void GUIFrame::InputEnter(wxCommandEvent& event)
{
	Input = std::stoi(std::string(InputsSet->GetLineText(0))); //I hate this. Two conversions. No specific ctrls. Validator keeps it an unsigned int, no check needed at least

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);

}

void GUIFrame::InterEnter(wxCommandEvent& event)
{
	Inter = std::stoi(std::string(InterSet->GetLineText(0))); 

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);
}

void GUIFrame::OutputEnter(wxCommandEvent& event)
{
	Output = std::stoi(std::string(OutputsSet->GetLineText(0))); 

	unsigned int combinedNeurons = Input + Inter + Output;
	ResizeWeightsGrid(combinedNeurons, combinedNeurons);
}

void GUIFrame::ThresholdEnter(wxCommandEvent& event)
{
	networkThreshold = std::stod(std::string(ThresholdSet->GetLineText(0)));
}



void GUIFrame::ResizeWeightsGrid(unsigned int rows, unsigned int cols)
{
	unsigned int numRows = Weights->GetNumberRows();
	if (numRows > rows)
		Weights->DeleteRows(rows, numRows - 1);
	else if (numRows < rows)
		Weights->AppendRows(rows - numRows);

	unsigned int numCols = Weights->GetNumberCols();
	if (numCols > cols)
		Weights->DeleteCols(cols, numCols - 1);
	else if (numCols < cols)
	{
		Weights->AppendCols(cols - numCols);
		for (unsigned int i = numCols; i <= cols; i++)
			Weights->SetColLabelValue(i-1, std::to_string(i));
	}

	NetworkSizer->Layout(); //Forces network sizer to reallocate space for the now increased WeightSizer which was expanded due to Resizing
	GlobalGUISizer->Layout(); //Forces the global sizer to reallocate space for the expanded NetworkSizer. Otherwise it would still be centered out
}


void GUIFrame::ResetNetworkToDefaults() //Function to be called to "clear" grid/outputp
{
	InputsSet->SetLabelText("1");
	OutputsSet->SetLabelText("1");
	InterSet->SetLabelText("1");
	ThresholdSet->SetLabelText("0");

	ResizeWeightsGrid(3, 3);
	Weights->ClearGrid();

}