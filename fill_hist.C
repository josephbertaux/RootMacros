#include <vector>
#include <TClass.h>

using namespace std;


void fill_hist(TFile* f)
{
	/*
		fills the histogram h with data from the tree "DecayTree" assumed in the root file named filename
		assumes we are interested in D0 to K and pi decays
	*/

	TTree* t = (TTree*)f->Get("DecayTree");


	//set addresses
	Float_t D0_mass;
	t->SetBranchAddress("D0_mass", &D0_mass);


	Int_t reco_ID1;
	t->SetBranchAddress("track_1_PDG_ID", &reco_ID1);
	
	Int_t true_ID1;
	t->SetBranchAddress("track_1_true_ID", &true_ID1);
	
	Int_t reco_ID2;
	t->SetBranchAddress("track_2_PDG_ID", &reco_ID2);

	Int_t true_ID2;
	t->SetBranchAddress("track_2_true_ID", &true_ID2);


	vector<Int_t> * track_1_hist_ID = 0;
	t->SetBranchAddress("track_1_true_track_history_PDG_ID", &track_1_hist_ID);

	vector<Float_t> * track_1_hist_px = 0;
	t->SetBranchAddress("track_1_true_track_history_px", &track_1_hist_px);

	vector<Float_t> * track_1_hist_py = 0;
	t->SetBranchAddress("track_1_true_track_history_py", &track_1_hist_py);

	vector<Float_t> * track_1_hist_pz = 0;
	t->SetBranchAddress("track_1_true_track_history_pz", &track_1_hist_pz);


	vector<Int_t> * track_2_hist_ID = 0;
	t->SetBranchAddress("track_2_true_track_history_PDG_ID", &track_2_hist_ID);

	vector<Float_t> * track_2_hist_px = 0;
	t->SetBranchAddress("track_2_true_track_history_px", &track_2_hist_px);

	vector<Float_t> * track_2_hist_py = 0;
	t->SetBranchAddress("track_2_true_track_history_py", &track_2_hist_py);

	vector<Float_t> * track_2_hist_pz = 0;
	t->SetBranchAddress("track_2_true_track_history_pz", &track_2_hist_pz);


	TH1F* h = new TH1F("h", "D0_mass", 20, 1.7, 2.2);


	int N = t->GetEntriesFast();
	int n;

	int i;
	int j;
	int k;

	for(n = 0; n < N; n++)
	{
		t->GetEntry(n);

//		if(reco_ID1 == true_ID1 and reco_ID2 == true_ID2)
//		{
//			h->Fill(D0_mass);
//		}

//		if(track_1_hist_ID->size() > 0){cout << track_1_hist_ID->at(0) << endl;}

//		for(j = 0; j < track_1_hist_ID->size(); j++)
//		{
//			cout << track_1_hist_ID->at(j) << " ";
//		}
//		cout << endl;
		
		i = -1;
		for(k = 0; k < track_1_hist_ID->size(); k++)
		{
			if(track_1_hist_ID->at(k) == 421) //if a decay parent was a D0
			{
				i = k;
				break;
			}
		}

		j = -1;
		for(k = 0; k < track_2_hist_ID->size(); k++)
		{
			if(track_2_hist_ID->at(k) == 421) //if a decay parent was a D0
			{
				j = k;
				break;
			}
		}


		//now i and j are the indexes of the D0 decay parent, if there was one
		if(i >= 0 and j >= 0)
		{
			if(track_1_hist_px->at(i) == track_2_hist_px->at(j) and track_1_hist_py->at(i) == track_2_hist_py->at(j) and reco_ID1 == true_ID1 and reco_ID2 == true_ID2)
			{
				//track1's parent D0 momentum was the same as track_2's parent D0 momentum, so they were probably the same D0
				//and the particles are actually what the recombination thought they were

				h->Fill(D0_mass);
			}
		}

	}

	h->Draw();
}
