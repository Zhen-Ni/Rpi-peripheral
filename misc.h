#ifndef MISC_H
#define MISC_H


// Loop unrolling
template<unsigned N> struct Unroll {
  template<typename F> static void call (const F &f) {
    f();
    Unroll<N-1>::call(f);
  }
};

template<> struct Unroll<0> {
  template<typename F> static void call (const F &f) {
    f();
  }
};


#endif
