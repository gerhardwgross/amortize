/* **************************************************************************
// amortize -- utility program written by Gerhard Gross, 1999
//
// THIS SOFTWARE IS PROVIDED BY GERHARD W. GROSS ``AS IS'' AND ANY
// EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GERHARD W. GROSS 
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//
// PERMISSION TO USE, COPY, MODIFY, AND DISTRIBUTE THIS SOFTWARE AND ITS
// DOCUMENTATION FOR ANY PURPOSE AND WITHOUT FEE IS HEREBY GRANTED,
// PROVIDED THAT THE ABOVE DISCLAIMER NOTICE, THIS PERMISSION NOTICE,
// AND THE FOLLOWING ATTRIBUTION NOTICE APPEAR IN ALL SOURCE CODE FILES
// AND IN SUPPORTING DOCUMENTATION AND THAT GERHARD W. GROSS BE
// GIVEN ATTRIBUTION AS THE MAIN AUTHOR OF THIS PROGRAM IN THE FORM
// OF A TEXTUAL MESSAGE AT PROGRAM STARTUP OR IN THE DISPLAY OF A
// USAGE MESSAGE, OR IN DOCUMENTATION (ONLINE OR TEXTUAL) PROVIDED--
// WITH THIS PROGRAM.
//
// ALL OR PARTS OF THIS CODE WERE WRITTEN BY GERHARD W. GROSS, 1999.
//
//
// Function: Generates amortization tables and summary
//
// Usage:    amortize [-f] LoanAmt IntPcnt LoanPeriod [ExtraPymts ExPmStartYr]
//
// Description:
//
//           Calculates amortization tables and summary information using
//           standard amortization formula (interest compounded monthly). 
//
// *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <string.h>

using namespace std;

/***************************************************************************
Global variables.
***************************************************************************/

static int g_printFullSchedule  = false;

/***************************************************************************
Utility methods.
***************************************************************************/

double RoundToCent(double num)
{
    double intPart;

    modf(num * 100, &intPart) >= 0.5 ? intPart++ : 0;
    return intPart / 100.0;
}

void PrintColumnHeaders()
{
    cout <<             "Month"
        << setw(12) << "Principal"
        << setw(12) << "----> accum"
        << setw(12) << "Interest"
        << setw(12) << "----> accum"
        << setw(12) << "Balance"
        << '\n';
}

/***************************************************************************
This function removes the second character from the passed string.  This
is called from deal_with_options() to remove the special purpose slash
character.
***************************************************************************/

void shift(char *str)
{
    int i, len;
    len = strlen(str);

    for(i = 1; i < len; i++)
        str[i] = str[i + 1];
}

/***************************************************************************
This function checks all args to see if they begin with a hyphen.
If so the necessary flags are set.  argc and argv[] are adjusted
accordingly, set back to the condition of the option not having been
supplied at the com line (i.e. all except the first argv[] ptr are
bumped back in the array).
***************************************************************************/

int deal_with_options(int arrgc, char *arrgv[])
{
    long i, j, num_opts;

    for(j = 1; j < arrgc; j++)
    {
        // If encounter a pipe symbol, '|', args handled by this app are done
        if(*arrgv[j] == '|')
            arrgc = j;  // Set num args for this app to current loop counter value

        if(*arrgv[j] == '-')
        {
            if(*(arrgv[j] + 1) == '/')
            {
                // Remove the backslash for option processing then go on to
                // next command line arg. Succeeding with '/' signals that
                // the hyphen is not to be interpreted as a cmd line option.
                shift(arrgv[j]);
            }
            else
            {
                num_opts = strlen(arrgv[j]) - 1;
                for(i = 1; i <= num_opts; i++)
                {
                    switch(*(arrgv[j] + i))
                    {
                    case 'f':
                        g_printFullSchedule = true;
                        break;
                        //case 'w':
                        //    // This option must be succeeded with an integer and
                        //    // then a space. Convert string integer to numeric value.
                        //    g_numColWidth = _wtoi(arrgv[j] + i + 1);

                        //    // Error check
                        //    if (g_numColWidth < 1)
                        //        g_numColWidth = BYTES_DEF_NUM_COLS;

                        //    // Now move loop counter to end of this option
                        //    i = num_opts;
                        //    break;
                    default:
                        fprintf(stderr, "Invalid option");
                        break;
                    }
                }

                for(i = j; i < arrgc - 1; i++)
                    arrgv[i] = arrgv[i + 1];

                arrgc--;
                j--;
            }
        }
    }

    return arrgc;
}

// *************************************************************************/
// Function Definitions
// *************************************************************************/

void GenerateReport(double price,
                    double annInt,
                    double numMonths,
                    double extraPymnts,
                    long   startMon)
{
    double balance      = price;
    double monInt       = annInt / (100.0 * 12.0);
    double monPmnt      = 0.0;
    double monPrincipal = 0.0;
    double intAmt       = 0.0;
    double totPrincipal = 0.0;
    double totInterest  = 0.0;
    int monCntr         = 1;
    int yearCntr        = 1;
    int i, j, startYr, limit;

    // Set cout params
    cout.setf(ios::fixed, ios::floatfield);
    cout.precision(2);

    // Run amortization formula
    monPmnt = price * ( monInt / (1.0 - pow(1.0 + monInt, -numMonths)) );

    // Are we printing full schedule or just summary
    limit = g_printFullSchedule ? 2 : 1;

    // Run through payment loop twice.  First time gather and print summary data. Second time print
    // out payment schedule.
    for (j = 0; j < limit; j++)
    {
        monCntr = yearCntr = 1;
        totPrincipal = totInterest = 0.0;
        balance = price;

        for (i = 1; RoundToCent(balance) > 0.0; i++)
        {
            intAmt = balance * monInt;

            if ((balance + intAmt) < (monPmnt + extraPymnts))
            {
                monPrincipal  = balance;
                balance       = 0.0;
            }
            else
            {
                monPrincipal  = monPmnt - intAmt;
                balance       -= monPrincipal;
                if (yearCntr*12 + monCntr >= startMon)
                {
                    monPrincipal  += extraPymnts;
                    balance       -= extraPymnts;
                }
            }

            totPrincipal  += monPrincipal;
            totInterest   += intAmt;

            if (j == 1)
            {
                cout << setw(5)  << monCntr
                    << setw(12) << RoundToCent(monPrincipal)
                    << setw(12) << totPrincipal
                    << setw(12) << RoundToCent(intAmt)
                    << setw(12) << totInterest
                    << setw(12) << RoundToCent(balance)
                    << '\n';
            }
            if (!(i % 12))
            {
                if (j == 1)
                {
                    cout << endl;
                    PrintColumnHeaders();
                    cout << "-------------------------------------------------------------------";
                    cout << " End Year " << yearCntr << endl;
                }
                monCntr = 1;
                yearCntr++;
            }
            else {
                monCntr++;
            }
        }

        if (j == 0)
        {
            startYr = startMon / 12;

            cout << "\n\n";
            cout << setw(40) << "Amortization Schedule" << "\n";
            cout << setw(40) << "---------------------" << "\n\n\n";
            cout << "Loan amount\t\t= " << price << endl;
            cout.precision(3);
            cout << "Annual interest\t\t= " << annInt << endl;
            cout.precision(2);
            cout << "Loan period (years)\t= " << numMonths / 12.0 << endl;
            cout << "Extra monthly payments\t= " << extraPymnts << endl;
            cout << "      starting in month\t= " << startMon;
            cout << "   (Year " << startYr << ", Month " << startMon-startYr*12 << ")\n\n";

            cout << "Monthly Payments\t= " << RoundToCent(monPmnt);
            if (extraPymnts > 0.0)
            {
                cout << "\t(not incl extra pymnts)\n";
                cout << "Total Monthly Payments\t= " << RoundToCent(monPmnt + extraPymnts)
                    << "\t(incl extra pymnts)\n\n";
            }
            else
                cout << "\n\n";

            cout << "Last payment on\t\t: Year " << --yearCntr
                << ", Month " << --monCntr << "\n\n";

            cout << "Total Principal Paid\t= " << RoundToCent(totPrincipal) << endl;
            cout << "Total Interest Paid\t= " << RoundToCent(totInterest) << endl;
            cout << "Total Paid\t\t= " << RoundToCent(totPrincipal + totInterest)
                << "\n\n\n";

            if (g_printFullSchedule)
                PrintColumnHeaders();

            cout << "*******************************************************************\n\n";
        }
    }
}

void PrintMsg(int selector)
{
    fflush(stdout);

    if (selector != 0)
        fprintf(stderr, "\nError %d - ", selector);

    switch (selector)
    {
    case 1:
        fprintf(stderr, "Incorrect number of arguments.\n");
        break;
    case 2:
        fprintf(stderr, "If start month is 0 extra payments must be 0.\n");
        break;
    case 3:
        fprintf(stderr, "Start month must be less than or equal to loan period.\n");
        break;
    case 4:
        fprintf(stderr, "If extra payments is 0 start month must be 0.\n");
        break;
    }

    fprintf(stderr,
        "\n Usage:\n\n     amortize [-f] LoanAmt IntPcnt LoanPeriod [ExtraPymts ExPmStartMo]\n\n"
        "  Generates amortization table and summary info based on standard\n"
        "  amortization formula (interest compounded monthly).\n\n"
        "  -f          - print full monthly payment schedule for all years of loan\n"
        "  LoanAmt     - initial amount of loan in dollars (decimal OK, no commas)\n"
        "  IntPcnt     - annual interest rate as a percent (decimal OK)\n"
        "  LoanPeriod  - loan period in years (decimal OK)\n"
        "  ExtraPymts  - amount of extra monthly payments (decimal OK, no commas)\n"
        "  ExPmStartMo - Month in which to start extra payments (enter integer,\n"
        "                from 1 up to number of months in loan period; e.g. 180\n"
        "                for 15 year loan.)\n\n     ****** Gerhard W. Gross ******\n\n"
        );

    exit (1);
}

// *************************************************************************/
// Main
// *************************************************************************/

int main(int argc, char* argv[])
{
    double price;
    double annInt;
    double numMonths;
    double extraPymnts  = 0;
    long   startMon     = 0;

    argc = deal_with_options(argc, argv);

    if (argc == 1)
        PrintMsg(0);

    if (argc != 4 && argc != 6)
        PrintMsg(1);    // Wrong number of args

    price        = strtod(argv[1], NULL);
    annInt       = strtod(argv[2], NULL);
    numMonths    = strtod(argv[3], NULL) * 12;

    if (argc == 6)
    {
        extraPymnts  = strtod(argv[4], NULL);
        startMon     = strtol(argv[5], NULL, 10);

        if (extraPymnts != 0.0 && startMon == 0)
            PrintMsg(2);

        if (startMon > (long)numMonths)
            PrintMsg(3);

        if (extraPymnts == 0.0 && startMon != 0)
            PrintMsg(4);
    }

    GenerateReport(price, annInt, numMonths, extraPymnts, startMon);

    return 0;
}
