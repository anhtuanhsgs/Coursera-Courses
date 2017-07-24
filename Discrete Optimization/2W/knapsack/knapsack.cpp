#include <bits/stdc++.h>

using namespace std;

int n, k;

#define maxn 1000111

int v[maxn], w[maxn];
bool ans[maxn];

void readData (){
	freopen ("tmp.data", "r", stdin);
	cin >> n >> k;
	for (int i = 0; i < n; i++){
		cin >> v[i] >> w[i];
	}
}

void printAns (int ret, int opt){
	// freopen ("tmp.out", "w", stdout);
	// cout << n << ' ' << k << endl;
	cout << ret << ' ' << opt << endl;

	int tmp = 0;
	for (int i = 0; i < n; i++) 
		tmp += v[i] * ans[i];


	for (int i = 0; i < n; i++) cout << ans[i] << ' ';
}

int cal (vector< vector<int> > &f, int u, int x, vector< vector<int> > &tr){
	if (u < 0) return 0;
	if (f[u][x]) return f[u][x];
	
	int &ret = f[u][x];
	ret = cal (f, u - 1, x, tr);
	tr[u][x] = 0;
	if (x - w[u] >= 0) {
		int tmp = cal (f, u - 1, x - w[u], tr) + v[u];
		if (ret < tmp){
			ret = tmp;
			tr[u][x] = 1;
		}
	}
	return ret;
}

void dpSol (){

	vector < vector <int> > dp (n, vector <int> (k, 0));
	vector < vector <int> > tr (n, vector <int> (k, -1));

	int ret = cal (dp, n - 1, k, tr);
	int u = n - 1, x = k;
	while (u >= 0 && tr[u][x] != -1 ){

		if (tr[u][x]) {
			ans[u] = 1;
			x -= w[u];
		}
		u--;
	}
	printAns (ret, 1);
}

int ratio_greedy (){
	memset (ans, 0, sizeof (ans));
	vector < pair<double, int> > a (n);
	for (int i = 0; i < n; i++){
		a[i].first = 1.0 * v[i] / w[i];
		a[i].second = i;
	}

	sort (a.rbegin(), a.rend ());
	int x = k, ret = 0;
	for (int i = 0; i < n; i++){
		if (x - w[a[i].second] >= 0){
			x -= w[a[i].second];
			ret += v[a[i].second];
			ans[a[i].second] = 1;
		}
		else break;
	}
	return ret;
}

int weight_greedy (){
	memset (ans, 0, sizeof (ans));
	vector < pair<int, int> > a (n);
	for (int i = 0; i < n; i++){
		a[i].first = w[i];
		a[i].second = i;
	}

	sort (a.begin(), a.end ());
	int x = k, ret = 0;
	for (int i = 0; i < n; i++){
		if (x - w[a[i].second] >= 0){
			x -= w[a[i].second];
			ret += v[a[i].second];
			ans[a[i].second] = 1;
		}
		else break;
	}
	return ret;
}

int value_greedy (){
	memset (ans, 0, sizeof (ans));
	vector < pair<int, int> > a (n);
	for (int i = 0; i < n; i++){
		a[i].first = v[i];
		a[i].second = i;
	}

	sort (a.rbegin(), a.rend ());
	int x = k, ret = 0;
	for (int i = 0; i < n; i++){
		if (x - w[a[i].second] >= 0){
			x -= w[a[i].second];
			ret += v[a[i].second];
			ans[a[i].second] = 1;
		}
		else break;
	}
	return ret;
}

#define fi first
#define se second

int LP_relaxation (int u, int curW, vector< pair<double, int> > a){
	int ret = 0;
	for (int i = u; i < n; i++){
		if (w[a[i].se] <= curW){
			ret += v[a[i].se];
			curW -= w[a[i].se];
		}
		else{
			ret += ceil (a[i].fi * curW);
			return ret;
		}
	}
	return ret;
}

int iter = 0;

void dfs (int u, int& ret, int curV, int curW, vector<bool> &path, vector< pair<double, int> >& a){
	iter++;
	if (iter % 50000000 == 0) cout << iter << endl;
	if (u == n && curV > ret){
		ret = curV;
		for (int i = 0; i < n; i++)
			ans[i] = path[i];
	}

	if (u == n) return;

	if (curV + LP_relaxation (u, curW, a) < ret) return;

	int id = a[u].se;
	if (w[id] <= curW){
		path[id] = 1;
		dfs (u + 1, ret, curV + v[id], curW - w[id], path, a);
	}
	path[id] = 0;
	dfs (u + 1, ret, curV, curW, path, a);
}

int tree_prunning (){
	long long iter = 0;

	vector < pair<double, int> > a (n);
	for (int i = 0; i < n; i++){
		a[i].first = 1.0 * v[i] / w[i];
		a[i].second = i;
	}

	sort (a.rbegin(), a.rend ());

	vector <bool> path (n, 0);
	int ret = -1;
	dfs (0, ret, 0, k, path, a);

	return ret;
}

void greedy (){
	// int rat = ratio_greedy ();
	// int val = value_greedy ();
	// int wei = weight_greedy ();
	int mx;
	// mx = max (rat, max(val, wei));
	// if (rat == mx) ratio_greedy ();
	// else if (wei == mx) weight_greedy ();
	// else value_greedy ();

	mx = tree_prunning ();
	printAns (mx, 1);

}

int main (){
	ios_base::sync_with_stdio(0);
	readData ();

	if ((long long)n * k <= 50000000){
		dpSol ();
	}
	else {
		greedy ();
	}

	return 0;
}