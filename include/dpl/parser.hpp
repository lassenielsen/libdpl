#ifndef DPL_PARSER_HPP
#define DPL_PARSER_HPP

#include <string>
#include <vector>
#include <set>
#include <map>
#include <dpl/tokenizer.hpp>

namespace dpl
{

/* parsed_tree represents a tree structure where each node holds a token.
 * This is the type returned by the Parse method in the Parser class.
 * The tree represents elements of the given abstract syntax.
 */
class parsed_tree // {{{
{
  public:
    parsed_tree(const token &t);
    parsed_tree(const std::string &type_name,
                const std::string &case_name,
                std::vector<parsed_tree*> &content);
    parsed_tree(const std::string &error="");
    parsed_tree(const parsed_tree &rhs);
    ~parsed_tree();

    /* Gives a string representation of the tree with parentheses
     * This is different from the Unparse method with parentheses,
     * because the result contains the token names instead of the content. 
     */
    std::string ToString(bool include_cases=false);
    std::string type_name;
    std::string case_name;
    token root; // only used if tree is a single token
    std::vector<parsed_tree*> content;
    bool is_token;
}; // }}}

/* bnf_case represents the definition of one case of an bnf type.
 * This is an internal structure, and is not relevant for client users.
 */
class bnf_case // {{{
{
  public:
    bnf_case(const std::string &name="");

    std::string name; // This is not the same as the type name
    std::vector<std::string> args; // the (possibly empty) sequence of types the case contains
}; // }}}

/* bnf_type represents the definition of one bnf type.
 * This is an internal structure, and is not relevant for client users.
 * A bnf_type object represents a bnf equation of the form
 * name ::= case1 | case2 | case3 ...
 */
class bnf_type // {{{
{
  public:
    bnf_type(const std::string &bnf_string="");
    virtual ~bnf_type();

    const std::string &Name();
    //Do not use SetName directly
    void SetName(const std::string &name);
    std::vector<bnf_case> &Cases();
    bool Empty();

    void DefCase(const bnf_case &c);

  private:
    std::string myName;
    std::vector<bnf_case> myCases;
}; // }}}

/* Parser represents a tree structure where each node holds a token.
 * This is the type returned by the Parse method in the Parser class.
 * The tree represents elements of the given abstract syntax.
 */
class Parser : public Tokenizer // {{{
{
  public:
    /* Create empty parser based on the given tokenizer
     */
    Parser();
    /* Create parser and tokenizer from definitions in file
     * File uses syntax
     * <tokenname> := "<tokendef>"
     * <typename> ::= <case1> | <case2> ...
     */
    Parser(const std::string &filename);
    virtual ~Parser();

    /* DefCase adds a type case to the parsing rules.
     */
    void DefCase(const std::string &type_name,
                 const std::string &constructor_name,
                 const std::vector<std::string> &def);

    /* DefType takes a string holding a definition in Bacus-Nauer-Form,
     * and adds the definition it describes.
     * Note that BNF's does not give any way to define
     * case names, so the case names
     * will be case1, case 2 and so on.
     */
    bool DefType(const std::string &bnf);

    /* Parse is the main function, and turns the given buffer into a tree using the given rules
     * The result is a parsed_tree
     * If an error occured the returned tree will be a single node named "_ERROR"
     * where the content is the error message.
     */
    parsed_tree *Parse(const std::string &buffer);

    /* Unparse takes a parsed_tree and returns a string
     * containing the original buffer.
     */
    std::string Unparse(const parsed_tree &tree);

  protected:
    // Accessor
    bnf_type &GetType(const std::string &name);
    bool IsType(const std::string &name);

    /* Fixpoint functions
     * Used to determin the Nullable, First, Last,
     * Pre and Post(known as Follow) properties
     */
    /* FixNullable finds the last fixpoint of the
     * nullable property.
     */
    void FixNullable();
    /* FixFirst finds the last fixpoint of the
     * first property.
     */
    void FixFirst();
    /* FixLast finds the last fixpoint of the
     * last property.
     */
    void FixLast();
    /* FixPre finds the last fixpoint of the
     * pre property.
     */
    void FixPre();
    /* FixPost finds the last fixpoint of the
     * post property.
     */
    void FixPost();
    /* FixAll finds the least fixpoint of the semantical properties
     * Nullable
     * First
     * Last
     * Pre
     * Post.
     */
    void FixAll();

    /* Internal function to search for cases that can be applied to the end of a list of tokens
     * FIXME: Optimize this to obtain a better theoretical upper timelimit for parsing
     */ 
    bool make_reduction(std::vector<parsed_tree*> &peephole, bnf_case this_case, bnf_type &this_type, std::vector<parsed_tree*> &buffer);

    // Variables
    std::map<std::string,bnf_type> myTypes;
}; // }}}

}

#endif
