#include <iostream>
#include <dpl/parser.hpp>

using namespace dpl;
using namespace std;

class Printer : public Parser
{
  public:
    Printer(const string &filename) : Parser(filename) {}
    ~Printer() { }

    void PrintDetails() // Print meta information about types
    {
      FixAll();
      for (map<string,bnf_type>::iterator it=myTypes.begin(); it!=myTypes.end(); ++it)
      {
        if (!IsType(it->first))
          continue;
        bnf_type &t = it->second;
        cout << "Type: " << t.Name() << endl;
        cout << "\tNullable: " << t.Nullable() << endl;
        cout << "\tFirst: ";
        for (set<string>::const_iterator it=t.First().begin(); it!=t.First().end(); ++it)
          cout << *it << " ";
        cout << endl;
        cout << "\tLast: ";
        for (set<string>::const_iterator it=t.Last().begin(); it!=t.Last().end(); ++it)
          cout << *it << " ";
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
        for (vector<bnf_case>::const_iterator it=t.Cases().begin(); it!=t.Cases().end(); ++it)
        { cout << "\t\t" << it->name << " ::=";
          for (vector<string>::const_iterator arg=it->args.begin(); arg!=it->args.end(); ++arg)
            cout << " " << *arg;
          cout << endl;
        }

      }
    }
};

int main(int argc, char **argv)
{
  if (argc<2)
  {
    cout << "Syntax: showdefs <xfilename> [<parse string>]" << endl;
    return 1;
  }
  Printer printer(argv[1]);
  printer.PrintDetails();
  if (argc>2)
  {
    parsed_tree *tree = printer.Parse(argv[2]);
    cout << tree->ToString() << endl;
    delete tree;
  }
  return 0;
}
