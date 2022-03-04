#include "TMVA/DataLoader.h"
#include "TMVA/Factory.h"

#include "TFile.h"
#include "TString.h"
#include "TTree.h"


void train()
{
	//--Option specification--//
	const char* out_file_name = "training_out.root";
	const char* training_variables_filename = "training_variables.list";
	const char* sgnl_tree_filename = "sgnl.root";
	const char* bkgd_tree_filename = "bkgd.root";

	const char* weights_filename = "weights";
	const char* data_loader_filename = "dataset";

	TString factory_options = "V:!Silent:Color:Transformations=I:DrawProgressBar:AnalysisType=Classification";
	TString dataset_options = "SplitMode=Random";
	TString method_options = "";

	TCut sgnl_cut = "";
	TCut bkgd_cut = "";

	//--Training starts here--//
	TMVA::Tools::Instance();
	
	TFile* out_file = TFile::Open(out_file_name, "RECREATE");
	TMVA::Factory *factory = new TMVA::Factory(weights_filename, out_file,factory_options);
	TMVA::DataLoader *dl = new TMVA::DataLoader(data_loader_filename);

	//Add training variables to the DataLoader which are the branches of interest
	ifstream training_variables(training_variables_filename);
	string variable_name;

	while(true)
	{
		training_variables >> variable_name;
		if(training_variables.eof())
		{
			break;
		}

		cout << variable_name.c_str() << endl;
		dl->AddVariable(variable_name.c_str(), 'F');
	}
	
	TFile* sgnl_file(TFile::Open(sgnl_tree_filename));
	TTree* sgnl_tree = (TTree*)sgnl_file->Get("Signal"); //The name of the signal tree is "Signal"
	dl->AddSignalTree(sgnl_tree, 1.0); //Nothing special with the weights

	TFile* bkgd_file(TFile::Open(bkgd_tree_filename));
	TTree* bkgd_tree = (TTree*)bkgd_file->Get("Background"); //The name of the background tree is "Background"
	dl->AddBackgroundTree(bkgd_tree, 1.0); //Nothing special with the weights

	dl->PrepareTrainingAndTestTree(sgnl_cut, bkgd_cut, dataset_options);
	factory->BookMethod(dl, TMVA::Types::kBDT, "BDT", method_options);

	factory->TrainAllMethods();
	factory->TestAllMethods();
	factory->EvaluateAllMethods();

	out_file->Close();

	if(!gROOT->IsBatch()){TMVA::TMVAGui(out_file_name);}
}
