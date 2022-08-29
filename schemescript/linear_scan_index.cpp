#include "linear_scan_index.h"
#include "visitor.h"
#include <algorithm>

COMPILER_BEGIN

namespace
  {
    
  struct linear_scan_index_state
    {
    enum struct e_lsi_state
      {
      T_INIT,
      T_STEP_1
      };
    Expression* p_expr;
    e_lsi_state state;

    linear_scan_index_state() : p_expr(nullptr), state(e_lsi_state::T_INIT) {}
    linear_scan_index_state(Expression* ip_expr) : p_expr(ip_expr), state(e_lsi_state::T_INIT) {}
    linear_scan_index_state(Expression* ip_expr, e_lsi_state s) : p_expr(ip_expr), state(s) {}
    };
    
  struct linear_scan_index_helper
    {
    uint64_t pre_index, index;

    std::vector<linear_scan_index_state> expressions;

    void treat_expressions()
      {
      while (!expressions.empty())
        {
        linear_scan_index_state lsi_state = expressions.back();
        expressions.pop_back();
        Expression& e = *lsi_state.p_expr;
        switch (lsi_state.state)
          {
          case linear_scan_index_state::e_lsi_state::T_INIT:
          {
          if (std::holds_alternative<Literal>(e))
            {
            Literal& lit = std::get<Literal>(e);
            //linear_scan_index_visitor lsiv;
            //lsiv.index = index;
            //lsiv.pre_index = pre_index;
            //visitor<Literal, linear_scan_index_visitor>::visit(lit, &lsiv);switch (lit.index())
            switch (lit.index())
            { //std::variant<Character, False, Fixnum, Flonum, Nil, String, Symbol, True>
              case index_character:
                std::get<Character>(lit).pre_scan_index = pre_index++;
                std::get<Character>(lit).scan_index = pre_index;
                break;
              case index_false:
                std::get<False>(lit).pre_scan_index = pre_index++;
                std::get<False>(lit).scan_index = pre_index;
                break;
              case index_fixnum:
                std::get<Fixnum>(lit).pre_scan_index = pre_index++;
                std::get<Fixnum>(lit).scan_index = pre_index;
                break;
              case index_flonum:
                std::get<Flonum>(lit).pre_scan_index = pre_index++;
                std::get<Flonum>(lit).scan_index = pre_index;
                break;
              case index_nil:
                std::get<Nil>(lit).pre_scan_index = pre_index++;
                std::get<Nil>(lit).scan_index = pre_index;
                break;
              case index_string:
                std::get<String>(lit).pre_scan_index = pre_index++;
                std::get<String>(lit).scan_index = pre_index;
                break;
              case index_symbol:
                std::get<Symbol>(lit).pre_scan_index = pre_index++;
                std::get<Symbol>(lit).scan_index = pre_index;
                break;
              case index_true:
                std::get<True>(lit).pre_scan_index = pre_index++;
                std::get<True>(lit).scan_index = pre_index;
                break;
              default:
              break;
            }
            }
          else if (std::holds_alternative<Variable>(e))
            {
            std::get<Variable>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            }
          else if (std::holds_alternative<Nop>(e))
            {
            std::get<Nop>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            }
          else if (std::holds_alternative<Quote>(e))
            {
            std::get<Quote>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            }
          else if (std::holds_alternative<Set>(e))
            {
            std::get<Set>(e).pre_scan_index = pre_index++;
            //Set& s = std::get<Set>(e);
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            expressions.push_back(&std::get<Set>(e).value.front());
            }
          else if (std::holds_alternative<If>(e))
            {
            std::get<If>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            for (auto rit = std::get<If>(e).arguments.rbegin(); rit != std::get<If>(e).arguments.rend(); ++rit)
              expressions.push_back(&(*rit));
            }
          else if (std::holds_alternative<Begin>(e))
            {
            std::get<Begin>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            for (auto rit = std::get<Begin>(e).arguments.rbegin(); rit != std::get<Begin>(e).arguments.rend(); ++rit)
              expressions.push_back(&(*rit));
            }
          else if (std::holds_alternative<PrimitiveCall>(e))
            {
            std::get<PrimitiveCall>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            for (auto rit = std::get<PrimitiveCall>(e).arguments.rbegin(); rit != std::get<PrimitiveCall>(e).arguments.rend(); ++rit)
              expressions.push_back(&(*rit));
            }
          else if (std::holds_alternative<ForeignCall>(e))
            {
            std::get<ForeignCall>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            for (auto rit = std::get<ForeignCall>(e).arguments.rbegin(); rit != std::get<ForeignCall>(e).arguments.rend(); ++rit)
              expressions.push_back(&(*rit));
            }
          else if (std::holds_alternative<Lambda>(e))
            {
            std::get<Lambda>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            Lambda& l = std::get<Lambda>(e);
            expressions.push_back(&l.body.front());
            }
          else if (std::holds_alternative<FunCall>(e))
            {
            std::get<FunCall>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            expressions.push_back(&std::get<FunCall>(e).fun.front());
            for (auto rit = std::get<FunCall>(e).arguments.rbegin(); rit != std::get<FunCall>(e).arguments.rend(); ++rit)
              expressions.push_back(&(*rit));
            }
          else if (std::holds_alternative<Let>(e))
            {
            std::get<Let>(e).pre_scan_index = pre_index++;
            expressions.emplace_back(&e, linear_scan_index_state::e_lsi_state::T_STEP_1);
            Let& l = std::get<Let>(e);
            expressions.push_back(&l.body.front());
            for (auto rit = l.bindings.rbegin(); rit != l.bindings.rend(); ++rit)
              expressions.push_back(&(*rit).second);
            }
          else
            throw std::runtime_error("Compiler error!: Linear scan index: not implemented");
          break;
          }
          case linear_scan_index_state::e_lsi_state::T_STEP_1:
          {
          if (std::holds_alternative<Variable>(e))
            {
            std::get<Variable>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Nop>(e))
            {
            std::get<Nop>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Quote>(e))
            {
            std::get<Quote>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Set>(e))
            {
            std::get<Set>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<If>(e))
            {
            std::get<If>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Begin>(e))
            {
            std::get<Begin>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<PrimitiveCall>(e))
            {
            std::get<PrimitiveCall>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<ForeignCall>(e))
            {
            std::get<ForeignCall>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Lambda>(e))
            {
            std::get<Lambda>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<FunCall>(e))
            {
            std::get<FunCall>(e).scan_index = pre_index;
            }
          else if (std::holds_alternative<Let>(e))
            {
            std::get<Let>(e).scan_index = pre_index;
            }
          else
            throw std::runtime_error("Compiler error!: Linear scan index: not implemented");
          break;
          }
          } // switch
        }
      }
    };
  }
  
void compute_linear_scan_index(Program& prog)
  {
  linear_scan_index_helper lsih;
  lsih.index = 0;
  lsih.pre_index = 0;
  for (auto& expr : prog.expressions)
    lsih.expressions.push_back(&expr);
  std::reverse(lsih.expressions.begin(), lsih.expressions.end());
  lsih.treat_expressions();

  prog.linear_scan_indices_computed = true;
  }

COMPILER_END
