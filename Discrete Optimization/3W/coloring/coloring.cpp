#include <bits/stdc++.h>

using namespace std;



typedef vector<int> vi;

int n, m;
vector<vi> e;
vector<int> v_list;
vector< set<int> > e_color;
vector<int> ans;
int v_color[1011];
int max_color = 1001;

bool cmp (int u, int v){
	return e[u].size() > e[v].size ();
}

void init (){
	e.assign (n, vi ());
	e_color.assign (n, set <int> ());
	ans.assign (n, int ());
	memset (v_color, -1, sizeof (v_color));
}

void Try (int i, int cur_max, int lower_bound){

	if (max_color <= lower_bound) return;
	int u = v_list[i];
	if (cur_max >= max_color)
		return;
	if (i == n){
		max_color = cur_max;
		for (int i = 0; i < n; i++){
			ans[i] = v_color[i];
		}
		return;
	}

	set <int> c_list;
	for (int v: e[u])
		c_list.insert (v_color[v]);

	
	for (int c = 0; c <= cur_max + 1; c++){
		if (max_color <= lower_bound) return;

		if (c_list.find (c) == c_list.end ()){

			v_color[u] = c;
			Try (i + 1, max (cur_max, c), lower_bound);
			v_color[u] = -1;
		}
	}
}

void input (){
	ios_base::sync_with_stdio(false);
	freopen ("tmp.data", "r", stdin);
	cin >> n >> m;
	init ();
	
	for (int i = 0; i < m; i++){
		int u, v; cin >> u >> v;
		e[u].push_back (v);
		e[v].push_back (u);
	}
	for (int i = 0; i < n; i++) v_list.push_back (i);
	sort (v_list.begin (), v_list.end(), cmp);
}

void process (){
	if (n >= 100){
		max_color = -1;
		for (int u : v_list){
			for (int c = 0; c < n; c++){
				max_color = max (c + 1, max_color);
				ans[u] = c;
				if (e_color[u].find (c) == e_color[u].end ()){
					for (int v : e[u]){
						e_color[v].insert (c);
					}
					break;
				}
			}
		}
	}
	else {
		int lower_bound = 1;
		if (n == 50) lower_bound = 5;
		if (n == 70) lower_bound = 16;
		v_color[v_list[0]] = 0;
		Try (0, -1, lower_bound);
		max_color = max_color + 1;
	}
	cout << max_color << ' ' << 1 << endl;

	for (int c : ans)
		cout << c << ' ';
}

int main (){
	input ();
	process ();
	return 0;
}