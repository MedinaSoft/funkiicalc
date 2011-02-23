
//uncomment next line to enable DEBUG... prepare to be flooded
//#define CALC_DEBUG
#include "../src/calc.h"
int main() {

    cout << "In order to understand this output... please review the source code\n\n";
    /**
    **  This sample code should help you understand the usage, which is very simple
    **  below you can see i declared 3 strings with formulas.
    **      - Formula 1: is a simple multiplication
    **      - Formula 2: has an Error (can you see it?)
    **      - Formula 3: is an example of a formula with defined Vars
    */
    string f1("6.47*19.5"), f2("(((389,945.55 * 0.50) * 0.25) * 0.10");
    string f3("a=12,b=32;sqrt(a^2 + b^2)"), f4("10 + 20 * 50 /2 > 10*PI");

    /**
    **  First off we create an instance of the Calc class with the formula we want evaluated.
    */
    cout << "Formula string: " << f1 << "\n\n\t";
    Calc c(f1);

    /**
    **  And you are done, the result has already been calculated.. now you just need to decide
    **  how you want the output... you have 3 options
    **      - as a string       :   c.result_s();
    **      - as a char *       :   c.result_c_str();
    **      - as a long double  :   c.result_d();
    */
    cout << "result_s :     \t" << c.result_s() << "\n\t";
    cout << "result_c_str : \t" << c.result_c_str(calc_formula) << "\n\t";
    cout << "result_d :     \t" << c.result_d() << endl;
    /**
    **  As you can see in the 'result_c_str()' There are some flags you can set to control the output
    **  this flags are only valid in 'result_s()' and 'result_c_str()'
    **      calc_formula        -   Display the formula with the results
    **      calc_noerror        -   Do not show error msg
    **      calc_noformat       -   Do not Comma Separate (i.e from 1000 to 1,000)
    **      calc_numtruefalse   -   Represent True/False as Number not text
    */


    /**
    **  Next lets reuse the Calc instance and make it evaluate a new formula
    */
    cout << "\n\nFormula string: " << f2 << "\n\n\t";
    c.assign(f2);

    /**
    **  Formula 2 has an error remember? let's see what the output is...
    */
    cout << c.result_s() << "\n\t";

    /**
    **  we could also check for errors like this
    */
    if (c.error()) {
        /* oo no an error :(.. lets do something about it.. */
        /**
        **  we could also get the error string with get_error()
        */
        //cout << c.get_error() << "\n\t";
        /* Parenthesis error ehh?.. well lets fix that and try again */
        f2 += ")";
        cout << "\n\nFormula string (fixed): " << f2 << "\n\n\t";
        c.assign(f2);
        cout << c.result_s(calc_formula) << endl;
    }
    /**
    **  Next lets assign that formula with the variables, lets see what we get
    */
    cout << "\n\nFormula string: " << f3 << "\n\n\t";
    c.assign(f3);
    cout << c.result_s(calc_formula) << endl;

    /**
    **  Last but not least .. Lets see the comparison output
    */

    cout << "\n\nFormula string: " << f4 << "\n\n\t";
    c.assign(f4);
    cout << c.result_s() << "\n\t";
    if (c.result_d() == 1) { cout << "It was true!!"; }
    else { cout << "It was false :("; }


    return 0;
}
