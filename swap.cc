
int INF = 1e9;

pair<int, int> best_swap(int fix) {
	int min_id = 0;
	int min_val = INF;
	
	for (int var = 0; var < n; ++var) {
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
	
	for (int i = 0; i < n; ++i) {
		pair<int, int> best_var = best_swap(fix);
		if (best_var.first < min_fix_val) {
			min_fix_val = best_var.first;
			min_fix_id = i;
		}
	}
	
	int fix = min_fix_id;
	int var = best_swap(min_fix_id);
	swap(sol[fix], sol[var]);
}
