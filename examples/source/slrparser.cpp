#include <iostream>
#include <dpl/slrparser.hpp>

using namespace dpl;
using namespace std;

int main(int argc, char **argv) // {{{
{
  try {
    if (argc<3)
    {
      cout << "Syntax: slrparser <filename.bnf> StartSymbol [<parse string>]" << endl;
      return 1;
    }
    SlrParser parser(argv[2]);
    parser.LoadFile(argv[1]);
    if (argc>3)
    {
      parsetree *tree = parser.Parse(argv[3]);
      cout << tree->ToString() << endl;
      delete tree;
    }
  }
  catch (string s) {
    cout << "Exception: " << s << endl;
  }
  return 0;
} // }}}
