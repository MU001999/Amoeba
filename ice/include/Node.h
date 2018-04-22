#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include "IceObject.h"
#include "Token.h"

namespace Ice
{
	class Env;

	class Stmt;
	class Expr;
	class IdentifierExpr;

	typedef std::vector<std::shared_ptr<Stmt>> StatementList;
	typedef std::vector<std::shared_ptr<Expr>> ExpressionList;
	typedef std::vector<std::shared_ptr<IdentifierExpr>> VariableList;

	class Node
	{
	public:
		virtual ~Node() {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env>&) = 0;
	};

	class Stmt : public Node
	{
	};
	class Expr : public Node
	{
	};

	class BlockExpr : public Expr
	{
	public:
		StatementList statements;
		BlockExpr() {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class IntegerExpr : public Expr
	{
	public:
		long value;
		IntegerExpr(long value) : value(value) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class DoubleExpr : public Expr
	{
	public:
		double value;
		DoubleExpr(double value) : value(value) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class IdentifierExpr : public Expr
	{
	public:
		std::string name;
		IdentifierExpr(std::string &name) : name(name) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class MethodCallExpr : public Expr
	{
	public:
		std::shared_ptr<IdentifierExpr> id;
		ExpressionList arguments;
		MethodCallExpr(std::shared_ptr<IdentifierExpr> id, ExpressionList &arguments) :id(id), arguments(arguments) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class BinaryOperatorExpr : public Expr
	{
	public:
		std::shared_ptr<Expr> lhs;
		Token::TOKEN op;
		std::shared_ptr<Expr> rhs;
		BinaryOperatorExpr(std::shared_ptr<Expr> lhs, Token::TOKEN op, std::shared_ptr<Expr> rhs) : lhs(lhs), op(op), rhs(rhs) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class ExprStmt : public Stmt
	{
	public:
		std::shared_ptr<Expr> assignment;
		ExprStmt(std::shared_ptr<Expr> assignment) : assignment(assignment) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class VariableDeclarationStmt : public Stmt
	{
	public:
		std::shared_ptr<IdentifierExpr> id;
		std::shared_ptr<Expr> assignment;
		VariableDeclarationStmt(std::shared_ptr<IdentifierExpr> id, std::shared_ptr<Expr>assignment) : id(id), assignment(assignment) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class FunctionDeclarationStmt : public Stmt, public std::enable_shared_from_this<FunctionDeclarationStmt>
	{
	public:
		std::shared_ptr<IdentifierExpr> id;
		VariableList arguments;
		std::shared_ptr<BlockExpr> block;
		FunctionDeclarationStmt(std::shared_ptr<IdentifierExpr> id, const VariableList &arguments, std::shared_ptr<BlockExpr> block) : id(id), arguments(arguments), block(block) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class ReturnStmt : public Stmt
	{
	public:
		std::shared_ptr<Expr> assignment;
		ReturnStmt(std::shared_ptr<Expr> assignment) :assignment(assignment) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};

	class IfElseStmt : public Stmt
	{
	public:
		std::shared_ptr<Expr> cond;
		std::shared_ptr<BlockExpr> blockTrue;
		std::shared_ptr<BlockExpr> blockFalse;
		IfElseStmt(std::shared_ptr<Expr> cond, std::shared_ptr<BlockExpr> blockTrue, std::shared_ptr<BlockExpr> blockFalse) : cond(cond), blockTrue(blockTrue), blockFalse(blockFalse) {}
		virtual std::shared_ptr<IceObject> runCode(std::shared_ptr<Env> &);
	};
}

#endif //__NODE_H__