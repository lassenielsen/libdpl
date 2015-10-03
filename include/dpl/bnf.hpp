#ifndef DPL_BNF_HPP
#define DPL_BNF_HPP

#include <string>
#include <vector>
#include <set>
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
 *  example case definitions are listed below
 *  \li exp + exp
 *  \li ::exp_plus exp + exp
 *  \li ::exp_plus exp::lhs + exp::rhs
 *  \li ::exp_par LPAR exp::content RPAR ::-> :"::content"
 *  \li ::exp_sum exp::from +...+ exp::to ::-> :"(::to * (::to-1))/2 - ((::from)*((::from)-1))/2"
 *
 *  Defining the BNF language with a BNF can be done as follows:
 *  := "[ \t\r\n][ \t\r\n]*"         # Ignore whitespace
 *  DDEF := "::="                    # Use the name DDEF for the ::= token
 *  CSEP := "\|"                     # Use the name CSEP for the | case seperator
 *  tag := "::[a-zA-Z0-9_]*"         # Tag names start with ::
 *  string := ":\"(\\.|[^\"])*\"     # String literals are preceded by :
 *  SDEF := ":->"                    # Use the name SDEF for the token :-> used for syntactic sugar
 *  id := "[^:| \t\r\n][^| \t\r\n]*" # Everything else is tokenized as identifiers
 *
 *  bnf ::= id DDEF cases            # A BNF starts with a name followed by ::= and the cases
 *  cases ::= case                   # Cases is either a single case
 *          | case CSEP cases        # or more cases seperated by CSEP
 *  case ::= items                   # A case is a sequence of items
 *         | tag items               # A case can include a case name (tag)
 *         | items SDEF string       # A case can also be a syntactic sugar definition
 *  items ::= item items             # Items either starts with an item
 *          |                        # or an empty sequence
 *  item ::= id tag
 *         | id
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
    /*! Returns ourVoidCase if case name is undefined.
     */
    // }}}
    const std::vector<std::string> &Case(const std::string &name) const;
    // DOCUMENTATION {{{
    //! Tags returns the tags defined for the given case name.
    /*! Returns ourVoidTags if case name is undefined.
     */
    // }}}
    const std::map<std::string,size_t> &Tags(const std::string &name) const;
    // DOCUMENTATION {{{
    //! Sugar returns the syntactic sugar string definition for the given case name.
    /*! Returns ourVoidTags if case name is undefined.
     */
    // }}}
    const std::string &Sugar(const std::string &name) const;
    //! CaseNames returns a list names of the defined cases
    std::vector<std::string> CaseNames() const;

  private:
    std::string myName;
    //! myCases is a map from case name to the definition the case by a vector of token and type names.
    std::map<std::string,std::vector<std::string> > myCases;
    // DOCUMENTATION {{{
    /*! For each production myTags contains a map of tag names to index,
     * enabeling the use of tag names.
     */
    // }}}
    std::map<std::string,std::map<std::string,size_t> > myTags;
    // DOCUMENTATION {{{
    /*! mySugar contains a set of suntactic sugar definitions.
     */
    // }}}
    std::map<std::string,std::string> mySugar;
    // DOCUMENTATION {{{
    /*! ourVoid is a static empty case definition used to return a case
     * when invalid specification (such as undefined case name) is
     * requested.
     */
    // }}}
    static const std::vector<std::string> ourVoidCase;
    static const std::map<std::string,size_t> ourVoidTags;
    static const std::string ourVoidSugar;
}; // }}}

}

#endif
