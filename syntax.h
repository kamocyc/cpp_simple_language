#include <vector>

enum class Op { Add, Sub, Mul, Div };
enum class CompOp { Lt, Gt, Eq, Neq };
enum class ExprType { Arith, IntLiteral };

/* プログラム */
class Expr {
public:
  virtual ~Expr() = default;
};

class CompExpr : public Expr {
public:
  CompExpr(CompOp op_, std::vector<Expr*> operands_) : op(op_), operands(operands_) {}
  CompOp op;
  std::vector<Expr*> operands;
};

class ArithExpr : public Expr {
public:
  ArithExpr(Op op_, std::vector<Expr*> operands_) : op(op_), operands(operands_) {}
  Op op;
  std::vector<Expr*> operands;
  // ~ArithExpr();
};

class IntLiteralExpr : public Expr {
public:
  IntLiteralExpr(int data_) : data(data_) {}
  int data;
};

class AssignmentExpr : public Expr {
public:
  AssignmentExpr(int var_id_, Expr *rhs_) : var_id(var_id_), rhs(rhs_) {}
  int var_id;
  Expr *rhs;
  // ~AssignmentExpr();
};

class VariableExpr : public Expr {
public:
  VariableExpr(int var_id_) : var_id(var_id_) {}
  int var_id;
};

class SeqExpr : public Expr {
public:
  SeqExpr(Expr *e1_, Expr *e2_) : e1(e1_), e2(e2_) {}
  Expr *e1;
  Expr *e2;
  // ~SeqExpr();
};

class NewExpr : public Expr {
public:
  NewExpr(int class_id_) : class_id(class_id_) {}
  int class_id;
};

class LoopExpr : public Expr {
public:
  LoopExpr(Expr *e_) : e(e_) {}
  Expr *e;
};

class IfExpr : public Expr {
public:
  IfExpr(Expr *cond_, Expr *block_) : cond(cond_), block(block_) {}
  Expr *cond;
  Expr *block;
};

class BreakExpr : public Expr {
public:
  BreakExpr() {}
};

class AppendExpr : public Expr {
public:
  AppendExpr(int var_id_, Expr* e_) : var_id(var_id_), e(e_) {}
  int var_id;
  Expr* e;
};

class GetExpr : public Expr {
public:
  GetExpr(int var_id_, Expr* e_) : var_id(var_id_), e(e_) {}
  int var_id;
  Expr* e;
};

class PrintExpr : public Expr {
public:
  PrintExpr(Expr *e_) : e(e_) {}
  Expr *e;
};

class DeleteExpr : public Expr {
public:
  DeleteExpr(int var_id_) : var_id(var_id_) {}
  int var_id;
};
