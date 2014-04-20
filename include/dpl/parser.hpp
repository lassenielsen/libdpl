#ifndef DPL_PARSER_HPP
#define DPL_PARSER_HPP

#include <string>
#include <vector>
#include <dpl/tokenizer.hpp>
#include <dpl/parsetree.hpp>
#include <dpl/bnf.hpp>

namespace dpl
{

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
    virtual ~Parser();

    /* DefType takes a string holding a definition in Backus-Nauer-Form,
     * and adds the definition it describes.
     * Note that BNF's does not give any way to define
     * case names, so the case names
     * will be case1, case2 and so on.
     */
    bool DefType(const std::string &bnf);

    /* Parse is the main function, and turns the given buffer into a tree using the given rules
     * The result is a parsetree
     * If an error occured the returned tree will be a single node named "_ERROR"
     * where the content is the error message.
     */
    virtual parsetree *Parse(const std::string &buffer)=0;

    /* Unparse takes a parsetree and returns a string
     * containing the original buffer.
     */
    std::string Unparse(const parsetree &tree);

  protected:
    void LoadFile(const std::string &filename);
    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c)=0;
    virtual void AddType(const std::string &type_name,
                         const Bnf &t) =0;
}; // }}}

}

#endif
