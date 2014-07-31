#ifndef DPL_SYMBNF_HPP
#define DPL_SYMBNF_HPP

#include <dpl/bnf.hpp>
#include <dpl/parsetree.hpp>

namespace dpl {

// DOCUMENTATION {{{
//! SymBnf in an extension of the Bnf class with common properties usefull
//! for mamy parsing algorighms.
/*! SymBnf includes the properties used by LL and SLR parsers: 
 *    \li First
 *    \li Post - also known as Follow
 *
 *  SymBnf also includes the symmetric properties (hence the name):
 *    \li Last
 *    \li Pre
 *
 *  SymBnf includes the Frame property which is a refined combination of Pre and Post.
 *
 *  Finally a parsetree for the empty string is stored if one exists.
 */
// }}}
class SymBnf : public Bnf
{
  public:
    //! SymBnf constructor is a simple wrapper around the Bnf constructior
    SymBnf(const std::string &bnf_string="");
    //! Copy Constructor
    SymBnf(const Bnf &bnf);
    //! Destructor
    virtual ~SymBnf();

    //! Accessor for the First property
    const std::set<std::string> &First() const {return myFirst;}
    //! Accessor for the Last property
    const std::set<std::string> &Last() const {return myLast;}
    // DOCUMENTATION {{{
    //! Accessor for the Frame property
    /*! The frame property is a refined combination of the Pre and Post
     *  property, containing the pairs of token- and type-names that can precede and
     *  succeed the current type. The refinement is that not all combinations from
     *  Pre and Post are possible, and this is accounted for by the element in
     *  Frame.
     */
    // }}}
    const std::set<std::pair<std::string,std::string> > &Frame() const {return myFrame;}
    //! Accessor for the Pre property
    const std::set<std::string> &Pre() const {return myPre;}
    //! Accessor for the Post property
    const std::set<std::string> &Post() const {return myPost;}

    //! Check if token- or type-name can begin occur at beginning of current type.
    bool HasFirst(const std::string &token) const {return (myFirst.find(token) != myFirst.end());}
    //! Check if token- or type-name can begin occur at end of current type.
    bool HasLast(const std::string &token) const {return (myLast.find(token) != myLast.end());}
    //! Check if token- or type-names can frame of current type (occur immediately before and after).
    bool HasFrame(const std::string &pre, const std::string &post) const {return (myFrame.find(std::pair<std::string,std::string>(pre,post)) != myFrame.end());}
    //! Check if token- or type-names can occur immediately after current type.
    bool HasPre(const std::string &token) const {return (myPre.find(token) != myPre.end());}
    //! Check if token- or type-names can occur immediately before current type.
    bool HasPost(const std::string &token) const {return (myPost.find(token) != myPost.end());}

    //! Add token (or type) name to the First set
    bool AddFirst(const std::string &token);
    //! Add token (or type) names in set to the First set
    bool AddFirst(const std::set<std::string> &source);
    //! Add token (or type) name to the Last set
    bool AddLast(const std::string &token);
    //! Add token (or type) names in set to the Last set
    bool AddLast(const std::set<std::string> &source);
    //! Add token (or type) name to the Pre set
    bool AddPre(const std::string &token);
    //! Add token (or type) name to the Post set
    bool AddPost(const std::string &token);
    //! Add pair of token (or type) names to the Frame set
    bool AddFrame(const std::string &pre, const std::string &post);

    //! Returns if the empty string can be accepted (recognised)
    bool Nullable() const {return myVoidRep!=NULL;}
    //! Returns a parsetree for the empty string.
    const parsetree &VoidRep() const {return *myVoidRep;}
    //! Set the parsetree used for parsing the empty string.
    void SetVoidRep(parsetree *rep);

  private:
    std::set<std::string> myFirst;
    std::set<std::string> myLast;
    std::set<std::pair<std::string,std::string> > myFrame;
    std::set<std::string> myPre;
    std::set<std::string> myPost;
    parsetree *myVoidRep;
};

}
#endif
