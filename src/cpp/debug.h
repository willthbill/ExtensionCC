#pragma once

#include<bits/stdc++.h>

void _debug(std::string&t);
template <typename T> void _debug(T t);
template<typename T1,typename T2> void _debug(bool b);
template<typename T1,typename T2> void _debug(std::pair<T1,T2>p);
template<typename T1>void _debug(std::tuple<T1>&t);
template<typename T1,typename T2>void _debug(std::tuple<T1,T2>&t);
template<typename T1,typename T2,typename T3>void _debug(std::tuple<T1,T2,T3>&t);
template<typename T1,typename T2,typename T3,typename T4>void _debug(std::tuple<T1,T2,T3,T4>&t);
template<typename T1,typename T2,typename T3,typename T4,typename T5>void _debug(std::tuple<T1,T2,T3,T4,T5>&t);
template<typename T>void _debug(std::vector<T>v);
template<typename T>void _debug(std::set<T>v);
template<typename T1,typename T2>void _debug(std::map<T1,T2>v);
template<typename T>void _debug(std::deque<T>v);
template<typename T,size_t N>void _debug(std::array<T,N>v);
template<typename A>void _debug(std::queue<A>&q);
template<typename A>void _debug(std::priority_queue<A>&pq);
void _masterdebug();
template<typename T,typename... V> void _masterdebug(T t,V... v);
template<typename T> void _debugall(T c, std::string n);

/*#ifdef __local__
#define debug(...)(std::cerr<<"["<<#__VA_ARGS__<<"] = [",_masterdebug(__VA_ARGS__))
#define debugall(c)(_debugall(c,#c))
#else*/
#define debug(...)
#define debugall(c)
// #endif

void _debug(std::string&t){std::cerr<<t;} // has to be specified to that strings are not interpreted as basic_string<char>
template <typename T> void _debug(T t){std::cerr<<t;}
template<typename T1,typename T2> void _debug(bool b){std::cerr<<(b?"1":"0");}
template<typename T1,typename T2> void _debug(std::pair<T1,T2> p){std::cerr<<"pr{";_debug(p.first);std::cerr<<", ";_debug(p.second);std::cerr<<"}";}
template<typename T1>void _debug(std::tuple<T1>&t){std::cerr<<"tp{";_debug(fir(t));std::cerr<<"}";}
template<typename T1,typename T2>void _debug(std::tuple<T1,T2>&t){std::cerr<<"tp{";_debug(fir(t));std::cerr<<", ";_debug(sec(t));std::cerr<<"}";}
template<typename T1,typename T2,typename T3>void _debug(std::tuple<T1,T2,T3>&t){std::cerr<<"tp{";_debug(fir(t));std::cerr<<", ";_debug(sec(t));std::cerr<<", ";_debug(thi(t));std::cerr<<"}";}
template<typename T1,typename T2,typename T3,typename T4>void _debug(std::tuple<T1,T2,T3,T4>&t){std::cerr<<"tp{";_debug(fir(t));std::cerr<<", ";_debug(sec(t));std::cerr<<", ";_debug(thi(t));std::cerr<<", ";_debug(fou(t));std::cerr<<"}";}
template<typename T1,typename T2,typename T3,typename T4,typename T5>void _debug(std::tuple<T1,T2,T3,T4,T5>&t){std::cerr<<"tp{";_debug(fir(t));std::cerr<<", ";_debug(sec(t));std::cerr<<", ";_debug(thi(t));std::cerr<<", ";_debug(fou(t));std::cerr<<", ";_debug(fif(t));std::cerr<<"}";};
template<typename T>void _debug(std::vector<T>v){bool f=1;std::cerr<<"(";for(auto e:v)std::cerr<<(f?"":", "),f=0,_debug(e);std::cerr << ")";}
template<typename T>void _debug(std::set<T>v){bool f=1;std::cerr<<"(";for(auto e:v)std::cerr<<(f?"":", "),f=0,_debug(e);std::cerr << ")";}
template<typename T1,typename T2>void _debug(std::map<T1,T2>v){bool f=1;std::cerr<<"(";for(auto e:v)std::cerr<<(f?"":", "),f=0,_debug(e);std::cerr << ")";}
template<typename T>void _debug(std::deque<T>v){bool f=1;std::cerr<<"(";for(auto e:v)std::cerr<<(f?"":", "),f=0,_debug(e);std::cerr << ")";}
template<typename T,size_t N>void _debug(std::array<T,N>v){bool f=1;std::cerr<<"(";for(auto e:v)std::cerr<<(f?"":", "),f=0,_debug(e);std::cerr << ")";}
template<typename A> void _debug(std::queue<A>&q){std::queue<A>cp=q; std::vector<A>v;while(!cp.empty()) v.pb(cp.front()), cp.pop();std::cerr<<"q";_debug(v);}
template<typename A> void _debug(std::priority_queue<A>&pq){std::priority_queue<A>cp=pq; std::vector<A>v;while(!cp.empty()) v.pb(cp.top()), cp.pop();std::cerr<<"pq";_debug(v);}
void _masterdebug(){std::cerr<<"]"<<std::endl;}
template<typename T,typename... V> void _masterdebug(T t,V... v){_debug(t);if(sizeof...(v))std::cerr<<", ";_masterdebug(v...);}
template<typename T> void _debugall(T c, std::string n){_debug("debugall: ");_debug(n);std::cerr<<std::endl;for(auto&_e:c){_debug("  ");_debug(_e);std::cerr<<std::endl;}} //2d vectors and similar
