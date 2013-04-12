#ifndef lsmpoller_h
#define lsmpoller_h
#include <libconfig.h++>
#include <deque>

using namespace std;

class lsmpoller
{
 public:
  lsmpoller();

  void init(libconfig::Config& c);
  void run(void);
  int get_pitch(void);
  int get_heading(void);

  bool running;

 private:
  void add_deque(deque<int>& q, int& v, unsigned int& s);
  float get_avg(deque<int>& q);

  int stime;
  deque<int> p_deque;
  unsigned int p_size;
  deque<int> h_deque;
  unsigned int h_size;

};

#endif
