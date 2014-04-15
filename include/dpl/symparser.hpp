#ifndef SYMPARSER_HPP
#define SYMPARSER_HPP

class sym_bnf_type : public bnf_type
{
  sym_bnf_type(const std::string &bnf_string="");
  virtual ~sym_bnf_type();

    const std::set<std::string> &First();
    const std::set<std::string> &Last();
    const std::set<std::string> &Pre();
    const std::set<std::string> &Post();

    bool HasFirst(const std::string &token);
    bool HasLast(const std::string &token);
    bool HasPre(const std::string &token);
    bool HasPost(const std::string &token);

    bool AddFirst(const std::string &token);
    bool AddFirst(const std::set<std::string> &source);
    bool AddLast(const std::string &token);
    bool AddLast(const std::set<std::string> &source);
    bool AddPre(const std::string &token);
    bool AddPre(const std::set<std::string> &source);
    bool AddPost(const std::string &token);
    bool AddPost(const std::set<std::string> &source);

    bool Nullable();
    void SetNullable(bool value=true);
    parsed_tree VoidRep();
    void SetVoidRep(const parsed_tree &rep);


  private:
    // Calculated info
    std::set<std::string> myFirst;
    std::set<std::string> myLast;
    std::set<std::string> myPre;
    std::set<std::string> myPost;
    bool myNullable;
    parsed_tree myVoidRep;
};

#endif
