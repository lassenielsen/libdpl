#include <iostream>
#include <dpl/slrparser.hpp>
#include <sstream>
#include <dpl/common.hpp>

using namespace dpl;
using namespace std;

void print_equals(bool first, int indent, const parsetree *tree, ostream &out) // {{{
{ if (tree->Case()=="none")
    ;
  else if (tree->Case()=="some") // id eq string equals
  { if (first)
      out << " ";
    else
    { out << endl;
      for (int i=0; i<indent; ++i)
        out << " ";
    }
    out << tree->Child(0)->Token().Content();
    out << " = ";
    out << tree->Child(2)->Token().Content();
    print_equals(false,indent,tree->Child(3),out);
  }
  else
    throw string("Unknown equals case: ") + tree->Case();
} // }}}
void print_tree(bool first, int indent, const parsetree *tree, ostream &out) // {{{
{ if (tree->Case()=="none")
    ;
  else if (tree->Case()=="some") // < id equals > tree < / id > tree
  { if (!first)
      out << endl;
    for (int i=0; i<indent; ++i)
      out << " ";
    out << "<" << tree->Child(1)->Token().Content();
    print_equals(true,indent+2+tree->Child(1)->Token().Content().size(),tree->Child(2),out);
    out << ">";
    print_tree(false,indent+2,tree->Child(4),out);
    out << endl;
    for (int i=0; i<indent; ++i)
      out << " ";
    out << "</";
    if (tree->Child(1)->Token().Content()!=tree->Child(6)->Token().Content())
      throw string("Start and end tags do not match at: ") + tree->Position();
    out << tree->Child(6)->Token().Content();
    out << ">";
    print_tree(false,indent,tree->Child(8),out);
  }
  else
    throw string("Unknown tree case: ") + tree->Case();
} // }}}

int main(int argc, char **argv)
{
  try
  { if (argc<2) { cout << "Syntax: xmlparser \"<xmlstring>\"" << endl; return 1; }
    string xmlstring = argv[1];
    SlrParser parser("tree");
    parser.DefGeneralToken(""," \n\r\f\t");
    parser.DefToken("id", "[a-zA-Z][a-zA-Z]*");
    parser.DefToken("string", "\"(\\\\[^]+[^\\\\\"])*\"");
    parser.DefToken("eq","=");
    parser.DefKeywordToken("<",2);
    parser.DefToken("</","<[ \n\r\f\t]*/",1);
    parser.DefKeywordToken(">");
    parser.DefType("equals ::= ::none | ::some id eq string equals");
    parser.DefType("tree ::= ::none | ::some < id equals > tree </ id > tree");
    
    parsetree *tree=parser.Parse(xmlstring);
    stringstream ss;
    print_tree(true,0,tree,ss);
    cout << ss.str() << endl;
    delete tree;
  }
  catch (string s)
  { cout << "Error parsing given xml: " << s << endl;
  }
  return 0;
}
