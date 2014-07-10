#include <iostream>
#include <dpl/symparser.hpp>

using namespace dpl;
using namespace std;

int main(int argc, char **argv)
{
  try
  { if (argc<2) { cout << "Syntax: xmlparser \"<xmlstring>\"" << endl; return 1; }
    string xmlstring = argv[1];
    SymParser parser;
    parser.DefGeneralToken(""," \n\t");
    parser.DefToken("id", "[a-z][a-z]*");
    parser.DefToken("string", "\"[a-zA-Z0-9 ]*\"");
    parser.DefToken("eq","=");
    parser.DefKeywordToken("<",2);
    parser.DefKeywordToken("</",1);
    parser.DefKeywordToken(">");
    parser.DefKeywordToken("/>");
    parser.DefType("branch ::= < id equals > tree </ id > | < id equals />");
    parser.DefType("equals ::= | id eq string equals");
    parser.DefType("tree ::= | branch tree");
    
    parsetree *tree=parser.Parse(xmlstring);
    cout << tree->ToString() << endl;
    delete tree;
  }
  catch (string s)
  { cout << "Error parsing given xml: " << s << endl;
  }
  return 0;
}
