/*
 * LookAheadExtractor.cpp
 *
 *  Created on: 7/Set/2009
 *      Author: Helder
 */

// look ahead extractor values
#define N 262144
#define K 181882
#define D 165383
#define L 160
#define T 584
#define NW 262144
#define KW 149675
#define DW 161
#define LW 128
#define NQ 165255
#define KQ 171
#define DQ 161
#define LQ 128
//define



/*
*/

#include <stdexcept>
//#include "Composition.h"
#include "Extractor.h"
#include "LookAheadExtractor.h"
#include "Composition.h"

LookAheadExtractor::LookAheadExtractor() {
}

LookAheadExtractor::LookAheadExtractor(int _n, int _k, int _d, int _l, int _t, float _epsilon) {
	n = _n;
	k = _k;
	d = _d;
	l = _l;
	t = _t;
	epsilon = _epsilon;

}

LookAheadExtractor::~LookAheadExtractor() {

}

vector<bitstring > LookAheadExtractor::extract1(bitstring w, bitstring x, vector< vector <bitstring> > extwSVector,vector< vector <bitstring> > extwMskVector, vector< vector <bitstring> > extqSVector,vector< vector <bitstring> > extqMskVector, vector<int> deltas) {
	cout << "LookAheadExtractor::extract1 "<<endl;
	
	vector<bitstring > result;

	bitstring q;
	bitstring si;
	bitstring ri;

	
	 q  = x.subbits(0,((int)x.size()-l)); // X=[    q       | S1=256bits  ]
	 si = x.subbits(((int)x.size()-l), l);

	Composition *msk_w = new Composition(l,w,si,extqSVector.at(0),extqMskVector.at(0),deltas.at(0));// TODO idem + random delta. len(si) = outputsize 
	ri= msk_w->extract();
	result.push_back(bitstring(ri));
	
	delete msk_w;

	for (int i = 1; i < t; ++i) { 
	
		Composition *temp_extq = new Composition(l,q,ri, extwSVector.at(i-1),extwMskVector.at(i-1),deltas.at(i)); // Todas as Compositions sao independentes apenas usam um S_0 que depende da anterior.
		
		si= temp_extq->extract();
		
		delete temp_extq;
		Composition *temp_extw = new Composition(l,w,si, extqSVector.at(i),extqMskVector.at(i),deltas.at(i)); // Todas as Compositions sao independentes apenas usam um S_0 que depende da anterior.	
		ri=temp_extw->extract();
		result.push_back(bitstring(ri));
		delete temp_extw;
	}


return result;
}




/***************************************************/






vector<bitstring > LookAheadExtractor::extract(bitstring w, bitstring x,vector< vector< bitstring > > &extqSVector,vector< vector< bitstring > > &extqMskVector,vector< vector< bitstring > > &extwSVector,vector< vector< bitstring > > &extwMskVector, vector<int> &deltas) {
	
	vector<bitstring > result;
	bitstring q = x.subbits(0,((int)x.size()-l)); // X=[    q       | Si[1]=256bits  ]
	bitstring si = x.subbits(((int)x.size()-l), l);
	
	/* ExtW*/
	int delta=4;

	Composition *msk_w = new Composition(l,w,si,delta);// TODO idem + random delta. len(si) = outputsize 
	msk_w->creat_Composition();

	vector<bitstring> extw_S = msk_w->S; 
	vector<bitstring> extw_aMsk = msk_w->Msk; 

	
	extwSVector.push_back((vector <bitstring>)extw_S);//extwSVector[0]= extw.S[];
	extwMskVector.push_back((vector <bitstring>)extw_aMsk);
	
	deltas.push_back(4); // TODO must be random and one delta for each "extractorComposition", 2 vectors for ri and si.
	
	bitstring ri= msk_w->extract();
	result.push_back(ri);	 // result[0]= ri;
	//bitstring sit;

	
	for (int i = 2; i <= t; ++i) {
		delta = 4;
		
		deltas.push_back(delta); // TODO must be random and one delta for each "extractorComposition", 2 vectors for ri and si.

		Composition *temp_extq = new Composition(l,q,ri,delta); // Todas as Compositions sao independentes apenas usam um S_0 que depende da anterior.
	
		temp_extq->creat_Composition();

		extqSVector.push_back((vector <bitstring>) temp_extq->S); // extqSVector[i] = extq.S[];
		extqMskVector.push_back((vector <bitstring>) temp_extq->Msk); //extwSVector[i]= extw.S[];		

		bitstring si= temp_extq->extract();

		delete temp_extq;
		

		Composition *temp_extw = new Composition(l,w,si,delta);
		temp_extw->creat_Composition();
		
			extwSVector.push_back((vector <bitstring>) temp_extw->S); //extwSVector[i]= extw.S[];
			extwMskVector.push_back((vector <bitstring>) temp_extw->Msk); //extwSVector[i]= extw.S[];
		
		ri=temp_extw->extract();
		
		result.push_back(bitstring(ri)); 
		
		delete temp_extw;	
	}
		


return result;
}


/*int main(){

srand(time(0));
//LookAheadExtractor la1 = LookAheadExtractor();
//LookAheadExtractor la1 = LookAheadExtractor();


	vector <bitstring> r_auth;


	bitstring x_auth = bitstring((size_t)8192,rand()); //TODO must have at least 256bits for S1 and at least 512 for Q to create a Composition
	//x_auth = bitstring(8192, rand()); 

	LookAheadExtractor ext_auth(N, K, D, L, 6, 0); // dummy for tests
	//LookAheadExtractor ext_auth(1024,512,512,256,0,0);


	vector < vector <bitstring> > extwSVector;
	vector<  vector <bitstring> > extqSVector;
	vector<int> deltas;
	
	
	bitstring w = bitstring((size_t)8192,rand());

	r_auth = ext_auth.extract(w, x_auth,extqSVector,extwSVector,deltas);


	for(int k=0;k<(int)deltas.size(); k++){

		//cout << "delta["<<k<<"]= "<<deltas[k]<<endl;
	}

	LookAheadExtractor receptor(N, K, D, L, 6, 0);
	
	vector<bitstring> result2 = receptor.extract1(w,x_auth,extqSVector,extwSVector,deltas);

	int vale=(int)result2.size()-1;
	for (int i=0; i< (int) result2.size();i++){
		cout << "R_xxxx["<<i<< "]= "<<result2.at(i).to_string() << endl;	
	}
	cout << endl;
	for (int i=0; i< (int) r_auth.size();i++){

			cout << "R_auth["<<i<< "]= "<<r_auth.at(i).to_string() << endl;	
	
	}

}*/
