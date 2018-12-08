#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef pair<int, int> pii;
typedef vector<int> VI;
typedef vector<VI> Matriu;
typedef vector<vector<bool>> BMatriu;

// Variables Globals
int pen_opt;
int C, M, K;
vector<pii> proporcions;
vector<int> sortida_opt;

string filein, fileout;
clock_t tStart;

struct Classe {
	int id;
	int ncars;
	vector<bool> millores; 
};

int count_finestra(BMatriu& seq, int nfin, int j, int k) {
	int counter = 0;
	for (int i = 0; i < nfin; ++i){
		if (seq[j][k - i]) ++counter;
	}
	return counter;
}

int act_pen(const vector<Classe>& classes, BMatriu& sequencies, int id, int k) {
	int total_pen = 0;
	// Finestres fins a k
	for (int j = 0; j < M; ++j) {
		sequencies[j][k] = classes[id].millores[j];
		int nfin = min(proporcions[j].second, k + 1);
		int mills_fin = count_finestra(sequencies, nfin, j, k);
		total_pen += max(0, mills_fin - proporcions[j].first);
    }
    // Últimes finestres
	if (k == C - 1) {
		for (int j = 0; j < M; ++j) {
			int nfin = min(proporcions[j].second, k + 1); 
			while (--nfin > 0) {
				int mills_fin = count_finestra(sequencies, nfin, j, k);
				total_pen += max(0, mills_fin - proporcions[j].first);
			}
		}
	}
	return total_pen;
}

void overwrite() {
    ofstream out;
    out.open(fileout, std::ios::trunc);
    
    out.setf(ios::fixed);
	out.precision(1);
	
	double temps = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	out << pen_opt << " " << temps << endl;
	out << sortida_opt[0];
	for (int i = 1; i < C; ++i) out << " " << sortida_opt[i];
	out << endl;
    
    // Debugging porpouses
    cout << pen_opt << " " << temps << endl;
	cout << sortida_opt[0];
	for (int i = 1; i < C; ++i) cout << " " << sortida_opt[i];
	cout << endl;
      
    out.close();
}

// Agafem la classe que té més cotxes com a primera
int get_maxcars_id(vector<Classe>& classes) {
	int max_cars = 0;
	int max_cars_id = 0;
	for (Classe car : classes) {
		if (car.ncars > max_cars) {
			max_cars = car.ncars;
			max_cars_id = car.id;
		}
	}
	return max_cars_id;
}

void greedy(vector<Classe>& classes, BMatriu& sequencies, vector<int>& sortida) {
    int total_pen = 0;
    
	// Canviem el valor de k = 0
	int first_mill = get_maxcars_id(classes);
	sortida[0] = first_mill;
	total_pen += act_pen(classes, sequencies, first_mill, 0);
	--classes[first_mill].ncars;

	// Ara fem tots de 1..C-1
    for (int k = 1; k < C; ++k) {
        int min_pen = 1e9;
        int min_id = 0;
        
        for (int i = 0; i < K; ++i){
			// Si no hi ha cotxes a millorar de la classe i, continue
            if (classes[i].ncars == 0) continue;
            
            // Calculem la penalització
            int pen_i = act_pen(classes, sequencies, i, k);
                        
            bool isminpen = pen_i < min_pen;
            bool ismaxcar = pen_i == min_pen
							and classes[i].ncars > classes[min_id].ncars;
            
            // Mirem si és mínima la penalització
            // Si són iguals, mirem que no se'ns acumulin cotxes
            if (isminpen or ismaxcar) {
                min_pen = pen_i;
                min_id = i;
            }
		}
		
        // Canviem el valor de la columna k
        sortida[k] = min_id;
        total_pen += act_pen(classes, sequencies, min_id, k);
        --classes[min_id].ncars;
    }
    
    // Escrivim el resultat
    pen_opt = total_pen;
    sortida_opt = sortida;
    overwrite();
}

int main(int argc, char** argv) {
	
	filein = argv[1];
	fileout = argv[2];
	
    ifstream in(filein);
    
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
			bool a;
			in >> a;
			cl.millores[i] = a;
		}
	}

    in.close();

	pen_opt = 1e9;
	sortida_opt = vector<int> (C);
	vector<int> sortida(C);

	tStart = clock();

	BMatriu sequencies(M, vector<bool> (C,false));
	greedy(classes, sequencies, sortida);
}

 
