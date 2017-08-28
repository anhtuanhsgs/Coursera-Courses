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
void optimize ();

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
		if (!l){
			return 0;
		}

		return cost_val = dist[index][l->index] + dist[index][r->index];
	}

	double insert (Customer *other);
	double swap (Customer *l, Customer* r, int sum_cap);
	double calculate_swap (Customer *l, Customer* r, int sum_cap);
	double calculate_insert (Customer* other);
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
			rou[other->picked].cost += dist[other->l->index][other->r->index];
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

// Cost change for insertion
// Both customer has a route
double Customer::calculate_insert(Customer *other){
	double old_cost = dist[index][r->index] + other->cost ();

	double new_cost = dist[index][other->index] + dist[other->index][r->index];
	new_cost += dist[other->l->index][other->r->index];
	return new_cost - old_cost;
}


// swap this customer with a sequence of customer in another car
// currently wrong changing cost
double Customer::swap (Customer* L, Customer* R, int sum_cap){

	double old_cost = this->cost () + dist [L->index][L->l->index] + dist[R->index][R->r->index];
	double new_cost = 0;


	rou[L->picked].cost += dist[this->index][L->l->index] + dist[this->index][R->r->index] 
			- (dist [L->index][L->l->index] + dist[R->index][R->r->index]);
	rou[this->picked].cost += dist [L->index][this->l->index] + dist[R->index][this->r->index] - this->cost (); 

	L->l->r = this;
	R->r->l = this;
	this->l->r = L;
	this->r->l = R;

	rou[L->picked].cap += this->demand - sum_cap;
	rou[this->picked].cap += sum_cap - this->demand;

	Customer *tmp_l = this->l, *tmp_r = this->r;

	this->l = L->l;
	this->r = R->r;

	L->l = tmp_l;
	R->r = tmp_r;

	std::swap (this->picked, L->picked);

	new_cost += this->cost () + dist [L->index][L->l->index] + dist[R->index][R->r->index];
	int tmp_picked = L->picked;
	while (L != R->r){
		L->picked = tmp_picked;
		L = L->r;
	}

	return new_cost - old_cost;
}

struct Ans {
	double cost;
	vector < vector <int> > ans;

	Ans (int n, int m){
		ans.assign (m, vector<int> ());
	}

	void update (double value){

		this->cost = value;
		for (int i = 0; i < m; i++){
			ans[i].clear ();
			Customer *cur = rou[i].start;
			while (cur != rou[i].end){
				ans[i].push_back (cur->index);
				cur = cur->r;
			}
			ans[i].push_back (0);
		}
	}

	void prin (){
		cout << cost << ' ' << 0 << endl;
		for (int i = 0; i < m; i++){
			for (int j = 0; j < ans[i].size (); j++){
				cout << ans[i][j] << ' ';
			}
			cout << endl;
		}
	}
};

Ans ans(0, 0);

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

	srand (time (0));
	ans = Ans (n, m);

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
	optimize ();
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
		if (mcost == oo){
			// cerr << "Fuk!!!! " << tmp << endl;  
		}
		else{
			best_pos->insert (best_cus);
		}
	}
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

	// Fix overload
	int iter = 100;
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
			best_cus->swap (bL, bR, tmp_sum_cap);
			overload += overload_change;
		}
		else {
			cerr << "Fuk";
		}
	}
}

struct Tabu {
	int size;
	queue <unsigned int> q;
	set <unsigned int> s;

	Tabu (){
		//size = 1085 ~ 101_10
		//size = 100 ~ 16_3, 51-5

		size = 2000;
	}

	unsigned int hash (int l, int m, int r){
		return (unsigned int) l * 10000 + m *100 + r;
	}

	void add (int l, int m, int r){
		// cerr << l << ' ' << m << ' ' << r << endl;
		s.insert (hash (l, m, r));
		q.push (hash (l, m, r));

		// cerr << q.front () << ' ' << q.size () << endl;
		if (q.size () > size){
			s.erase (q.front ());
			q.pop ();
		}
	}

	bool has (int l, int m, int r){
		return s.find (hash (l, m, r)) != s.end ();
	}
};

double total (){
	double total_cost = 0;
	for (int i = 0; i < m; i++)
		total_cost += rou[i].cost;
	return total_cost;
}

void optimize (){
	int iter = 1000;
	ans.update (total ());
	int max_step = sqrt (n);

	double min_total = oo;
	Tabu tabu = Tabu ();

	double alpha = 0.99, temp = 200;

	if (n < 100) {
		max_step = 1;
		alpha = 1;
		tabu.size = 100;
	}
	else if (n < 200) {
		iter = 5000;
		tabu.size = 100;
	}
	else tabu.size = 120;

	if (n == 51){
		max_step = n / 7;
		temp = 100;
		alpha = 0.99995;
		tabu.size = 100;
		iter = 30000;
	}
	if (n == 101){
		iter = 11000;
		tabu.size = 200;
		alpha = 0.999;
		temp = 200;
	}
	if (n == 200){
		iter = 3000;
		tabu.size = 200;
	}
	if (n == 421){
		iter = 9000;
		tabu.size = 250;
		alpha = 0.9993;
		temp = 300;
	}

	while (iter--){
		bool _insert, swap, exchange;
		_insert = swap = exchange = 0;
		
		Customer* best_cus, *bL, *bR, *best_pos;
		int tmp_sum_cap = 0;
		double bcost_change = oo;
		//Find best insertion or exchange
		for (int i = 1; i < cus.size (); i++){
			for (int j = 1 + rand () % (max_step); j < n; j++){
				Customer* cus_i = cus[i];
				Customer* cus_j = cus[j];

				//Not next to each other
				if (i != j && cus_i->r != cus_j && cus_j->r != cus_i){
					//Tabue check 
					if (!tabu.has (cus_i->index, cus_j->index, cus_i->r->index)){
						//Calculate insertion
						if (rou[cus_i->picked].cap + cus_j->demand <= capacity){

							if (cus_i->calculate_insert (cus_j) < bcost_change){
								_insert = 1;
								swap = 0;
								exchange = 0;
								// cerr << 111 << endl;
								bcost_change = cus_i->calculate_insert (cus_j);
								best_pos = cus_i;
								best_cus = cus_j;
							}
						}
					}

					//Calculate exhange
					if (cus_i->index != 0){
						//Tabu check 
						if (!tabu.has (cus_i->l->index, cus_j->index, cus_i->r->index) && 
							!tabu.has (cus_j->l->index, cus_i->index, cus_j->r->index)){
							if (rou[cus_i->picked].cap - cus_i->demand + cus_j->demand <= capacity
								&& rou[cus_j->picked].cap - cus_j->demand + cus_i->demand <= capacity){
								if (cus_i->calculate_swap (cus_j, cus_j, cus_j->demand) < bcost_change){
									exchange = 1;
									_insert = 0;
									swap = 0;
									// cerr << 222 << endl;
									bcost_change = cus_i->calculate_swap (cus_j, cus_j, cus_j->demand);
									best_cus = cus_i;
									bL = bR = cus_j;
									tmp_sum_cap = cus_j->demand;
								}
							}
						}
					}
					j += rand () % (max_step);
				}

			}
		}
		if (bcost_change < 0){
			// tabu.add (best_pos->index, best_cus->index, best_pos->r->index);
			// cerr << 111 << ' ' << tabu.has (best_pos->index, best_cus->index, best_pos->r->index) << endl;
			if (_insert){
				// cerr << best_pos->index << ' ' << best_cus->index << ' ' << best_pos->r->index << endl;
				best_pos->insert (best_cus);
				tabu.add (best_cus->l->index, best_cus->index, best_cus->r->index);				
			}
			else if (exchange){
				best_cus->swap (bL, bR, tmp_sum_cap);
				tabu.add (best_cus->l->index, best_cus->index, best_cus->r->index);
				// cerr << tabu.has (best_cus->l->index, best_cus->index, best_cus->r->index) << endl;
			}
			temp = temp * alpha;
		}
		else if (bcost_change != oo){
			double accept_prob = exp (1.0 * -bcost_change / temp);
			// cerr << accept_prob << endl;
			double prob = 1.0 * rand () / RAND_MAX;
			if (prob <= accept_prob){
				// cerr << 222 << ' ' << tabu.has (best_pos->index, best_cus->index, best_pos->r->index) << endl;
				if (_insert){
					best_pos->insert (best_cus);
					tabu.add (best_cus->l->index, best_cus->index, best_cus->r->index);
					// cerr << best_cus->l->index << ' ' << best_cus->index << ' ' << best_cus->r->index << endl;
					// tabu.add (best_pos->index, best_cus->index, best_pos->r->index);
				}
				else if (exchange){
					best_cus->swap (bL, bR, tmp_sum_cap);
					tabu.add (best_cus->l->index, best_cus->index, best_cus->r->index);
					// cerr << "sdfs " << best_cus->l->index << ' ' << best_cus->index << ' ' << best_cus->r->index << endl;
					// cerr << tabu.has (best_cus->l->index, best_cus->index, best_cus->r->index) << endl;
				}
				// cerr << bcost_changef << endl;
			}
		}
		double cur_total = total ();
		if (cur_total < min_total){
			min_total = cur_total;
			ans.update (cur_total);
		}
	}
}

void print_sol (){
	double total_cost = 0;
	for (int i = 0; i < m; i++)
		total_cost += rou[i].cost;
	ans.prin ();
	// cout << total_cost << ' ' << 0 << endl;

	// for (int i = 0; i < m; i++){
	// 	Customer *cur = rou[i].start;
	// 	while (cur != rou[i].end){
	// 		cout << cur->index << ' ';
	// 		cur = cur->r;
	// 	}
	// 	cout << 0 << endl;
	// }
}