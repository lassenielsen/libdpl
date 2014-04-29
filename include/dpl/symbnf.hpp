#ifndef DPL_SYMBNF_HPP
#define DPL_SYMBNF_HPP

#include <dpl/bnf.hpp>
#include <dpl/parsetree.hpp>

namespace dpl {

class SymBnf : public Bnf
{
  public:
    SymBnf(const std::string &bnf_string="");
    SymBnf(const Bnf &bnf);
    virtual ~SymBnf();

    const std::set<std::string> &First() const {return myFirst;}
    const std::set<std::string> &Last() const {return myLast;}
    const std::set<std::pair<std::string,std::string> > &Frame() const {return myFrame;}

    bool HasFirst(const std::string &token) const {return (myFirst.find(token) != myFirst.end());}
    bool HasLast(const std::string &token) const {return (myLast.find(token) != myLast.end());}
    bool HasFrame(const std::string &pre, const std::string &post) const {return (myFrame.find(std::pair<std::string,std::string>(pre,post)) != myFrame.end());}
    bool HasPre(const std::string &token) const {return (myPre.find(token) != myPre.end());}
    bool HasPost(const std::string &token) const {return (myPost.find(token) != myPost.end());}

    bool AddFirst(const std::string &token);
    bool AddFirst(const std::set<std::string> &source);
    bool AddLast(const std::string &token);
    bool AddLast(const std::set<std::string> &source);
    bool AddFrame(const std::string &pre, const std::string &post);

    bool Nullable() const {return myNullable;}
    void SetNullable(bool value=true);
    const parsetree &VoidRep() const {return myVoidRep;}
    void SetVoidRep(const parsetree &rep);

  private:
    // Calculated info
    std::set<std::string> myFirst;
    std::set<std::string> myLast;
    std::set<std::pair<std::string,std::string> > myFrame;
    std::set<std::string> myPre;
    std::set<std::string> myPost;
    bool myNullable;
    parsetree myVoidRep;
};

}
#endif
