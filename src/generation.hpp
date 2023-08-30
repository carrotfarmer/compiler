#pragma once

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "./parser.hpp"

class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term)
    {
        struct TermVisitor {
            Generator* gen;
            void operator()(const NodeTermIntLit* term_int_lit) const
            {
                gen->m_output << "    mov X0, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("X0");
            }

            void operator()(const NodeTermIdent* term_ident) const
            {
                if (!gen->m_vars.contains(term_ident->ident.value.value())) {
                    std::cerr << "Undeclared identifier: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto& var = gen->m_vars.at(term_ident->ident.value.value());
                gen->m_output << "    ldr X0, [SP, #" << (gen->m_stack_size - var.stack_loc - 1) * 16 << "]\n";
                gen->push("X0");
            }
        };

        TermVisitor visitor({ .gen = this });
        std::visit(visitor, term->var);
    };

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator* gen;

            void operator()(const NodeTerm* term) const
            {
                gen->gen_term(term);
            };

            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen->gen_expr(bin_expr->add->lhs);
                gen->gen_expr(bin_expr->add->rhs);
                gen->pop("X0");
                gen->pop("X1");
                gen->m_output << "    add X0, X0, X1\n";
                gen->push("X0");
            }
        };

        ExprVisitor visitor { .gen = this };
        std::visit(visitor, expr->var);
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const
            {
                gen->gen_expr(stmt_exit->expr);
                gen->pop("X0");
                gen->m_output << "    mov X16, #1\n";
                gen->m_output << "    svc #0x80\n";
            }
            void operator()(const NodeStmtLet* stmt_let) const
            {
                // .contains() doesn't work for some reason? (cmake is already configured to C++20)
                if (gen->m_vars.count(stmt_let->ident.value.value()) > 0) {
                    std::cerr << "identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({ stmt_let->ident.value.value(), Var { .stack_loc = gen->m_stack_size } });
                gen->gen_expr(stmt_let->expr);
            }
        };

        StmtVisitor visitor { .gen = this };
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog()
    {
        m_output << ".global _start\n.align 3\n\n_start:\n";

        for (const NodeStmt* stmt : m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "    mov X0, #0\n";
        m_output << "    mov X16, #1\n";
        m_output << "    svc #0x80";
        return m_output.str();
    }

private:
    void push(const std::string& reg)
    {
        m_output << "    stp " << reg << ", XZR, [SP, #-16]!\n";
        m_stack_size++;
    }

    void pop(const std::string& reg)
    {
        m_output << "    ldp " << reg << ", XZR, [SP], #16\n";
        m_stack_size--;
    }

    struct Var {
        size_t stack_loc;
    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars {};
};
