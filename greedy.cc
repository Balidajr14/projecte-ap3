#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int, int> pii;
typedef vector<int> VI;
typedef vector<VI> Matriu;

struct Classe {
	int id;
	int ncars;
	vector<bool> millores; 
};

// Variables Globals
string fileout;

int pen_opt;
int C, M, K;

vector<pii> proporcions;
vector<int> sortida;
vector<Classe> classes;

clock_t tStart;

bool count_finestra(int nfin, int m, int k, int& pen) {
	
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

void act_pen(int k, int& pen) {
	
	// Finestres fins a k
	for (int m = 0; m < M; ++m) {
		int nfin = min(proporcions[m].second, k + 1);
		if (count_finestra(nfin, m, k, pen)) return;
	}
	
	// Últimes finestres
	if (k == C - 1) {
		for (int m = 0; m < M; ++m) {
			int nfin = min(proporcions[m].second, k + 1); 
			while (--nfin > 0) if (count_finestra(nfin, m, k, pen)) return;
		}
	}
}

void overwrite() {
    ofstream out;
    out.open(fileout, std::ios::trunc);
    
    out.setf(ios::fixed);
	out.precision(1);
	
	double temps = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	out << pen_opt << " " << temps << endl;
	out << sortida[0];
	for (int i = 1; i < C; ++i) out << " " << sortida[i];
	out << endl;
    
    // Debugging porpouses
    cout << pen_opt << " " << temps << endl;
	cout << sortida[0];
	for (int i = 1; i < C; ++i) cout << " " << sortida[i];
	cout << endl;
      
    out.close();
}

void greedy(vector<int>& sortida) {
    int total_pen = 0;
  
	// Ara fem tots de 1..C-1
    for (int k = 0; k < C; ++k) {
        int min_pen = 1e9;
        int min_id = 0;

        
        for (int i = 0; i < K; ++i){
			// Si no hi ha cotxes a millorar de la classe i, continue
            if (classes[i].ncars == 0) continue;
            
            // Calculem la penalització
            
            int pen_ant = total_pen;
            sortida[k] = i;
            act_pen(k, total_pen);
            int pen_dif = total_pen - pen_ant;
            total_pen = pen_ant;
                        
            bool isminpen = pen_dif < min_pen;
            bool ismaxcar = pen_dif == min_pen
							and classes[i].ncars > classes[min_id].ncars;
            
            // Mirem si és mínima la penalització
            // Si són iguals, mirem que no se'ns acumulin cotxes
            if (isminpen or ismaxcar) {
                min_pen = pen_dif;
                min_id = i;
            }
		}
		
        // Canviem el valor de la columna k
        sortida[k] = min_id;
        act_pen(k, total_pen);
        --classes[min_id].ncars;
    }
    
    // Escrivim el resultat
    pen_opt = total_pen;
    overwrite();
}

int main(int argc, char** argv) {
	
	string filein = argv[1];
	fileout = argv[2];
	
    ifstream in(filein);
    
	//Lectura de les dades del fitxer
	in >> C >> M >> K;
	proporcions = vector< pair<int,int> > (M);

	for (pii& prop : proporcions) in >> prop.first;
	for (pii& prop : proporcions) in >> prop.second;

	classes = vector<Classe> (K);

	for (Classe& cl : classes) {
		in >> cl.id; // id de la classe
		in >> cl.ncars; // # de cotxes
		cl.millores = vector<bool> (M);
		for (int i = 0; i < M; ++i) {
			bool a;
			in >> a;
			cl.millores[i] = a;
		}
	}

    in.close();

	pen_opt = 1e9;
	sortida = vector<int> (C);

	tStart = clock();

	greedy(sortida);
}

 
