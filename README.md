# amortize
Windows/Linux command line tool to compute and print amortization schedules.

Calculates amortization tables and summary information using standard amortization formula (interest compounded monthly).

Usage:    amortize [-f] LoanAmt IntPcnt LoanPeriod [ExtraPymts ExPmStartYr]

  LoanAmt     - initial amount of loan in dollars (decimal OK, no commas)
  IntPcnt     - annual interest rate as a percent (decimal OK)
  LoanPeriod  - loan period in years (decimal OK)
  ExtraPymts  - amount of extra monthly payments (decimal OK, no commas)
  ExPmStartMo - Month in which to start extra payments (enter integer, from 1 up to number of months in loan period; e.g. 180 for 15 year loan.)
  -f            print full monthly payment schedule for all years of loan
