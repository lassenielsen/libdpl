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
    const std::set<std::string> &Pre() const {return myPre;}
    const std::set<std::string> &Post() const {return myPost;}

    bool HasFirst(const std::string &token) const {return (myFirst.find(token) != myFirst.end());}
    bool HasLast(const std::string &token) const {return (myLast.find(token) != myLast.end());}
    bool HasFrame(const std::string &pre, const std::string &post) const {return (myFrame.find(std::pair<std::string,std::string>(pre,post)) != myFrame.end());}
    bool HasPre(const std::string &token) const {return (myPre.find(token) != myPre.end());}
    bool HasPost(const std::string &token) const {return (myPost.find(token) != myPost.end());}

    bool AddFirst(const std::string &token);
    bool AddFirst(const std::set<std::string> &source);
    bool AddLast(const std::string &token);
    bool AddLast(const std::set<std::string> &source);
    bool AddPre(const std::string &token);
    bool AddPost(const std::string &token);
    bool AddFrame(const std::string &pre, const std::string &post);

    bool Nullable() const {return myVoidRep!=NULL;}
    const parsetree &VoidRep() const {return *myVoidRep;}
    void SetVoidRep(parsetree *rep);

  private:
    // Calculated info
    std::set<std::string> myFirst;
    std::set<std::string> myLast;
    std::set<std::pair<std::string,std::string> > myFrame;
    std::set<std::string> myPre;
    std::set<std::string> myPost;
    parsetree *myVoidRep;
};

}
#endif
