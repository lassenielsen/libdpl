#include <iostream>
#include <dpl/parser.hpp>

using namespace dpl;
using namespace std;

int main(int argc, char **argv)
{
  if (argc<2) { cout << "Syntax: xmlparser \"<xmlstring>\"" << endl; return 1; }
  string xmlstring = argv[1];
  Parser parser;
  parser.DefGeneralToken(""," \n\t");
  parser.DefToken("id", "[a-z][a-z]*");
  parser.DefToken("string", "\"[a-zA-Z0-9 ]*\"");
  parser.DefKeywordToken("=");
  parser.DefKeywordToken("<",2);
  parser.DefKeywordToken("</",1);
  parser.DefKeywordToken(">");
  parser.DefType("branch ::= < id equals > tree </ id >");
  parser.DefType("equals ::= | id = string equals");
  parser.DefType("tree ::= | branch tree");
  
  try
  { parsed_tree *tree=parser.Parse(xmlstring);
    cout << tree->ToString() << endl;
    delete tree;
  }
  catch (string s)
  { cout << "Error parsing given xml: " << s << endl;
  }
  return 0;
}
