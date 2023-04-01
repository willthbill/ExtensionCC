#pragma once

// #define vec vector
// #define str string
#define tup tuple
#define pb push_back
#define eb emplace_back
#define fi first
#define se second
// #define pi pair<int,int>
#define vi vector<int>
#define all(x) x.begin(),x.end()
#define rall(x) x.rbegin(),x.rend()
#define ll long long
// #define mp make_pair
#define sz(x) ((int)((x).size()))
/*#define fir(t) get<0>(t)
#define sec(t) get<1>(t)
#define thi(t) get<2>(t)
#define fou(t) get<3>(t)
#define fif(t) get<4>(t)*/
// loops
#define foabc(i,a,b,c) for (int i = (a); i < (b); i+=(c))
#define cbaof(i,a,b,c) for (int i = ((int)(b))-1; i >= (a); i-=(c))
#define foab(i,a,b) for (int i = (a); i < (b); i++)
#define baof(i,a,b) for (int i = ((int)(b))-1; i >= (a); i--)
#define fon(i,n) for(int i = 0; i < n; i++)
#define nof(i,n) for(int i = ((int)(n)) - 1; i >= 0; i--)
#define foe(e,a) for(auto& e : a)
//template<class T> class ReversedProxy {
//public: using iterator = typename T::reverse_iterator; iterator begin() { return begin_; } iterator end() { return end_; } ReversedProxy(const iterator& begin, const iterator& end) : begin_(begin), end_(end) {}
//private: iterator begin_; iterator end_;};
//template<class T> ReversedProxy<T> reversed(T& obj) {return ReversedProxy<T>(obj.rbegin(), obj.rend());}
//#define eof(e,a) for(auto e : reversed(a)) // this one is a bit slow, because of rbegin I think
#define rep(n) for(int _ = 0; _ < n; _++)
#define once for(int _=1;_--;) // thanks Ásþór
#define mkunique(x) x.resize(unique(all(x))-x.begin())

#define TIME std::chrono::steady_clock::time_point
#define NOW() std::chrono::steady_clock::now()
#define TO_MICRO(a,b) (std::chrono::duration_cast<std::chrono::microseconds> ((a)-(b)).count())
#define MICRO_TO_MS(a) ((a)/1000)
#define COUT_TIME(time) (time) << " microseconds"
