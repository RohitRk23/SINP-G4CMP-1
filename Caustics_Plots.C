//This is a root Program to Plot the Caustic Patterns from the phonon_hits.txt File
//Function to Plot Only Phonon Caustic.

const int nBinsX = 500;
const int nBinsY = 500;
const double minX = -2;
const double minY = -2;
const double maxX = 2;
const double maxY = 2;

 TCanvas *c1 = new TCanvas("c1","Canvas Example",200,10,800,800);
 TCanvas *c2 = new TCanvas("c2","Canvas Example",200,10,700,700);


//---------------------------------------------------------------------------------
//Only plot transverse fast phonons
void TransFast(const TString& fileName) {

  //Get the file output from the example
  ifstream in;
  in.open(fileName);
  TH2D *FCaustics= new TH2D("Caustics_TF","Phonon Caustics of Transverse Fast Phonons;x (mm);y(mm)",nBinsX,minX,maxX,nBinsY,minY,maxY);
  Int_t nlines = 0;
  Int_t EventID=-1, TrackID=-1;
  TString Name_Phonon;
  Double_t X_f,Y_f,Z_f;

  //Read in the phonons from the file
  while (1) {

     in>>EventID>>TrackID>>Name_Phonon>>X_f>>Y_f>>Z_f;
     cout << "Name_phonon: " << Name_Phonon << endl;
     if(Name_Phonon=="phononTF"){
      cout << "I am inside filling loop" << endl;
     FCaustics->Fill(X_f*1000,Y_f*1000);
    }
     if (!in.good()) break;
  nlines++;
  }

  //c1->SetFillColor(1);
  FCaustics->SetMarkerColorAlpha(kWhite, 0.2);
 // c1->SetLogz();

  //Caustics->Draw("colz");
  c1->cd();
  FCaustics->Draw("colz");
  c1->SaveAs("TF.pdf");
  in.close();

}


//---------------------------------------------------------------------------------
//Only plot transverse slow phonons
void TransSlow(const TString& fileName) {

  //Get the file output from the example
  ifstream in;
  in.open(fileName);
  TH2D *SCaustics= new TH2D("Caustics_TS","Phonon Caustics of Transverse Slow phonons;x (mm);y (mm)",nBinsX,minX,maxX,nBinsY,minY,maxY);
  Int_t nlines = 0;
  Int_t EventID=-1, TrackID=-1;
  TString Name_Phonon;
  Double_t X_f,Y_f,Z_f;
  
  //Read in the phonons from the file
  while (1) {    
    in>>EventID>>TrackID>>Name_Phonon>>X_f>>Y_f>>Z_f;
    if(Name_Phonon=="phononTS"){
      SCaustics->Fill(X_f*1000,Y_f*1000);
    }
    if (!in.good()) break;
    nlines++;
  }

  
  //printf(" found %d points\n",nlines);
  //gStyle->SetPalette(kBlack+3);
  //c1->SetFillColor(1);
  SCaustics->SetMarkerColorAlpha(kWhite, 0.2);
// c2->SetLogz();
  
  //Caustics->Draw("colz"); If you want to see on a Blue and yellow colors
  c2->cd();
  SCaustics->Draw("colz");
  c2->SaveAs("TS.pdf");
  in.close();

}


//---------------------------------------------------------------------------------
void TransFast_and_Slow(const TString& fileName) {

  //Get the file output from the example
  ifstream in;
  in.open(fileName);
  TH2D *Caustics= new TH2D("Caustics",";x (mm);y (mm)",nBinsX,minX,maxX,nBinsY,minY,maxY);
  Int_t nlines = 0;
  Int_t EventID=-1, TrackID=-1;
  TString Name_Phonon;
  Double_t X_f,Y_f,Z_f;
  Double_t start_X,start_Y,start_Z;
  //Read in the phonons from the file
  while (1) {
     cout << "X_f: " << X_f << " , Y_f: " << Y_f << endl;
     in>>EventID>>TrackID>>Name_Phonon>>X_f>>Y_f>>Z_f
     >>start_X>>start_Y>>start_Z;

     Caustics->Fill(X_f*1000,Y_f*1000);

     if (!in.good()) break;
  nlines++;
  }
  //printf(" found %d points\n",nlines);
  //gStyle->SetPalette(kBlack+3);
  //TCanvas *c1 = new TCanvas("c1","Canvas Example",200,10,600,480);
  //c1->SetFillColor(1);
  //Caustics->SetMarkerColorAlpha(kWhite, 0.2);
 c1->SetLogz();
 c1->cd();
// c1->SetPad(0.02,0.02,1,1);
 gStyle->SetPalette(1);
 gStyle->SetOptStat(0);
 Caustics->Draw("COLZ");
 gPad->Update();
 TPaletteAxis *palette = (TPaletteAxis*)Caustics->GetListOfFunctions()->FindObject("palette");
palette->SetX1NDC(0.88);
 palette->SetX2NDC(0.9);
 palette->SetY1NDC(0.12);
 palette->SetY2NDC(0.88);
 palette->SetLabelSize(0.05);
 gStyle->SetTitleFontSize(0.07);
 gPad->Modified();
 gPad->Update();
 Caustics->GetYaxis()->SetLabelSize(0.05);
 Caustics->GetXaxis()->SetLabelSize(0.05);
 Caustics->GetYaxis()->SetTitleSize(0.05);
 Caustics->GetXaxis()->SetTitleSize(0.05);
 Caustics->GetYaxis()->SetTitleOffset(1.2);
 Caustics->GetXaxis()->SetTitleOffset(1.2);
 c1->SetMargin(0.12,0.12,0.12,0.12);
 Caustics->GetZaxis()->SetRangeUser(40,80);
 Caustics->GetZaxis()->SetMoreLogLabels();
  Caustics->Draw("colz");
 c1->SaveAs("Both.pdf");
  //Caustics->Draw();
  in.close();

}

//---------------------------------------------------------------------------------
//Main function
#include <iostream>
using namespace std;
void Caustics_Plots(TString Phonon_Name, TString fileName="phonon_hits.txt") {
  
  TString  Phonon_Case = Phonon_Name;
  Int_t What_phonon1,What_phonon2,What_phonon3,What_phonon4;
  What_phonon1=Phonon_Case.CompareTo("Fast");
  What_phonon2=Phonon_Case.CompareTo("Slow");
  What_phonon3=Phonon_Case.CompareTo("Both");
  What_phonon4=Phonon_Case.CompareTo("Lazy");
  if (What_phonon1==0) {
    cout<<"Fast";
    TransFast(fileName);
  } else if (What_phonon2==0) {
    cout<<"Slow";
    TransSlow(fileName);
  } else if(What_phonon3==0){
    cout<<"Both";
    TransFast_and_Slow(fileName);
  }
  else
  {cout<<"So lazy\n";TransFast(fileName);TransSlow(fileName);}
  c1->Close();
  c2->Close();
}
