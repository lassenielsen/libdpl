#include <iostream>
#include <dpl/slrparser.hpp>
#include <sstream>
#include <dpl/common.hpp>

using namespace dpl;
using namespace std;

void print_equals(bool first, int indent, const parsetree *tree, ostream &out) // {{{
{ if (tree->case_name=="none")
    ;
  else if (tree->case_name=="some") // id eq string equals
  { if (first)
      out << " ";
    else
    { out << endl;
      for (int i=0; i<indent; ++i)
        out << " ";
    }
    out << tree->content[0]->root.content;
    out << " = ";
    out << tree->content[2]->root.content;
    print_equals(false,indent,tree->content[3],out);
  }
  else
    throw string("Unknown equals case: ") + tree->case_name;
} // }}}
void print_tree(bool first, int indent, const parsetree *tree, ostream &out) // {{{
{ if (tree->case_name=="none")
    ;
  else if (tree->case_name=="some") // < id equals > tree < / id > tree
  { if (!first)
      out << endl;
    for (int i=0; i<indent; ++i)
      out << " ";
    out << "<" << tree->content[1]->root.content;
    print_equals(true,indent+2+tree->content[1]->root.content.size(),tree->content[2],out);
    out << ">";
    print_tree(false,indent+2,tree->content[4],out);
    out << endl;
    for (int i=0; i<indent; ++i)
      out << " ";
    out << "</";
    if (tree->content[1]->root.content!=tree->content[6]->root.content)
    { pair<int,int> pos = tree->GetPosition();
      throw string("Start and end tags do not match at line: ") + to_string(pos.first) + ", col: " + to_string(pos.second);
    }
    out << tree->content[6]->root.content;
    out << ">";
    print_tree(false,indent,tree->content[8],out);
  }
  else
    throw string("Unknown tree case: ") + tree->case_name;
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
