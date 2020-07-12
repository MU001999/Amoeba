#pragma once

#include <map>
#include <vector>
#include <utility>
#include "token.hpp"
#include "helper.hpp"

namespace anole
{
class Code;

using StmtList  = std::vector<Ptr<struct Stmt>>;
using ExprList  = std::vector<Ptr<struct Expr>>;
using DeclList  = std::vector<Ptr<struct VariableDeclarationStmt>>;
using IdentList = std::vector<Ptr<struct IdentifierExpr>>;
using ArgumentList = std::vector<std::pair<Ptr<struct Expr>, bool>>; // boolean stands for whether it is unpacked
using ParameterList = std::vector<std::pair<Ptr<struct VariableDeclarationStmt>, bool>>; // boolean stands for whether it is packed

struct AST
{
    virtual ~AST() = 0;
    virtual void codegen(Code &) = 0;

    virtual bool is_integer_expr();
    virtual bool is_expr_stmt();

    // for codegen
    static bool &interpretive()
    {
        static bool mode = false;
        return mode;
    }

    // pos not be uesd in each node
    std::pair<std::size_t, std::size_t> pos = {0, 0};
};

struct Stmt : AST
{
    virtual ~Stmt() = 0;
    virtual void codegen(Code &) = 0;
};

struct Expr : AST
{
    virtual ~Expr() = 0;
    virtual void codegen(Code &) = 0;
};

struct BlockExpr : Expr
{
    StmtList statements;

    void codegen(Code &) override;
};

struct NoneExpr : Expr
{
    void codegen(Code &) override;
};

struct IntegerExpr : Expr
{
    int64_t value;

    IntegerExpr(int64_t value) : value(value) {}

    bool is_integer_expr() override;
    void codegen(Code &) override;
};

struct FloatExpr : Expr
{
    double value;

    FloatExpr(double value) : value(value) {}

    void codegen(Code &) override;
};

struct BoolExpr : Expr
{
    bool value;

    BoolExpr(bool value) : value(value) {}

    void codegen(Code &) override;
};

struct StringExpr : Expr
{
    std::string value;

    StringExpr(std::string value)
      : value(std::move(value)) {}

    void codegen(Code &) override;
};

struct IdentifierExpr : Expr
{
    std::string name;

    IdentifierExpr(std::string name)
      : name(std::move(name)) {}

    void codegen(Code &) override;
};

struct ParenOperatorExpr : Expr
{
    Ptr<Expr> expr;
    ArgumentList args;

    ParenOperatorExpr(Ptr<Expr> &&expr,
        ArgumentList &&args)
      : expr(std::move(expr))
      , args(std::move(args)) {}

    void codegen(Code &) override;
};

struct UnaryOperatorExpr : Expr
{
    Token op;
    Ptr<Expr> expr;

    UnaryOperatorExpr(Token op,
        Ptr<Expr> &&expr)
      : op(op), expr(std::move(expr)) {}

    void codegen(Code &) override;
};

struct BinaryOperatorExpr : Expr
{
    Token op;
    Ptr<Expr> lhs, rhs;

    BinaryOperatorExpr(Ptr<Expr> &&lhs,
        Token op, Ptr<Expr> &&rhs)
      : op(op), lhs(std::move(lhs))
      , rhs(std::move(rhs)) {}

    void codegen(Code &) override;
};

struct LambdaExpr : Expr
{
    ParameterList parameters;
    Ptr<BlockExpr> block;

    LambdaExpr(ParameterList &&parameters,
        Ptr<BlockExpr> &&block)
      : parameters(std::move(parameters))
      , block(std::move(block)) {}

    void codegen(Code &) override;
};

struct NewExpr : Expr
{
    Ptr<IdentifierExpr> id;
    ArgumentList args;

    NewExpr(Ptr<IdentifierExpr> &&id,
        ArgumentList &&args)
      : id(std::move(id))
      , args(std::move(args)) {}

    void codegen(Code &) override;
};

struct DotExpr : Expr
{
    Ptr<Expr> left;
    Ptr<IdentifierExpr> id;

    DotExpr(Ptr<Expr> &&left,
        Ptr<IdentifierExpr> &&id)
      : left(std::move(left))
      , id(std::move(id)) {}

    void codegen(Code &) override;
};

struct EnumExpr : Expr
{
    DeclList decls;

    EnumExpr() = default;

    void codegen(Code &) override;
};

struct MatchExpr : Expr
{
    Ptr<Expr> expr;
    std::vector<ExprList> keylists;
    std::vector<Ptr<Expr>> values;
    Ptr<Expr> else_expr;

    MatchExpr() {}

    void codegen(Code &) override;
};

struct ListExpr : Expr
{
    ExprList exprs;

    ListExpr() = default;

    void codegen(Code &) override;
};

struct IndexExpr : Expr
{
    Ptr<Expr> expr, index;

    IndexExpr(Ptr<Expr> &&expr,
        Ptr<Expr> &&index)
      : expr(std::move(expr))
      , index(std::move(index)) {}

    void codegen(Code &) override;
};

struct DictExpr : Expr
{
    ExprList keys, values;

    DictExpr() = default;

    void codegen(Code &) override;
};

struct DelayExpr : Expr
{
    Ptr<Expr> expr;

    DelayExpr(Ptr<Expr> &&expr)
      : expr(std::move(expr)) {}

    void codegen(Code &) override;
};

struct QuesExpr : Expr
{
    Ptr<Expr> cond, true_expr, false_expr;

    QuesExpr(Ptr<Expr> &&cond,
        Ptr<Expr> &&true_expr,
        Ptr<Expr> &&false_expr)
      : cond(std::move(cond))
      , true_expr(std::move(true_expr))
      , false_expr(std::move(false_expr)) {}

    void codegen(Code &code) override;
};

struct UseStmt : Stmt
{
    using NamesType = std::vector<std::pair<std::string, std::string>>;

    NamesType names;
    std::string from;

    UseStmt(NamesType &&names,
        std::string from)
      : names(std::move(names))
      , from(std::move(from)) {}

    void codegen(Code &) override;
};

struct ExprStmt : Stmt
{
    Ptr<Expr> expr;

    ExprStmt(Ptr<Expr> &&expr)
      : expr(std::move(expr)) {}

    bool is_expr_stmt() override;
    void codegen(Code &) override;
};

struct VariableDeclarationStmt : Stmt
{
    bool is_ref;
    Ptr<IdentifierExpr> id;
    Ptr<Expr> expr;

    VariableDeclarationStmt(
        Ptr<IdentifierExpr> &&id,
        Ptr<Expr> &&expr,
        bool is_ref = false)
      : is_ref(is_ref)
      , id(std::move(id))
      , expr(std::move(expr)) {}

    void codegen(Code &) override;
};

struct MultiVarsDeclarationStmt : Stmt
{
    std::vector<std::pair<Ptr<IdentifierExpr>, bool>> vars; // bool stands for whether it's ref
    Ptr<Expr> expr;

    MultiVarsDeclarationStmt(
        std::vector<std::pair<Ptr<IdentifierExpr>, bool>> &&vars,
        Ptr<Expr> &&expr)
      : vars(std::move(vars))
      , expr(std::move(expr)) {}

    void codegen(Code &) override;
};

struct FunctionDeclarationStmt : Stmt
{
    Ptr<IdentifierExpr> id;
    Ptr<LambdaExpr> lambda;

    FunctionDeclarationStmt(
        Ptr<IdentifierExpr> &&id,
        Ptr<LambdaExpr> &&lambda)
      : id(std::move(id))
      , lambda(std::move(lambda)) {}

    void codegen(Code &) override;
};

struct PrefixopDeclarationStmt : Stmt
{
    Ptr<IdentifierExpr> id;

    PrefixopDeclarationStmt(Ptr<IdentifierExpr> &&id)
      : id(std::move(id)) {}

    void codegen(Code &) override;
};

struct InfixopDeclarationStmt : Stmt
{
    size_t priority;
    Ptr<IdentifierExpr> id;

    InfixopDeclarationStmt(Ptr<IdentifierExpr> &&id,
        size_t priority)
      : priority(priority), id(std::move(id)) {}

    void codegen(Code &) override;
};

struct ClassDeclarationStmt : Stmt
{
    Ptr<IdentifierExpr> id;
    IdentList bases;
    Ptr<BlockExpr> block;

    ClassDeclarationStmt(Ptr<IdentifierExpr> &&id,
        IdentList &&bases, Ptr<BlockExpr> &&block)
      : id(std::move(id))
      , bases(std::move(bases))
      , block(std::move(block)) {}

    void codegen(Code &) override;
};

struct BreakStmt : Stmt
{
    void codegen(Code &) override;
};

struct ContinueStmt : Stmt
{
    void codegen(Code &) override;
};

struct ReturnStmt : Stmt
{
    ExprList exprs;

    ReturnStmt(ExprList &&exprs)
      : exprs(std::move(exprs)) {}

    void codegen(Code &) override;
};

struct IfElseStmt : Stmt
{
    Ptr<Expr> cond;
    Ptr<BlockExpr> true_block;
    Ptr<AST> false_branch;

    IfElseStmt(Ptr<Expr> &&cond,
        Ptr<BlockExpr> &&true_block,
        Ptr<AST> &&false_branch)
      : cond(std::move(cond))
      , true_block(std::move(true_block))
      , false_branch(std::move(false_branch)) {}

    void codegen(Code &) override;
};

struct WhileStmt : Stmt
{
    Ptr<Expr> cond;
    Ptr<BlockExpr> block;

    WhileStmt(Ptr<Expr> &&cond,
        Ptr<BlockExpr> &&block)
      : cond(std::move(cond))
      , block(std::move(block)) {}

    void codegen(Code &) override;
};

struct DoWhileStmt : Stmt
{
    Ptr<Expr> cond;
    Ptr<BlockExpr> block;

    DoWhileStmt(Ptr<Expr> &&cond,
        Ptr<BlockExpr> &&block)
      : cond(std::move(cond))
      , block(std::move(block)) {}

    void codegen(Code &) override;
};

struct ForeachStmt : Stmt
{
    Ptr<Expr> expr;
    Ptr<IdentifierExpr> id;
    Ptr<BlockExpr> block;

    ForeachStmt(Ptr<Expr> &&expr,
        Ptr<IdentifierExpr> &&id,
        Ptr<BlockExpr> &&block)
      : expr(std::move(expr))
      , id(std::move(id))
      , block(std::move(block)) {}

    void codegen(Code &) override;
};
}
