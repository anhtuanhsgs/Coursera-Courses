#include <bits/stdc++.h>

using namespace std;

#define fi first
#define se second

typedef pair<int, int> ii;
typedef pair<double, ii> fii;
typedef pair<double, int> fi;

// void gen_init_sol ();
// void input ();
// void init ();

struct Point;
struct GenSol;

struct Point{
	double x, y;

	double dist (const Point& other){
		return sqrt ((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y));
	}

	friend istream& operator >> (istream& is, Point& p){
		is >> p.x >> p.y;
	}
};

int n;
Point p[50111];
vector<fii> e_list;
vector<vector<fi> > adj_list;

int seq[50111];
int ans[50111]; 
double cost_ans;
// priority_queue < fii, vector <fii>, greater <fii> >  e_inSeq;

vector< vector < double > > adj_mat;
vector< vector<int> > tmp;
vector <bool> inSeq;
int g[50111];

void apply_ans (){
	for (int i = 0; i < n; i++)
		ans[i] = seq[i];
}

void re_apply_ans (){
	for (int i = 0; i < n; i++)
		seq[i] = ans[i];
}

double compute_cost (){
	double ret = 0;
	for (int i = 0; i < n; i++){
		if (n <= 10000) ret += adj_mat[ans[i]][ans[(i+1) % n]];
		else ret += p[ans[i]].dist (p[ans[ (i + 1) % n]]);
	}
	return ret;
}

vector <int> not_inQ;

struct GenSol{

	void init_mst (){
		for (int i = 0; i < n; i++)
			g[i] = i;
	}

	int findG (int u){
		return u == g[u] ? u : g[u] = findG (g[u]);
	}

	bool sameG (int u, int v){
		return findG(u) == findG(v);
	}

	void _union (int u, int v){
		g[findG (u)] = g[findG (v)];
	}

	double gen_mst (){
		init_mst ();
		// e_inSeq.clear ();
		tmp.assign (n, vector<int> ());
		double cost = 0;
		cerr << "here come the shit" << endl;
		for (fii e : e_list){
			int u = e.se.fi, v = e.se.se;
			if (sameG (u, v) || tmp[u].size () > 1 || tmp[v].size () > 1) continue;
			tmp[u].push_back (v);
			tmp[v].push_back (u);
			_union (u, v);
			cost += e.fi;
		}

		int s = 0, id = 0;
		for (; s < n; s++)
			if (tmp[s].size () == 1) break;
		inSeq.assign (n, 0);

		while (1){
			if (inSeq[s]) break;
			inSeq[s] = 1;
			seq[id++] = s;
			for (int v : tmp[s]){
				if (inSeq[v]) continue;
				s = v;
			}
		}
		cost += p[seq[n-1]].dist(p[seq[0]]);

		for (int i = 1; i < n; i++){
			if (!sameG (i, i-1)){
				cerr << "fuck" << ' ' << i << endl;
				return cost;
			}
		}

		return cost;
	}

	double gen_random (){
		for (int i = 0; i < n; i++) seq[i] = i;
		random_shuffle (seq, seq + n);
		return 0;
	}

	double gen_dfs_minE (){
		cerr << "oh the shit 2" << endl;
		int s = rand () % n, id = 0;
		inSeq.assign (n, 0);
		double cost = 0;

		if (n >= 10000){
			for (int i = 0; i <n; i++){
				if (inSeq[s]) break;
				inSeq[s] = 1;
				seq[id++] = s;
				for (fi dv : adj_list[s]){
					int v = dv.se;
					double d = dv.fi;
					if (v != s && ! inSeq[v]){
						s = v;
						cost += d;
						break;
					}
				}
			}
			cost += p[seq[n-1]].dist (p[seq[0]]);
			for (int i = 0; i < n; i++){
				if (!inSeq[i]) {
					not_inQ.push_back (i);
				}
			}
			int nleft = not_inQ.size ();
			while (nleft--){
				double minDist = -1;
				int nextS = -1;
				for (int v : not_inQ){
					if (inSeq[v]) continue;
					double d = p[s].dist (p[v]);
					if (minDist < 0 || (minDist > d && minDist > 0)) {
						minDist = d;
						nextS = v;
					}
				}
				cost += p[s].dist(p[nextS]);
				s = nextS;
				inSeq[s] = 1;
				seq[id++] = s;
			}
			for (int i = 0; i < n; i++){
				if (!inSeq[i]) {
					cerr << "FUKKKKKKK" << endl;
					break;
				}
			}
		}
		else {
			for (int i = 0; i < n; i++){

				inSeq[s] = 1;
				seq[id++] = s;
				double minDist = -1;
				int nextS = -1;
				for (int v = 0; v < n; v++){
					if (v != s && !inSeq[v]){
						double d;
						d = adj_mat[s][v];
						if (minDist < 0 || minDist > d){
							minDist = d;
							nextS = v;
						}
					}
				}
				cost += adj_mat[s][nextS];
				s = nextS;
			}
			cost += adj_mat[seq[n-1]][seq[0]];
		}
		return cost;
	}
};

bool cmp1 (Point p1, Point p2){
	if (p1.x < p2.x) return 1;
	if (p1.x > p2.x) return 0;
	if (p1.y < p2.y) return 1;
	return 0;
}

bool cmp2 (Point p1, Point p2){
	if (p1.y < p2.y) return 1;
	if (p1.y > p2.y) return 0;
	if (p1.x < p2.x) return 1;
	return 0;
}

void init (){
	if (n < 10000){
		srand(time(0));
		adj_mat.assign (n, vector <double> ());
		adj_list.assign (n, vector <fi> ());
		for (int i = 0; i < n; i++)
			adj_mat[i].assign (n, 0);
		for (int i = 0; i < n; i++){
			for (int j = i + 1; j < n; j++){
				double d = p[i].dist(p[j]);
				e_list.push_back (fii (d, ii (i, j)));
				adj_mat[i][j] = adj_mat[j][i] = d;
				adj_list[i].push_back (fi (d, j));
				adj_list[j].push_back (fi (d, i));
			}
		}

		sort (e_list.begin (), e_list.end ());
	}
	else {
		double interval = 55000;
		double rad = 600000;
		sort (p, p + n, cmp1);
		adj_list.assign (n, vector <fi> ());
		for (int r = 1; r < n; r++){
			for (int i = 0; i < n; i++){
				int j = i + r;
				if (j > n || abs (p[i].x - p[j].x) > interval)
					break;
				double d = p[i].dist(p[j]);
				if (d > rad) continue;
				e_list.push_back (fii (d, ii (i, j)));
				// adj_list[i].push_back (fi (d, j));
				// adj_list[j].push_back (fi (d, i));
			}
		}
		sort (p, p + n, cmp2);
		for (int r = 1; r < n; r++){
			for (int i = 0; i < n; i++){
				int j = i + r;
				if (j > n ||  abs (p[i].y - p[j].y) > interval)
					break;
				double d = p[i].dist(p[j]);
				if (d > rad) continue;
				e_list.push_back (fii (d, ii (i, j)));
				// adj_list[i].push_back (fi (d, j));
				// adj_list[j].push_back (fi (d, i));
			}
		}
		// cerr << "finish the init" << endl;
		// for (int i = 0; i < n; i++){
		// 	sort (adj_list[i].begin (), adj_list[i].end ()); 
		// }
		// cerr << "Oh " << e_list.size () <<  endl;
		// sort (e_list.begin (), e_list.end ());
	}
}

void input (){
	freopen ("tmp.data", "r", stdin);
	// freopen ("a.out", "w", stdout);
	
	ios_base::sync_with_stdio(0);
	cin >> n;

	for (int i = 0; i < n; i++){
		cin >> p[i];
	}
}

double _2_opt (){
	re_apply_ans ();
	double cost = cost_ans;
	int s = rand () % n;
	for (int i = 0; i < n; i++){
		int u = seq[(s + i) % n], u1 = seq[(s + i + 1) % n];

		double best_change = 1;
		int swap_post;

		for (int j = i + 2; j < n; j++){
			int v = seq[(s + j) % n], v1 = seq[(s + j + 1) % n];
			double cur_cost = adj_mat[u][u1] + adj_mat[v][v1];
			double new_cost = adj_mat[u][v] + adj_mat [u1][v1];
			double change = new_cost - cur_cost;

			if (best_change > change){
				best_change = change;
				swap_post = j;
			}
		}

		if (best_change < 0){
			// cerr << best_change << endl;
			// for (int j = i + 1; j <= swap_post;j ++)
			// 	cerr << seq[j] << ' ';
			// cerr << endl;
			for (int j = i + 1; j <= swap_post; j++)
				ans[(s + j) % n] = seq[(s + swap_post - j + i + 1) % n];
			for (int j = i + 1; j <= swap_post; j++)
				seq[(s + j) % n] = ans[(s + j) % n];
			// for (int j = i + 1; j <= swap_post;j ++)
			// 	cerr << seq[j] << ' ';
			// cerr << endl;
			cost += best_change;
		}
		// cerr << best_change << endl;
		
	}
	// cerr << compute_cost () << endl;
	// cerr << cost << endl;
	
	return cost;
}

void process (){

	GenSol gen;
	double cost = 2000000000;
	cost = gen.gen_mst ();
	apply_ans ();
	// double cost_dfs;
	// cost_dfs = gen.gen_dfs_minE ();

	// // // gen.gen_random ();
	// // // apply_ans ();

	// if (cost > cost_dfs){
	// 	cost = cost_dfs;
	// 	apply_ans ();
	// }
	// cost_ans = cost;
	// cost_ans = compute_cost ();
	// cerr << "111 " << cost_ans << endl;
	// cerr << "111 " << compute_cost () << endl;

	if (n < 10000){
		int _2_opt_max_iter = 800;
		for (int iter = 0; iter < _2_opt_max_iter; iter++){
			cost_ans = _2_opt ();
		}
	}
	
	cout << compute_cost () << ' ' << 0 << endl;
	for (int i = 0; i < n; i++)
		cout << ans[i] << ' ';

}

int main (){
	input ();
	init ();
	process ();
// 	gen_init_sol ();
	return 0;
}