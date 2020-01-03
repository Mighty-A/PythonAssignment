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
std::map<std::string, Python3Parser::FuncdefContext *> Function;

int callStack = 0;      //

int anonymousArgIndex = 0;
std::string AnonymousArg[] = {"000", "001", "002", "003", "004", "005", "006", "007", "008", "009", "010", "011", "012", "013", "014", "015", "016", "017", "018", "019", "020"};
/**********///Attention!
/*set the int type as warning value
if a visit return int 0 means nothing wrong
1 means break
2 means continue
3 means return(in python)
4 means none
110 means there is something wrong in this visit
*/


bool useVariableAsLvalue = false;
/*
every time trying to use a variable in Variables as a lvalue,
please make the bool value (useVariableAsValue) true before visit(ctx-><>()) 
so the visit fuction will return a pointer pointing to the space of that variable
and make the bool value false after using

PLUS: Although it is really bad code style to do that, but as the DDL looming, I have to use
such fool way to cover the bug I meet.

qwq         -yhy
*/

class EvalVisitor: public Python3BaseVisitor {
    int MinOfFour(int a, int b, int c, int d) {
        return std::min(a, std::min(b, std::min(c, d)));
    }
//todo:override all methods of Python3BaseVisitor

    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *ctx) override {
        std::map<std::string, antlrcpp::Any> GlobalVaraible;
        Variables.push_back(GlobalVaraible);
        for (int i = 0; i < ctx->stmt().size(); i++) { //visit all the stmt
            antlrcpp::Any tmp = visit(ctx->stmt(i));
            if (tmp.is<std::vector<antlrcpp::Any>*>() && tmp.as<std::vector<antlrcpp::Any>*>()) {
                delete tmp.as<std::vector<antlrcpp::Any>*>();
            }
        }
        //std::cout << Variables.size() << std::endl;     //debug
        return 0;
        
    }
    
    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext* ctx) override {
        std::string tmpFuncName = ctx->NAME()->toString();
        if (Function.count(tmpFuncName)) {      //calling the function
            if (ctx->parameters())
                visit(ctx->parameters());   //set parameters
            callStack++;
            //std::cout << "call" << callStack << std::endl;//debug
            antlrcpp::Any tmp = visit(ctx->suite());
            callStack--;
            //std::cout << "exitcall" << callStack + 1 << std::endl;//debug
            return tmp;
        } else {                                //definiton
            Function[tmpFuncName] = ctx;
            return 0;
        }
    }
    
    antlrcpp::Any visitParameters(Python3Parser::ParametersContext* ctx) override {
        if (ctx->typedargslist()) {
            return visit(ctx->typedargslist());
        } else {
            return 0;
        }
    }
    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext* ctx) override {
        int i = 0;
        while (i < ctx->tfpdef().size() && Variables[Variables.size() - 1].count(AnonymousArg[i])) {            //set the corresponding arg
            Variables[Variables.size() - 1][visit(ctx->tfpdef(i)).as<std::string>()] = Variables[Variables.size() - 1][AnonymousArg[i]];
            i++;
        }
        int j = 0;
        while (i < ctx->tfpdef().size()) {      //deal with the args that are assign or default
            std::string tmpArgName = visit(ctx->tfpdef(i)).as<std::string>();
            if (!Variables[Variables.size() - 1].count(tmpArgName)) {   //default;
                Variables[Variables.size() - 1][tmpArgName] = visit(ctx->test(j));
                j++;
            }
            i++;
        }
        return 0;
    }
    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext* ctx) override {
        return ctx->NAME()->toString();
    }
    
    antlrcpp::Any visitStmt(Python3Parser::StmtContext* ctx) override {     //visit simple_stmt and compound_stmt
        if (ctx->simple_stmt()) {
            return visit(ctx->simple_stmt());
        } else if (ctx->compound_stmt()) {
            return visit(ctx->compound_stmt());
        }
    }
    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext* ctx) override {       //visit small_stmt
        return visit(ctx->small_stmt());
    }
    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext* ctx) override { //visit expr_stmt adn flow_stmt 
        if (ctx->expr_stmt()) {
            return visit(ctx->expr_stmt());
        } else {
            return visit(ctx->flow_stmt());
        }
    }
    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext* ctx) override {
        if (ctx->augassign()) {                                     //deal with augassign
            useVariableAsLvalue = true;
            std::vector<antlrcpp::Any>* tmpPointer = visit(ctx->testlist(0)).as<std::vector<antlrcpp::Any>*>();
            antlrcpp::Any &left = *((*tmpPointer)[0].as<antlrcpp::Any*>());
            delete tmpPointer;
            useVariableAsLvalue = false;
            tmpPointer = visit(ctx->testlist(1)).as<std::vector<antlrcpp::Any>*>();
            antlrcpp::Any right = (*tmpPointer)[0];
            delete tmpPointer;
            antlrcpp::Any it = visit(ctx->augassign());
            
            switch(it.as<int>()) {                                //incomplete: calculation of 
                case 0:{
                    if (left.is<std::string>()) {
                        std::string tmpLeft = left.as<std::string>();
                        std::string tmpRight = right.as<std::string>();
                        tmpRight.erase(0, 1);
                        tmpLeft.erase(tmpLeft.length() - 1, 1);
                        tmpLeft = tmpLeft + tmpRight;
                        left = tmpLeft;
                    } else if (left.is<double>()) {
                        double tmpLeft = left.as<double>();
                        if (right.is<double>()) {
                            double tmpRight = right.as<double>();
                            tmpLeft += tmpRight;
                        } else if (right.is<BigInteger>()) {
                            tmpLeft += double(right.as<BigInteger>());
                        } else if (right.is<bool>()) {
                            if (right.as<bool>()) 
                                tmpLeft += 1;
                        }
                        left = tmpLeft;
                    } else if (left.is<BigInteger>()) {
                        BigInteger tmpLeft = left.as<BigInteger>();
                        if (right.is<BigInteger>()) {
                            tmpLeft += right.as<BigInteger>();
                            left = tmpLeft;
                        } else if (right.is<bool>()){
                            if (right.as<bool>()) {
                                BigInteger one("1");
                                tmpLeft += one;
                                left = tmpLeft;
                            }
                        } else if (right.is<double>()) {
                            double a = right.as<double>() + double(tmpLeft);
                            left = a;
                        }
                    }
                }
                    break;
                case 1: {
                    if (left.is<double>()) {
                        double tmpLeft = left.as<double>();
                        if (right.is<double>()) {
                            double tmpRight = right.as<double>();
                            tmpLeft -= tmpRight;
                        } else if (right.is<BigInteger>()) {
                            tmpLeft -= double(right.as<BigInteger>());
                        } else if (right.is<bool>()) {
                            if (right.as<bool>())
                                tmpLeft -= 1;
                        }
                        left = tmpLeft;
                    } else if (left.is<BigInteger>()) {
                        BigInteger tmpLeft = left.as<BigInteger>();
                        if (right.is<BigInteger>()) {
                            tmpLeft -= right.as<BigInteger>();
                            left = tmpLeft;
                        } else if (right.is<bool>()) {
                            if (right.as<bool>()) {
                                BigInteger one("1");
                                tmpLeft -= one;
                                left = tmpLeft;
                            } 
                        } else if (right.is<double>()) {
                            double a = double(tmpLeft) - right.as<double>();
                            left = a;
                        }
                    }
                }
                    break;
                case 2: {
                    if (left.is<std::string>()) {
                        std::string tmpLeft = left.as<std::string>();
                        BigInteger tmpRight = right.as<BigInteger>();
                        BigInteger one("1");
                        std::string copy = tmpLeft;
                        copy.erase(0, 1);
                        copy.erase(copy.length() - 1, 1);
                        tmpLeft = "";
                        tmpLeft += '"';
                        for (BigInteger i("0"); i < tmpRight; i = i + one) {
                            tmpLeft += copy;
                        }
                        tmpLeft += '"';
                        left = tmpLeft;
                    } else if (left.is<double>()) {
                        double tmpLeft = left.as<double>();
                        if (right.is<double>()) {
                            double tmpRight = right.as<double>();
                            tmpLeft *= tmpRight;
                        } else if (right.is<BigInteger>()) {
                            tmpLeft *= double(right.as<BigInteger>());
                        } else if (right.is<bool>()) {
                            if (!right.as<bool>())
                                tmpLeft *= 0;
                        }
                        left = tmpLeft;
                    } else if (left.is<BigInteger>()) {
                        BigInteger tmpLeft = left.as<BigInteger>();
                        if (right.is<BigInteger>()) {
                            tmpLeft *= right.as<BigInteger>();
                            left = tmpLeft;
                        } else if (right.is<bool>()) {
                            if (!right.as<bool>()) {
                                BigInteger zero("0");
                                tmpLeft = zero;
                                left = tmpLeft;
                            }
                        } else if (right.is<double>()) {
                            double a = double(tmpLeft) / right.as<double>();
                            left = a;
                        }
                        
                    }
                }
                    break;
                case 3:{
                    if (left.is<double>()) {
                        double tmpLeft = left.as<double>();
                        if (right.is<double>()) {
                            double tmpRight = right.as<double>();
                            tmpLeft /= tmpRight;
                        } else if (right.is<BigInteger>()) {
                            tmpLeft /= double(right.as<BigInteger>());
                        } 
                        left = tmpLeft;
                    } 
                }
                    break;
                case 4: {
                    if (left.is<BigInteger>()) {
                        BigInteger tmpLeft = left.as<BigInteger>();
                        if (right.is<BigInteger>()) {
                            tmpLeft /= right.as<BigInteger>();
                            left = tmpLeft;
                        }
                    }
                }
                    break;
                case 5:
                    if (left.is<BigInteger>()) {
                        BigInteger tmpLeft = left.as<BigInteger>();
                        if (right.is<BigInteger>()) {
                            tmpLeft %= right.as<BigInteger>();
                        }
                        left = tmpLeft;
                    }
                    break;
            }
            return 0; //
        } else {                //incomplete
            int length = ctx->testlist().size();
            if (length == 1) {
                return visit(ctx->testlist(0));
            } else {
                antlrcpp::Any value = visit(ctx->testlist(length - 1));
                std::vector<antlrcpp::Any>* Pointer = value.as<std::vector<antlrcpp::Any>*>();
                std::vector<antlrcpp::Any> temp = *Pointer;
                for (int i = length - 2; i >= 0; i--) {
                    useVariableAsLvalue = true;
                    antlrcpp::Any tmpVariable = visit(ctx->testlist(i));
                    useVariableAsLvalue = false;
                    std::vector<antlrcpp::Any>* tmpPointer = tmpVariable.as<std::vector<antlrcpp::Any>*>();
                    for (int j = 0; j < Pointer->size(); j++) {
                        antlrcpp::Any tmp = (*tmpPointer)[j];
                        if (tmp.is<std::string>()) {
                            Variables[Variables.size() - 1][tmp.as<std::string>()] = (*Pointer)[j];
                        } else if (tmp.is<antlrcpp::Any*>()) {
                            *(tmp.as<antlrcpp::Any*>()) = (*Pointer)[j];
                        }

                    }
                    delete tmpPointer;
                }
                delete Pointer;

                return temp;
            }
        }
    }
    
    
    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext* ctx) override {
        if (ctx->ADD_ASSIGN()) 
            return 0;
        if (ctx->SUB_ASSIGN())
            return 1;
        if (ctx->MULT_ASSIGN())
            return 2;
        if (ctx->DIV_ASSIGN())
            return 3;
        if (ctx->IDIV_ASSIGN())
            return 4;
        if (ctx->MOD_ASSIGN())
            return 5;
    }
    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext* ctx) override {
        if (ctx->break_stmt()) {
            return 1;
        } else if (ctx->continue_stmt()) {
            return 2;
        } else if (ctx->return_stmt()) {
            return visit(ctx->return_stmt());
        }
    }
    /*
    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext* ctx) override;
    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext* ctx) override;
    */
    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext* ctx) override {
        if (ctx->testlist()) {
            return visit(ctx->testlist());
        } else {
            return -1;          //no return function
        }
    }
    
    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext* ctx) override {
        if (ctx->if_stmt()) {
            antlrcpp::Any a = visit(ctx->if_stmt());
            return a;
        } else if (ctx->while_stmt()) {
            antlrcpp::Any a = visit(ctx->while_stmt());
            return a;
        } else if (ctx->funcdef()) {
            antlrcpp::Any a = visit(ctx->funcdef());
            return a;
        }
    }
    
    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext* ctx) override {
        std::map<std::string, antlrcpp::Any> LocalVariable;
        Variables.push_back(LocalVariable);
        int totalSuite = ctx->suite().size();
        if (ctx->ELSE())
            totalSuite--;
        int i = 0;
        while (i < totalSuite) {
            antlrcpp::Any tmp = visit(ctx->test(i));
            bool flag = false;
            if (tmp.is<bool>() && tmp.as<bool>()) {
                flag = true;
            } else if (tmp.is<BigInteger>()) {
                BigInteger zero("0");
                BigInteger zero1("-0");
                if (tmp.as<BigInteger>() != zero && tmp.as<BigInteger>() != zero1) {
                    flag = true;
                }
            } else if (tmp.is<double>() && tmp.as<double>() != 0) {
                flag = true;
            } else if (tmp.is<std::string>() && tmp.as<std::string>() != "") {
                flag = true;
            }
            if (flag) {
                antlrcpp::Any a = visit(ctx->suite(i));
                Variables.pop_back();
                return a;
            }
            i++;
        }
        if (ctx->ELSE()) {
            antlrcpp::Any a = visit(ctx->suite(ctx->suite().size() - 1));
            Variables.pop_back();
            return a;
        } else {
            Variables.pop_back();
            return 0;
        }
    }

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext* ctx) override {
        std::map<std::string, antlrcpp::Any> LocalVariable;
        Variables.push_back(LocalVariable);
        while (true) {
            antlrcpp::Any tmp = visit(ctx->test());
            bool flag = false;
            int control = 0;
            if (tmp.is<bool>() && tmp.as<bool>()) {
                flag = true;
            } else if (tmp.is<BigInteger>()) {
                BigInteger zero("0");
                BigInteger zero1("-0");
                if (tmp.as<BigInteger>() != zero && tmp.as<BigInteger>() != zero1) {
                    flag = true;
                }
            } else if (tmp.is<double>() && tmp.as<double>() != 0) {
                flag = true;
            } else if (tmp.is<std::string>() && tmp.as<std::string>() != "") {
                flag = true;
            }
            if (flag) {
                antlrcpp::Any returnFromSuite = visit(ctx->suite());
                if (returnFromSuite.is<int>()) {
                    if (returnFromSuite.as<int>() == 1){     //break
                        Variables.pop_back();
                        return 0;
                    }
                    else if (returnFromSuite.as<int>() == 2)//continue
                        continue;
                    else if (returnFromSuite.as<int>() == -1) {  //no value return 
                        Variables.pop_back();
                        return -1;
                    }
                } else if (returnFromSuite.is<std::vector<antlrcpp::Any>*>()) {
                    Variables.pop_back();
                    return returnFromSuite;
                }
            } else {
                Variables.pop_back();
                return 0;
            }
        }
    }
    
    antlrcpp::Any visitSuite(Python3Parser::SuiteContext* ctx) override {
        if (ctx->simple_stmt()) {
            antlrcpp::Any tmp = visit(ctx->simple_stmt());
            return tmp;
        } else {
            for (int i = 0; i < ctx->stmt().size(); i++) {
                antlrcpp::Any tmp = visit(ctx->stmt(i));
                if (tmp.is<int>()) {
                    int flow = tmp.as<int>();
                    if (flow == 1 || flow == 2)
                        return flow;
                } else if (tmp.is<std::vector<antlrcpp::Any>*>()) {      //return expr exit if
                    return tmp;
                }
            }
            return 0;
        }
    }
    antlrcpp::Any visitTest(Python3Parser::TestContext* ctx) override {

        return visit(ctx->or_test());
    }
    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext* ctx) override {
        if (ctx->and_test().size() > 1) {                   //incompelete
            bool result = false;
            for (int i = 0; i < ctx->and_test().size(); i++) {
                antlrcpp::Any tmpTest = visit(ctx->and_test(i));
                if (tmpTest.is<BigInteger>()) {
                    BigInteger tmpInt = tmpTest.as<BigInteger>();
                    BigInteger zero("0");
                    BigInteger zero1("-0"); // to avoid some bad bug caused by the clas BigInteger
                    if (tmpInt != zero && tmpInt != zero1) {
                        result = true;
                        return result;
                    }
                } else if (tmpTest.is<double>()) {
                    double tmpFloat = tmpTest.as<double>();
                    if (tmpFloat != 0) {
                        result = true;
                        return result;
                    }
                } else if (tmpTest.is<bool>()) {
                    if (tmpTest.as<bool>()) {
                        result = true;;
                        return result;
                    }
                } else if (tmpTest.is<std::string>()) {
                    if (tmpTest.as<std::string>() != "\"\"") {
                        result = true;
                        return result;
                    }
                }
            }
            return result;

        } else {
            return visit(ctx->and_test()[0]);
        }
    }
    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext* ctx) override {
        if (ctx->not_test().size() > 1) {
            bool result = true;
            for (int i = 0; i < ctx->not_test().size(); i++) {
                antlrcpp::Any tmpTest = visit(ctx->not_test(i));
                if (tmpTest.is<BigInteger>()) {
                    BigInteger tmpInt = tmpTest.as<BigInteger>();
                    BigInteger zero("0");
                    BigInteger zero1("-0"); // to avoid some bad bug caused by the clas BigInteger
                    if (tmpInt == zero || tmpInt == zero1) {
                        result = false;
                        return result;
                    } 
                } else if (tmpTest.is<double>()) {
                    double tmpFloat = tmpTest.as<double>();
                    if (tmpFloat == 0) {
                        result = false;
                        return result;
                    }
                } else if (tmpTest.is<bool>()) {
                    if (!tmpTest.as<bool>()) {
                        result = false;
                        return result;
                    }
                } else if (tmpTest.is<std::string>()) {
                    if (tmpTest.as<std::string>() == "\"\"") {
                        result = false;
                        return result;
                    }
                }
            }
            return result;
        } else {
            return visit(ctx->not_test()[0]);
        }
    }
    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext* ctx) override {
        if (ctx->not_test()) {              //complete
            antlrcpp::Any tmpTest = visit(ctx->not_test());
            bool result;
            if (tmpTest.is<BigInteger>()) {
                BigInteger tmpInt = tmpTest.as<BigInteger>();
                BigInteger zero("0");
                BigInteger zero1("-0");  // to avoid some bad bug caused by the clas BigInteger

                if (tmpInt == zero || tmpInt == zero1) {
                    result = false;
                    return result;
                } else {
                    result = true;
                    return result;
                }
            } else if (tmpTest.is<double>()) {
                double tmpFloat = tmpTest.as<double>();
                if (tmpFloat == 0) {
                    result = false;
                    return result;
                } else {
                    result = true;
                    return result;
                }
            } else if (tmpTest.is<bool>()) {
                if (tmpTest.as<bool>()) {
                    result = false;
                    return result;
                } else {
                    result = true;
                    return result;
                }
            } else if (tmpTest.is<std::string>()) {
                if (tmpTest.as<std::string>() == "\"\"") {
                    result = true;
                    return result;
                }
                else {
                    result= false;
                    return result;
                }
            }
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
        if (ctx->MINUS()) {
            antlrcpp::Any tmpFactor = visit(ctx->factor());
            if (tmpFactor.is<BigInteger>()) {
                BigInteger tmp = tmpFactor.as<BigInteger>();
                BigInteger zero("0");
                tmp = zero - tmp;
                return tmp;
            } else if (tmpFactor.is<bool>()){
                if (tmpFactor.as<bool>()) {
                    BigInteger a("-1");
                    return a;
                } else {
                    BigInteger a("0");
                    return a;
                }
            } else if (tmpFactor.is<double>()) {
                double tmp = tmpFactor.as<double>();
                tmp = - tmp;
                return tmp;
            }
        } else if (ctx->ADD()) {
            antlrcpp::Any tmpFactor = visit(ctx->factor());
            if (tmpFactor.is<bool>()) {
                if (tmpFactor.as<bool>()) {
                    BigInteger a("1");
                    return a;
                } else {
                    BigInteger a("0");
                    return a;
                }
            } else {
                return visit(ctx->factor());
            }
        } else {
            return visit(ctx->atom_expr());
        }
    }
    
    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext* ctx) override {
        antlrcpp::Any tmpAtom = visit(ctx->atom());
        if (ctx->trailer()) {           //function call
            std::string funcName = tmpAtom.as<std::string>();
            std::map<std::string, antlrcpp::Any> LocalVariable;
            Variables.push_back(LocalVariable);
            antlrcpp::Any a = visit(ctx->trailer());
            std::vector<antlrcpp::Any>* tmp = a.as<std::vector<antlrcpp::Any>*>();
            if (funcName == "print") {
                Variables.pop_back();
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
                        if (i != tmp->size() - 1) 
                            std::cout << ' ';
                        continue;
                    }
                    if ((*tmp)[i].is<bool>()) {
                        std::cout << (((*tmp)[i].as<bool>()) ? "True" : "False");
                        if (i != tmp->size() - 1)
                            std::cout << ' ';

                        continue;
                    }
                    if ((*tmp)[i].is<BigInteger>()) {
                        std::cout << (*tmp)[i].as<BigInteger>();
                        if (i != tmp->size() - 1)
                            std::cout << ' ';

                        continue;
                    }
                    if ((*tmp)[i].is<double>()) {
                        std::cout << std::fixed << std::setprecision(6) << (*tmp)[i].as<double>();
                        if (i != tmp->size() - 1)
                            std::cout << ' ';

                        continue;
                    } 
                    if ((*tmp)[i].is<int>() && (*tmp)[i].as<int>() == 4) {
                        std::cout << "None";
                        if (i != tmp->size() - 1)
                            std::cout << ' ';
                    }
               }
               std::cout << std::endl;
               delete tmp;
               return 0;
            } else if (funcName == "int") {
                Variables.pop_back();
                antlrcpp::Any arg = (*tmp)[0];
                delete tmp;
                if (arg.is<double>()) {
                    int tmp = int(arg.as<double>());
                    BigInteger result(tmp);
                    return result;
                } else if (arg.is<bool>()) {
                    if (arg.as<bool>()) {
                        BigInteger one("1");
                        return one;
                    } else {
                        BigInteger zero("0");
                        return zero;
                    }
                } else if (arg.is<std::string>()) {
                    std::string tmp = arg.as<std::string>();
                    tmp.erase(0, 1);
                    tmp.erase(tmp.size() - 1, 1);
                    BigInteger result(tmp);
                    return result;
                }
                return 0;
            } else if (funcName == "float") {
                Variables.pop_back();
                antlrcpp::Any arg = (*tmp)[0];
                delete tmp;
                if (arg.is<BigInteger>()) {
                    return double(arg.as<BigInteger>());
                } else if (arg.is<bool>()) {
                    if (arg.as<bool>()) {
                        double tmp = 1;
                        return tmp;
                    } else {
                        double tmp = 0;
                        return tmp;
                    }
                } else if (arg.is<std::string>()) {
                    std::string floatString = arg.as<std::string>();
                    floatString.erase(0, 1);
                    floatString.erase(floatString.size() - 1, 1);
                    double result = 0;
                    int j = 0;
                    for (j = 0; j < floatString.size(); j++) {
                        if (floatString[j] != '.') {
                            result *= 10;
                            result += (floatString[j] - '0');
                        } else break;
                    }
                    for (int k = j + 1; k < floatString.size(); k++) {
                        result += double(floatString[k] - '0') * pow(10, j - k);
                    }
                    return result;
                }
                return 0;
            } else if (funcName == "str") {
                Variables.pop_back();
                antlrcpp::Any arg = (*tmp)[0];
                delete tmp;
                std::string tmpString;
                if (arg.is<BigInteger>()) {
                    tmpString = std::string(arg.as<BigInteger>());
                    tmpString = '"' + tmpString + '"';
                    return tmpString;
                } else if (arg.is<double>()) {
                    double tmp = arg.as<double>();
                    tmpString = std::to_string(tmp);
                    tmpString = '"' + tmpString + '"';
                    return tmpString;
                } else if (arg.is<bool>()) {
                    if (arg.as<bool>()) {
                        std::string tmp = "\"True\"";
                        return tmp;
                    } else {
                        std::string tmp = "\"False\"";
                        return tmp;
                    }
                }
                return 0;
            } else if (funcName == "bool") {
                Variables.pop_back();
                antlrcpp::Any arg = (*tmp)[0];
                delete tmp;
                if (arg.is<BigInteger>()) {
                    BigInteger zero("0");
                    BigInteger zero1("-0");
                    if (arg.as<BigInteger>() == zero || arg.as<BigInteger>() == zero1) {
                        return false;
                    } else return true;
                } else if (arg.is<double>()) {
                    if (arg.as<double>() < 0.00000001) 
                        return false;
                    else 
                        return true;
                } else if (arg.is<std::string>()) {
                    if (arg.as<std::string>() == "\"\"")
                        return false;
                    else 
                        return true;
                }
            } else if (Function.count(funcName)) {          //call costom function
                antlrcpp::Any returnValue = visit(Function[funcName]);  
                Variables.pop_back();
                delete tmp;
                
                if (returnValue.is<std::vector<antlrcpp::Any>*>()) {
                    std::vector<antlrcpp::Any>* returnPointer = returnValue.as<std::vector<antlrcpp::Any>*>();
                    if (returnPointer->size() == 0) {
                        delete returnPointer;
                        return 0;       //non-return function
                    } else if (returnPointer->size() == 1) {
                        antlrcpp::Any returnAny = (*returnPointer)[0];
                        delete returnPointer;
                        return returnAny;
                    } else {
                        return returnValue;
                    }
                }
                
                return returnValue;
            }
            return tmp;   //
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
                        if (useVariableAsLvalue) {
                            antlrcpp::Any *value1 = &Variables[j][variableName];
                            return value1;
                        } else  {
                            antlrcpp::Any value1 = Variables[j][variableName];
                            return value1;
                        }
                    }
                }
            }
            return value;
        }
    }
    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext* ctx) override {
        if (ctx->arglist()) {
            return visit(ctx->arglist());
        } else {
            std::vector<antlrcpp::Any>* NoArg = new std::vector<antlrcpp::Any>;
            return NoArg;
        }
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
            std::string tmp = ctx->STRING(0)->toString();
            for (int i = 1; i < ctx->STRING().size(); i++) {
                tmp.erase(tmp.size() - 1, 1);
                std::string a = ctx->STRING(i)->toString();
                a.erase(0, 1);
                tmp += a;
            }
            return tmp;
        }
        if (ctx->NONE()){
            return 4;
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
        if (useVariableAsLvalue) {
            std::vector<antlrcpp::Any>* tmpTest = new std::vector<antlrcpp::Any>;
            for (int i = 0; i < ctx->test().size(); i++) {
                antlrcpp::Any tmp = visit(ctx->test(i));
                if (tmp.is<std::vector<antlrcpp::Any>*>()) {
                    delete tmpTest;
                    return tmp;
                }
                tmpTest->push_back(tmp);
            }
            return tmpTest;
        } else {
            
            std::vector<antlrcpp::Any>* tmpTest = new std::vector<antlrcpp::Any>;
            for (int i = 0; i < ctx->test().size(); i++) {
                antlrcpp::Any tmp = visit(ctx->test(i));
                tmpTest->push_back(tmp);
                
                if (tmp.is<int>() && tmp.as<int>() == 0) {
                    delete tmpTest;
                    return 0;
                }
                
            }
            return tmpTest;
        }
    }
    
    antlrcpp::Any visitArglist(Python3Parser::ArglistContext* ctx) override {
        std::vector<antlrcpp::Any>* tmpArg = new std::vector<antlrcpp::Any>;
        for (int i = 0; i < ctx->argument().size(); i++) {
            anonymousArgIndex = i;
            antlrcpp::Any tmp = visit(ctx->argument(i));
            if (i == 0 && tmp.is<std::vector<antlrcpp::Any>*>()) {
                delete tmpArg;
                return tmp;
            }
            tmpArg->push_back(tmp);
        }
        anonymousArgIndex = 0;
        return tmpArg;
    }
    
    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext* ctx) override {
        antlrcpp::Any tmp = visit(ctx->test());
        if (ctx->NAME()) {          //positional 
            Variables[Variables.size() - 1][ctx->NAME()->toString()] = tmp;
        } else {
            Variables[Variables.size() - 1][AnonymousArg[anonymousArgIndex]] = tmp;
        }
        return tmp;
    }
    
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
