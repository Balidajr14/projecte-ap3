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

bool count_finestra(const vector<Classe>& classes, const vector<int>& sortida, int nfin, int m, int k, int& pen) {
	int counter = -proporcions[m].first;
	// Recorrem la finestra de k a k - nfin
	for (int i = 0; i < nfin; ++i){
		int id = sortida[k - i];
		// Si trobem una millora sumem al counter
		if (classes[id].millores[m]) {
			if (++counter > 0) ++pen;
			if (pen >= pen_opt) return true;
		}
	}
	return false;
}

void act_pen(const vector<Classe>& classes, const vector<int>& sortida,
				int id, int k, int& pen) {
	// Finestres fins a k
	for (int j = 0; j < M; ++j) {
		int nfin = min(proporcions[j].second, k + 1);
		if (count_finestra(classes, sortida, nfin, j, k, pen)) return;
	}
	
	// Últimes finestres
	if (k == C - 1) {
		for (int j = 0; j < M; ++j) {
			int nfin = min(proporcions[j].second, k + 1); 
			while (--nfin > 0) {
				if (count_finestra(classes, sortida, nfin, j, k, pen)) {
					return; // pen >= pen_opt
				}
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

void search(int k, vector<Classe>& classes, vector<int>& sortida, int pen) {
	// Tenim una possible solució
	if (k == C) {
		pen_opt = pen;
		sortida_opt = sortida;
		overwrite(pen_opt, sortida_opt);
	} else {
		for (int id = 0; id < K; ++id){
			if (classes[id].ncars > 0) { 
				// Millorem cotxe i
				sortida[k] = id;
				--classes[id].ncars; 

				// Actualitzem la penalització
				int pen_ant = pen;
				act_pen(classes, sortida, id, k, pen);
				int pen_dif = pen - pen_ant;
				
				// Seguim amb la millora i
				if (pen < pen_opt) search(k+1, classes, sortida, pen);

				// No millorem el cotxe i
				pen -= pen_dif;
				++classes[id].ncars;
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

	// BMatriu sequencies(M, vector<bool> (C,false));
	search(0, classes, sortida, 0);

}
