
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

const int NUM_CALCS=10;

class factorial {
public:
    factorial(int x, long long& res) : x(x), res(res) { }
    void operator()() { res = calculate(x); }
    long long result() const { return res; }

private:
    long long calculate(int x) {       
      long long r = 1;

      if (x > 1)
	for (int i = 1 ; i <= x; ++i)
	  r *= i;

      boost::mutex::scoped_lock lock(mtx_);
      std::cout << x << "! = " << r << "\n";
      return r;
    }

private:
    static boost::mutex mtx_;
    int x;
    long long& res;
};

boost::mutex factorial::mtx_;

int main()
{
    long long results[NUM_CALCS];
    boost::thread_group thrds;
    for (int i = 0; i < NUM_CALCS; ++i)
        thrds.create_thread(factorial(i*2, results[i]));
    thrds.join_all();

    //for (int j = 0; j < NUM_CALCS; ++j)
    //    std::cout << j*2 << "! = " << results[j] << std::endl;
}
