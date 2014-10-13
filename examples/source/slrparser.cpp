#include <iostream>
#include <dpl/slrparser.hpp>

using namespace dpl;
using namespace std;

class Printer : public SymParser // {{{
{
  public:
    Printer(const string &filename) : SymParser(filename) { }
    virtual ~Printer() { }

    void PrintDetails() // Print meta information about types
    {
      FixAll();
      for (map<string,SymBnf>::const_iterator it=myTypes.begin(); it!=myTypes.end(); ++it)
      {
        if (!IsType(it->first))
          continue;
        const SymBnf &t = it->second;
        cout << "Type: " << t.GetName() << endl;
        cout << "\tNullable: " << t.Nullable() << endl;
        cout << "\tFirst: ";
        for (set<string>::const_iterator it=t.First().begin(); it!=t.First().end(); ++it)
          cout << *it << " ";
        cout << endl;
        cout << "\tLast: ";
        for (set<string>::const_iterator it=t.Last().begin(); it!=t.Last().end(); ++it)
          cout << *it << " ";
        cout << endl;
        cout << "\tFrame: ";
        for (set<pair<string,string> >::const_iterator it=t.Frame().begin(); it!=t.Frame().end(); ++it)
          cout << "(" << it->first << "," << it->second << ")" << " ";
        cout << endl;
        cout << "\tPre: ";
        for (set<string>::const_iterator it=t.Pre().begin(); it!=t.Pre().end(); ++it)
          cout << *it << " ";
        cout << endl;
        cout << "\tPost: ";
        for (set<string>::const_iterator it=t.Post().begin(); it!=t.Post().end(); ++it)
          cout << *it << " ";
        cout << endl;
        cout << "\tCases: " << endl;
        vector<string> case_names = t.CaseNames();
        for (vector<string>::const_iterator it=case_names.begin(); it!=case_names.end(); ++it)
        { cout << "\t\t" << *it << " ::=";
          for (vector<string>::const_iterator arg=t.Case(*it).begin(); arg!=t.Case(*it).end(); ++arg)
            cout << " " << *arg;
          cout << endl;
        }

      }
    }
}; // }}}

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
