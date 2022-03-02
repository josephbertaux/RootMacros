#include <iostream>
#include <fstream>
#include <vector>


void make_trees()
{
	const char* training_branches_filename = "training_branches.list";
	const char* training_files_filename = "training_files.list";

	//Fill signal and background trees with events from a list of output files
	TTree* sgnl_tree = new TTree("Signal", "Signal");
	TTree* bkgd_tree = new TTree("Background", "Background");

	//Add variables and branches to write data to these trees
	//The branches we want to use for training are contained in a .list file
	ifstream training_branches(training_branches_filename);

	int max_branches = 5; //The most branches our analysis will need; we can manually set this directly for good mem usage
	int num_branches = 0;
	Float_t branch_vals[max_branches];
	string branch_names[max_branches];
	
	string branch_name;
	while(true)
	{
		training_branches >> branch_name;
		if(training_branches.eof())
		{
			break;
		}

		branch_vals[num_branches] = 0.0;
		branch_names[num_branches] = branch_name;

		sgnl_tree->Branch(branch_names[num_branches].c_str(), &(branch_vals[num_branches]));
		bkgd_tree->Branch(branch_names[num_branches].c_str(), &(branch_vals[num_branches]));

		num_branches++;
	}

	//prepare for reading in the data from a list of .root files
	//need to declare variables to store truth information in the following analysis and also D0_mass for sideband
	Float_t m = 0;

	vector<Int_t> * hist1 = 0;
	vector<Int_t> * hist2 = 0;

	vector<Float_t> * hist1pT = 0;
	vector<Float_t> * hist2pT = 0;

	//Now actually begin reading in the files
	ifstream training_files(training_files_filename);
	string current_file_name;
	while(true)
	{
		training_files >> current_file_name;
		if(training_files.eof())
		{
			break;
		}

		TFile* current_file(TFile::Open(current_file_name.c_str()));
		TTree* t = (TTree*)current_file->Get("DecayTree");
		t->SetBranchStatus("*",0);

		//Address the branches of t to the ones we'll need for training
		for(int i = 0; i < num_branches; i++)
		{
			t->SetBranchStatus(branch_names[i].c_str(), 1);
			t->SetBranchAddress(branch_names[i].c_str(), &(branch_vals[i]));
		}

		//Address the branches of t to correspond to what we need to determine if we fill to signal or background
		t->SetBranchStatus("D0_mass", 1);				t->SetBranchAddress("D0_mass", &m);
		t->SetBranchStatus("track_1_true_track_history_PDG_ID", 1);	t->SetBranchAddress("track_1_true_track_history_PDG_ID", &hist1);
		t->SetBranchStatus("track_2_true_track_history_PDG_ID", 1);	t->SetBranchAddress("track_2_true_track_history_PDG_ID", &hist2);
		t->SetBranchStatus("track_1_true_track_history_pT", 1);		t->SetBranchAddress("track_1_true_track_history_pT", &hist1pT);
		t->SetBranchStatus("track_2_true_track_history_pT", 1);		t->SetBranchAddress("track_2_true_track_history_pT", &hist2pT);

		bool fill_sgnl = false;
		//For the current tree, fill the signal and background trees with the values of the corresponding training branches
		//Fill to the signal tree or fill to the background tree depending on the truth classification of the current event in the tree
		for(int n = 0; n < t->GetEntriesFast(); n++)
		{
			t->GetEntry(n);

			//Determine whether to fill to signal tree or background tree based on the truth values of t
			//Assume it is background
			fill_sgnl = false;


			//If it is in the sideband, we can categorize it as background immediately
			//So check if it is between the sidebands (signal range)
			if(1.7 < m and m < 2.2) //change the bounds to the 3-sigma ones
			{
				//we are in the mass regime where it is worth it to check
				int i = -1;
				for(int k = 0; k < hist1->size(); k++)
				{
					if(abs(hist1->at(k)) == 421) //If a track_1 parent is a D0 or D0bar
					{
						i = k;
						break;
					}
				}

				int j = -1;
				for(int k = 0; k < hist2->size(); k++)
				{
					if(abs(hist2->at(k)) == 421) //If a track_2 parent is a D0 or D0bar
					{
						j = k;
						break;
					}
				}

				if(i > -1 and j > -1) //The parent D0s exist
				{
					if(hist1pT->at(i) == hist2pT->at(j)) //The are the same D0 (exact same transverse momentum)
					{
						fill_sgnl = true;
					}
				}
			}

			if(fill_sgnl)
			{
				sgnl_tree->Fill();
			}
			else
			{
				bkgd_tree->Fill();
			}
		}

		t->ResetBranchAddresses();
	}

	//Now the signal and background trees have been filled
	//Write them to different files
	TFile* sgnl_file(TFile::Open("sgnl.root", "RECREATE"));
	sgnl_tree->Write();
	sgnl_file->Write();
	sgnl_file->Close();

	TFile* bkgd_file(TFile::Open("bkgd.root", "RECREATE"));
	bkgd_tree->Write();
	bkgd_file->Write();
	sgnl_file->Close();
}
