/*
 * SNP_PW.cpp
 *
 */
#include "SNP_PW.h"
using namespace std;

SNP_PW::SNP_PW(){

}



SNP_PW::SNP_PW(string rs, string c, int p, double b1, double b2, vector<bool> an, vector<int> ds, vector<vector<pair<int, int> > > dmodels){
	//for pairwise
	id = rs;
	chr = c;
	pos = p;
	BF = b1;
	BF2 = b2;
	for (vector<bool>::iterator it = an.begin(); it != an.end(); it++) {
		annot.push_back(*it);
		if (*it) annot_weight.push_back(1.0);
		else annot_weight.push_back(0.0);
	}

	//distribute weights
	float s = 0;
	for (vector<float>::iterator it = annot_weight.begin(); it != annot_weight.end(); it++) s += *it;
	if (s > 0) for (int i = 0; i < annot_weight.size(); i++) annot_weight[i]  = annot_weight[i]/s;

	for (vector<int>::iterator it = ds.begin(); it != ds.end(); it++) dists.push_back(*it);
	// append distance annotations
	append_distannots(dmodels);
	nannot = annot.size();
}


void SNP_PW::append_distannots(vector<vector<pair<int, int> > > dmodels){
	for (int i = 0; i < dists.size(); i++){
		int dist = dists[i];
		bool found = false;
		vector<pair<int, int> > model = dmodels[i];
		for (vector<pair<int, int> >::iterator it = model.begin(); it != model.end(); it++){
			int st = it->first;
			int sp = it->second;
			if (dist >= st && dist < sp) {
				if (found){
					cerr << "ERROR: SNP "<< id << " is in more than one distance bin for distance measure number "<< i << "\n";
					exit(1);
				}
				annot.push_back(true);
				annot_weight.push_back(1.0);
				found = true;
			}
			else {
				annot.push_back(false);
				annot_weight.push_back(0.0);
			}
		}
	}
}

double SNP_PW::get_x(vector<double> lambda){
	//no annotations yet
	return 0.0;
	/*
	if (lambda.size() != nannot){
		cerr << "ERROR: SNP "<< id << ". Lambda has "<< lambda.size()<< " entries. nannot is " << nannot << "\n";
		exit(1);
	}
	if (nannot == 0) return 0;
	double toreturn = 0;
	for (int i = 0; i < nannot; i++) {
		if (annot[i]) toreturn += lambda[i];
	}
	return toreturn;
	*/
}

double SNP_PW::get_x_cond(vector<double> lambda, double lambdac){
	if (lambda.size() != nannot){
		cerr << "ERROR: SNP "<< id << ". Lambda has "<< lambda.size()<< " entries. nannot is " << nannot << "\n";
		exit(1);
	}
	if (nannot == 0) return 0;
	double toreturn = 0;
	for (int i = 0; i < nannot; i++) {
		if (annot[i]) toreturn += lambda[i];
	}
	if (condannot) toreturn+= lambdac;
	return toreturn;
}


double SNP_PW::calc_logBF1(){
	double toreturn = 0;
	double r = W/ (V1+W);
	toreturn += -log ( sqrt(1-r) );
	toreturn += - (Z*Z*r/2);
	return -toreturn;
}


double SNP_PW::calc_logBF2(){
	double toreturn = 0;
	double r = W/ (V2+W);
	toreturn += -log ( sqrt(1-r) );
	toreturn += - (Z2*Z2*r/2);
	return -toreturn;
}


double SNP_PW::calc_logBF3( double C){
	double toreturn = 0;
	//vector<>
}

double SNP_PW::approx_v1(){
	double toreturn;
	toreturn = 2*f*(1-f) * (double) N1;
	toreturn = 1.0/toreturn;
	return toreturn;
}


double SNP_PW::approx_v2(){
	double toreturn;
	toreturn = 2*f*(1-f) * (double) N2;
	toreturn = 1.0/toreturn;
	return toreturn;
}

double SNP_PW::ln_MVN(vector<double> beta, vector<vector<double> > S){
	// log bivariate normal density
	// assume mu = [0,0]
	// S = covariance matrix
	// beta = [\hat \beta_1, \hat \beta_2]


	double toreturn = 0;

	//determinant
	double det = S.at(0).at(0) * S.at(1).at(1) - S.at(0).at(1)*S.at(1).at(0);
	cout << "det " << det << "\n";
	//invert
	vector<vector<double> > invS;
	vector<double> tmp (2,0.0);
	vector<double> tmp2 (2, 0.0);
	invS.push_back(tmp); invS.push_back(tmp2);
	invS.at(0).at(0) = S.at(1).at(1) / det;
	invS.at(1).at(1) = S.at(0).at(0)/ det;
	invS.at(0).at(1) = -S.at(0).at(1)/ det;
	invS.at(1).at(0) = -S.at(1).at(0)/ det;
	cout << "\n";
	for (int i = 0; i <2 ; i++){
		for (int j = 0; j < 2; j++){
			cout << invS[i][j]<< " ";
		}
		cout << "\n";
	}
	//exponent
	double t = beta.at(0)*beta.at(0)*invS.at(0).at(0) + beta.at(1)*beta.at(1)*invS.at(1).at(1) + 2*beta.at(0)*beta.at(1)*invS.at(0).at(1);
	cout << "\nt "<< t<< "\n\n";
	// density
	toreturn = -log(2*M_PI) - log(sqrt(det));
	toreturn+= -0.5 * t;
	return toreturn;
}

