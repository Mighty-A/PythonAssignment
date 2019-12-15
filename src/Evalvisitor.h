#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include "BigInteger.h"
#include "Python3BaseVisitor.h"
#include <map>
#include <cmath>
#include <string>
#include <iomanip>
#include <vector>

std::vector<std::map<std::string, antlrcpp::Any>> Variables;
class EvalVisitor: public Python3BaseVisitor {
    int MinOfFour(int a, int b, int c, int d) {
        return std::min(a, std::min(b, std::min(c, d)));
    }
//todo:override all methods of Python3BaseVisitor
    
    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        std::map<std::string, antlrcpp::Any> GlobalVaraible;
        Variables.push_back(GlobalVaraible);
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
                    for (int j = Variables.size() - 1; j >= 0; j--) {
                        if (Variables[j].count(variableName)) {

                            Variables[j][variableName] = value;          //update or new a varible
                            flag = true;
                            
                            break;
                        }


                    }
                    if (!flag) {
                        Variables[Variables.size() - 1][variableName] = value;
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
            std::vector<antlrcpp::Any> tmpArith_expr;
            for (int i = 0; i < ctx->arith_expr().size(); i++) {            //visit arith
                tmpArith_expr.push_back(visit(ctx->arith_expr(i)));
            }
            bool result = true;
            for (int i = 0; i < ctx->comp_op().size(); i++) {               //deal with comparison;
                int tmpComp = visit(ctx->comp_op(i)).as<int>();
                if (tmpArith_expr[i].is<std::string>()) {
                    std::string tmp1 = tmpArith_expr[i].as<std::string>();
                    std::string tmp2 = tmpArith_expr[i + 1].as<std::string>();
                    switch (tmpComp)
                    {
                    case 0:
                        if (tmp1 >= tmp2) 
                            result = false;
                        break;
                    case 1:
                        if (tmp1 <= tmp2)
                            result = false;
                        break;
                    case 2:
                        if (tmp1 != tmp2)
                            result = false;
                        break;
                    case 3:
                        if (tmp1 < tmp2)
                            result = false;
                        break;
                    case 4:
                        if (tmp1 > tmp2) 
                            result = false;
                        break;
                    case 5:
                        if (tmp1 == tmp2)
                            result = false;
                        break;
                    default:
                        break;
                    }

                } else if (tmpArith_expr[i].is<BigInteger>() && tmpArith_expr[i + 1].is<BigInteger>()) {
                    BigInteger tmp1 = tmpArith_expr[i].as<BigInteger>();
                    BigInteger tmp2 = tmpArith_expr[i + 1].as<BigInteger>();
                    switch (tmpComp) {
                    case 0:
                        if (tmp1 >= tmp2)
                            result = false;
                        break;
                    case 1:
                        if (tmp1 <= tmp2)
                            result = false;
                        break;
                    case 2:
                        if (tmp1 != tmp2)
                            result = false;
                        break;
                    case 3:
                        if (tmp1 < tmp2)
                            result = false;
                        break;
                    case 4:
                        if (tmp1 > tmp2)
                            result = false;
                        break;
                    case 5:
                        if (tmp1 == tmp2)
                            result = false;
                        break;
                    default:
                        break;
                    }
                } else if (tmpArith_expr[i].is<double>() && tmpArith_expr[i + 1].is<double>()) {
                    double tmp1 = tmpArith_expr[i].as<double>();
                    double tmp2 = tmpArith_expr[i + 1].as<double>();
                    switch (tmpComp) {
                    case 0:
                        if (tmp1 >= tmp2)
                            result = false;
                        break;
                    case 1:
                        if (tmp1 <= tmp2)
                            result = false;
                        break;
                    case 2:
                        if (tmp1 != tmp2)
                            result = false;
                        break;
                    case 3:
                        if (tmp1 < tmp2)
                            result = false;
                        break;
                    case 4:
                        if (tmp1 > tmp2)
                            result = false;
                        break;
                    case 5:
                        if (tmp1 == tmp2)
                            result = false;
                        break;
                    default:
                        break;
                    }
                } else if (!tmpArith_expr[i].is<BigInteger>() || !tmpArith_expr[i + 1].is<BigInteger>()) {              //one of the two is not BigInteger
                    double tmp1 = 0, tmp2 = 0;
                    if (tmpArith_expr[i].is<BigInteger>()) 
                        tmp1 = double(tmpArith_expr[i].as<BigInteger>());
                    else if (tmpArith_expr[i].is<double>()) 
                        tmp1 = double(tmpArith_expr[i].as<double>());
                    else if (tmpArith_expr[i].is<bool>()) 
                        if (tmpArith_expr[i].as<bool>()) 
                            tmp1 = 1;
                        else 
                            tmp1 = 0;
                    if (tmpArith_expr[i + 1].is<BigInteger>())
                        tmp2 = double(tmpArith_expr[i + 1].as<BigInteger>());
                    else if (tmpArith_expr[i + 1].is<double>())
                        tmp2 = double(tmpArith_expr[i + 1].as<double>());
                    else if (tmpArith_expr[i + 1].is<bool>())
                        if (tmpArith_expr[i + 1].as<bool>())
                            tmp2 = 1;
                        else
                            tmp2 = 0;
                    switch (tmpComp) {
                    case 0:
                        if (tmp1 >= tmp2)
                            result = false;
                        break;
                    case 1:
                        if (tmp1 <= tmp2)
                            result = false;
                        break;
                    case 2:
                        if (tmp1 != tmp2)
                            result = false;
                        break;
                    case 3:
                        if (tmp1 < tmp2)
                            result = false;
                        break;
                    case 4:
                        if (tmp1 > tmp2)
                            result = false;
                        break;
                    case 5:
                        if (tmp1 == tmp2)
                            result = false;
                        break;
                    default:
                        break;
                    }
                }
                if (!result) //short circuit
                    break;
            }
            return result;
        } else {
            return visit(ctx->arith_expr()[0]);
        }
    }
    //return the index of comp;
    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext* ctx) override {
        if (ctx->LESS_THAN())
            return 0;
        if (ctx->GREATER_THAN())
            return 1;
        if (ctx->EQUALS())
            return 2;
        if (ctx->GT_EQ())
            return 3;
        if (ctx->LT_EQ()) 
            return 4;
        if (ctx->NOT_EQ_2())
            return 5;
    }

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext* ctx) override {
        if (ctx->ADD().size() == 0 && ctx->MINUS().size() == 0) {           //no arith
            return visit(ctx->term()[0]);
        } else {                                                        //arith
            int countOfADD = ctx->ADD().size();
            int countOfMINUS = ctx->MINUS().size();
            int countOfTerm = ctx->term().size();
            antlrcpp::Any firstTerm = visit(ctx->term(0));         //get the last term first to check the type of the term
            if (firstTerm.is<std::string>()) {   //deal with string
                std::string ansString = visit(ctx->term(0)).as<std::string>();
                for (int i = 1; i < ctx->term().size(); i++) {
                    std::string tmp = visit(ctx->term(i)).as<std::string>();
                    ansString.erase(ansString.length() - 1, 1);
                    tmp.erase(0, 1);
                    ansString += tmp;
                }
                return ansString;
            } else {            //deal with numbers
                bool isInteger = true;
                BigInteger resultInt("0");
                double resultFloat = 0;
                if (firstTerm.is<BigInteger>()) {
                    resultInt = firstTerm.as<BigInteger>();
                }
                if (firstTerm.is<double>()) {
                    isInteger = false;
                    resultFloat = firstTerm.as<double>();
                }
                while (countOfADD > 0 || countOfMINUS > 0) {
                    int tmpIndexOfADD = 0;
                    int tmpINdexOfMINUS = 0;
                    

                    if (countOfADD > 0) {
                        tmpIndexOfADD = ctx->ADD(countOfADD - 1)->getSymbol()->getTokenIndex();
                    } else {
                        tmpINdexOfMINUS = ctx->MINUS(countOfMINUS - 1)->getSymbol()->getTokenIndex();
                    }
                    if (tmpIndexOfADD > tmpINdexOfMINUS) {
                        countOfADD--;
                        countOfTerm--;
                        antlrcpp::Any tmp = visit(ctx->term(countOfTerm));
                        if (tmp.is<BigInteger>() && isInteger) {
                            resultInt += tmp.as<BigInteger>();
                        } else if (tmp.is<BigInteger>() && !isInteger) {
                            resultFloat += double(tmp.as<BigInteger>());
                        } else if (tmp.is<double>() && isInteger) {
                            resultFloat = double (resultInt);
                            resultFloat += tmp.as<double>();
                            isInteger = false;
                        } else if (tmp.is<double>() && !isInteger) {
                            resultFloat += tmp.as<double>();
                        } else if (tmp.is<bool>() && isInteger) {
                            if (tmp.as<bool>()) {
                                BigInteger one0("1");
                                resultInt += one0;
                            }
                        } else if (tmp.is<bool>() && !isInteger) {
                            if (tmp.as<bool>()) {
                                resultFloat += 1.0;
                            }
                        }
                    } else {
                        countOfMINUS--;
                        countOfTerm--;
                        antlrcpp::Any tmp = visit(ctx->term(countOfTerm));
                        if (tmp.is<BigInteger>() && isInteger) {
                            resultInt -= tmp.as<BigInteger>();
                        } else if (tmp.is<BigInteger>() && !isInteger) {
                            resultFloat -= double(tmp.as<BigInteger>());
                        } else if (tmp.is<double>() && isInteger) {
                            resultFloat = double(resultInt);
                            resultFloat -= tmp.as<double>();
                            isInteger = false;
                        } else if (tmp.is<double>() && !isInteger) {
                            resultFloat -= tmp.as<double>();
                        } else if (tmp.is<bool>() && isInteger) {
                            if (tmp.as<bool>()) {
                                BigInteger one0("1");
                                resultInt -= one0;
                            }
                        } else if (tmp.is<bool>() && !isInteger) {
                            if (tmp.as<bool>()) {
                                resultFloat -= 1.0;
                            }
                        }
                    }
                }
                if (isInteger) {
                    return resultInt;
                } else {
                    return resultFloat;
                }
            }
        }
    }

    antlrcpp::Any visitTerm(Python3Parser::TermContext* ctx) override {
        if (ctx->factor().size() > 1) {
            antlrcpp::Any tmpfirstFactor = visit(ctx->factor(0));
            if (tmpfirstFactor.is<std::string>()) {
                std::string tmpString = tmpfirstFactor.as<std::string>();
                BigInteger tmpFactor = visit(ctx->factor(1)).as<BigInteger>();
                BigInteger one("1");
                std::string resultString;
                resultString += '"';
                tmpString.erase(0,1);
                tmpString.erase(tmpString.length() - 1, 1);
                for (BigInteger i("0"); i < tmpFactor; i = i + one) {
                    resultString += tmpString;
                }
                resultString += '"';
                return resultString;
            }
            bool resultIsBigInteger = true;
            BigInteger resultInt("0");
            double resultFloat = 0;
            if (tmpfirstFactor.is<BigInteger>()) {
                resultInt = tmpfirstFactor.as<BigInteger>();
            } else if (tmpfirstFactor.is<bool>()) {
                if (tmpfirstFactor.as<bool>()) 
                    resultInt = BigInteger("1");
                
            } else if (tmpfirstFactor.is<double>()) {
                resultIsBigInteger = false;
                resultFloat = tmpfirstFactor.as<double>();
            }
            int countOfSTAR = ctx->STAR().size();
            int countOfDIV = ctx->DIV().size();
            int countOfIDIV = ctx->IDIV().size();
            int countOfMOD = ctx->MOD().size();
            int iSTAR, iDIV, iIDIV, iMOD, iFactor;
            iSTAR = iDIV = iIDIV = iMOD = 0;
            iFactor = 1;
            while (iFactor < ctx->factor().size()) {
                int tmpIndexOfSTAR = 0x3f3f3f, tmpIndexOfDIV = 0x3f3f3f, tmpIndexOfIDIV = 0x3f3f3f, tmpIndexOfMOD = 0x3f3f3f;
                if (iSTAR < countOfSTAR) 
                    tmpIndexOfSTAR = ctx->STAR(iSTAR)->getSymbol()->getTokenIndex();
                if (iDIV < countOfIDIV)
                    tmpIndexOfIDIV = ctx->IDIV(iIDIV)->getSymbol()->getTokenIndex();
                if (iDIV < countOfDIV)
                    tmpIndexOfDIV = ctx->DIV(iDIV)->getSymbol()->getTokenIndex();
                if (iMOD < countOfMOD) 
                    tmpIndexOfMOD = ctx->MOD(iMOD)->getSymbol()->getTokenIndex();
                int min0 = MinOfFour(tmpIndexOfSTAR, tmpIndexOfDIV, tmpIndexOfIDIV, tmpIndexOfMOD);
                antlrcpp::Any tmpFactor = visit(ctx->factor(iFactor));
                if (tmpIndexOfSTAR == min0) {
                    if (tmpFactor.is<double>()) {
                        if (resultIsBigInteger) {
                            resultIsBigInteger = false;
                            resultFloat = double(resultInt);
                        }
                        resultFloat *= tmpFactor.as<double>();
                    } else if (tmpFactor.is<BigInteger>()) {
                        if (resultIsBigInteger) {
                            resultInt *= tmpFactor.as<BigInteger>();
                        } else {
                            resultFloat *= double(tmpFactor.as<BigInteger>());
                        }
                    } else if (tmpFactor.is<bool>()) {
                        if (resultIsBigInteger) {
                            if (!tmpFactor.as<bool>())          //* 0
                                resultInt = BigInteger("0");
                        } else {
                            if (!tmpFactor.as<bool>())          //* 0
                                resultFloat = 0;
                        }
                    }
                    iSTAR++;
                } else if (tmpIndexOfDIV == min0) {
                    if (resultIsBigInteger) {
                        resultFloat = double(resultInt);
                        resultIsBigInteger = false;
                    }
                    if (tmpFactor.is<double>()) 
                        resultFloat /= tmpFactor.as<double>();
                    else if (tmpFactor.is<BigInteger>())
                        resultFloat /= double(tmpFactor.as<BigInteger>());
                    else if (tmpFactor.is<bool>()) {    //nothing to do

                    }
                    iDIV++;
                } else if (tmpIndexOfIDIV == min0) {   
                    if (tmpFactor.is<BigInteger>()) {
                        resultInt /= tmpFactor.as<BigInteger>();
                    } else if (tmpFactor.is<bool>()) {      //nothing to do

                    }
                    iIDIV++;
                } else if (tmpIndexOfMOD == min0) {
                    if (tmpFactor.is<BigInteger>()) {
                        resultInt %= tmpFactor.as<BigInteger>();
                    } else if (tmpFactor.is<bool>()) {
                        resultInt = BigInteger("0");
                    }
                    iMOD++;
                }
                iFactor++;
            }
            if (resultIsBigInteger) 
                return resultInt;
            else 
                return resultFloat;
           
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
                            for (int k = Variables.size() - 1; k >= 0; k--) {
                                if (Variables[k].count(tmpString)) {
                                    antlrcpp::Any ret = Variables[k][tmpString];
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
            antlrcpp::Any value = visit(ctx->atom());
            /*  debug: checking the name of variable visited
            if (value.is<std::string>())
                std::cout << value.as<std::string>() << std::endl;
                */
            if (value.is<std::string>() && (value.as<std::string>()[0] != '"' || value.as<std::string>()[0] != '\'')) { //the value is a variable name
                std::string variableName = value.as<std::string>();
                for (int j = Variables.size() - 1; j >= 0; j--) {
                    if (Variables[j].count(variableName)) {
                        value = Variables[j][variableName];

                    }
                }
            }
            return value;
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
                    tmp += tmpNumber[j] - '0';
                }
                for (int j = i + 1; j < tmpNumber.length(); j++) {
                    tmp += (double)(tmpNumber[j] - '0') * pow(10, i - j);
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
