#ifndef DPL_SYMPARSER_HPP
#define DPL_SYMPARSER_HPP

#include <unordered_map>
#include <dpl/symbnf.hpp>
#include <dpl/parser.hpp>

namespace dpl
{

/*! Parser represents a tree structure where each node holds a token.
 *  This is the type returned by the Parse method in the Parser class.
 *  The tree represents elements of the given abstract syntax.
 */
class SymParser : public Parser // {{{
{
  public:
    SymParser();
    SymParser(const std::string &filename);
    virtual ~SymParser();

    virtual parsetree *Parse(const std::string &buffer);

  protected:
    // Accessor
    SymBnf &GetType(const std::string &name);
    bool IsType(const std::string &name);

    /*! FixNullable finds the least fixpoint of the
        nullable property.
     */
    void FixNullable();
    /*! FixFirst finds the least fixpoint of the
        first property.
     */
    void FixFirst();
    /*! FixLast finds the least fixpoint of the
        last property.
     */
    void FixLast();
    /*! FixFrame finds the least fixpoint of the
        (pre,post) frame property.
     */
    void FixFrame();
    bool FramePostRec(const std::string &pre, SymBnf &t, const std::string &post);
    bool FrameAllPosts(const std::string &pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator end, const std::vector<std::string> &follow);
    bool SymParser::FramePreRec(const std::string &pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator end, const std::vector<std::string> &follow);
    bool FrameAllPres(std::vector<std::string>::const_iterator pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end, const std::vector<std::string> &precede, const std::vector<std::string> &follow);

    /*! FixAll finds the least fixpoint of the semantical properties
        Nullable
        First
        Last
        Pre
        Post.
     */
    void FixAll();

    /*! Internal function to search for cases that can be applied to the end of a list of tokens
     */ 
    bool make_reduction(std::vector<parsetree*> &peephole, const std::string &case_name, std::vector<std::string> &case_def, const SymBnf &this_type, std::vector<parsetree*> &buffer, std::string post="_EOF");

    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c);
    virtual void AddType(const std::string &type_name,
                         const Bnf &t);
    
    //! Map from type name to definition
    std::unordered_map<std::string,SymBnf> myTypes;
};

}

#endif
