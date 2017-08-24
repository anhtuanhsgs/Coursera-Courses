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
	double swap (Customer *l, Customer* r, int sum_cap);
	double calculate_swap (Customer *l, Customer* r, int sum_cap);
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

// insert a customer after this customer
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
		this->r->l = other;
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


// swap this customer with a sequence of customer in another car
// currently wrong changing cost
double Customer::swap (Customer* L, Customer* R, int sum_cap){

	double old_cost = this->cost () + dist [L->index][L->l->index] + dist[R->index][R->r->index];
	double new_cost = 0;

	// cerr << this->l->index << ' ' << this->r->index << endl;

	L->l->r = this;
	R->r->l = this;
	this->l->r = L;
	this->r->l = R;

	rou[L->picked].cap += this->demand - sum_cap;
	rou[L->picked].cost -= (dist [L->index][L->l->index] + dist[R->index][R->r->index]);
	rou[this->picked].cap += sum_cap - this->demand;
	rou[this->picked].cost -= this->cost ();

	Customer *tmp_l = this->l, *tmp_r = this->r;
	// cerr << tmp_l->index << ' ' << tmp_r->index << endl;

	this->l = L->l;
	this->r = R->r;

	L->l = tmp_l;
	R->r = tmp_r;

	// cerr << "dfasdf " << L->l->index << ' ' << R->r->index << endl;

	std::swap (this->picked, L->picked);

	rou[this->picked].cost += this->cost ();
	rou[L->picked].cost += dist[L->index][L->l->index] + dist[R->index][R->r->index];
	new_cost += this->cost () + dist [L->index][L->l->index] + dist[R->index][R->r->index];
	int tmp_picked = L->picked;
	while (L != R->r){
		L->picked = tmp_picked;
		L = L->r;
	}

	return new_cost - old_cost;
}

// change in cost after swap
double Customer::calculate_swap (Customer* L, Customer* R, int sum_cap){
	double old_cost = this->cost () + dist [L->index][L->l->index] + dist[R->index][R->r->index];
	double new_cost = dist [this->index][L->l->index] + dist[this->index][R->r->index];
	new_cost += dist[this->l->index][L->index] + dist[this->r->index][R->index];
	return new_cost - old_cost;
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
	// int tmp = 0;
	// for (int i = 0; i < m; i++){
	// 	tmp += rou[i].cap;
	// 	cerr << "asdfas " << rou[i].cap << endl;
	// }
	// cerr << "sum " << tmp << ' ' << 200 * (m) << endl;
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
	
	// cerr << cc;
}

void fix_failure (){
	// Overload
	int overload = 0;
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
			overload += max (0.0, rou[best_pos->picked].cap - capacity);
		}
	}


	// print_sol ();
	// cerr << cus[20]->r->index << endl;
	// cerr << cus[5]->l->index << ' ' << cus[5]->r->index << endl;	

	// Fix overload
	int iter = 1000;
	while (overload != 0){
		Customer* best_cus, *bL, *bR;
		double best_cost_change = oo, overload_change = 0, tmp_sum_cap = 0;
		for (int i = 0; i < m; i++){
			// cerr << i << endl;
			if (rou[i].cap > capacity){

				Customer *cus_i = rou[i].start->r;
				while (cus_i != rou[i].end){
					// cerr << cus_i->index << ' ' << cus_i->r->index << endl;
					for (int c = 1; c < n; c++){
						if (cus[c]->picked != i){
							int j = cus[c]->picked;
							Customer *L = cus[c];
							Customer *R = L;
							int sum_cap = L->demand;
							int d_overload = max (0., rou[j].cap + cus_i->demand - sum_cap - capacity);
							d_overload += max (0., rou[i].cap - cus_i->demand + sum_cap - capacity);
							d_overload = d_overload - ((rou[i].cap - capacity) + max (0., rou[L->picked].cap - capacity));

							// d_overload = - ((rou[i].cap - capacity) + max (0., rou[j].cap - capacity));

							if (d_overload < 0){
								if (best_cost_change > cus_i->calculate_swap (L, R, sum_cap)){
									best_cost_change = cus_i->calculate_swap (L, R, sum_cap);
									tmp_sum_cap = sum_cap;
									best_cus = cus_i;
									bL = L;
									bR = R;
									overload_change = d_overload;
								}
							}
						}	
					}
					cus_i = cus_i->r;
				}
				
			}
		}

		if (best_cost_change != oo){
			// cerr << overload << endl;
			// cerr << overload_change << ' ' << bL->index << ' ' << bR->index << ' ' << best_cus->index << endl;
			best_cus->swap (bL, bR, tmp_sum_cap);
			overload += overload_change;
		}
		else {
			cerr << "Fuk";
		}
	}
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