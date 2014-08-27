#ifndef DPL_BNF_HPP
#define DPL_BNF_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <map>

namespace dpl
{

// DOCUMENTATION {{{
//! Bnf represents the definition of one bnf type.
/*! This is an internal structure, and is not relevant for client users.
 *  A Bnf object represents a bnf equation of the form
 *
 *  name ::= case1 | case2 | case3 ...
 *
 *  where each cases is in one of the forms
 *  \li ::case_name name1 name2 ...
 *  \li name1 name2 ...
 */
// }}}
class Bnf // {{{
{
  public:
    // DOCUMENTATION {{{
    /*! Constructor takes string with bnf syntax of type, and parses it to
        the internal representation of bnf types.
        The bnf syntax used is:
        bnf   ::= id \:\:\= cases
        cases ::= case | case \| cases
        case  ::= tag case1 | case1
        case1 ::= | id case1
     */
    // }}}
    Bnf(const std::string &bnf_string="");
    //! Destructor
    virtual ~Bnf();

    //! Returns the type name of the bnf
    const std::string &GetName() const { return myName; }
    //! Update type name of bnf
    void SetName(const std::string &name) { myName = name; }
    //! Empty returns if the bnf has no cases
    bool Empty() const { return myCases.size()==0; }

    //! AddCase adds a case unde the given name to the BNF
    void AddCase(const std::string &name, const std::vector<std::string> &def);
    // DOCUMENTATION {{{
    //! Case returns the case definition under the specified case name.
    /*! Returns ourVoid if case name is undefined.
     */
    // }}}
    const std::vector<std::string> &Case(const std::string &name) const;
    //! CaseNames returns a list names of the defined cases
    std::vector<std::string> CaseNames() const;

  private:
    std::string myName;
    //! myCases is a map from case name to the definition the case by a vector of token and type names.
    std::map<std::string,std::vector<std::string> > myCases;
    // DOCUMENTATION {{{
    /*! ourVoid is a static empty case definition used to return a case
     * when invalid specification (such as undefined case name) is
     * requested.
     */
    // }}}
    static std::vector<std::string> ourVoid;
}; // }}}

}

#endif
