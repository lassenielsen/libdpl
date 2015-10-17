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
              std::vector<parsetree*> &content,
              std::map<std::string,size_t> &tags);
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

    //! Returns the line in (in input) of earliest token used in parsetree
    int Line() const {return GetPosition().first;}
    //! Returns the column in (in input) of earliest token used in parsetree
    int Column() const {return GetPosition().second;}
    //! Returns the position (in input) of earliest token used in parsetree as
    //! a string formatted as <line>:<col>
    std::string Position() const;

    //! Access contained token
    const token &Token() const {if (is_token) return myToken; throw std::string("Token called on none-token parsetree: ")+Type()+"."+Case();}
    //! Access Type name
    const std::string Type() const {return myType;}
    //! Access Case name
    const std::string Case() const {return myCase;}
    //! Returns Type.Case string
    const std::string TypeCase() const {return Type()+"."+Case();}

    const parsetree *Child(size_t index) const;
    const parsetree *Child(const std::string &tag) const;
    size_t Children() const { return myContent.size(); }

    bool IsToken() const { return is_token; }

  private:
    //! GetPosition returns position (in input) of earliest token used in parsetree
    std::pair<int,int> GetPosition() const;
    //! Type-name used in top constructor (BNF name or Token name)
    std::string myType;
    //! Case-name used in top constructor (BNF case name)
    std::string myCase;
    //! myToken is used only if tree is a single token
    token myToken;
    //! myContent is used if tree is not a single token
    std::vector<parsetree*> myContent;
    //! is_token declares if this is a single token or a parsetree (containing
    //0 or more tokens in a tree structure)
    bool is_token;
    //! myTags is a map from tagnames to index, used to look up child trees
    //! from tagnames
    std::map<std::string,size_t> myTags;
}; // }}}

}

#endif

