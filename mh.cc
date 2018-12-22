#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>


using namespace std;

typedef pair<int, int> pii;
typedef vector<int> VI;
typedef vector<VI> Matriu;
typedef vector<vector<bool>> BMatriu;

struct Classe {
	int id;
	int ncars;
	vector<bool> millores; 
};

// Variables Globals
int INF = 1e9;
int pen_opt;
int C, M, K;
vector<pii> proporcions;
vector<int> sortida;
vector<Classe> classes;

string filein, fileout;
clock_t tStart;



void count_finestra(int nfin, int m, int k, int& pen_dif) {
	
	int counter = -proporcions[m].first;
	// Recorrem la finestra de k a k - nfin
	for (int i = 0; i < nfin; ++i){
		int id = sortida[k - i];
		// Si trobem una millora sumem al counter
		if (classes[id].millores[m]) {
			if (++counter > 0) ++pen_dif;
		}
	}
}

int act_pen(int k) {
	int pen_dif = 0;

	// Finestres fins a k
	for (int m = 0; m < M; ++m) {
		int nfin = min(proporcions[m].second, k + 1);
		count_finestra(nfin, m, k, pen_dif);
	}
	
	// Últimes finestres
	if (k == C - 1) {
		for (int m = 0; m < M; ++m) {
			int nfin = min(proporcions[m].second, k + 1); 
			while (--nfin > 0) count_finestra(nfin, m, k, pen_dif);
		}
	}

	return pen_dif;
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

vector<int> greedy() {
	int total_pen = 0;

	// Ara fem tots de 1..C-1
    for (int k = 0; k < C; ++k) {
        int min_pen = 1e9;
        int min_id = 0;

        
        for (int i = 0; i < K; ++i){
			// Si no hi ha cotxes a millorar de la classe i, continue
            if (classes[i].ncars == 0) continue;
            
            // Calculem la penalització
            sortida[k] = i;
            int pen_dif = act_pen(k);
                        
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
        total_pen += act_pen(k);
        --classes[min_id].ncars;
    }
    
    // Escrivim el resultat
    pen_opt = total_pen;

    return sortida;
}

int rand_int(int min, int max) {
	// Extret de StackOverflow

	random_device rd;     // only used once to initialise (seed) engine
	mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	uniform_int_distribution<int> uni(min,max); // guaranteed unbiased

	auto random_integer = uni(rng);

	return random_integer;
}

int get_pen() {
	int pen_s = 0;
	for (int k = 0; k < C; ++k) pen_s += act_pen(k);

	return pen_s;
}

// ---------------------- N1 SWAP ------------------------ //

pair<int, int> best_swap(int fix) {
	int min_id = 0;
	int min_val = INF;
	
	for (int var = 0; var < C; ++var) {
		swap(sortida[fix], sortida[var]);
		
		int pen_i = get_pen();
		if (pen_i < min_val) {
			min_val = pen_i;
			min_id = var;
		}
		
		swap(sortida[fix], sortida[var]);
	}
	
	return {min_val, min_id};
}

void best_swap() {
	int min_fix_id = 0;
	int min_fix_val = INF;
	
	for (int i = 0; i < C; ++i) {
		pair<int, int> best_var = best_swap(i);
		if (best_var.first < min_fix_val) {
			min_fix_val = best_var.first;
			min_fix_id = i;
		}
	}
	
	int fix = min_fix_id;
	int var = best_swap(min_fix_id).second;
	swap(sortida[fix], sortida[var]);
}

// ---------------------- N2 REVERSE ------------------------ //

void reverse(int a, int b) {
	VI sortida_new = sortida;
	for (int i = a; i <= b; ++i) {
		sortida_new[i] = sortida[b - i + a];
	}
	sortida = sortida_new;
}

pair<int, int> best_reverse(int fix) {
	int min_id = 0;
	int min_val = INF;
	
	for (int var = fix; var < C; ++var) {
		VI sortida_ant = sortida;
		reverse(fix, var);
		
		int pen_i = get_pen();
		if (pen_i < min_val) {
			min_val = pen_i;
			min_id = var;
		}
		
		sortida = sortida_ant;
	}
	
	return {min_val, min_id};
}

void best_reverse() {
	int min_fix_id = 0;
	int min_fix_val = INF;
	
	for (int i = 0; i < C; ++i) {
		pair<int, int> best_var = best_reverse(i);
		if (best_var.first < min_fix_val) {
			min_fix_val = best_var.first;
			min_fix_id = i;
		}
	}
	
	int fix = min_fix_id;
	int var = best_swap(min_fix_id).second;
	reverse(fix, var);
}

// ---------------------- N3 SWAP ------------------------ //

// ---------------------- N4 SWAP ------------------------ //

void VNS() {
	
	// Solució inicial
	sortida = greedy();
	int i = 0;
	while(i < 20) {
		int id_n = rand_int(1,1);
		//if (id_n == 1) best_swap();
		//else if (id_n == 2) N_block_swap(ini_sol);
		//else if (id_n == 3) N_shift(ini_sol);
		//else 
		best_reverse();
		pen_opt = min(pen_opt, get_pen());
		overwrite();
		++i;
	}
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

	VNS();
}