#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "BigInteger.h"
#include "Python3BaseVisitor.h"
#include <map>
#include <cmath>
#include <string>
#include <iomanip>
#include <vector>

std::vector<std::map<std::string, antlrcpp::Any>> Varibles;

class EvalVisitor: public Python3BaseVisitor {

//todo:override all methods of Python3BaseVisitor
    
    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        std::map<std::string, antlrcpp::Any> GlobalVarible;
        Varibles.push_back(GlobalVarible);
        std::vector<Python3Parser::StmtContext *> stmtlist;
        stmtlist = ctx->stmt();
        for (auto i : stmtlist) { //visit all the stmt
            visit(i);
        }
        return 0;
        
    }
    /*
    
    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext* ctx) override;
    antlrcpp::Any visitParameters(Python3Parser::ParametersContext* ctx) override;
    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext* ctx) override;
    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext* ctx) override;
    

    antlrcpp::Any visitStmt(Python3Parser::StmtContext* ctx) override {     //visit simple_stmt and compound_stmt
        return visitChildren(ctx);
    }
    
    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext* ctx) override {       //visit small_stmt
        return visitChildren(ctx);
    }
    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext* ctx) override { //visit expr_stmt adn flow_stmt 
        return visitChildren(ctx);                                           //**warning: continue break return still incomplete
    }
    */
    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext* ctx) override {
        if (ctx->augassign()) {
            antlrcpp::Any left = visit(ctx->testlist(0));
            antlrcpp::Any right = visit(ctx->testlist(1));
            antlrcpp::Any it = visit(ctx->augassign());
            
            switch(it.as<int>()) {                                //incomplete: calculation of 
                case Python3Parser::ADD_ASSIGN:
                    break;
                case Python3Parser::SUB_ASSIGN:
                    break;
                case Python3Parser::MULT_ASSIGN:
                    break;
                case Python3Parser::DIV_ASSIGN:
                    break;
                case Python3Parser::IDIV_ASSIGN:
                    break;
                case Python3Parser::MOD_ASSIGN:
                    break;
            }
            return 0; //
        } else {                //incomplete
            int length = ctx->testlist().size();
            antlrcpp::Any value = visit(ctx->testlist(length - 1));
            for (int i = length - 2; i >= 0; i--) {
                /*
                std::vector<antlrcpp::Any>* tmpTest = visit(ctx->testlist(i)).as<std::vector<antlrcpp::Any>*>();
                bool flag = false;       //has been used
                for (int k = 0; k < tmpTest->size(); k++) {
                    */
                    bool flag = false;
                    std::string variableName = visit(ctx->testlist(i)).as<std::string>();
                    for (int j = Varibles.size() - 1; j >= 0; j--) {
                        if (Varibles[j].count(variableName)) {

                            Varibles[j][variableName] = value;          //update or new a varible
                            flag = true;
                            
                            break;
                        }


                    }
                    if (!flag) {
                        Varibles[Varibles.size() - 1][variableName] = value;
                    }
                
                //delete tmpTest;
            }
            return value;
        }
    }
    
    
    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext* ctx) override {
        if (ctx->ADD_ASSIGN()) 
            return Python3Parser::ADD_ASSIGN;
        if (ctx->SUB_ASSIGN())
            return Python3Parser::SUB_ASSIGN;
        if (ctx->MULT_ASSIGN())
            return Python3Parser::MULT_ASSIGN;
        if (ctx->DIV_ASSIGN())
            return Python3Parser::DIV_ASSIGN;
        if (ctx->IDIV_ASSIGN())
            return Python3Parser::IDIV_ASSIGN;
        if (ctx->MOD_ASSIGN())
            return Python3Parser::MOD_ASSIGN;
    }
    /*
    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext* ctx) override;
    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext* ctx) override;
    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext* ctx) override;
    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext* ctx) override;
    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext* ctx) override;
    
    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext* ctx) override;
    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext* ctx) override;
    antlrcpp::Any visitSuite(Python3Parser::SuiteContext* ctx) override;*/
    antlrcpp::Any visitTest(Python3Parser::TestContext* ctx) override {

        return visit(ctx->or_test());
    }
    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext* ctx) override {
        if (ctx->and_test().size() > 1) {                   //incompelete

        } else {
            return visit(ctx->and_test()[0]);
        }
    }
    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext* ctx) override {
        if (ctx->not_test().size() > 1) {
                                            //incomplete
        } else {
            return visit(ctx->not_test()[0]);
        }
    }
    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext* ctx) override {
        if (ctx->not_test()) {              //incomplete

        } else {
            return visit(ctx->comparison());
        }
    }

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext* ctx) override {
        if (ctx->comp_op().size() > 0) {

        } else {
            return visit(ctx->arith_expr()[0]);
        }
    }
    //return the index of comp;
    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext* ctx) override {
        if (ctx->LESS_THAN())
            return Python3Parser::LESS_THAN;
        if (ctx->GREATER_THAN())
            return Python3Parser::GREATER_THAN;
        if (ctx->EQUALS())
            return Python3Parser::EQUALS;
        if (ctx->GT_EQ())
            return Python3Parser::GT_EQ;
        if (ctx->LT_EQ()) 
            return Python3Parser::LT_EQ;
        if (ctx->NOT_EQ_2())
            return Python3Parser::NOT_EQ_2;
    }
    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext* ctx) override {
        if (ctx->ADD().size() > 0) {

        } else if (ctx->MINUS().size() > 0) {

        } else {
            return visit(ctx->term()[0]);
        }
    }

    antlrcpp::Any visitTerm(Python3Parser::TermContext* ctx) override {
        if (ctx->factor().size() > 1) {

        } else {
            return visit(ctx->factor()[0]);
        }
    }
    antlrcpp::Any visitFactor(Python3Parser::FactorContext* ctx) override {
        if (ctx->factor()) {

        } else {
            return visit(ctx->atom_expr());
        }
    }
    
    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext* ctx) override {
        antlrcpp::Any tmpAtom = visit(ctx->atom());
        if (ctx->trailer()) {           //function call
            std::string funcName = tmpAtom.as<std::string>();
            antlrcpp::Any a = visit(ctx->trailer());
            std::vector<antlrcpp::Any>* tmp = a.as<std::vector<antlrcpp::Any>*>();
            if (funcName == "print") {
                for (int i = 0; i < tmp->size(); i++) {
                    if ((*tmp)[i].is<std::string>()) {
                        std::string tmpString = (*tmp)[i].as<std::string>();
                        if (tmpString[0] == '"' || tmpString[0] == '\'') {
                            for (int j = 1; j < tmpString.length() - 1; j++) {
                                std::cout << tmpString[j];
                            }
                        } else {
                            for (int k = Varibles.size() - 1; k >= 0; k--) {
                                if (Varibles[k].count(tmpString)) {
                                    antlrcpp::Any ret = Varibles[k][tmpString];
                                    if (ret.is<std::string>()) {
                                        std::string ret1 = ret.as<std::string>();
                                        for (int j = 1; j < ret1.length() - 2; j++) {
                                            std::cout << ret1[j];
                                        }
                                    }
                                    if (ret.is<BigInteger>()) {
                                        std::cout << ret.as<BigInteger>();
                                    }
                                    if (ret.is<bool>()) {
                                        std::cout <<((ret.as<bool>()) ? "True" : "False");
                                    }
                                    if (ret.is<double>()) {
                                        std::cout << ret.as<double>();
                                    }
                                    break;
                                }
                            }
                        }
                        std::cout << ' ';
                        continue;
                    }
                    if ((*tmp)[i].is<bool>()) {
                        std::cout << (((*tmp)[i].as<bool>()) ? "True" : "False") << ' ';
                        continue;
                    }
                    if ((*tmp)[i].is<BigInteger>()) {
                        std::cout << (*tmp)[i].as<BigInteger>() << ' ';
                        continue;
                    }
                    if ((*tmp)[i].is<double>()) {
                        std::cout << std::fixed << std::setprecision(6) << (*tmp)[i].as<double>() << ' ';
                        continue;
                    }
               }
               std::cout << std::endl;
               delete tmp;
               return 0;
            }
            return 0;   //
        } else {
            return visit(ctx->atom());
        }

    }
    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext* ctx) override {
        return visit(ctx->arglist());
    }
    
    antlrcpp::Any visitAtom(Python3Parser::AtomContext* ctx) override {
        if (ctx->NAME()) {
            std::string tmp = ctx->NAME()->toString();
            return tmp;
        }
        if (ctx->NUMBER()) {
            std::string tmpNumber = ctx->NUMBER()->toString();
            bool isFloat = false;
            int i;
            for (i = 0; i  < tmpNumber.length(); i++) {
                if (tmpNumber[i] == '.') {
                    isFloat = true;
                    break;
                }
            }
            if (isFloat) {
                double tmp = 0;
                for (int j = 0; j < i; j++) {
                    tmp *= 10;
                    tmp += tmpNumber[i] - '0';
                }
                for (int j = i + 1; j < tmpNumber.length(); j++) {
                    tmp += (double)(tmpNumber[i] - '0') * pow(10, i - j);
                }
                return tmp;
            } else {
                BigInteger tmp(tmpNumber);
                return tmp;
            }
        }
        if (ctx->STRING().size() > 0) {
            std::string tmp;
            for (int i = 0; i < ctx->STRING().size(); i++) {
                tmp += ctx->STRING(i)->toString();
            }
            return tmp;
        }
        if (ctx->NONE()){
            bool tmp = false;
            return tmp;
        }
        if (ctx->TRUE()) {
            bool tmp = true;
            return tmp;
        }
        if (ctx->FALSE()) {
            bool tmp = false;
            return tmp;
        }
        if (ctx->OPEN_PAREN())
            return visit(ctx->test());
    }
    
    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext* ctx) override {
        return visit(ctx->test(0));
        /*
        std::vector<antlrcpp::Any>* tmpTest = new std::vector<antlrcpp::Any>;
        for (int i = 0; i < ctx->test().size(); i++) {
            antlrcpp::Any tmp = ctx->test(i);
            tmpTest->push_back(tmp);
        }
        return tmpTest;
        */
    }
    
    antlrcpp::Any visitArglist(Python3Parser::ArglistContext* ctx) override {
        std::vector<antlrcpp::Any>* tmp = new std::vector<antlrcpp::Any>;
        for (int i = 0; i < ctx->argument().size(); i++) {
            tmp->push_back(visit(ctx->argument(i)));
        }
        return tmp;
    }
    
    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext* ctx) override {
        if (ctx->NAME()) {          //positional 

        } else {
            
            return visit(ctx->test());
        }
    }
    
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
