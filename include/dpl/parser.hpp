#ifndef DPL_PARSER_HPP
#define DPL_PARSER_HPP

#include <string>
#include <vector>
#include <dpl/tokenizer.hpp>
#include <dpl/parsetree.hpp>
#include <dpl/bnf.hpp>

namespace dpl
{

// DOCUMENTATION {{{
//! Parser is a superclass and interface for the parser implementations.
/*! Parser contains a grammar as named types defined by BNFs, and provides
 *  functionality for parsing input texts with the contained grammar.
 *
 *  The text is tokenized by the Tokenizer superclass, and the token list is
 *  parsed using the method implemented by the used subclass.  The result is
 *  returnes as a parsetree object.
 */
// }}}
class Parser : public Tokenizer // {{{
{
  public:
    //! Constructor for creating empty parser
    Parser();
    //! Destructor
    virtual ~Parser();

    // DOCUMENTATION {{{
    //! Create parser and tokenizer from definitions in file
    /*! File uses syntax
     *  <tokenname> := "<tokendef>"
     *  <typename> ::= <case1> | <case2> ...
     */
    // }}}
    void LoadFile(const std::string &filename);

    // DOCUMENTATION {{{
    //! DefType takes a string holding a definition in Backus-Nauer-Form, and adds the definition it describes.
    /*! Note that BNF's does not give any way to define case names.
     *  This functionality has been added to the syntax, by disallowing token-
     *  and type-names to start with ::, and cases starting with ::case_name
     *  will use the specified case name.
     */
    // }}}
    bool DefType(const std::string &bnf);

    // DOCUMENTATION {{{
    //! Parse is the main function, and turns the given buffer into a tree using the given rules
    /*! The result is a parsetree
     *  If an error occured the returned tree will be a single node named "_ERROR"
     *  where the content is the error message.
     */
    // }}}
    virtual parsetree *Parse(const std::string &buffer)=0;

    //! Unparse takes a parsetree and returns a string containing the original buffer.
    std::string Unparse(const parsetree &tree);

  protected:
    //! AddCase adds the case definition to the type specified by type_name, under the case_name specified
    virtual void AddCase(const std::string &type_name,
                         const std::string &case_name,
                         const std::vector<std::string> &c)=0;
    //! AddType adds the type definition to the parser grammar under the type_name specified
    virtual void AddType(const std::string &type_name,
                         const Bnf &t) =0;
}; // }}}

}

#endif
