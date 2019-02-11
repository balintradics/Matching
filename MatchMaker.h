#ifndef MATCHMAKER_H
#define MATCHMAKER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <utility>
#include <algorithm>

using namespace std;

// This class does one round of College Admission algorithm

typedef vector<string> PrefList;
typedef map<string, PrefList> PrefMap;
typedef multimap<string, string> Couples;// 1:college, 2: student
typedef map<string, int> Quotas;// college quotas
typedef map<string, bool> QuotasState;

// possible to increase/decrease the quotas
const int quota[] = {2, 2};

class MatchMaker {

public:
  MatchMaker();
  ~MatchMaker();

  void init_data();
  bool prefers(const PrefList &prefer, const string &first, const string &second);  
  void check_stability(const Couples &admitted, const PrefMap &student_pref, const PrefMap &college_pref);
  void do_matching();
  void print_admitted();
  
public:

  Couples admitted;
  PrefMap student_pref, college_pref;
  queue<string> freestudents;//bachelors
  Quotas college_quotas;
  QuotasState college_quotas_st;
  
};

#endif // MATCHMAKER_H
