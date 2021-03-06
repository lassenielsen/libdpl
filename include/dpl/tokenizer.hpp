#ifndef DPL_TOKENIZER
#define DPL_TOKENIZER

#include <string>
#include <vector>
#include <rcp/re.hpp>
#include <rcp/bitcode.hpp>

namespace dpl
{

// DOCUMENATAION {{{
//! This is the type containing a token.
/*! This is used as the result of the PopToken method in Tokenizer.
 *  If name is "_ERROR" then it is not an actual token, but the result of an
 *  error. The error description can be found in the content field.
 */
// }}}
class token // {{{
{
  public:
    token(std::string name="", std::string content="", std::string pre_content=" ", int line=0, int column=0);

    const std::string &Name() const {return myName;}
    const std::string &Content() const {return myContent;}
    const std::string &Prelude() const {return myPrelude;}
    std::string Position() const;
    int Line() const {return myLine;}
    int Column () const {return myColumn;}

  private:
    std::string myName;
    std::string myContent;
    std::string myPrelude;
    int myLine;
    int myColumn;
}; // }}}

// DOCUMENATAION {{{
//! This class represents the definition of a token.
/*! This is an internal structure, and is not relevant for client users.
 */
// }}}
class tokendef // {{{
{
  public:
    std::string name;
    std::string definition;
    int priority;
}; // }}}

// DOCUMENATAION {{{
//! This is the actual tokenizer class.
/*! It holds the methods necessary to define the tokens to be recognized,
 *  set the buffer to tokenize and finally obtain a sequence of tokens.
 */
// }}}
class Tokenizer // {{{
{
  public:
    // DOCUMENATAION {{{
    //! The constructor takes as input the parser mode, which specifies how the tokenization should be disambiguated.
    /*! The default mode is "gl" denoting a greedy leftmost disambiguation.
     */
    // }}}
    Tokenizer(std::string mode="gl");
    Tokenizer(const Tokenizer &init);
    virtual ~Tokenizer();

    // DOCUMENATAION {{{
    //! DefToken is used to define a token type.
    /*! The name will be used to identify the token type.
     *  If the name is "" then tokens of this type will be ignored,
     *  this is usefull for whitespace etc.
     *  The exp is a regular expression, defining when a string matches
     *  the given token type, for whitespace this could be "[ \t\n][ \t\n]*"
     *  The priority defines what token types should overrule others.
     *  Low priorities overrules high priorities.
     */
    // }}}
    void DefToken(const std::string &name, const std::string &exp, int priority=0);

    // DOCUMENATAION {{{
    //! DefToken is used to define a token type.
    /*! The given string must be on the form
     *  <name> := "<regular expression>"
     *  This call is then translated to
     *  DefToken(<name>, <regular expression>, priority);
     */
    // }}}
    bool DefToken(const std::string &def, int priority=-1);

    // DOCUMENATAION {{{
    //! DefKeywordToken is like DefToken,
    /*! but the expression is the same as the name.
     *  This is usefull since many token defs such as parentheses and keyword
     *  only accept a single string.
     */
    // }}}
    void DefKeywordToken(const std::string &keyword, int priority=0);

    // DOCUMENATAION {{{
    /*! DefGeneralToken is like DefToken,
     *  but the chars are automatically elaborated to [chars][chars]* .
     *  This is usefull since many token defs such as whitespace and
     *  numbers use this expression pattern.
     */
    // }}}
    void DefGeneralToken(const std::string &name, const std::string &chars, int priority=0);

    // DOCUMENATAION {{{
    //! DefSpecialToken is like DefToken, except the initchars and chars are
    //! automatically elaborated to [initchars][chars]* .
    /*! This is useful since many token definitions for variable names  etc.
     *  use this expression pattern.
     */
    // }}}
    void DefSpecialToken(const std::string &name, const std::string &initchars, const std::string &chars, int priority=0);

    // DOCUMENATAION {{{
    //! SetBuffer initializes or overwrites the buffer.
    /*! The buffer can later be translated into tokens using the
     *  PopToken or Tokenize methods.
     */
    // }}}
    void SetBuffer(const std::string &buffer);

    // DOCUMENATAION {{{
    //! PopToken returns the first token from the buffer.
    /*! The tokens are found using the given token definitions.
     *  If the name of the first token is "" then the token is ignored,
     *  and PopToken returns the next token.
     */
    // }}}
    token PopToken();

    //! Tokenize pops all the tokens and adds them to the given list.
    void Tokenize(std::vector<token> &dest);

    //! Serialize takes a list of tokens, and returns a string containing the
    //! original buffer.
    std::string Serialize(const std::vector<token> &source);

    // DOCUMENATAION {{{
    //! Empty returns true, if the buffer is empty.
    /*! The last token returned by PopToken before the buffer is
     *  empty will alway be "_EOF".
     *  If PopToken is called on an empty buffer, the token
     *  "_ERROR" is returned.
     */
    // }}}
    bool Empty();

    //! Determin if name has token definition
    bool IsTokenDef(const std::string &name) const { for (std::vector<tokendef>::const_iterator it=myTokenDefs.begin(); it!=myTokenDefs.end(); ++it) if (it->name==name) return true; return false; }
    //! Clear clears the token definitions as well as the buffer.
    void Clear();

  protected:
    std::vector<tokendef> myTokenDefs;
    BitCode myCompressed;
    int myPos;
    RE_Star *myStar;
    bool myDirty;
    std::string myMode;
    int myCurrentLine;
    int myCurrentColumn;
}; // }}}

}
#endif
