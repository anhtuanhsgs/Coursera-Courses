#include <bits/stdc++.h>

using namespace std;

#define maxn 500
#define oo 2000000000

struct Customer;

int n, m, capacity;
double dist [maxn][maxn];

void input ();
void init ();
void distance_greedy_sol ();
void process ();
void print_sol ();

struct Customer {
	int demand, index;
	double x, y, cost_val;

	Customer* l;
	Customer* r;

	Customer (){
		l = NULL;
		r = NULL;
		cost_val = -1;
	};

	double cost (){
		if (cost_val < 0){
			return dist[index][l->index] + dist[index][r->index];
		}
		return cost_val;
	}
};

struct Route {
	Customer* start;
	Customer* end;
	double cost;
	double cap;

	Route (double x, double y){
		start = new Customer ();
		end = new Customer ();
		start->index = end->index = 0;
		start->r = end;
		start->x = end->x = x;
		start->y = end->y = y;
		cost = cap = 0;
	}
};

istream& operator >> (istream& is, Customer& c){
	is >> c.demand >> c.x >> c.y;
	return is;
}

ostream& operator << (ostream& os, const Customer& c){
	os << c.demand << ' ' << c.x << ' ' << c.y;
	return os;
}

vector <Customer*> cus;
vector <Route> rou;

int main (){
	input ();
	init ();
	process ();
	print_sol ();
}

void input (){
	freopen ("tmp.data", "r", stdin);
	cin >> n >> m >> capacity;
	cus.assign (n, NULL);
	for (int i = 0; i < n; i++){
		cus[i] = new Customer();
		cin >> *cus [i];
		cus[i]->index = i;
	}

}

void init (){
	// Build distance mat
	for (int i = 0; i < n; i++){
		for (int j = i + 1; j < n; j++){
			double tmp =  sqrt((cus[i]->x - cus[j]->x) * (cus[i]->x - cus[j]->x) +
				(cus[i]->y - cus[j]->y) * (cus[i]->y - cus[j]->y));
			dist[i][j] = dist[j][i] = tmp;
		}
	}
	// Setup the route
	rou.assign (m, Route (cus[0]->x, cus[0]->y));
	for (int i = 0; i < m; i++){
		rou[i] = Route (cus[0]->x, cus[0]->y);
	}
}

void process (){
	distance_greedy_sol ();
}

void distance_greedy_sol (){
	int tmp = n - 1;
	bool picked[555];

	memset (picked, 0, sizeof (picked));
	while (tmp--){
		double mcost = oo;
		Customer* best_pos, *best_cus;
		int car = -1;
		for (int i = 1; i < n; i++){
			if (picked[i]) continue;
			for (int j = 0; j < m; j++){
				if (rou[j].cap + cus[i]->demand <= capacity){
					Customer *cur = rou[j].start;

					while (cur != rou[j].end){
						// cerr << cur->index << endl;
						double cost = dist[cur->index][cus[i]->index] + dist[cur->r->index][cus[i]->index]
									- dist[cur->index][cur->r->index];

						if (mcost > cost){
							mcost = cost;
							best_pos = cur;
							best_cus = cus[i];
							car = j;
						}
						cur = cur->r;
					}
				}
			}
		}
		// cerr << best_cus->index << ' ' << mcost << endl;
		if (mcost == oo){
			cerr << "Fuk!!!! " << tmp << endl;  
		}
		else{
			best_cus->r = best_pos->r;
			best_cus->l = best_pos;
			best_pos->r->l = best_cus;
			best_pos->r = best_cus;
			picked[best_cus->index] = 1;
			rou[car].cap += best_cus->demand;
			rou[car].cost += mcost;

			// cerr << best_cus->l->index << ' ' << best_cus->index << ' ' << best_cus->r->index << endl;
		}
	}

	// double cc = 0;
	// for (int i = 1; i < n; i++){
	// 	if (!picked[i]) cerr << "Fuk " << i << endl;
	// }
	// for (int i = 0; i < m; i++){
	// 	cerr << rou[i].cap << endl;
	// }
	// cerr << cc;
}

void print_sol (){
	double total_cost = 0;
	for (int i = 0; i < m; i++)
		total_cost += rou[i].cost;

	cout << total_cost << endl;

	for (int i = 0; i < m; i++){
		Customer *cur = rou[i].start;
		while (cur != rou[i].end){
			cout << cur->index << ' ';
			cur = cur->r;
		}
		cout << 0 << endl;
	}
}