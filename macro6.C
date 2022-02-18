void macro6(const char* filename)
{
	double xmin = 1.7;
	double xmax = 2.2;

	TFile* myfile(TFile::Open(filename));
	TTree* mytree = (TTree*)myfile->Get("DecayTree");
	
	Float_t d0_mass;
	Float_t d0_massErr;

	mytree->SetBranchAddress("D0_mass", &d0_mass);
	mytree->SetBranchAddress("D0_massErr", &d0_massErr);

	TH1F *h = new TH1F("h","D0 Mass counts", 100, xmin, xmax);

	int N = mytree->GetEntriesFast();
	int i;
	for(i = 0; i < N; i++)
	{
		mytree->GetEntry(i);

		h->Fill(d0_mass);
	}

	TCanvas* c1 = new TCanvas("c1", "", 700, 500);
	c1->cd();

//	TF1 *fit = new TF1("fit", "exp(0)+gaus(2)", xmin, xmax);
	TF1 *fit = new TF1("fit", "[0]*exp(-[1]*x)+[2]*exp(-0.5*((x-[3])/[4])^2)", xmin, xmax);

	fit->SetParameter(0, 1.0);
	fit->SetParameter(1, 0.0);
	fit->SetParameter(2, 0.0);
	fit->SetParameter(3, 0.0);
	fit->SetParameter(4, 1.0);

	fit->ReleaseParameter(0);
	fit->ReleaseParameter(1);
	fit->FixParameter(2, fit->GetParameter(2));
	fit->FixParameter(3, fit->GetParameter(3));
	fit->FixParameter(4, fit->GetParameter(4));
	h->Fit("fit", "Q", "", xmin, xmax);

	fit->FixParameter(0, fit->GetParameter(0));
	fit->FixParameter(1, fit->GetParameter(1));
	fit->ReleaseParameter(2);
	fit->ReleaseParameter(3);
	fit->ReleaseParameter(4);
	h->Fit("fit", "Q", "", xmin, xmax);

	fit->ReleaseParameter(0);
	fit->ReleaseParameter(1);

	h->Fit("fit", "", "", xmin, xmax);

	h->Draw();
	fit->Draw("e same");
}

