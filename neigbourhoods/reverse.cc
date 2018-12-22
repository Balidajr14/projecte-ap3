
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
