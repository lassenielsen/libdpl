#include <dpl/slrparser.hpp>
#include <iostream>
#include <dpl/common.hpp>

using namespace std;
using namespace dpl;

SlrParser::SlrParser(const string &init) // {{{
{ SetInit(init);
} // }}}

SlrParser::SlrParser(const string &init, const string &filename) // {{{
{ SetInit(init);
  LoadFile(filename);
} // }}}

SlrParser::~SlrParser() // {{{
{
} // }}}

parsetree *SlrParser::Parse(const string &buffer) // {{{
{
  // FIXME: IF DIRTY
  myStates.clear();
  myTransitions.clear();
  if (! IsType("__INIT__"))
    DefType("__INIT__ ::= ::init " + myInit);
  GetType("__INIT__").AddPost("_EOF");

  FixAll();
  // Initialize buffer
  SetBuffer(buffer);


  // Create initial state
  { set<node> init_state;
    SymBnf &init=GetType("__INIT__");
    vector<string> init_cases=init.CaseNames();
    for (int i=0; i<init_cases.size(); ++i)
    { node n;
      n.t="__INIT__";
      n.c=init_cases[i];
      n.p=0;
      init_state.insert(n);
    }
    EpsilonClosure(init_state);
    myStates.push_back(init_state);
  }
  // Accept initial symbol
  { action result;
    result.sr="SHIFT";
    result.dest=AddState(set<node>());
    myTransitions[pair<int,string>(0,"__INIT__")]=result;
  }
  

  /* Build states dynamically
  // Build reachable states, and transitions
  size_t processed=0;
  while (processed<states.size())
  { map<string,set<state>> steps;
    FindSteps(states[processed],steps);
    for (map<string,set<state> >::const_iterator step=steps.begin(); step!=steps.end(); ++step)
    { EpsilonClosure(step->second);
      vector<set<state> >::const_iterator target=states.find(step->second);
      if (target==states.end())
      { states.push_back(step->second);
        transitions[pair<int,string>(processed,step->first)]=states.size()-1;
      }
      else
      { transitions[pair<int,string>(processed,step->first)]=target-states.begin();
      }
    }
    ++processed;
  }
  */

  // Perform parsing
  vector<int> state_stack;
  state_stack.push_back(0);
  vector<parsetree*> tree_stack;
  vector<parsetree*> peephole;
  
  // Do parsing
  while (!Empty() || peephole.size()>0)
  {
    parsetree *next=NULL;
    if (peephole.size()==0)
    { // Receive token from Tokenizer
      token post_token = PopToken();
      if (post_token.Name() == "_ERROR") // Tokenizer error {{{
      {
        SetBuffer("");
        string msg= "Tokenizer error: ";
        msg += post_token.Content();
        // Clean up
        DeleteVector(peephole);
        DeleteVector(tree_stack);
        // Report error
        throw(msg);
      } // }}}
      else
        next=new parsetree(post_token);
    }
    else
    { // Take subtree from peephole
      next=peephole.back();
      peephole.pop_back();
    }

    //cout << "NEXT: " << next->ToString() << endl;
    // Perform actions from next
    action a = FindAction(state_stack.back(),next->Type());

    if (a.sr=="REDUCE")
    { SymBnf &red_bnf = GetType(a.t);
      vector<string> red_prd = red_bnf.Case(a.c);
      vector<parsetree*> red_content;
      red_content.insert(red_content.end(),tree_stack.end()-red_prd.size(),tree_stack.end());
      tree_stack.erase(tree_stack.end()-red_prd.size(),tree_stack.end());
      state_stack.erase(state_stack.end()-red_prd.size(),state_stack.end());
      peephole.push_back(next);
     
      string sugar=myTypes[a.t].Sugar(a.c);
      if (sugar!="")
      { Tokenizer *sugarTokenizer=new Tokenizer(*this);
        sugarTokenizer->DefToken("::tag","::[a-z][a-z]*");
        sugarTokenizer->SetBuffer(sugar);
        vector<token> sugarTokens;
        sugarTokenizer->Tokenize(sugarTokens);
        delete sugarTokenizer;
        for (int i=sugarTokens.size()-1; i>=0; --i)
        { if (sugarTokens[i].Name()=="_EOF")
            continue;
          if (sugarTokens[i].Name()=="::tag")
          { map<string,size_t>::const_iterator tag=myTypes[a.t].Tags(a.c).find(sugarTokens[i].Content().substr(2));
            if (tag==myTypes[a.t].Tags(a.c).end())
            { throw string("Unknown tag ") + sugarTokens[i].Content() + " at current location: " + next->Position();
            }
            peephole.push_back(new parsetree(*red_content[tag->second]));
          }
          else
            peephole.push_back(new parsetree(sugarTokens[i]));
        }
        DeleteVector(red_content);
      }
      else
      { // Perform reduce
        map<string,size_t> tags=myTypes[a.t].Tags(a.c);
        parsetree *new_tree=new parsetree(a.t, a.c, red_content,tags);
        peephole.push_back(new_tree);
      }
    }
    else if (a.sr=="SHIFT") {
      // Move to peephole, and update stack
      tree_stack.push_back(next);
      state_stack.push_back(a.dest);
    }
    else
    { // Unknown action - parser error
      if (tree_stack.size()==1 && tree_stack[0]->Type()=="__INIT__" && next->Type()=="_EOF") // Accept
      { delete next;
        break;
      }
      else
      { stringstream ss;
        ss << "next=" << next->Type() << " is not accepted at current location: " << next->Position() << endl
           << "With istack:" << endl;
        for (size_t i=0; i<tree_stack.size(); ++i)
          ss << " - " << tree_stack[i]->TypeCase() << "@" << tree_stack[i]->Position() << endl;
        ss << "Exprected symbols are:" << endl;
        set<string> expected=ExpectedSymbols(state_stack.back());
        for (set<string>::const_iterator it=expected.begin(); it!=expected.end(); ++it)
          ss << " + " << *it << endl;
        throw ss.str();
      }
    }
  }
  SetBuffer("");
  if (tree_stack.size() != 1)
  {
    string error = "Parser error at: ";
    for (vector<parsetree*>::iterator it = tree_stack.begin(); it != tree_stack.end(); ++it)
    {
      error+= " ";
      error+= (*it)->Type();
      if ((*it)->Case() != "_TOKEN")
      { error+= ".";
        error+= (*it)->Case();
      }
      delete *it;
    }
    throw error;
  }

  if (tree_stack[0]->Type()!="__INIT__" || tree_stack[0]->Case()!="init")
    throw string("Unexpected result: ") + tree_stack[0]->Type() + "." + tree_stack[0]->Case();

  parsetree *result = new parsetree(*tree_stack[0]->Child(0));
  DeleteVector(tree_stack);

  // If no error, return the found tree
  return result;
} // }}}

void SlrParser::EpsilonClosure(set<node> &state) // {{{
{ set<node> workset = state;

  while (workset.size()>0)
  {
    node w=*workset.begin();
    workset.erase(workset.begin());
    vector<string> def=GetType(w.t).Case(w.c);
    if (w.p<def.size() && IsType(def[w.p]))
    { // Not Currently
      //if (GetType(def[p]).Nullable())
      //{
      //  node n = *workset.begin();
      //  ++n.p;
      //  if (state.find(n)==state.end())
      //  { state.insert(n);
      //    workset.insert(n);
      //  }
      //}

      vector<string> cases=GetType(def[w.p]).CaseNames();
      for (vector<string>::const_iterator c=cases.begin(); c!=cases.end(); ++c) 
      { node n;
        n.t=def[w.p];
        n.c=*c;
        n.p=0;
        if (state.find(n)==state.end())
        { state.insert(n);
          workset.insert(n);
        }
      }
    }
   
  }
} // }}}

set<string> SlrParser::ExpectedSymbols(int state) // {{{
{ set<string> result;
  set<node> source=myStates[state];
  for (set<node>::const_iterator n=source.begin(); n!=source.end(); ++n)
  { SymBnf &n_bnf=GetType(n->t);
    const vector<string> &n_case=n_bnf.Case(n->c);
    if (n->p<n_case.size())
      result.insert(n_case[n->p]);
    else if (n->p==n_case.size())
      result.insert(n_bnf.Post().begin(),n_bnf.Post().end());
  }
  return result;
} // }}}

SlrParser::action SlrParser::FindAction(int state, const std::string &symbol) // {{{
{ map<pair<int,string>,action>::const_iterator edge = myTransitions.find(pair<int,string>(state,symbol));
  if (edge!=myTransitions.end())
    return edge->second;

  // If no hit, calculate action
  set<node> source=myStates[state];
  //cout << "FindAction({";
  //for (set<node>::const_iterator i = source.begin(); i!=source.end(); ++i)
  //  cout << i->t << "." << i->c << ":" << i->p << ",";
  //cout << "}, " << symbol << ")";

  set<node> shift_dest;
  string reduce_type;
  string reduce_case;

  for (set<node>::const_iterator n=source.begin(); n!=source.end(); ++n)
  { SymBnf &n_bnf=GetType(n->t);
    const vector<string> &n_case=n_bnf.Case(n->c);
    if (n->p<n_case.size() && n_case[n->p]==symbol)
    { node s=*n;
      ++s.p;
      shift_dest.insert(s);
    }
    else if (n->p==n_case.size() && n_bnf.Post().find(symbol)!=n_bnf.Post().end()) { // Reduce
      if (reduce_type.size()>0)
      { map<string,int>::const_iterator prio1=myRRRules.find(reduce_type);
        map<string,int>::const_iterator prio2=myRRRules.find(n->t);
        if (prio1==myRRRules.end() || prio2==myRRRules.end() || prio1->second==prio2->second)
          cerr << "Reduce-reduce conflict for " << reduce_type << "." << reduce_case << " and " << n->t << "." << n->c << endl;
        else if (prio1->second < prio2->second)
        { // Do Nothing
        }
        else
        { // Use new reduction
          reduce_type = n->t;
          reduce_case = n->c;
        }
      }
      else
      { reduce_type = n->t;
        reduce_case = n->c;
      }
    }
  }

  if (shift_dest.size()>0 && reduce_type.size()>0)
  { map<string,bool>::const_iterator rule=mySRRules.find(reduce_type);
    if (rule==mySRRules.end())
      cerr << "Shift-reduce conflict on type:" << reduce_type << endl;
    else if (rule->second)
      shift_dest.clear();
    else
      reduce_type="";
  }

  action result;
  if (reduce_type.size()>0)
  { result.sr="REDUCE";
    result.t=reduce_type;
    result.c=reduce_case;
  }
  else if (shift_dest.size()>0)
  { result.sr="SHIFT";
    EpsilonClosure(shift_dest);
    result.dest=AddState(shift_dest);
  }
  else
  {  result.sr="EMPTY";
  }

  // Store result
  myTransitions[pair<int,string>(state,symbol)]=result;

  //cout << " = " << result.sr << endl;
  return result;
} // }}}

int SlrParser::FindState(const set<node> &state) const // {{{
{
  for (int index=0;index<myStates.size(); ++index)
  { bool eq=true;
    // Check myStates[index] included in shift_dest
    for (set<node>::const_iterator s=myStates[index].begin(); eq && s!=myStates[index].end(); ++s)
      if (state.find(*s)==state.end())
        eq=false;
    // Check shift_dest included in myStates[index]
    for (set<node>::const_iterator s=state.begin(); eq && s!=state.end(); ++s)
      if (myStates[index].find(*s)==myStates[index].end())
        eq=false;
    if (eq)
      return index;
  }
  return -1;
} // }}}

int SlrParser::AddState(const set<node> &state) // {{{
{
  int index=FindState(state);

  if (index>=0)
    return index;
  else
  { myStates.push_back(state);
    return myStates.size()-1;
  }
} // }}}
