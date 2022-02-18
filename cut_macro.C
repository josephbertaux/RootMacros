#include <TClass.h>
#include <TNtuple.h>

TNtuple* Fill_TNtuple(TFile* f);
void Draw_TNtuple(TNtuple* nt, const char* branch_name);

void cut_macro_simple(TFile* f, const char* branch_name)
{
	TNtuple* nt = Fill_TNtuple(f);
	Draw_TNtuple(nt, branch_name);
}

TNtuple* Fill_TNtuple(TFile* f)
{
	/*
	
	   Fills an NTuple with the parameters of the D0 reconstruction
	   The values we fill correspond with values used to do cuts in HFReco
	
	*/


	/* Prepare the tree from which we will fill the TNtuple */
	//Assuming the file contains a TTree called "DecayTree"
	TTree* t = (TTree*)f->Get("DecayTree");

	//Define the branches which correspond with the cuts we used
	int num_branches = 10; //There are 10 branches which correspond to the cuts we're interested in

	const char* branch_names[num_branches]; //An array to store the names of the branches we'll need; these must be names of branches in DecayTree
	Float_t branch_vals[num_branches]; //An array to store the values of the branches with the corresponding name

	branch_names[0] = "D0_IPchi2"; //corresponds to setMotherIPchi2()
	branch_names[1] = "D0_FDchi2"; //corresponds to setFlightDistancechi2()
	branch_names[2] = "D0_DIRA"; //corresponds to setMinDIRA()

	branch_names[3] = "D0_pT"; //corresponds to setMotherPT()

	branch_names[4] = "track_1_pT"; //collectively correspond to setMinimumTrackPT()
	branch_names[5] = "track_2_pT";

	branch_names[6] = "track_1_IPchi2"; //collectively correspond to setMinimumTrackIPchi2
	branch_names[7] = "track_2_IPchi2";

	branch_names[8] = "track_1_IP"; //collectively correspond to setMaximumDaughterDCA() (I think -- Impact Paramter and Distance of Closest Approach might be the same)
	branch_names[9] = "track_2_IP";

	//Set the addresses of these named branches
	for(int i = 0; i < num_branches; i++)
	{
		t->SetBranchAddress(branch_names[i], &(branch_vals[i]));
	}

	//We will also need branches to determine if an entry was a true D0
	//The values for these branches are not Float_t so we'll treat them separately
	vector<Int_t>* hist1 = 0;
	t->SetBranchAddress("track_1_true_track_history_PDG_ID", &hist1);

	vector<Float_t>* hist1pt = 0;
	t->SetBranchAddress("track_1_true_track_history_pT", &hist1pt);


	vector<Int_t>* hist2 = 0;
	t->SetBranchAddress("track_2_true_track_history_PDG_ID", &hist2);

	vector<Float_t>* hist2pt = 0;
	t->SetBranchAddress("track_2_true_track_history_pT", &hist2pt);


	/* Make the NTuple */
	//We can use the same names as the branches of the DecayTree for the names in the TNtuple
	string varlist = (string)branch_names[0];
	for(int i = 1; i < num_branches; i++)
	{
		varlist += ":";
		varlist += (string)branch_names[i];
	}
	//and one additional branch for the truth information
	varlist += ":truth";

	//We'll need an array to Fill() the TNtuple with
	//we could use branch_vals but there is one extra piece of information, "truth", that we'll need, so we'll make a separate array
	Float_t cut_vals[num_branches+1];

	//Declare the TNtuple itself
	TNtuple* nt = new TNtuple("nt", "Cut Tuple", varlist.c_str()); //note this is not a pointer but the actual object, so members called with . instead of ->


	//Now to fill the TNtuple
	int N = t->GetEntriesFast();
	
	//We'll use these indices in the subsequent for loop; I don't like declaring variables inside the loop
	int i;
	int j;
	int k;


	for(int n = 0; n < N; n++)
	{
		t->GetEntry(n);
		//we can copy the values of the branches to our cut array
		for(k = 0; k < num_branches; k++)
		{
			cut_vals[k] = branch_vals[k];
		}

		//but we're not done, one extra branch in the TNtuple for the truth information

		//for cut_vals[num_branches], we need a little extra to get the truth value since there wasn't just one branch that gives this
		//for this entry, see if the tracks had a parent D0 (or D0 bar)
		i = -1;
		for(k = 0; k < hist1->size(); k++)
		{
			if(abs(hist1->at(k)) == 421) //if a parent was a D0 (or its anti particle)
			{
				i = k;
				break;
			}
		}

		j = -1;
		for(k = 0; k < hist2->size(); k++)
		{
			if(abs(hist2->at(k)) == 421) //if a parent was a D0 (or its antii particle)
			{
				j = k;
				break;
			}
		}


		//now i and j are the indexes of the D0 (bar) parent for each track, if there was one
		//need to see if they are the same D0 (bar); we'll say it is if the transverse momentums of the parent are *exactly* the same
		
		cut_vals[num_branches] = 0.0; //assume false
		if(i > -1 and j > -1) //parent D0s exist for each track
		{
			if(hist1pt->at(i) == hist2pt->at(j)) //the transverse momentums are exactly the same, so we'll say they have the same parent D0 (bar)
			{
				if(hist1->at(i) == +421)cut_vals[num_branches] = +1.0; //+1 for D0
				if(hist1->at(i) == -421)cut_vals[num_branches] = -1.0; //-1 for D0 bar
			}
		}

		nt->Fill(cut_vals);
	}


	return nt;
}



void Draw_TNtuple(TNtuple* nt, const char* branch_name)
{
	/*
		Draws the background and signal distributions for the branch_name of a TNtuple* nt
		Assumes the TNtuple* has a last column, "truth", to distinguish between particle/anti-particle/background
	
	*/

	//Draw the whole thing, mostly background
	nt->SetLineColor(1); //Black
	nt->Draw(branch_name);

	//Draw the particle distribution
	nt->SetLineColor(2); //Red
	nt->Draw(branch_name, "truth == 1.0", "same");

	//Draw the anti-particle distribution
	nt->SetLineColor(4); //Blue
	nt->Draw(branch_name, "truth == -1.0", "same");

	//Draw the particle/anti-particle distribution
	nt->SetLineColor(6); //Purple = Blue + Red
	nt->Draw(branch_name, "abs(truth) == 1", "same");
}
