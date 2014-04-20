#ifndef DPL_BNF_HPP
#define DPL_BNF_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <dpl/parsetree.hpp>

namespace dpl
{

/*! Bnf represents the definition of one bnf type.
 *  This is an internal structure, and is not relevant for client users.
 *  A Bnf object represents a bnf equation of the form
 *  name ::= case1 | case2 | case3 ...
 */
class Bnf // {{{
{
  public:
    // DOCUMENTATION {{{
    /*! Constructor takes string with bnf syntax of type, and parses it to
        the internal representation of bnf types.
        The bnf syntax used is:
        bnf    ::= id \:\:\= cases
        cases  ::= | cases1
        cases1 ::= case | case \| cases1
        case   ::= tag case1 | case1
        case1  ::= | id case1
     */
    // }}}
    Bnf(const std::string &bnf_string="");
    virtual ~Bnf();

    const std::string &GetName() const { return myName; }
    void SetName(const std::string &name) { myName = name; }
    bool Empty() const { return myCases.size()==0; }

    void AddCase(const std::string &name, const std::vector<std::string> &def);
    const std::vector<std::string> &Case(const std::string &name) const;
    std::vector<std::string> CaseNames() const;

  private:
    std::string myName;
    /*! myCases is a map from case name to the definition the case by a vector of token and type names.
     */
    std::unordered_map<std::string,std::vector<std::string> > myCases;
    static std::vector<std::string> ourVoid;
}; // }}}

}

#endif
