#ifndef DPL_SYMPARSER_HPP
#define DPL_SYMPARSER_HPP

#include <map>
#include <dpl/symbnf.hpp>
#include <dpl/parser.hpp>

namespace dpl
{

/*! Parser represents a tree structure where each node holds a token.
 *  This is the type returned by the Parse method in the Parser class.
 *  The tree represents elements of the given abstract syntax.
 */
class SlrParser : public Parser // {{{
{
  public:
    SlrParser();
    SlrParser(const std::string &filename);
    virtual ~SlrParser();

    virtual parsetree *Parse(const std::string &buffer);

  protected:
    struct node
    { std::string t;
      std::string c;
      int p;
    };
    struct action
    { std::string sr; // shift, reduce or empty
      int dest;       // dest state if shift
      std::string t;  // type if reduce
      std::string c;  // case if reduce
    };

    SymBnf &GetType(const std::string &name);
    bool IsType(const std::string &name);
    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c);
    virtual void AddType(const std::string &type_name,
                         const Bnf &t);

    void SetInit(std::string init) {myInit=init;}
    
    void EpsilonClosure(std::set<node> &state);
    std::pair<int,action> FindAction(int state, const std::string &symbol);
    //! Map from type name to definition
    std::map<std::string,SymBnf> myTypes;
    std::string myInit;

    std::vector<std::set<node> > myStates;
    std::map<std::pair<int,std::string>,std::pair<int,action>> myTransitions;
};

}

#endif
