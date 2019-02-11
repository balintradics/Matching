#include "MatchMaker.h"

#include <iostream>

using namespace std;

MatchMaker::MatchMaker(){


}

MatchMaker::~MatchMaker(){

}

void MatchMaker::init_data(){

 const char *student_data[][3] = {
   {"1", "c2", "c1"},
   {"2", "c2", "c1"},
   {"3", "c2", "c1"},
   {"4", "c1", "c2"},
   {"5", "c1", "c2"}
 };

 const char *college_data[][6] = {
   {"c1", "5", "3", "4", "1", "2"},
   {"c2", "3", "5", "2", "1", "4"}
 };

  
      // init data structures
 for (int i = 0; i < 5; ++i) // students
   {
     for (int j = 1; j < 3; ++j) // preference
       {
	 student_pref[  student_data[i][0]].push_back(  student_data[i][j]);
       }
     freestudents.push(student_data[i][0]);
   }
 
 // init data structures
 for (int i = 0; i < 2; ++i) // college
   {
     for (int j = 1; j < 6; ++j) // preference
       {
	 college_pref[  college_data[i][0]].push_back(  college_data[i][j]);
	 college_quotas[ college_data[i][0]] = quota[i];
	 college_quotas_st[ college_data[i][0]] = false;// initially all quotas are unpopulated
       }
   }
  
}

// Does 'first' appear before 'second' in preference list?
bool MatchMaker::prefers(const PrefList &prefer, const string &first, const string &second){
    for (PrefList::const_iterator it = prefer.begin(); it != prefer.end(); ++it)
    {
        if (*it == first) return true;
        if (*it == second) return false;
    }
    return false; // no preference
}
 
void MatchMaker::check_stability(const Couples &admitted, const PrefMap &student_pref, const PrefMap &college_pref){
  cout << "Stablility:\n";
  bool stable = true;
  for (Couples::const_iterator it = admitted.begin(); it != admitted.end(); ++it)
    {
      const string &college = it->first;// in SMP: bride
      const string &student = it->second;//in SMP: groom
      const PrefList &preflist = student_pref.at(student);
 
        for (PrefList::const_iterator it = preflist.begin(); it != preflist.end(); ++it)
        {
            if (*it == college) // he prefers this college
                break;

	    
            if (prefers(preflist, *it, college)){ // he prefers another college
	      // check if any other college prefers him
	      
	      for (Couples::const_iterator it_ad = admitted.begin(); it_ad != admitted.end(); ++it_ad)
		if(it_ad->first == *it){
		  const string &student_other = it_ad->second;
		
		// check if there is other college that prefers him over any other student
		  if (student_other != student && prefers(college_pref.at(*it), student, student_other))
		    {
		      cout << "\t" << *it <<
			" prefers " << student <<
			" over " << student_other <<
			" and " << student <<
			" prefers " << *it <<
			" over " << college << "\n";
		      stable = false;
		    }
		}
	    }	  
        }
    }
    if (stable) cout << "\t(all admissions stable)\n";
}

void MatchMaker::do_matching(){

  cout << "Matchmaking:\n";
  // until either all students have applied 
  // Note: we use "goto" statment to jump out of the application loop because
  // the free students have to apply to the 1st college on their list, and wait until
  // waiting list is full and be perhaps rejected, before applying to the 2nd college on their
  // list. 
  bool quotas_full = false;
  
  int count = 0;
  while (!freestudents.empty())
    {
      
      
      const string &freestudent = freestudents.front();
      const PrefList &preflist = student_pref[freestudent];
      
      for (PrefList::const_iterator it = preflist.begin(); it != preflist.end(); ++it)
	{
	  const string &college = *it;
	  
	  if (admitted.find(college) == admitted.end() || admitted.count(college) < college_quotas[college]) // college is available, quota not full
	    {
	      // check that this student is not yet on the waiting list of this college
	      bool onwaiting = false;
	      for (Couples::const_iterator it_ad = admitted.begin(); it_ad != admitted.end(); ++it_ad){
		if(it_ad->first == college && it_ad->second == freestudent)
		  onwaiting = true;
	      }
	      
	      if(onwaiting == false){
		cout << "\t" << college << " puts " << freestudent << " on waiting list \n";
		admitted.insert(pair<string, string>(college, freestudent)); // application and placement to waiting list of college
		goto finish;
	      }
	      
	    }
	  
	  // even if the above placement does or does not happen, the student in the next round
	  // may be swapped with others in the waiting list of this college
	  for (Couples::iterator it_ad = admitted.begin(); it_ad != admitted.end(); ++it_ad){
	    if(it_ad->first == college){
	      string &student = it_ad->second;
	      
	      // check if there is another student that is more
	      // prefered over the current student, and the other student is not
	      // on the admission list yet
	      if (freestudent != student && prefers(college_pref[college], freestudent, student)){
		// check that this freestudent is not yet on the waiting list of this college
		bool onwaiting = false;
		for (Couples::const_iterator it_ad = admitted.begin(); it_ad != admitted.end(); ++it_ad){
		  if(it_ad->first == college && it_ad->second == freestudent)
		    onwaiting = true;
		}
		if(onwaiting == false){
		  cout << "\t" << college << " dumped " << student << " for " << freestudent << "\n";
		  freestudents.push(student); // dump that zero
		  it_ad->second = freestudent; // get a hero
		  goto finish;
		}
	      }
	    }
	  }
	  
	  if(admitted.count(college) == college_quotas[college])
	    college_quotas_st[ college ] = true;// quota full
	}
    finish: ;
      
      
      freestudents.pop(); // pop at the end to not invalidate suitor reference
      
      // check if quotas are full - is this needed ?
      quotas_full = false;
      for (QuotasState::iterator it = college_quotas_st.begin(); it != college_quotas_st.end(); ++it){
	bool &college_st = it->second;
	quotas_full = quotas_full && college_st;
      }
      
    }

}

void MatchMaker::print_admitted(){

  cout << "Admissions:\n";
  for (Couples::const_iterator it = admitted.begin(); it != admitted.end(); ++it)
  {
      cout << "\t" << it->first << " and " << it->second << "\n";
  }
  
  // check_stability(admitted, student_pref, college_pref);

}
