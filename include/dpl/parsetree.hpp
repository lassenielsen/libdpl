#ifndef DPL_PARSETREE_HPP
#define DPL_PARSETREE_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <dpl/tokenizer.hpp>

namespace dpl
{

/*! parsetree represents a tree structure where each node holds a token.
 *  This is the type returned by the Parse method in the Parser class.
 *  The tree represents elements of the given abstract syntax.
 */
class parsetree // {{{
{
  public:
    parsetree(const token &t);
    parsetree(const std::string &type_name,
                const std::string &case_name,
                std::vector<parsetree*> &content);
    parsetree(const std::string &error="");
    parsetree(const parsetree &rhs);
    ~parsetree();

    /* Gives a string representation of the tree with parentheses
     * This is different from the Unparse method with parentheses,
     * because the result contains the token names instead of the content. 
     */
    std::string ToString(bool include_cases=false);
    std::string type_name;
    std::string case_name;
    token root; // only used if tree is a single token
    std::vector<parsetree*> content;
    bool is_token;
}; // }}}

}

#endif

