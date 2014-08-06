#ifndef DPL_SLRPARSER_HPP
#define DPL_SLRPARSER_HPP

#include <map>
#include <dpl/symparser.hpp>
#include <string>

namespace dpl
{

// DOCUMENTATION {{{
//! SlrParser implements the standars SLR parsing algorithm.
/*! The SLR (Simple Left-to-right Rightmost-derivation) parsing algorithm is
 *  often used in teaching, as it is a simple (hence the name) algorithm that
 *  works for a very large set of grammers.
 */
// }}}
class SlrParser : public SymParser // {{{
{
  public:
    //! Construct parser with empty grammer.
    SlrParser(const std::string &init);
    //! Construct parser and load grammer from file.
    SlrParser(const std::string &init, const std::string &filename);
    //! Destructor
    virtual ~SlrParser();

    //! Perform SLR algorithm on buffer and return the found parsetree
    virtual parsetree *Parse(const std::string &buffer);

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

  protected:
    // DOCUMENTATION {{{
    //! Represents the action to be performed when observing a token (at a specific state).
    /*! The possible actions are Shift and Reduce, with the necessary info to perform the action.
     */
    // }}}
    struct action
    { std::string sr; //!< SHIFT, REDUCE, ACCEPT or EMPTY
      int dest;       //!< dest state if shift
      std::string t;  //!< type if reduce
      std::string c;  //!< case if reduce
    };

    //! Sets the initial (main) type. Parsetree should have this type as the root constructor.
    void SetInit(std::string init) {myInit=init;}
    
    //! Standard epsilon.closure, implemented on the bare BNF representation of the NFAs
    void EpsilonClosure(std::set<node> &state);
    //! Lookup action, and calculate and stores transition if it is not found
    action FindAction(int state, const std::string &symbol);
    //! Map from type name to definition
    std::map<std::string,SymBnf> myTypes;
    std::string myInit;

    //! List of known states (can be referenced by index)
    std::vector<std::set<node> > myStates;
    //! For each edge (state and token- or type-name) stores the action and destination (if shift) state
    std::map<std::pair<int,std::string>,action> myTransitions;
};

inline bool operator<(const SlrParser::node &lhs, const SlrParser::node &rhs) // {{{
{ return lhs.t<rhs.t || (lhs.t==rhs.t && lhs.c<rhs.c) || (lhs.t==rhs.t && lhs.c==rhs.c && lhs.p<rhs.p);
} // }}}
inline bool operator==(const SlrParser::node &lhs, const SlrParser::node &rhs) // {{{
{ return lhs.t==rhs.t && lhs.c==rhs.c && lhs.p==rhs.p;
} // }}}

}

#endif
