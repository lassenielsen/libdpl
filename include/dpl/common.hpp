#ifndef COMMON_HPP
#define COMMON_HPP
/* Common contains the help-functions for the picalc-mps library
 */

#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>
#include <typeinfo>
#include <algorithm>

/* Simple help functions
 */
inline std::string double2string(double d) // {{{
{
  std::stringstream ss;
  ss.clear();
  ss << d;
  std::string s = ss.str();
  return s;
} // }}}
inline double string2double(const std::string &s) // {{{
{
  std::stringstream ss;
  ss.clear();
  ss << s;
  double result;
  ss >> result;
  return result;
} // }}}
inline std::string int2string(int i) // {{{
{
  std::stringstream ss;
  ss.clear();
  ss << i;
  std::string s = ss.str();
  std::replace(s.begin(),s.end(),'-','~');
  return s;
} // }}}
inline int string2int(std::string s) // {{{
{
  // Allow the use of '~' as minus
  std::replace(s.begin(),s.end(),'~','-');
  std::stringstream ss;
  ss.clear();
  ss << s;
  int result;
  ss >> result;
  return result;
} // }}}
inline std::string stringreplace(const std::string &source, const std::string &from, const std::string &to) // {{{
{
  if (from=="")
    return source;
  std::string dest="";
  int pos = 0;
  while (pos < source.size())
  {
    int newpos = source.find(from,pos);
    if (newpos!=std::string::npos)
    {
      dest += source.substr(pos,newpos-pos);
      dest += to;
      pos = newpos + from.size();
    }
    else
    {
      dest += source.substr(pos);
      pos = source.size();
    }
  }
  return dest;
} // }}}
inline std::string stuff_string(std::string input) // {{{
{ return stringreplace(stringreplace(input,"\\","\\\\"),"\"","\\\"");
} // }}}
inline std::string wrap_string(const std::string &input) // {{{
{ return (std::string)"\""+stuff_string(input)+"\"";
} // }}}
inline std::string unwrap_string(const std::string &input) // {{{
{ std::string value = input.substr(1,input.size()-2);
  return stringreplace(stringreplace(value,"\\\\","\\"),"\\\"","\"");
} // }}}

inline bool disjoint(const std::set<int> &lhs, const std::set<int> &rhs) // {{{
{
  for (std::set<int>::const_iterator it=lhs.begin(); it!=lhs.end(); ++it)
    if (rhs.find(*it) != rhs.end()) // Not disjoint (*it common elt)
      return false;
  return true;
} // }}}

template <class T>
inline std::vector<T*> CopyVector(const std::vector<T*> &container) // {{{
{ std::vector<T*> result;
  for (typename std::vector<T*>::const_iterator it=container.begin(); it!=container.end(); ++it)
    result.push_back((*it)->Copy());
  return result;
} // }}}
template <class T>
inline void DeleteVector(std::vector<T*> &container) // {{{
{
  while (container.size() > 0)
  {
    delete container.back();
    container.pop_back();
  }
} // }}}
template <class T>
inline void DeleteMap(std::map<std::string,T*> &container) // {{{
{
  while (container.size() > 0)
  {
    delete container.begin()->second;
    container.erase(container.begin());
  }
} // }}}

#endif

