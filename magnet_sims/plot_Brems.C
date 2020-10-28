// plot spacial and Energy distribution of Bremstrahlung



void plot_Brems() {


  TFile *f = new TFile("jenn_brems_test_mono60MeV_Gauss_2mmW_gamma_t0.root");
  TTree *T=(TTree*)f->Get("lxtsim");

  //Create a new Canvas at random Position
  TCanvas *C1 = new TCanvas("C1", // canvas name
                            "C1", // canvas title
                            800, // canvas size in pixels along x
                            600); // canvas size in pixels along y

//Creates a 2D-histogram with floating numbers
  TH2F *XY_pos = new TH2F("XY_pos", //Name
                          "Photon XY-Pos at exit;X / mm;Y / mm",// title; axes label
                          100, // number of bins X
                          -1500,// xlow
                          1500,// xup
                          100,// nbins Y
                          -1500,// ylow
                          1500); //yup

// will draw a TGraph (or TH1 if only 1 Variable) called XY_pos
// "ColZ" option for a 2D histogram with colour coding,
//      if "" default scatter plot,
//      "SURF2" for surface plot with colours
  T->Draw("y:x>>XY_pos", //varexp; "e1" produces TH1F, "e1:e2" unbinned 2D scatter plot
          "", // selection; if boolean expression is ture, hist is filled with a weight = value
          "ColZ");// drawing option


//For logarithmic axes
// C1->SetLogy()



}
