/************************************************************************************\
** Copyright (c) 2010-2011, Rafael Medina                                           **
** All rights reserved.                                                             **
**                                                                                  **
** Redistribution and use in source and binary forms, with or without               **
** modification, are permitted provided that the following conditions are met:      **
**     * Redistributions of source code must retain the above copyright             **
**       notice, this list of conditions and the following disclaimer.              **
**     * Redistributions in binary form must reproduce the above copyright          **
**       notice, this list of conditions and the following disclaimer in the        **
**       documentation and/or other materials provided with the distribution.       **
**     * Neither the name of the Developer nor the names of its contributors        **
**       may be used to endorse or promote products derived from this software      **
**       without specific prior written permission.                                 **
**                                                                                  **
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  **
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    **
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           **
** DISCLAIMED. IN NO EVENT SHALL THE DEVELOPER BE LIABLE FOR ANY DIRECT, INDIRECT,  **
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT     **
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,      **
** OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF        **
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE  **
** OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN           **
** IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                    **
\************************************************************************************/
/**
*   $Rev$
*   $LastChangedDate$
*/
#ifndef _FUNKII_CALC_H_
#define _FUNKII_CALC_H_

/* INCLUDES! */
#include <cerrno>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
using namespace std;

/* Let us Disable some annoying warnings... */
#ifdef _MSC_VER
    #pragma warning( disable : 4127 )   //conditional exp?? :S..
    #pragma warning( disable : 4365 )   //signed/unsigned
    #pragma warning( disable : 4820 )   //padding?..
    #pragma warning( disable : 4996 )   //strcpy not safe
#endif


/* GLOBALS! */
#ifdef CALC_DEBUG_CONSOLE
    #define C_DBG_INIT
    #define C_DBG_START printf("(Line %i):%s:(begin)\n",__LINE__,__FUNCTION__);
    #define C_DBG_MSG(...) \
        do{\
            printf(__VA_ARGS__);\
            printf("\n");\
        } while(0);
    #define C_DBG_END printf("(Line %i):%s:(end)\n",__LINE__,__FUNCTION__);
    #define C_DBG_FINISH
#elif defined(CALC_DEBUG_FILE)
    #include <time.h>
    FILE *calc_debug;
    clock_t calc_t_init;
    #define C_DBG_INIT \
        do { \
            calc_t_init = clock();\
            calc_debug = fopen("calc_debug.log","a+"); \
            fprintf(calc_debug, "=================================================\n");\
            fprintf(calc_debug, "   Session Start %s :: %s\n", __DATE__, __TIME__);\
            fprintf(calc_debug, "=================================================\n");\
        } while(0);
    #define C_DBG_START fprintf(calc_debug, "(Line %i):%s:(begin)\n", __LINE__, __FUNCTION__);
    #define C_DBG_MSG(...) \
        do {\
            fprintf(calc_debug, __VA_ARGS__);\
            fprintf(calc_debug, "\n");\
        } while(0);
    #define C_DBG_END fprintf(calc_debug, "(Line %i):%s:(end)\n",__LINE__,__FUNCTION__);
    #define C_DBG_FINISH \
        do { \
            fprintf(calc_debug, "=================================================\n");\
            double calc_t_end = (clock() - calc_t_init);\
            calc_t_end /= CLOCKS_PER_SEC;\
            fprintf(calc_debug, "   Session End :: Elapsed time: %Gs\n", calc_t_end);\
            fprintf(calc_debug, "=================================================\n\n\n");\
            fclose(calc_debug);\
        } while (0);
#else
    #define C_DBG_INIT
    #define C_DBG_START
    #define C_DBG_MSG(...)
    #define C_DBG_END
    #define C_DBG_FINISH
#endif

#define PI  3.1415926535897932384626433832795
#define EXP 2.7182818284590452353602874713527

enum FunkiiCalcOptions_t {
    calc_formula        =   1 << 0, //Display the formula with the results
    calc_noresult       =   1 << 1, //Do not output the result (maybe you want the formula only)
    calc_noerror        =   1 << 2, //Do not show error msg
    calc_noformat       =   1 << 3, //Do not Comma Separate (i.e from 1000 to 1,000)
    calc_numtruefalse   =   1 << 4, //Represent True/False as Number not text
    calc_formulaonly    =   calc_formula | calc_noresult, //Only the formula

    calc_default        =   0       //Default Options
};
enum FunkiiCalcErrors_t {
        CE_NADA             =   0,
        CE_EMPTY            =   1,
        CE_SYNTAX           =   2,
        CE_SYNTAX_VARS      =   3,
        CE_SYN_VARS_INFLOOP =   4,
        CE_SYN_PAR          =   5,
        CE_SYN_EMPTY_PAR    =   6,
        CE_SYN_INVALIDCHAR  =   7,
        CE_DIV0             =   8,
        CE_EDOM             =   9,
        CE_ERANGE           =   10,
        CE_FIB_OB           =   11,
        CE_BIN              =   12,
        CE_OCT              =   13,
        CE_HEX              =   14,
        CE_FACT_OB          =   15,
        CE_INT_BITSHIFT     =   16,

        CE_EPIC             =   100
    };
/**
 * Calc Class
 *
 *  This Class is a Calculator that evaluates an Input Formula
 *  and returns a result in various forms (string, char * or long double)
 *  it also serves as a 'converter' of sorts
 *
 *  i.e: converting from celcius to fahrenheit (Calc::tofahrenheit())
 *
 *  Usage Example #1:
 *      string formula("20*10");
 *      Calc calculator(formula);
 *      cout << formula << " = " << calculator.result_s();
 *
 *  Output:
 *      20*10 = 200
 *
 *  Usage Example #2:
 *      Calc calculator("300");
 *      cout << "300 seconds is " << calculator.duration();
 *
 *  Output:
 *      300 seconds is 5mins
 */
class Calc {
public:
    /**
     *  Calc Constructor
     *
     *  Will call Calc::assign with "0" as a formula
     *
     */
    Calc();
    /**
     *  Calc Constructor
     *
     *  Will call Calc::assign with the input string as a formula
     *
     * @param   string     string containing the raw formula.
     */
    Calc(string);
    /**
     *  Calc Constructor
     *
     *  Overload function to take in an INT
     *
     * @param   int
     */
    Calc(int);
    /**
     *  Calc Constructor
     *
     *  Overload function to take in an float
     *
     * @param   float
     */
    Calc(float);
    /**
     *  Calc Constructor
     *
     *  Overload function to take in an double
     *
     * @param   double
     */
    Calc(double);
    /**
     *  Calc Constructor
     *
     *  Overload function to take in an long double
     *
     * @param   long double
     */
    Calc(long double);
    /**
     * ~Calc Destructor
     */
    ~Calc();
    /**
     *  assign
     *
     *  Assigns a new formula to be evaluated (like using a Calc constructor)
     *
     * @param   formula         string containing the raw formula.
     */
    void assign(string);
    /**
     *  assign overload function
     *
     *  mimics the Calc Constructor overload function
     *
     * @param   int
     */
    void assign(int);
    /**
     *  assign overload function
     *
     *  mimics the Calc Constructor overload function
     *
     * @param   float
     */
    void assign(float);
    /**
     *  assign overload function
     *
     *  mimics the Calc Constructor overload function
     *
     * @param   double
     */
    void assign(double);
    /**
     *  assign overload function
     *
     *  mimics the Calc Constructor overload function
     *
     * @param   long double
     */
    void assign(long double);
    /**
     *  result_s
     *
     *  Returns the result as a std::string with default options
     *
     * @return  string
     */
    string result_s();
    /**
     *  result_s overload function
     *
     *  Returns the result as a std::string with specified options
     *
     *  @param  enum FunkiiCalcOptions_t
     *  @return string
     */
    string result_s(enum FunkiiCalcOptions_t);
    /**
     * result_c_str
     *
     * @return  (char *)        NULL terminated string containing Result of Formula or Error message.
     */
    char *result_c_str();
    /**
     *  result_c_str overload function
     *
     *  Returns the result as a (char *) with specified options
     *
     * @return  (char *)        NULL terminated string containing Result of Formula or Error message.
     */
    char *result_c_str(enum FunkiiCalcOptions_t);
    /**
     * result_d
     *
     * @return  (long double)   Result of Formula as a Number (check with calc_error() to see if there was an error).
     */
    long double result_d();
    /**
     * calc_error
     *
     *  Checks to see if there was an error
     *
     * @return  true    there was an error :(
     * @return  false   there was NO error!!
     */
    bool error();
    /**
     * calc_error
     *
     *  Returns the error string (if any)
     *
     * @return  string  the error message
     */
    string get_error();
    /**
     * fibonacci
     *
     *  Uses mResult to calculate Fibonacci/Negafibonacci number.
     *
     * @return  string      Fibonacci/Negafibonacci number or Error.
     */
    string fibonacci();
    /**
     * tocelsius
     *
     *  Treats mResult as Fahrenheit and converts it to Celsius.
     *
     * @return  string      Converted degrees string with °C appended or Error string.
     */
    string tocelsius();
    /**
     * tofahrenheit
     *
     *  Treats mResult as Celsius and converts it to Fahrenheit.
     *
     * @return  string      Converted degrees string with °F appended or Error string.
     */
    string tofahrenheit();
    /**
     * duration
     *
     *  Treats mResult as seconds and returns it as a formatted string
     *
     * @param   type        Specifies the format to return: <br>
     *                          0:    year/week/day/hour/minute/second format <br>
     *                          1:    00:00:00 format
     *
     * @return  string      Specified Format or Error string.
     */
    string duration(int);
private:
    enum FunkiiCalcErrors_t mError;   /* Error String. */
    long double mResult;        /* Result of the Formula. */
    string mFormula;            /* Sanity Checked Formula. */
            /* Comparison Global Vars */
    bool mIsCompare;            /* Flag to determine if it's a comparison Formula i.e: 3 > 2 */
    bool mCompare[1000];        /* List of Comparison Results */
    vector<string> mList;       /* List of Comparison Formulas */
    string mOutput;             /* Comparison Output Message. */
    string mCompOutputRes;      /* Comparison Result. */
    long double mCompRes[1000]; /* List of each Option for comparison Results */
    static const char *func_array[];

    /**
     * calcthis
     *
     *  Input a formula to be evaluated (will be sanity checked). <br>
     *      - will set mFormula and mResult if the formula is valid. <br>
     *      - will set mError on Error.
     *
     * @param   formula         string containing the raw formula.
     */
    void calcthis(string);
    /**
     * calcthis
     *
     *  Returns string equivalent of error number
     *
     *  @param   enum FunkiiCalcErrors_t
     *  @result string
     */
    string get_error_string(enum FunkiiCalcErrors_t);
    /**
     * parse_vars
     *
     *  if the formula has variables this function is called to parse and replace them in the formula
     *  sets mError on error.
     *
     *  @param  int         position where the vars end and the formula begins
     *  @param  string      string, containing the raw formula.
     *
     *  @return  string     Formula with variables replaced
     */
    string parse_vars(int, string);
    /**
     * syntax
     *
     *  This Function takes in a string containing a raw formula to be evaluated
     *  checks for syntax errors and returns true or false.
     *
     * @param   formula     string, containing the raw formula.
     *
     * @return  false       Some kind of  Error. Not a Valid Formula.
     * @return  true        Valid Formula! (also sets mFormula).
     */
    bool syntax(string);
    /**
     * format
     *
     *  This Function takes in a number as a string and returns it as
     *  a the formatted Comma separated value. (it considers decimals and E)
     *
     *  ie:
     *      input: 10000
     *      output: 10,000
     *
     * @param   input       string.
     *
     * @return  string      Formatted string.
     */
    string format(string);
    /**
     *  checkandcompare
     *
     *  Evaluates comparisons in the formula
     *
     *  @param   string  formula
     */
    void checkandcompare(string);
    /**
     * IsValidNum
     *
     *  This Function takes in a number as a NULL Terminated String
     *  and returns TRUE if the number is a valid one or FALSE if the number is not.
     *
     * @param   n       A String to be evaluated.
     *
     * @return  true    n is a valid number.
     * @return  false   n is a NOT valid number.
     */
    bool IsValidNum(string);
    /**
     * isFunc
     *
     *  Checks if input string is a valid function i want to parse/eval.
     *
     * @param   in              string to be compared
     *
     * @return  (int) > 0       Number of the function represented as an int.
     * @return  0               Not a Function.
     */
    int isFunc(string);
    /**
     * fib
     *
     *  Recursive function to calculate fibonacci number.
     *
     * @param   n               Input number to calculate the Fibonacci of.
     *
     * @return  (long double)   Fibonacci of n.
     */
    long double fib(long double);
    /**
     * calculate
     *
     *  Input a Formula and outputs the result as a long double.
     *  if there is an error it sets private var `mError` accordingly.
     *
     * @param   formula         A NULL terminated string containing the Valid Formula.
     * @param   level           An int representing the level of operation it is in.
     * @param   oper_func       What operation to evaluate the formula against (i.e floor, ceil...)
     *
     * @return  (long double)   Always returns a number.
     */
    long double calculate(string, int, int);
    /**
     * bin2dec
     *
     *  Input a Binary number (as string) and outputs number in base10
     *
     * @param   string           A string containing the number to be converted.
     *
     * @return  (long double)   input converted into a decimal.
     */
    long double bin2dec(string);
    /**
     * oct2dec
     *
     *  Input an Octal number (as string) and outputs number in base10
     *
     * @param   string           A string containing the number to be converted.
     *
     * @return  (long double)   input converted into a decimal.
     */
    long double oct2dec(string);
    /**
     * hex2dec
     *
     *  Input a Hex number (as string) and outputs number in base10
     *
     * @param   string           A string containing the number to be converted.
     *
     * @return  (long double)   input converted into a decimal.
     */
    long double hex2dec(string);
    /**
     * factorial
     *
     *      Calculates Factorial of number
     *
     * @param   long double
     *
     * @return  long double
     */
    long double factorial(long double);
};
const char *Calc::func_array[] = {  "sqrt", "floor","ceil", "sin",  "cos",
                                    "tan",  "asin", "acos", "atan", "sinh",
                                    "cosh", "tanh", "ln",   "log",  "abs",
                                    "fabs", "bin",  "oct",  "hex",  "round",
                                    "fact"
                                };
Calc::Calc() {  assign("0"); }
Calc::Calc(string formula) { assign(formula); }
Calc::Calc(int number) { assign(number); }
Calc::Calc(float number) { assign(number); }
Calc::Calc(double number) { assign(number); }
Calc::Calc(long double number) { assign(number); }
Calc::~Calc() { }
void Calc::assign(string formula) { calcthis(formula); }
void Calc::assign(int number) { stringstream ss; ss << number; calcthis(ss.str()); }
void Calc::assign(float number) { stringstream ss; ss << number; calcthis(ss.str()); }
void Calc::assign(double number) { stringstream ss; ss << setprecision(15) << number; calcthis(ss.str()); }
void Calc::assign(long double number) { stringstream ss; ss << setprecision(15) << number; calcthis(ss.str()); }
void Calc::calcthis(string formula) {
    C_DBG_INIT;
    C_DBG_START;
    mError = CE_NADA; mFormula.clear(); mResult=0; mIsCompare=false;
    if ( syntax(formula) ) {
        C_DBG_MSG("oo, you returned '%s' ",mFormula.c_str());
        int e = mFormula.find("="), g = mFormula.find_first_of(">"), l = mFormula.find_first_of("<");
        bool check=true;
        while (check && g>0) {
            if (mFormula.at(g+1) == '>') { g = mFormula.find_first_of(">",(g+2)); }
            else { check = false; }
        }
        check=true;
        while (check && l>0) {
            if (mFormula.at(l+1) == '<') { l = mFormula.find_first_of("<",(l+2)); }
            else { check = false; }
        }
        if (e > 0 || g > 0 || l > 0) { checkandcompare(mFormula); }
        else { mResult = calculate(mFormula,0,0); }
    }
    C_DBG_END;
    C_DBG_FINISH;
}
string Calc::parse_vars(int found, string formula) {
    C_DBG_START;
    string tmp,tmp2;
    vector<string> _vars,_vals;
    int pos=-1;
    /**
     * This is a Formula with defined variables, syntax is:
     *        var1=(20+10), var2=3, var1+30-var2
     *            Variables are defined before the ';'
     *            formula comes after the ';'
     *            you can define 1 or more variables.
     *            Nested variables accepted.
     */
    tmp2 = formula.substr(found + 1);
    tmp = formula.substr(0,found);
    C_DBG_MSG("Vars:: %s\tFormula:: %s",tmp.c_str(),tmp2.c_str());
    formula = tmp2;
    found = tmp.find_last_of('=');
    C_DBG_MSG("newstring[%d] '%s' and '%s'",found,tmp.c_str(),tmp2.c_str());
    if (found < 1) {
        mError = CE_SYNTAX_VARS;
        C_DBG_END;
        return formula;
    }
    while (found > 0) { //we found something... lets parse it!
        pos++;
        tmp2 = tmp.substr(found + 1);
        tmp2.insert(tmp2.begin(),'('); tmp2.insert(tmp2.end(),')');
        _vars.push_back(tmp2);
        tmp2 = tmp.substr(0,found);
        tmp = tmp2;
        found = tmp.find_last_of(',');
        if (found > 0) {
            tmp2 = tmp.substr(found + 1);
            _vals.push_back(tmp2);
            tmp2 = tmp.substr(0,found);
            tmp = tmp2;
        }
        else { _vals.push_back(tmp2); }
        found = tmp.find_last_of('=');
    }
    //now we replace the variables in the formula
    for (int i = 0, j = 0; i < (int)_vars.size() && j < 6; i++) {
        if (j == 0) { C_DBG_MSG("vars[%d]: '%s' == '%s'",i,_vals[i].c_str(),_vars[i].c_str()); }
        int x = formula.find(_vals[i]);
        if (j == 5 && x >=0) { mError = CE_SYN_VARS_INFLOOP; C_DBG_END; return formula; }
        while (x >= 0) {
            formula.replace(x, (int)(_vals[i].length()), _vars[i]);
            C_DBG_MSG("\t\tReplaced :: '%s'",formula.c_str());
            x = formula.find(_vals[i]);
        }
        if ((i + 1) == (int)_vars.size()) { j++; i=-1; }
    }
    C_DBG_END;
    return formula;
}
bool Calc::syntax(string formula) {
    C_DBG_START;
    mFormula.clear();
    string tmp;
    int found = formula.find_last_of(';');
    if (found > 0) { formula = parse_vars(found, formula); }
    int p=0, type=0; //Type 0=dec ; 1=bin ; 2=oct ; 3=hex;
    tmp.assign(formula); formula.clear();
    C_DBG_MSG("\tBefore:: %s",tmp.c_str());
    //Clean up the Formula: Make all Lowercase, Remove Spaces and make sure it's all valid chars.
    for (int i = 0; i < (int)tmp.length(); i++) {
        if ((int)tmp.at(i) == 32) { continue; /* IGNORE SPACES */ }
        else {
            if (type == 1) {
                if (tmp.at(i) == '0' || tmp.at(i) == '1') { formula.push_back(tmp.at(i)); }
                else { formula.push_back(')'); type=0; i--; }
            }
            else if (type == 2) {
                if ((int)tmp.at(i) >= 48 && (int)tmp.at(i) <= 56 ) { formula.push_back(tmp.at(i)); }
                else { formula.push_back(')'); type=0; i--; }
            }
            else if (type == 3) {
                if (((int)tmp.at(i) >= 48 && (int)tmp.at(i) <= 57) ||
                    ((int)tmp.at(i) >= 65 && (int)tmp.at(i) <= 90) ||
                    ((int)tmp.at(i) >= 97 && (int)tmp.at(i) <= 122)
                    ) {
                        if ((int)tmp.at(i) >= 65 && (int)tmp.at(i) <= 90) { formula.push_back( (char)((int)tmp.at(i) + 32) ); }
                        else { formula.push_back(tmp.at(i)); }
                }
                else { formula.push_back(')'); type=0; i--; }
            }
            else {
                if (tmp.at(i) == '(') { p++; }
                else if (tmp.at(i) == ')') { p--; }
                if ((int)tmp.at(i) != 37 &&                             //%
                    !((int)tmp.at(i) >= 40 && (int)tmp.at(i) <= 43) &&  //(, ), *, +
                    !((int)tmp.at(i) >= 45 && (int)tmp.at(i) <= 57) &&  //-, ., /, 0-9
                    !((int)tmp.at(i) >= 65 && (int)tmp.at(i) <= 90) &&  //A-Z
                    (int)tmp.at(i) != 94 &&                             //^
                    !((int)tmp.at(i) >= 97 && (int)tmp.at(i) <= 122) && //a-z
                    !((int)tmp.at(i) >= 60 && (int)tmp.at(i) <= 62)     //<, =, >
                    ) {
                        if (tmp.at(i) == '\\' && (i+1) < (int)tmp.length()) {
                            if (tmp.at(i+1) == 'b') { formula += "bin("; i++; type=1; }
                            else if (tmp.at(i+1) == 'o') { formula += "oct("; i++; type=2; }
                            else if (tmp.at(i+1) == 'x') { formula += "hex("; i++; type=3; }
                            else {
                                mError = CE_SYNTAX;
                                C_DBG_END;
                                return false;
                            }
                        }
                        else { C_DBG_MSG("\t\tDO NOT WANT pos[%d]: %c",i,tmp.at(i)); }
                }
                else {
                     //DO WANT... but Lowercase all :D..
                    if ((int)tmp.at(i) >= 65 && (int)tmp.at(i) <= 90) { formula.push_back( (char)((int)tmp.at(i) + 32) ); }
                    else { formula.push_back(tmp.at(i)); }
                }
            }
        }
    }
    if (type != 0) { formula.push_back(')'); }
    C_DBG_MSG("\tAfter :: %s",formula.c_str());
    C_DBG_MSG("if (p == 0) :: p: %d",p);
    if(p == 0 && !formula.empty()) {
        int c = formula.at(0);
        //check if the first char is not a valid char
        if (c != 40 && c != 43 && c != 45 &&    //(, +, -
            !(c >= 48 && c <= 57) &&            //0-9
            !(c >= 97 && c <= 122)              //a-z
            ) {
                //error invalid char :(
                mError = CE_SYN_INVALIDCHAR;
                C_DBG_END;
                return false;
        }
        c =(int)formula.length() - 1;
        //Formula ended with a Cliffhanger? .. SYNTAX ERROR!
        if (formula.at(c) == '+' || formula.at(c) == '-' ||
            formula.at(c) == '*' || formula.at(c) == '/' ||
            formula.at(c) == '%' || formula.at(c) == '^' ||
            formula.at(c) == '>' || formula.at(c) == '<' ||
            formula.at(c) == '=' || formula.at(c) == '(') {
                mError = CE_SYNTAX;
                C_DBG_END;
                return false;
        }
        //lets check for a Parenthesis error
        if (formula.find_last_of('(') > formula.find_last_of(')')) {
            mError = CE_SYN_PAR;
            C_DBG_END;
            return false;
        }
        //check if theres a syntax error.. like "3+*2" or "12*-3" or "6>*2"
        for (int i = 0; i < (int)formula.length() - 1; i++) {
            C_DBG_MSG("[%d] %c::\ti+1 : %c",i,formula.at(i),formula.at(i+1));
            //while here lets check if theres an empty pair of parenthesis
            if (formula.at(i) == '(' && formula.at(i+1) == ')') {
                mError = CE_SYN_EMPTY_PAR;
                C_DBG_END;
                return false;
            }
            if (formula.at(i) == '+' || formula.at(i) == '-' ||
                formula.at(i) == '*' || formula.at(i) == '/' ||
                formula.at(i) == '%' || formula.at(i) == '^' ||
                formula.at(i) == '>' ||
                formula.at(i) == '<' || formula.at(i) == '=') {
                    if ((i+2) < (int)formula.length()) {
                        if (formula.at(i+1) == '-' &&
                            (((int)formula.at(i+2) >= 48 && (int)formula.at(i+2) <= 57) ||
                            ((int)formula.at(i+2) >= 97 && (int)formula.at(i+2) <= 122) ||
                             formula.at(i+2) == '(')) {
                                i+=2; continue; //all is ok
                        }
                    }
                    if (formula.at(i+1) != '(' &&
                            !((int)formula.at(i+1) >= 48 && (int)formula.at(i+1) <= 57) &&
                            !((int)formula.at(i+1) >= 97 && (int)formula.at(i+1) <= 122)
                            ) {
                                if (formula.at(i) == '>' &&
                                    (formula.at(i+1) == '='|| formula.at(i+1) == '>')) { i++; continue; }
                                else if (formula.at(i) == '<' &&
                                    (formula.at(i+1) == '=' ||
                                     formula.at(i+1) == '>' ||
                                     formula.at(i+1) == '<')) { i++; continue; }
                                else if (formula.at(i) == '=' && formula.at(i+1) == '=') { i++; continue; }
                                else {
                                    mError = CE_SYNTAX;
                                    C_DBG_END;
                                    return false;
                                }
                     }
            }
        }
        //we've reached the end! this Formula seems valid :D
        C_DBG_MSG("Final Product!! :: '%s'",formula.c_str());
        mFormula = formula;
        C_DBG_END;
        return true;
    }
    else if (p != 0) {
        mError = CE_SYN_PAR;
        C_DBG_END;
        return false;
    }
    mError = CE_EMPTY;
    C_DBG_END;
    return false;
}

void Calc::checkandcompare(string formula) {
    mIsCompare=true; mList.clear(); mOutput.clear(); mCompOutputRes.clear();
    int last=0, found=formula.find_first_of("<>="), j=0, type[1000];
    /*
        types:
            [0]  <   [1]  >   [2]  =   [3]  <>
            [4]  <=  [5]  >=  [6]  ==
    */
    while (found != (int)string::npos && j < 1000) {
        C_DBG_MSG("FOUND!! :: %c :: %s",formula.at(found),formula.substr(last, (found - last)).c_str());
        /* Lets check it's not a bit shift */
        if (formula.at(found) == '<' && formula.at(found + 1) == '<') { found = formula.find_first_of("<>=", found + 2); continue; }
        if (formula.at(found) == '>' && formula.at(found + 1) == '>') { found = formula.find_first_of("<>=", found + 2); continue; }

        mList.push_back(formula.substr(last, (found - last)));
        mCompRes[j] = calculate(mList[j],0,0);
        last = found;
        switch(formula.at(found)) {
            case '<':{
                        if(formula.at(found+1) == '>') { type[j] = 3; }
                        else if(formula.at(found+1) == '=') { type[j] = 4; }
                        else { type[j] = 0; }
                     } break;
            case '>': {
                        if(formula.at(found+1) == '=') { type[j] = 5; }
                        else { type[j] = 1; }
                      } break;
            case '=': {
                        if(formula.at(found+1) == '=') { type[j] = 6; }
                        else { type[j] = 2; }
                      } break;
        }
        if(type[j] > 2) { last++; }
        last++; j++; found = formula.find_first_of("<>=", last);
    }
    mList.push_back(formula.substr(last));
    mCompRes[j] = calculate(mList[j],0,0);
    for(int i=0; i < (int)mList.size(); i++) { C_DBG_MSG("\t\tFormula[%d]: %s",i,mList[i].c_str()); }
    for(int i=0; i < j; i++) {
        stringstream ss;
        if (i == 0) {
            ss << setprecision(15) << mCompRes[i];
            mOutput += format(ss.str());
        }
        switch(type[i]) {
            case 0: {
                mOutput += " < ";
                if (mCompRes[i] < mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 1: {
                mOutput += " > ";
                if (mCompRes[i] > mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 2: {
                mOutput += " = ";
                if (mCompRes[i] == mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 3: {
                mOutput += " <> ";
                if (mCompRes[i] != mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 4: {
                mOutput += " <= ";
                if (mCompRes[i] <= mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 5: {
                mOutput += " >= ";
                if (mCompRes[i] >= mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
            case 6: {
                mOutput += " == ";
                if (mCompRes[i] == mCompRes[i+1]) { mCompare[i]=true; }
                else { mCompare[i]=false; }
                    } break;
        }
        ss.str(""); ss.clear();
        ss << setprecision(15) << mCompRes[i + 1];
        mOutput += format(ss.str());
    }
    bool tmpcmp = mCompare[0];
    for(int i=1; i < j; i++) {
        if (!tmpcmp) { break; }
        if (tmpcmp && mCompare[i]) { continue; }
        else { tmpcmp = false; }
    }
    if (tmpcmp) { mResult = 1; }
    else { mResult = 0; }
}
string Calc::result_s() { enum FunkiiCalcOptions_t nada = calc_default; return result_s(nada); }
string Calc::result_s(enum FunkiiCalcOptions_t Options) {
    if ((mError != CE_NADA) && !(Options & calc_noerror)) { return get_error_string(mError); }
    else {
        string res;
        if (mIsCompare) {
            if (Options & calc_formula) { res = mOutput; }
            if (!(Options & calc_noresult)) {
                if (Options & calc_formula) { res += " :: "; }
                if (Options & calc_numtruefalse) { res += (mResult == 1 ? "1" : "0"); }
                else { res += (mResult == 1 ? "true" : "false"); }
            }
        }
        else {
            stringstream ss;
            ss << setprecision(15) << mResult;
            if (Options & calc_formula) { res = mFormula; }
            if (!(Options & calc_noresult)) {
                if (Options & calc_formula) { res += " = "; }
                if (Options & calc_noformat) { res += ss.str(); }
                else { res += format(ss.str()); }
            }
        }
        return res;
    }
}
char *Calc::result_c_str() { enum FunkiiCalcOptions_t nada = calc_default; return result_c_str(nada); }
char *Calc::result_c_str(enum FunkiiCalcOptions_t Options) { return (char *)result_s(Options).c_str(); }
long double Calc::result_d() {
    if (mError == CE_NADA) { return mResult; }
    else { return 0; }
}
bool Calc::error() {
    if (mError == CE_NADA) { return false; }
    else { return true; }
}
string Calc::get_error() { return get_error_string(mError); }
string Calc::fibonacci() {
    C_DBG_START;
    if (mError == CE_NADA) {
        if (mResult > 1475 || mResult < 1475) { return get_error_string(CE_FIB_OB); }
        else {
            stringstream ss;
            ss << setprecision(15) << fib(floor(mResult));
            C_DBG_END;
            return ss.str();
        }
    }
    else { C_DBG_END; return get_error_string(mError); }
}
string Calc::tocelsius() {
    C_DBG_START;
    if (mError == CE_NADA) {
        string conv("(("); conv += this->result_s(); conv += "-32)*(5/9))";
        Calc x(conv); conv.clear();
        conv.assign(x.result_s()); conv += "°C";
        C_DBG_END;
        return conv;
    }
    else { C_DBG_END; return get_error_string(mError); }
}
string Calc::tofahrenheit() {
    C_DBG_START;
    if (mError == CE_NADA) {
        string conv("(("); conv += this->result_s(); conv += "*1.8)+32)";
        Calc x(conv); conv.clear();
        conv.assign(x.result_s()); conv += "°F";
        C_DBG_END;
        return conv;
    }
    else { C_DBG_END; return get_error_string(mError); }
}
string Calc::duration(int type = 0) {
    C_DBG_START;
    if (mError == CE_NADA) {
        stringstream ss;
        int t = (int)mResult;
        //1min              60 secs
        //1hr               3600 secs
        //1day              86400 secs
        //7days/1week       604800 secs
        //30days/1month     2592000 secs
        //52weeks/1yr       31449600 secs
        int n,h,m;
        double r,tt;
        const char *txt[] = { "sec", "min", "hr", "day", "wk", "mo", "yr" };
        if (t < 0) { ss << t << "secs"; }
        else {
            switch(type) {
                case 1: {
                            if (t == 0) { ss << "00:00:00"; }
                            else {
                                h = (int)floor((double)(t / 3600)); t -= (h * 3600);
                                m = (int)floor((double)(t / 60)); t -= (m * 60);
                                ss << (h < 10 ? "0" : "") << h << ":" ;
                                ss << (m < 10 ? "0" : "") << m << ":";
                                ss << (t < 10 ? "0" : "") << t;
                            }
                        } break;
                default: {
                            if (t == 0) { ss << "0secs"; }
                            else {
                                while(t > 0) {
                                    if (t >= 31449600) { tt = 31449600; n = 6; }
                                    else if (t >= 2592000) { tt = 2592000; n = 5; }
                                    else if (t >= 604800) { tt = 604800; n = 4; }
                                    else if (t >= 86400) { tt = 86400; n = 3; }
                                    else if (t >= 3600) { tt = 3600; n = 2; }
                                    else if (t >= 60) { tt = 60; n = 1; }
                                    else { tt = 1; n = 0; }
                                    r =  floor(t / tt);
                                    ss << (int)r << txt[n] << ((int)r != 1 ? "s" : "") << (n != 0 ? " " : "");
                                    t -= (int)(r * tt);
                                }
                            }
                         }
            }
        }
        C_DBG_END;
        return ss.str();
    }
    else { C_DBG_END; return get_error_string(mError); }
}
string Calc::format(string input) {
    C_DBG_START;
    string in(input);
    string e;
    int epos = in.find_last_of('e');
    if ( epos > 0 ) { e = in.substr(epos); in.erase(epos); }
    int found = in.find_last_of('.');
    if (found <= 0) { found = in.length(); }
    for (int i = found - 3 ; i > (in.at(0) == '-' ? 1 : 0); i=i-3 ) { in.insert(in.begin() + i,','); }
    if (epos > 0) { in.append(e); }
    C_DBG_END;
    return in;
}
bool Calc::IsValidNum(string n) {
    C_DBG_START;
    bool is_e = false;
    C_DBG_MSG("IsValidNum... in: %s",n.c_str());
    for (int i = 0; i < (int)n.length(); i++) {
        if (!((int)n.at(i) >= 48 && (int)n.at(i) <= 57) &&
            (int)n.at(i) != 46 &&
            !((int)n.at(i) == 45 && i==0)
            ) {
                if (is_e) {
                    if ((n.at(i) == '+' || n.at(i) == '-')) { is_e = false; }
                    else if (((int)n.at(i) >= 97 && (int)n.at(i) <= 102)) { is_e = false; }
                    else { C_DBG_END; return false; }
                }
                else {
                    if(n.at(i) == 'e') { is_e = true; }
                    else if ((int)n.at(i) >= 97 && (int)n.at(i) <= 102) { /* ...DO WANT HEX.. */ }
                    else { C_DBG_END; return false; }
                }
        }
    }
    C_DBG_END;
    return true;
}

int Calc::isFunc(string in) {
    C_DBG_START;
    int num = (int)((sizeof(func_array)/sizeof(char *)) - 1), ret=0;
    while (num >= 0) {
        if (in.compare(func_array[num]) == 0) { ret = (num + 1); break; }
        num--;
    }
    C_DBG_MSG("Return Func #%d",ret);
    C_DBG_END;
    return ret;
}
long double Calc::fib(long double n) {
    C_DBG_START;
    long double fx = 3, f1 = 1, f2 = 1,tmp;
    bool neg = false;
    if (n < 0) { n *= -1; neg = true; }
    if (n < 1476) {
        if (n == 0) { C_DBG_END; return 0; }
        else if (n == 1 || n == 2) { C_DBG_END; return ((neg && n==2) ? -1 : 1); }
        else {
            while (fx <= n) {
                tmp = f1 + f2;
                f1 = f2;
                f2 = tmp;
                fx++;
            }
            if (neg) { return (pow(-1,(n + 1)) * f2); }
            else { C_DBG_END; return f2; }
        }
    }
    else { C_DBG_END; return 0; }
}
long double Calc::calculate(string formula, int level = 0, int oper_func = 0) {
    C_DBG_START;
    bool skip = false;
    long double res=0, tmp;
    C_DBG_MSG("Formula: '%s' :\tLevel: %d :\tOper: %d",formula.c_str(),level,oper_func);
    if (IsValidNum(formula)) {
        C_DBG_MSG("IsValidNum");
        if(oper_func == 17) { res = bin2dec(formula); }
        else if(oper_func == 18) { res = oct2dec(formula); }
        else if(oper_func == 19) { res = hex2dec(formula); }
        else if(formula.compare("e") == 0) { C_DBG_MSG("return e"); res = EXP; }
        else { res = strtod(formula.c_str(),NULL); }
        skip = true;
    }
    else if (level > 4) {
        skip = true;
        if (formula.compare("pi") == 0) { C_DBG_MSG("return PI"); res = PI; }
        else { C_DBG_MSG("IsNothing"); res = strtod(formula.c_str(),NULL); }
    }
    if (!skip) {
        vector<string> oper, operations;
        oper.resize(1000); operations.resize(1000);
        string s;
        int oper_flags[1000], p = 0, o = 0, op = 0, is_f[1000];
        bool record, is_e = false, neg = false;
        oper_flags[0] = 0;
        is_f[0] = -1;
        C_DBG_MSG("\t\tBegin the FOR");
        for (int i = 0; i <= (int)formula.length(); i++) {
            record = false;
            char c = '\0';
            if (i == (int)formula.length()) { record = true; oper_flags[o] = level + 1; }
            else {
                c = formula.at(i);
                C_DBG_MSG("'%s':\tc: %c\tpos: %d\to : %d\tp : %d\top : %d\tlvl : %d ::: neg : %s\t is_e : %s",formula.c_str(),c,i,o,p,op,level,(neg?"true":"false"),(is_e?"true":"false"));
                if (o < 999) {
	                if (c == '(') {
	                    if (p == 0) {
	                        oper_flags[o] = 0;
	                        if (neg) {
	                            if (s.length() > 1) { record = true; }
	                            else { s.assign("(-1)*("); }
	                        }
	                        else if (!s.empty()) { record = true; }
	                    }
	                    p++;
	                    if (p > 1) { s.push_back(c); }
	                }
	                else if (c == ')') {
	                    p--;
	                    if (p==0) {
	                        if (neg) { s.push_back(c); neg = false; }
	                        record = true;
	                    }
	                    else { s.push_back(c); }
	                }
	                else if (p == 0) {
                        switch(c) {
                            case '>':
	                        case '<': i++;
	                        case '^':
	                        case '%':
	                        case '/':
                            case '*': oper[op] = c; op++; record = true; break;
	                        case '+': {
                                        if (is_e) { s.push_back(c); is_e = false; }
	                                    else { oper[op] = c; op++; record = true; }
                                    } break;
                            case '-': {
                                        if (is_e) { s.push_back(c); is_e = false; }
                                        else if (i == 0 || (op > 0 && (oper[op-1].at(0) == formula.at(i-1)))) { s.push_back(c); neg = true; }
	                                    else if (!s.empty() &&
                                                (s.at(s.length() - 1) == '^' || s.at(s.length() - 1) == '%' ||
                                                s.at(s.length() - 1) == '/' || s.at(s.length() - 1) == '*' ||
                                                s.at(s.length() - 1) == '<' || s.at(s.length() - 1) == '>'
                                                ))  { s.push_back(c); }
                                        else { oper[op] = c; op++; record = true; }
                                    } break;
                            default: s.push_back(c); break;
                        }
	                }
	                else { s.push_back(c); }
	            }
	            else { s.push_back(c); }
            }
            if (record) {
                if (s.empty()) { continue; }
                C_DBG_MSG("RECORD: '%s'",s.c_str());
                bool juntar = false;
                if (op > 0 && o > 0) {
                    if (level == 0) {
                        if (oper[op-1].at(0) != '+' && oper[op-1].at(0) != '-') { juntar = true; }
                    }
                    else if (level == 1) {
                        if (oper[op-1].at(0) != '*') { juntar = true; }
                    }
                    else if (level == 2) {
                        if (oper[op-1].at(0) != '/' && oper[op-1].at(0) != '%') { juntar = true; }
                    }
                    else if (level == 3) {
                        if (oper[op-1].at(0) != '^') { juntar = true; }
                    }
                    else if (level == 4) {
                        if (oper[op-1].at(0) != '<' && oper[op-1].at(0) != '>') { juntar = true; }
                    }
                }
                C_DBG_MSG("op: %d\to: %d\tlevel: %d\tjuntar: %s",(op-1),o,level,(juntar ? "true" : "false"));
                if (juntar) {
                    if (op > o) {
                        op--;
                        if (oper[op].at(0) == '<' || oper[op].at(0) == '>') {
                            //we need to check if it's a bitshift to prepend the operations 1 << 2 << 3 should be (1 << 2) << 3
                            if(oper[(op-1)].at(0) == '<' || oper[(op-1)].at(0) == '>') {
                                o--; op--;
                                string tmpops;
                                if (is_f[o] > 0) { tmpops = func_array[(is_f[o] - 1)]; is_f[o] = -1; }
                                tmpops += "("; tmpops += operations[o]; tmpops += ")";
                                if (oper[op].at(0) == '<' || oper[op].at(0) == '>') { tmpops += oper[op]; }
                                tmpops += oper[op];
                                tmpops += "("; tmpops += s; tmpops += ")";
                                operations[o] = tmpops;
                                oper_flags[o] = level + 1;
                                oper[op] = oper[(op + 1)];
                                s.clear(); op++; o++;
                                C_DBG_MSG("\t\tFound something![op: %d][o: %d].. %s %s %s [%s] :: %s",op,o,operations[o].c_str(),oper[op].c_str(),operations[(o+1)].c_str(),s.c_str(),tmpops.c_str());
                                continue;
                            }
                            else { s.push_back(c); }
                        }
                        s.push_back(c); continue;
                    }
                    else {
                        o--; op--;
                        string tmpops;
                        if (is_f[o] > 0) { tmpops = func_array[(is_f[o] - 1)]; is_f[o] = -1; }
                        tmpops += "("; tmpops += operations[o]; tmpops += ")";
                        if (oper[op].at(0) == '<' || oper[op].at(0) == '>') { tmpops += oper[op]; }
                        tmpops += oper[op];
                        tmpops += "("; tmpops += s; tmpops += ")";
                        operations[o] = tmpops;
                        oper_flags[o] = level + 1;
                    }
                }
                else { operations[o].assign(s); }
                int is_f_tmp = isFunc(operations[o]);
                C_DBG_MSG("\tOperation :: %s : %d ", operations[o].c_str(), is_f_tmp);
                if (is_f_tmp > 0) { is_f[o] = is_f_tmp; is_f[(o + 1)] = -1; } // o--;
                else { o++; }
                s.clear(); neg = false;
            }
        }
        C_DBG_MSG("\t\tEND the FOR");
        res = calculate(operations[0], oper_flags[0], is_f[0]);
        for (int i = 1,j = 0; i < o && j < op; i++,j++) {
            tmp = calculate(operations[i],oper_flags[i],is_f[i]);
            switch (oper[j].at(0)) {
                case '-': res -= tmp; break;
                case '+': res += tmp; break;
                case '*': res = res * tmp; break;
                case '%':
                case '/': {
                            if (tmp == 0) { mError = CE_DIV0; res = -1; i=o; } //ERROR: can't divide by 0
                            else {
                                if (oper[j].at(0) == '%') { res = fmod(res,tmp); }
                                else { res = res / tmp; }
                            }
                        } break;
                case '^': res = powl(res,tmp); break;
                case '>':
                case '<': {
                            if( (res - floor(res)) == 0 ) {
                                if (oper[j].at(0) == '<') { res = (int)res << (int)tmp; }
                                else { res = (int)res >> (int)tmp; }
                            }
                            else { mError = CE_INT_BITSHIFT; res = -1; i=o; }
                        } break;
            }
        }
    }
    if (oper_func > 0) {
        switch (oper_func) {
            case 1: { res = sqrt(res); } break;
            case 2: { res = floor(res); } break;
            case 3: { res = ceil(res); } break;
            case 4: { res = sin(res); } break;
            case 5: { res = cos(res); } break;
            case 6: { res = tan(res); } break;
            case 7: { res = asin(res); } break;
            case 8: { res = acos(res); } break;
            case 9: { res = atan(res); } break;
            case 10: { res = sinh(res); } break;
            case 11: { res = cosh(res); } break;
            case 12: { res = tanh(res); } break;
            case 13: { res = log(res); } break;
            case 14: { res = log10(res); } break;
            case 15:
            case 16: { res = fabs(res); } break;
            case 20: {
                    if( (res - floor(res)) >= 0.5 ) { res = ceil(res); }
                    else { res = floor(res); }
                     } break;
            case 21: { res = factorial(res); }break;
            default: break;
        }
        if (errno) {
            switch(errno) {
                case EDOM: mError = CE_EDOM; break;
                case ERANGE: mError = CE_ERANGE; break;
                default: mError = CE_EPIC;
            }
            res = 0;
        }
    }
    C_DBG_MSG("return %LG",res);
    C_DBG_END;
    return res;
}
long double Calc::bin2dec(string num) {
    long double ret = 0;
    for(int z = num.length() - 1; z >= 0; z--) {
        if (num.at(z) == '0') { continue; }
        else if (num.at(z) == '1') { ret += powl((long double)2, (long double)((num.length() - 1) - z)); }
        else {
            mError = CE_BIN;
            ret = 0;
            break;
        }
    }
    C_DBG_MSG("BIN to DEC :: '%s' :: '%LG'",num.c_str(),ret);
    return ret;
}
long double Calc::oct2dec(string num) {
    long double ret = 0;
    int oct;
    C_DBG_MSG("OCT to DEC :: %s",num.c_str());
    for(int z = num.length() - 1; z >= 0; z--) {
        switch(num.at(z)) {
            case '0': { oct = 0; } break;
            case '1': { oct = 1; } break;
            case '2': { oct = 2; } break;
            case '3': { oct = 3; } break;
            case '4': { oct = 4; } break;
            case '5': { oct = 5; } break;
            case '6': { oct = 6; } break;
            case '7': { oct = 7; } break;
            default: { mError = CE_OCT; ret = 0; oct = -1; }
        }
        if (oct == -1) { break; }
        else { ret += oct * powl((long double)8,(long double)((num.length() - 1) - z)); }
    }
    return ret;
}
long double Calc::hex2dec(string num) {
    long double ret = 0;
    int nhex;
    C_DBG_MSG("HEX to DEC :: %s",num.c_str());
    for( int z = num.length() - 1; z >= 0; z--) {
        switch(num.at(z)) {
            case '0': { nhex = 0; } break;
            case '1': { nhex = 1; } break;
            case '2': { nhex = 2; } break;
            case '3': { nhex = 3; } break;
            case '4': { nhex = 4; } break;
            case '5': { nhex = 5; } break;
            case '6': { nhex = 6; } break;
            case '7': { nhex = 7; } break;
            case '8': { nhex = 8; } break;
            case '9': { nhex = 9; } break;
            case 'a': { nhex = 10; } break;
            case 'b': { nhex = 11; } break;
            case 'c': { nhex = 12; } break;
            case 'd': { nhex = 13; } break;
            case 'e': { nhex = 14; } break;
            case 'f': { nhex = 15; } break;
            default: { mError = CE_HEX; ret = 0; nhex = -1; }
        }
        if (nhex == -1) { break; }
        else { ret += nhex * powl((long double)16,(long double)((num.length() - 1) - z)); }
    }
    return ret;
}
long double Calc::factorial(long double num) {
    long double ret=0;
    if (num < 0) { mError = CE_FACT_OB; }
    else if( (num - floor(num)) > 0 ) { mError = CE_FACT_OB; }
    else { ret = 1; for (int i = 0; i <= num; i++) { ret*=i; } }
    return ret;
}
string Calc::get_error_string(enum FunkiiCalcErrors_t n) {
    string e;
    if (n != CE_NADA) {
        e = "[CALC] Error: ";
        switch((int)n) {
            case CE_EMPTY:              e += "wut? no formula?"; break;
            case CE_SYNTAX:             e += "Syntax Error!! l2syntax~!"; break;
            case CE_SYNTAX_VARS:        e += "Syntax Error!! you suck at assigning Vars!"; break;
            case CE_SYN_VARS_INFLOOP:   e += "Possible Infinite Loop While assigning Vars!"; break;
            case CE_SYN_PAR:            e += "Parentheses Error!! you suck at punctuation!"; break;
            case CE_SYN_EMPTY_PAR:      e += "Syntax Error!! l2fillparenthesis!"; break;
            case CE_SYN_INVALIDCHAR:    e += "Syntax Error!! you suck chars!"; break;
            case CE_DIV0:               e += "Yeah... i can't divide by 0.. :("; break;
            case CE_EDOM:               e += "Domain Error... l2calc!!"; break;
            case CE_ERANGE:             e += "Out of Range... damn you basterd l2stayinrange!!"; break;
            case CE_FIB_OB:             e += "Out of Bounds, cannot Fibonacci!"; break;
            case CE_BIN:                e += "Dude l2binary . . ."; break;
            case CE_OCT:                e += "srsly man l2octal . . ."; break;
            case CE_HEX:                e += "l2hex . . . *sigh* "; break;
            case CE_FACT_OB:            e += "Can only factorial POSITIVE integers... "; break;
            case CE_EPIC:               e += "oo noes Epic error... l2noterror!!"; break;
            case CE_INT_BITSHIFT:       e += "Can only shift int! l2bitshift~!"; break;
            default:                    e += "Epic Error!";
        }
    }
    return e;
}
#endif
