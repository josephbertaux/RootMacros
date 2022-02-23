#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TClass.h"

using namespace std;

TTree* read_files(int num_files)
{
	/*
		Reads in a list of files assumed to have a name like

			"outputData_myHeavyFlavorReco_00000.root" (39 chars)

		which are numbered in sequence 0 throught num_files-1, and that each file has a tree called

			"DecayTree".

		Copies the contents of each file to a common tree and returns the tree
	*/

	//Prepare the TTree
	TTree* t = new TTree("t", "DecayTree"); vector<const char*> branch_names = {};
	
	//Explicitly name the branches we'll want to use
	//This might only be a couple but in the future I may need something more robust
	branch_names.push_back("D0_mass");				Float_t m = 0.0;		t->Branch(branch_names[branch_names.size()-1], &m);
//	branch_names.push_back("pT");					Float_t pt = 0.0;		t->Branch(branch_names[branch_names.size()-1], &pt);
	branch_names.push_back("track_1_true_track_history_PDG_ID");	vector<Int_t> *hist1 = 0;	t->Branch(branch_names[branch_names.size()-1], &hist1);
	branch_names.push_back("track_1_true_track_history_pT");	vector<Float_t> *hist1pt = 0;	t->Branch(branch_names[branch_names.size()-1], &hist1pt);
	branch_names.push_back("track_2_true_track_history_PDG_ID");	vector<Int_t> *hist2 = 0;	t->Branch(branch_names[branch_names.size()-1], &hist2);
	branch_names.push_back("track_2_true_track_history_pT");	vector<Float_t> *hist2pt = 0;	t->Branch(branch_names[branch_names.size()-1], &hist2pt);

	//Set up for reading 
	int max_name_length = 40; //lenght of a file name is 40 chars, including the end of string char '\0'
	char current_file[max_name_length]; //stores the name of the current file we are reading from


	for(int i = 0; i < num_files; i++)
	{
		snprintf(current_file, max_name_length, "outputData_myHeavyFlavorReco_%05d.root\0", i); //I will explicitly include the null terminator \0 if I want to, you can't stop me

		//Instantiate and set the read tree
		TFile* f(TFile::Open(current_file));
		TTree* r = (TTree*)f->Get("DecayTree");
		r->SetBranchStatus("*", 0); //Don't read all branches, we're only interested in a couple

		for(int j = 0; j < branch_names.size(); j++)
		{
			//Use the same addresses as the write tree; they're organized by name in branch_names
			(r->GetBranch(branch_names[j]))->SetStatus(1);
			(r->GetBranch(branch_names[j]))->SetAddress((t->GetBranch(branch_names[j]))->GetAddress());
		}

		//Branch addresses of the read tree are set commensurately with the branch addresses of the write tree
		//Copy the entries
		for(int j = 0; j < r->GetEntriesFast(); j++)
		{
			r->GetEntry(j);
			t->Fill();
		}

		r->ResetBranchAddresses(); //Not sure if this is doing anything for me
//		f->Close(); //Was causing a segfault exactly every other time the program was run
	}


	//Reset the memory addresses of our write tree
	//the local variables will not exist outside the scope of this program
	t->ResetBranchAddresses();

	return t;
}
