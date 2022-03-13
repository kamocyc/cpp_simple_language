#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <stdio.h>
#include "syntax.h"
#include "y.tab.h"

extern "C" FILE *yyin;
extern "C" int yydebug;

using namespace std;

enum class ValueType { Int, Unit, Pointer };

class ObjectValue {
public:
  virtual ~ObjectValue() = default;
};

union ValueData {
  int value;
  ObjectValue *pointer;
};

class Value {
public:
  Value() : value({.value=0}), type(ValueType::Unit) {}
  Value(ValueData value_, ValueType type_) : value(value_), type(type_) {}
  ValueData value;
  ValueType type;
};
Value createInt(int i) {
  return Value { { .value=i }, ValueType::Int };
}
Value createUnit() {
  return Value { { .value=0 }, ValueType::Unit };
}

class List : public ObjectValue {
public:
  List(Value value_, List *tail_) : value(value_), tail(tail_) {}
  Value value;
  List *tail;
};
List *createSingleton(Value value) {
  return new List { value, nullptr };
}
List *consList(Value value, List* tail) {
  return new List { value, tail };
}
Value createPointer(ObjectValue *o) {
  return Value { (ValueData){ .pointer=o }, ValueType::Pointer };
}

// Value getInitialValue(ValueType typ) {
//   switch(typ) {
//   case ValueType::Unit:
//     return createUnit();
//   case ValueType::Int:
//     return createInt(0);
//   }
// }

class Program {
public:
  Program(Expr *expr_) : expr(expr_) {}
// private:
  Expr *expr;
};


int extractInt(Value v) {
  if(v.type == ValueType::Int) {
    return v.value.value;
  } else {
    cout << "ERROR" << endl;
    exit(1);
  }
}

class Interpreter {
private:
  bool broken;
public:
  Interpreter(int var_num) {
    for(int i=0; i<var_num; i++) {
      env.push_back(createUnit());
    }
  }
  
  vector<Value> env;
  vector<vector<Value>*> objects;
  
  Value eval(Expr* expr);
  
  ~Interpreter() {
    for(auto object : objects) {
      delete object;
    }
  }
};

void log(const char *text) {
  // cout << text << endl;
}

void printValue(Value v) {
  switch(v.type) {
  case ValueType::Int:
    cout << extractInt(v) << endl;
    break;
  case ValueType::Unit:
    cout << "()" << endl;
    break;
  case ValueType::Pointer:
    if(List *list = dynamic_cast<List*>(v.value.pointer)) {
      cout << "[" << endl;
      for(;;) {
        printValue(list->value);
        list = list->tail;
        if(list == nullptr) break;
      }
      cout << "]" << endl;
    } else {
      cout << "(pointer)" << endl;
    }
    break;
  }
}

void deleteList(List *list) {
  if(list->tail != nullptr) deleteList(list->tail);
  delete list;
}

Value Interpreter::eval(Expr *expr_) {
  int a = 0;
  if(ArithExpr *expr = dynamic_cast<ArithExpr*>(expr_)) {
    log("Arith");
    int opr1 = extractInt(eval(expr->operands[0]));
    int opr2 = extractInt(eval(expr->operands[1]));
    switch(expr->op) {
    case Op::Add:
      return createInt(opr1 + opr2);
    case Op::Sub:
      return createInt(opr1 - opr2);
    case Op::Mul:
      return createInt(opr1 * opr2);
    case Op::Div:
      return createInt(opr1 / opr2);
    }
  } else if(IntLiteralExpr *expr = dynamic_cast<IntLiteralExpr*>(expr_)) {
    log("Int");
    return createInt(expr->data);
  } else if(AssignmentExpr *expr = dynamic_cast<AssignmentExpr*>(expr_)) {
    log("Assign");
    auto opr = eval(expr->rhs);
    // cout << expr->var_id << " <- " << extractInt(opr) << endl;
    env[expr->var_id] = opr;
    return createUnit();
  } else if(VariableExpr *expr = dynamic_cast<VariableExpr*>(expr_)) {
    log("Variable");
    return env[expr->var_id];
  } else if(SeqExpr *expr = dynamic_cast<SeqExpr*>(expr_)) {
    log("Seq");
    Value v = eval(expr->e1);
    return eval(expr->e2);
  } else if(NewExpr *expr = dynamic_cast<NewExpr*>(expr_)) {
    log("New");
    // Class c = classes[expr->class_id];
    // vector<Value>* field_values = new vector<Value>;
    // for(int i=0; i<c.fields.size(); i++){
    //   field_values->push_back(getInitialValue(c.fields[i]));
    // }
    // objects.push_back(field_values);
    return createUnit();
  } else if(LoopExpr *expr = dynamic_cast<LoopExpr*>(expr_)) {
    log("Loop");
    for(;;) {
      broken = false;
      eval(expr->e);
      if(broken) break;
    }
    broken = false;
    return createUnit();
  } else if(BreakExpr *expr = dynamic_cast<BreakExpr*>(expr_)) {
    log("Break");
    broken = true;
    return createUnit();
  } else if(IfExpr *expr = dynamic_cast<IfExpr*>(expr_)) {
    log("If");
    Value v = eval(expr->cond);
    if(extractInt(v) != 0) {
      eval(expr->block);
    }
    return createUnit();
  } else if(CompExpr *expr = dynamic_cast<CompExpr*>(expr_)) {
    log("Comp");
    Value v1 = eval(expr->operands[0]);
    Value v2 = eval(expr->operands[1]);
    int v1i = extractInt(v1);
    int v2i = extractInt(v2);
    switch(expr->op) {
    case CompOp::Lt:
      if(v1i < v2i) return createInt(1);
      else return createInt(0);
    case CompOp::Gt:
      if(v1i > v2i) return createInt(1);
      else return createInt(0);
    case CompOp::Eq:
      if(v1i == v2i) return createInt(1);
      else return createInt(0);
    case CompOp::Neq:
      if(v1i != v2i) return createInt(1);
      else return createInt(0);
    }
  } else if(AppendExpr *expr = dynamic_cast<AppendExpr*>(expr_)) {
    Value v = env[expr->var_id];
    List *tail;
    if(v.type == ValueType::Pointer && (dynamic_cast<List*>(v.value.pointer)) != nullptr) {
      tail = dynamic_cast<List*>(v.value.pointer);
    } else {
      tail = createSingleton(v);
    }
    Value rhs = eval(expr->e);
    List *list = consList(rhs, tail);
    env[expr->var_id] = createPointer(list);
  } else if(GetExpr *expr = dynamic_cast<GetExpr*>(expr_)) {
    Value v = env[expr->var_id];
    List *list = dynamic_cast<List*>(v.value.pointer);
    int index = extractInt(eval(expr->e));
    Value ret;
    for(;;) {
      if(index == 0) {
        ret = list->value;
        break;
      }
      index --;
      list = list->tail;
    }
    return ret;
  } else if(PrintExpr *expr = dynamic_cast<PrintExpr*>(expr_)) {
    Value v = eval(expr->e);
    printValue(v);
  } else if(DeleteExpr *expr = dynamic_cast<DeleteExpr*>(expr_)) {
    Value v = env[expr->var_id];
    if(v.type != ValueType::Pointer) {
      cout << "should be pointer" << endl;
      exit(1);
    }
    List *list = dynamic_cast<List*>(v.value.pointer);
    deleteList(list);
  }
}

Expr* parse(const char *filename) {
  FILE *file = fopen(filename, "r");
  if(!file) {
    cout << "error on opening file" << endl;
    return nullptr;
  }
  yyin = file;
  yydebug = 1;
  
  Expr *root;
  if(yyparse(&root)) {
    printf("ERROR!!");
    return nullptr;
  }
  return root;
}

int main(int argc, char *argv[]) {
  {
    Expr *result = parse(argv[1]);
    if(result == nullptr) return 1;
    
    Program program { result };
    Interpreter interp { 26 };
    
    cout << "EVAL START:" << endl;
    Value v = interp.eval(program.expr);
    int i = extractInt(v);
    cout << "result: " << i << endl;
    
    delete result;
  }
  cout << "C" << endl;
  
  cout << "A" << endl;
  return 0;
}
