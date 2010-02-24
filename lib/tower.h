// -*- C++ -*-

#ifndef __TOWER_H__
#define __TOWER_H__

#include "chromabase.h"
using namespace QDP;

namespace Chroma { 

template<typename T>
class Tower { 
public: 

  //! Base
  Tower() {}

  //! Create empty
  Tower(int n) { 
    tower_data.resize(n);
  }
  
  // Destroy
  ~Tower() {}

  void resize(int n) {
    tower_data.resize(n);
  }

  int size() const { 
    return tower_data.size();
  }

  //! get
  const T& operator[](int i) const { 
    return tower_data[i];
  }
  
  //! add item
  T& operator[](int i) {
    return tower_data[i];
  }

  //! Copy
  Tower(const Tower<T>& a) { 
    // Avoid self assignment
    if( this == &a ) return;
    
    this->resize(a.size());
    for(int i=0; i < a.size(); i++) {
      (*this)[i] = a[i];
    }
  }
      
  //! assign 
  Tower<T>& operator=(const Tower<T>&a) 
  {
    // Avoid self assignment
    if( this == &a ) {
      return *this;
    }

    this->resize(a.size());
    for(int i=0; i < a.size(); i++) {
      (*this)[i] = a[i];
    }

    return *this;
  }

  // Add: Assumes types T and T1 have += operation
  Tower<T>& operator+=(const Tower<T>& a)
  {
    for(int i=0; i < a.size(); i++) {
      (*this)[i] += a[i];
    }
    return *this;
  }

  Tower<T>& operator-=(const Tower<T>& a)
  {
    for(int i=0; i < a.size(); i++) {
      (*this)[i] -= a[i];
    }
    return *this;
  }
     
  //! Scalar multiply?
  template<typename T1>
  Tower<T>& operator*=(const OScalar<T1>& alpha)
  {
    for(int i=0; i < this->size(); i++) {
      (*this)[i] *= alpha;
    }
    return *this;
  }
 
  Tower<T>& operator*=(const Tower<T>& b)
  {

    for(int level=b.size()-1; level >=0; --level) { 
      T tmp = (*this)[0]*b[level];
      for(int i=1; i <= level; i++) { 
	T tmp2 = (*this)[i]*b[level-i];
        tmp += Choose(level,i)*tmp2;
      }
      (*this)[level] = tmp;
    }
    return (*this);
    
  }


private:
  //! Evaluate (m,n) binomial coefficient recursively
  int Choose(int m, int n) 
  {
    if ( m==0 && n==0 ) return 1;  // Pinnacle

    if ( m==0 ) return 0; 

    return Choose(m-1,n-1)+Choose(m-1,n);
  }


  multi1d<T> tower_data;
};
 


  template<typename T>
  Tower<T> operator*(const Tower<T>& a, const Tower<T>& b) 
  {
    Tower<T> ret_val(a);
    ret_val *= b;
    return ret_val;
  }
   
  template<typename T>
  Tower<T> operator+(const Tower<T>& a, const Tower<T>& b)
  {
    Tower<T> ret_val(a);
    ret_val += b;
    return ret_val;
  }

  template<typename T>
  Tower<T> operator-(const Tower<T>& a, const Tower<T>& b)
  {
    Tower<T> ret_val(a);
    ret_val -= b;
    return ret_val;

  }

  template<typename T, typename T1>
  Tower<T> operator*(const OScalar<T1>& a, const Tower<T>& b)
  {
    Tower<T> ret_val(b);
    ret_val *= a;
    return ret_val;
  }

  template<typename T>
  Tower<T> adj(const Tower<T>& a) 
  {
    Tower<T> ret_val(a.size());
    for(int i=0; i < a.size(); i++) { 
      ret_val[i] = adj(a[i]);
    }
    return ret_val;
  }

  template<typename T>
  Tower<T> shiftTower(const Tower<T>& a, int mu, int dir) 
  {
    Tower<T> ret_val(a.size());
    for(int i=0; i < a.size(); i++) { 
      ret_val[i] = QDP::shift(a[i], mu, dir);
    }
    return ret_val;
  }

  template<typename T>
  void random(Tower<T>& t) 
  {
    for(int i=0; i < t.size(); i++) {
      random(t[i]);
    }
  }

  template<typename T>
  void gaussian(Tower<T>& t) 
  {
    for(int i=0; i < t.size(); i++) {
      gaussian(t[i]);
    }
  }

  template<typename T>
  void print(Tower<T>& t) { 
    for(int i=0; i < t.size(); i++) {
      cout << "Level["<<i<<" ]"<< t[i] << endl;
    }
  }

  template<typename T>
  void zeroTower(Tower<T>& t)
  {
    for(int i=0; i < t.size(); i++) { 
      t[i] = zero;
    }
  }

}

#endif