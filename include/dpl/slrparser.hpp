#ifndef DPL_SYMPARSER_HPP
#define DPL_SYMPARSER_HPP

#include <map>
#include <dpl/symbnf.hpp>
#include <dpl/parser.hpp>

namespace dpl
{

// DOCUMENTATION {{{
//! SlrParser implements the standars SLR parsing algorithm.
/*! The SLR (Simple Left-to-right Rightmost-derivation) parsing algorithm is
 *  often used in teaching, as it is a simple (hence the name) algorithm that
 *  works for a very large set of grammers.
 */
// }}}
class SlrParser : public Parser // {{{
{
  public:
    //! Construct parser with empty grammer.
    SlrParser();
    //! Construct parser and load grammer from file.
    SlrParser(const std::string &filename);
    //! Destructor
    virtual ~SlrParser();

    //! Perform SLR algorithm on buffer and return the found parsetree
    virtual parsetree *Parse(const std::string &buffer);

  protected:
    // DOCUMENTATION {{{
    //! Represents a NFA node.
    /*!This is specified by position in a specific case in a specific type.
     */
    // }}}
    struct node
    { std::string t; //!< The type_name
      std::string c; //!< The case_name
      int p;         //!< The position (index)
    };
    // DOCUMENTATION {{{
    //! Represents the action to be performed when observing a token (at a specific state).
    /*! The possible actions are Shift and Reduce, with the necessary info to perform the action.
     */
    // }}}
    struct action
    { std::string sr; //!< shift, reduce or empty
      int dest;       //!< dest state if shift
      std::string t;  //!< type if reduce
      std::string c;  //!< case if reduce
    };

    //! BNF type Accessor
    SymBnf &GetType(const std::string &name);
    //! Looks up name in the type list, to determine if the name is a type name. Otherwise it is a token name.
    bool IsType(const std::string &name);
    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c);
    virtual void AddType(const std::string &type_name,
                         const Bnf &t);

    //! Sets the initial (main) type. Parsetree should have this type as the root constructor.
    void SetInit(std::string init) {myInit=init;}
    
    //! Standard epsilon.closure, implemented on the bare BNF representation of the NFAs
    void EpsilonClosure(std::set<node> &state);
    //! Lookup action, and calculate it if it is not found
    std::pair<int,action> FindAction(int state, const std::string &symbol);
    //! Map from type name to definition
    std::map<std::string,SymBnf> myTypes;
    std::string myInit;

    //! List of known states (can be referenced by index)
    std::vector<std::set<node> > myStates;
    //! For each edge (state and token- or type-name) stores the action and destination (if shift) state
    std::map<std::pair<int,std::string>,action> myTransitions;
};

}

#endif
