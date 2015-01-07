#ifndef DPL_PARSETREE_HPP
#define DPL_PARSETREE_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <dpl/tokenizer.hpp>

namespace dpl
{

// DOCUMENTATION {{{
//! parsetree represents a tree structure where each node holds a token.
/*! This is the type returned by the Parse method in the Parser class.
 *  The tree represents elements of the given abstract syntax.
 */
// }}}
class parsetree // {{{
{
  public:
    //! Constructor to create a parsetree consisting of a single token (leaf)
    parsetree(const token &t);
    //! Constructor of a parsetree with typename, casename and list of subtrees.
    parsetree(const std::string &type_name,
                const std::string &case_name,
                std::vector<parsetree*> &content);
    //! Constructor of an error parsetree
    parsetree(const std::string &error="");
    //! Copy Constructor
    parsetree(const parsetree &rhs);
    //! Destructor
    ~parsetree();

    // DOCUMENTATION {{{
    //! ToString gives a string representation of the tree with parentheses
    /*  This is different from the Unparse method with parentheses,
     *  because the result contains the token names instead of the content. 
     */
    // }}}
    std::string ToString(bool include_cases=false);

    //! GetPosition returns position (in input) of earliest token used in parsetree
    std::pair<int,int> GetPosition() const;

    //! Type-name used in top constructor (BNF name)
    std::string type_name;
    //! Case-name used in top constructor (BNF case name)
    std::string case_name;
    //! root is used only if tree is a single token
    token root;
    //! content is used if tree is not a single token
    std::vector<parsetree*> content;
    //! is_token declares if this is a single token or a parsetree (containing 0 or more tokens in a tree structure)
    bool is_token;
}; // }}}

}

#endif

