#ifndef DPL_SYMPARSER_HPP
#define DPL_SYMPARSER_HPP

#include <map>
#include <dpl/symbnf.hpp>
#include <dpl/parser.hpp>

namespace dpl
{

// DOCUMENATAION {{{
//! SymParser implements the Parser interface using a simple parsing method.
/*! The parsing method provided by SymParser is based on recursive descend, and
 ** backtracking is eliminated by only descending when the frame matches.
 **
 ** This is less precise than SLR, but sufficient for many grammars, simpler
 ** and elegant - based on adding the symmetric equivilants of First and Post
 ** (known as Follow) use by most parsing algorithms.
 */
// }}}
class SymParser : public Parser // {{{
{
  public:
    //! Constructor
    SymParser();
    //! Constructor loading definitions in provided file
    SymParser(const std::string &filename);
    virtual ~SymParser();

    //! Parse implements the SymParser parsing algorithm, returning the found parsetree
    virtual parsetree *Parse(const std::string &buffer);

  protected:
    //! BNF type Accessor
    SymBnf &GetType(const std::string &name);
    //! Looks up name in the type list, to determine if the name is a type name. Otherwise it is a token name.
    bool IsType(const std::string &name);

    //! FixNullable finds the least fixpoint of the nullable property.
    void FixNullable();
    //! FixFirst finds the least fixpoint of the first property.
    void FixFirst();
    //! FixLast finds the least fixpoint of the last property.
    void FixLast();
    //! FixFrame finds the least fixpoint of the (pre,post) frame property.
    void FixFrame();
    //! FramePostRec is part of the FixFrame implementation, it adds (pre,post) frame and recursively uses the First set of the post type.
    bool FramePostRec(const std::string &pre, SymBnf &t, const std::string &post);
    //! FrameAllPosts is part of the FixFrame implementation, it calls FramePostRec (pre,post) and recursively proceeds to next post in definition if current is nullable.
    bool FrameAllPosts(const std::string &pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator end, const std::set<std::string> &follow);
    //! FramePostRec is part of the FixFrame implementation, it calls FrameAllPosts (pre,post) frame and recursively uses the Last set of the pre type.
    bool FramePreRec(const std::string &pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator end, const std::set<std::string> &follow);
    //! FrameAllPres is part of the FixFrame implementation, it calls FramePreRec (pre) and recursively proceeds to previous pre in definition if current is nullable.
    bool FrameAllPres(std::vector<std::string>::const_iterator pre, SymBnf &t, std::vector<std::string>::const_iterator post, std::vector<std::string>::const_iterator begin, std::vector<std::string>::const_iterator end, const std::set<std::string> &precede, const std::set<std::string> &follow);

    /*! FixAll finds the least fixpoint of the semantical properties
        \it Nullable
        \it First
        \it Last
        \it Pre
        \it Post.
     */
    void FixAll();

    //! Internal function to search for cases that can be applied to the end of a list of parsetrees
    bool make_reduction(std::vector<parsetree*> &peephole, const std::string &case_name, std::vector<std::string> &case_def, const SymBnf &this_type, std::vector<parsetree*> &buffer, std::string post, bool nonempty);

    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c);
    virtual void AddType(const std::string &type_name,
                         const Bnf &t);
    
    //! Map from type name to definition
    std::map<std::string,SymBnf> myTypes;
};

}

#endif
