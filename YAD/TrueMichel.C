#include "YADtools.h"

const size_t n_file=1;
const vector<string> filelist = yad::ReadFileList(n_file,"ijclab.list");

void TrueMichel() {

    vector<TGraph2D*> gPrt(2);
    vector<size_t> igPrt = {0,0};
    gPrt[0] = new TGraph2D();
    gPrt[0]->SetMarkerColor(kAzure+2);
    gPrt[0]->SetMarkerStyle(20);
    gPrt[0]->SetMarkerSize(0.3);

    gPrt[1] = new TGraph2D();
    gPrt[1]->SetMarkerColor(kOrange+7);
    gPrt[1]->SetMarkerStyle(20);
    gPrt[1]->SetMarkerSize(0.3);

    vector<TGraph2D*> gDep(2);
    vector<size_t> igDep = {0,0};
    gDep[0] = new TGraph2D();
    gDep[0]->SetMarkerColor(kAzure+2);
    gDep[0]->SetMarkerStyle(4);
    gDep[0]->SetMarkerSize(1);

    gDep[1] = new TGraph2D();
    gDep[1]->SetMarkerColor(kOrange+7);
    gDep[1]->SetMarkerStyle(4);
    gDep[1]->SetMarkerSize(1);

    size_t N_michel=0;

    size_t i_file=0;
    for (string filename : filelist) {
        
        cout << "\e[3mOpening file #" << ++i_file << "/" << n_file << ": " << filename << "\e[0m" << endl;

        yad::Truth T(filename.c_str());
        yad::Reco R(filename.c_str());

        size_t n_evt = R.GetEntries();
        for (size_t i_evt=0; i_evt < n_evt; i_evt++) {
        // size_t i_evt=45; {

            cout << "Event#" << i_evt << endl;

            T.GetEntry(i_evt);
            R.GetEntry(i_evt);

            for (size_t i_prt=0; i_prt < T.NPrt; i_prt++) {

                if (T.PrtPdg->at(i_prt)!=11) continue; //electrons only
                if (T.PrtNPt->at(i_prt) < 10) continue; //no short tracks
                // if (T.PrtNPt->at(i_prt) > 500) continue;

                int i_mom = T.PrtMomID->at(i_prt);
                if (i_mom==-1) continue; //no orphelin electrons
                if (T.PrtPdg->at(i_mom)!=13) continue; //electrons coming from muons only

                double PrtE_michel=0;
                double DepE_michel=0;
                double Length=0;

                bool is_inside=true;
                for (size_t i_ppt=0; i_ppt < T.PrtNPt->at(i_prt); i_ppt++) {


                    double X = (*T.PrtX)[i_prt][i_ppt];
                    double Y = (*T.PrtY)[i_prt][i_ppt];
                    double Z = (*T.PrtZ)[i_prt][i_ppt];

                    // double Mag =TMath::Sqrt(X*X+Y*Y+Z*Z); 
                    // Length+=Mag;

                    is_inside = -350 < X && X < 350 &&
                                -350 < Y && Y < 350 &&
                                   0 < Z && Z < 300;
                    
                    if (!is_inside) break;
                } //end particlepoint loop
                if (!is_inside) continue; //electrons inside the detector

                for (size_t i_ppt=0; i_ppt < T.PrtNPt->at(i_mom); i_ppt++) {
                    double X = (*T.PrtX)[i_mom][i_ppt];
                    double Y = (*T.PrtY)[i_mom][i_ppt];
                    double Z = (*T.PrtZ)[i_mom][i_ppt];

                    is_inside = -350 < X && X < 350 &&
                                -350 < Y && Y < 350 &&
                                   0 < Z && Z < 300;
                    
                    if (!is_inside) break;
                } //end particlepoint loop
                if (!is_inside) continue; //from muons inside the detector

                N_michel++;

                for (size_t i_ppt=0; i_ppt < T.PrtNPt->at(i_prt); i_ppt++) {
                    double X = (*T.PrtX)[i_prt][i_ppt];
                    double Y = (*T.PrtY)[i_prt][i_ppt];
                    double Z = (*T.PrtZ)[i_prt][i_ppt];
                    double E = (*T.PrtE)[i_prt][i_ppt];

                    double Mag =TMath::Sqrt(X*X+Y*Y+Z*Z); 
                    Length-=Mag;
                    PrtE_michel+=E;

                    gPrt[0]->SetPoint(igPrt[0]++,Y,Z,X);
                } //end particlepoint loop

                for (size_t i_ppt=0; i_ppt < T.PrtNPt->at(i_mom); i_ppt++) {
                    double X = (*T.PrtX)[i_mom][i_ppt];
                    double Y = (*T.PrtY)[i_mom][i_ppt];
                    double Z = (*T.PrtZ)[i_mom][i_ppt];
                    double E = (*T.PrtE)[i_mom][i_ppt];

                    gPrt[1]->SetPoint(igPrt[1]++,Y,Z,X);
                } //end particlepoint loop

                for (size_t i_dep=0; i_dep < T.NDep->at(i_prt); i_dep++) {

                    double X = (*T.DepX)[i_prt][i_dep];
                    double Y = (*T.DepY)[i_prt][i_dep];
                    double Z = (*T.DepZ)[i_prt][i_dep];
                    double E = (*T.DepE)[i_prt][i_dep];
                    DepE_michel+=E;

                    gDep[0]->SetPoint(igDep[0]++,Y,Z,X);
                } //end depopoint loop

                for (size_t i_dep=0; i_dep < T.NDep->at(i_mom); i_dep++) {

                    double X = (*T.DepX)[i_mom][i_dep];
                    double Y = (*T.DepY)[i_mom][i_dep];
                    double Z = (*T.DepZ)[i_mom][i_dep];
                    double E = (*T.DepE)[i_mom][i_dep];

                    gDep[1]->SetPoint(igDep[1]++,Y,Z,X);
                } //end depopoint loop

                cout << "evt#" << i_evt << ":prt#" << i_prt << endl;
                cout << "\tDepE=" << DepE_michel << "\tPrtE" << PrtE_michel << endl;

            } //end particle loop
        } //end event loop
    } //end file loop

    // cout << "#michel=" << N_michel << ":#ppt=" << igPrt[0] << ":#depo=" << igDep[0] << endl;

    TCanvas* c1 = new TCanvas("c1","True Michel");
    c1->cd();
    gPrt[1]->Draw("p");
    gPrt[0]->Draw("samep");
    gDep[0]->Draw("samep");
    // gDep[1]->Draw("samep");
}