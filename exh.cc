#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int, int> pii;
typedef vector<int> VI;
typedef vector<VI> Matriu;
typedef vector<vector<bool>> BMatriu;

// Variables Globals
ifstream in;
string fileout;

int pen_opt;
int C, M, K;
vector<pii> proporcions;
vector<int> sortida_opt;

clock_t tStart;

struct Classe {
	int id;
	int ncars;
	vector<bool> millores; 
};

void change_k_col(const vector<Classe>& classes, BMatriu& sequencies, int id, int k) {
	for (int j = 0; j < M; ++j) {
		sequencies[j][k] = classes[id].millores[j];
	}
}

int count_finestra(BMatriu& seq, int nfin, int j, int k) {
	int counter = 0;
	for (int i = 0; i < nfin; ++i){
		if (seq[j][k - i]) ++counter;
	}
	return counter;
}

void act_pen(const vector<Classe>& classes, BMatriu& sequencies,
				int id, int k, int& pen) {
	// Finestres fins a k
	for (int j = 0; j < M; ++j) {
		int nfin = min(proporcions[j].second, k + 1);
		int mills_fin = count_finestra(sequencies, nfin, j, k);
		pen += max(0, mills_fin - proporcions[j].first);
		if (pen >= pen_opt) return;
	}
	
	// Últimes finestres
	if (k == C - 1) {
		for (int j = 0; j < M; ++j) {
			int nfin = min(proporcions[j].second, k + 1); 
			while (--nfin > 0) {
				int mills_fin = count_finestra(sequencies, nfin, j, k);
				pen += max(0, mills_fin - proporcions[j].first);
				if (pen >= pen_opt) return;
			}
		}
	}
}

void overwrite(int pen, const vector<int>& sortida) {
	
	ofstream out(fileout);
	out.setf(ios::fixed);
	out.precision(1);
	
	double temps = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	out << pen << " " << temps << endl;
	out << sortida[0];
	for (int i = 1; i < C; ++i) out << " " << sortida[i];
	out << endl;
	
	out.close();
	
	// Debugging porpouses
	cout << pen << " " << temps << endl;
	cout << sortida[0];
	for (int i = 1; i < C; ++i) cout << " " << sortida[i];
	cout << endl;
}

void search(int k, vector<Classe>& classes, BMatriu& sequencies, vector<int>& sortida, int pen) {
	// La solució no és millor que la que ja hem calculat
	if (pen >= pen_opt) return; 
	
	// Tenim una possible solució
	if (k == C) { 
		pen_opt = pen;
		sortida_opt = sortida;
		overwrite(pen_opt, sortida_opt);
	} else {
		for (int i = 0; i < K; ++i){
			if (classes[i].ncars > 0) { 
				// Millorem cotxe i
				sortida[k] = i;
				--classes[i].ncars; 

				// Actualitzem la penalització
				int pen_ant = pen;
				change_k_col(classes, sequencies, i, k);
				act_pen(classes, sequencies, i, k, pen);
				int pen_dif = pen - pen_ant;
				
				// Seguim amb la millora i
				search(k+1, classes, sequencies, sortida, pen);

				// No millorem el cotxe i
				pen -= pen_dif;
				++classes[i].ncars;
			}
		}
	}
}

int main(int argc, char** argv) {
	// Rebem els fitxers per la terminal
	fileout = argv[2];
	in.open(argv[1]);
	
	//Lectura de les dades del fitxer
	in >> C >> M >> K;
	proporcions = vector< pair<int,int> > (M);

	for (pii& prop : proporcions) in >> prop.first;
	for (pii& prop : proporcions) in >> prop.second;

	vector<Classe> classes(K);
	
	for (Classe& cl : classes) {
		in >> cl.id; // id de la classe
		in >> cl.ncars; // # de cotxes
		cl.millores = vector<bool> (M);
		for (int i = 0; i < M; ++i) {
			bool a; in >> a;
			cl.millores[i] = a;
		}
	}

	in.close();

	pen_opt = 1e9;
	sortida_opt = vector<int> (C);
	vector<int> sortida(C);

	tStart = clock();

	BMatriu sequencies(M, vector<bool> (C,false));
	search(0, classes, sequencies, sortida, 0);

}
