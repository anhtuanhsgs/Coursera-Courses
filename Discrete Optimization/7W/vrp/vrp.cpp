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
void fix_failure ();

struct Customer {
	int demand, index;
	double x, y, cost_val;
	int picked;

	Customer* l;
	Customer* r;

	Customer (){
		picked = -1;
		l = NULL;
		r = NULL;
		cost_val = -1;
	};

	double cost (){
		if (!this->l){
			cost_val = -1;
			return 0;
		}

		if (cost_val < 0){
			return cost_val = dist[index][l->index] + dist[index][r->index];
		}
		return cost_val;
	}

	double insert (Customer *other);
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

double Customer::insert(Customer *other){
	{

		double old_cost = dist[this->index][this->r->index] + other->cost ();
		double new_cost = 0;
		if (other->l != NULL){

			other->l->r = other->r;
			other->r->l = other->l;

			new_cost += dist[other->l->index][other->r->index];
			rou[other->picked].cap -= other->demand;
			rou[other->picked].cost -= other->cost ();
		}
		rou[this->picked].cost -= dist[index][r->index];
		other->r = this->r;
		other->l = this;
		this->r = other;
		other->picked = this->picked;
		rou[this->picked].cap += other->demand;
		rou[this->picked].cost += other->cost ();

		new_cost += dist[this->index][other->index];
		
		return new_cost - old_cost;
	}
}

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
		rou[i].start->picked = i;
		cus.push_back (rou[i].start);
	}
}

void process (){
	distance_greedy_sol ();
	fix_failure ();
}

void distance_greedy_sol (){
	int tmp = n - 1;
	bool picked[555];

	memset (picked, 0, sizeof (picked));
	while (tmp--){
		double mcost = oo;
		Customer* best_pos, *best_cus;
		for (int i = 1; i < n; i++){
			if (cus[i]->picked >= 0) continue;
			for (int j = 0; j < m; j++){
				if (rou[j].cap + cus[i]->demand <= capacity){
					Customer *cur = rou[j].start;

					while (cur != rou[j].end){
						double cost = dist[cur->index][cus[i]->index] + dist[cur->r->index][cus[i]->index]
									- dist[cur->index][cur->r->index];

						if (mcost > cost){
							mcost = cost;
							best_pos = cur;
							best_cus = cus[i];
						}
						cur = cur->r;
					}
				}
			}
		}
		// cerr << best_cus->index << ' ' << mcost << endl;
		if (mcost == oo){
			// cerr << "Fuk!!!! " << tmp << endl;  
		}
		else{
			best_pos->insert (best_cus);
		}
	}

	// double cc = 0;
	// for (int i = 1; i < n; i++){
	// 	if (cus[i]->picked < 0) cerr << "Fukkk " << i << endl;
	// }
	// for (int i = 0; i < m; i++){
	// 	cerr << rou[i].cap << endl;
	// }
	// cerr << cc;
}

void fix_failure (){
	// Overload
	for (int i = 1; i < n; i++){
		if (cus[i]->picked < 0){
			double mcost = oo;
			Customer *best_pos;
			for (int j = 1; j < cus.size (); j++){
				if (cus[j]->picked < 0) continue;
				double cost = dist[cus[i]->index][cus[j]->index] + dist[cus[i]->index][cus[j]->r->index]
							 - dist[cus[j]->index][cus[j]->r->index];

				if (cost < mcost){
					mcost = cost;
					best_pos = cus[j];
				}
			}

			best_pos->insert (cus[i]);
		}
	}

	// Fix overload
	
}

void print_sol (){
	double total_cost = 0;
	for (int i = 0; i < m; i++)
		total_cost += rou[i].cost;

	cout << total_cost << ' ' << 0 << endl;

	for (int i = 0; i < m; i++){
		Customer *cur = rou[i].start;
		while (cur != rou[i].end){
			cout << cur->index << ' ';
			cur = cur->r;
		}
		cout << 0 << endl;
	}
}