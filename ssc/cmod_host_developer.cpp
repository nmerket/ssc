/*******************************************************************************************************
*  Copyright 2017 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  (�Alliance�) under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as �System Advisor Model� or �SAM�. Except
*  to comply with the foregoing, the terms �System Advisor Model�, �SAM�, or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/

#include "common_financial.h"
#include "lib_financial.h"
using namespace libfin;
#include <sstream>

#ifndef WIN32
#include <float.h>
#endif

static var_info _cm_vtab_host_developer[] = {

/*   VARTYPE           DATATYPE         NAME                                      LABEL                                                            UNITS              META                      GROUP                       REQUIRED_IF                 CONSTRAINTS                      UI_HINTS*/
// -------------------
// Host specific inputs and outputs
	{ SSC_INPUT,        SSC_ARRAY,       "annual_energy_value",             "Host energy value",                       "$",            "",                      "Host",      "*",                       "",                                         "" },
	{ SSC_INPUT,        SSC_ARRAY,       "elec_cost_with_system",             "Host energy bill with system",                       "$",            "",                      "Host",      "*",                       "",                                         "" },
	{ SSC_INPUT,        SSC_ARRAY,       "elec_cost_without_system",             "Host energy bill without system",                       "$",            "",                      "Host",      "*",                       "",                                         "" },
	{ SSC_INPUT,        SSC_NUMBER,       "host_real_discount_rate",             "Host real discount rate",                       "%",            "",                      "Host",      "*",                       "",                                         "" },

	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_agreement_cost",      "Host agreement cost",                  "$",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_after_tax_net_equity_cost_flow",        "Host after-tax annual costs",           "$",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_after_tax_cash_flow",                   "Host after-tax cash flow",                      "$",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_payback_with_expenses",                 "Host payback with expenses",                    "$",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_cumulative_payback_with_expenses",      "Host cumulative payback with expenses",         "$",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,        SSC_NUMBER,     "npv",                      "Host net present value",				   "$",            "",                      "Cash Flow",      "*",                       "",                                         "" },

	{ SSC_OUTPUT,        SSC_ARRAY,      "cf_nte",      "Host indifference point by year",         "cents/kWh",            "",                      "Cash Flow",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "host_nominal_discount_rate",                  "Host nominal discount rate",            "%",     "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,        SSC_NUMBER,     "year1_nte",                "Host indifference point in Year 1",                          "cents/kWh",    "",                      "Cash Flow",      "*",                       "",                                         "" },
	{ SSC_OUTPUT,        SSC_NUMBER,     "lnte_real",                "Host indifference point real levelized value",                          "cents/kWh",    "",                      "Cash Flow",      "*",                       "",                                         "" },
	{ SSC_OUTPUT,        SSC_NUMBER,     "lnte_nom",                 "Host indifference point nominal levelized value",                       "cents/kWh",    "",                      "Cash Flow",      "*",                       "",                                         "" },
// End Host specific inputs and outputs
// -------------------

	{ SSC_INPUT, SSC_ARRAY, "gen", "Power generated by renewable resource", "kW", "", "", "*", "", "" },


	{ SSC_INPUT, SSC_ARRAY, "degradation", "Annual energy degradation", "", "", "System Output", "*", "", "" },
	{ SSC_INPUT,        SSC_NUMBER,     "system_capacity",			              "System nameplate capacity",		                               "kW",                "",                        "System Output",             "*",					   "MIN=1e-3",                      "" },
    
	/* return on equity from SAM for India */
	{ SSC_INPUT, SSC_ARRAY, "roe_input", "Return on equity", "", "", "Return on Equity", "?=20", "", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_return_on_equity", "Return on equity", "$/kWh", "", "Return on Equity", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_return_on_equity_input", "Return on equity input", "%", "", "Return on Equity", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_return_on_equity_dollars", "Return on equity dollars", "$", "", "Return on Equity", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_lcog_costs", "Total LCOG costs", "$", "", "Return on Equity", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog_om", "LCOG O and M", "cents/kWh", "", "Return on Equity", "*", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog_depr", "LCOG depreciation", "cents/kWh", "", "Return on Equity", "*", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog_loan_int", "LCOG loan interest", "cents/kWh", "", "Return on Equity", "*", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog_wc_int", "LCOG working capital interest", "cents/kWh", "", "Return on Equity", "*", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog_roe", "LCOG return on equity", "cents/kWh", "", "Return on Equity", "*", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "lcog", "LCOG Levelized cost of generation", "cents/kWh", "", "Return on Equity", "*", "", "" },

	/*loan moratorium from Sara for India Documentation\India\Loan Moratorum
	assumptions:
	1) moratorium period begins at beginning of loan term 
	2) moratorium affects principal payment and not interest
	3) loan term remains the same
	4) payments increase after moratorium period
	*/
	{ SSC_INPUT, SSC_NUMBER, "loan_moratorium", "Loan moratorium period", "years", "", "Moratorium", "?=0", "INTEGER,MIN=0", "" },


/* Recapitalization */                                                            														           
	{ SSC_INOUT,        SSC_NUMBER,     "system_use_recapitalization",	          "Recapitalization expenses",	                                   "0/1",               "0=None,1=Recapitalize",   "Recapitalization",          "?=0",					   "INTEGER,MIN=0",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "system_recapitalization_cost",	          "Recapitalization cost",	                                       "$",                 "",                        "Recapitalization",          "?=0",					   "",                              "" },
	{ SSC_INPUT,        SSC_NUMBER,     "system_recapitalization_escalation",     "Recapitalization escalation (above inflation)",	               "%",	                "",					       "Recapitalization",          "?=0",                     "MIN=0,MAX=100",      		    "" },
	{ SSC_INPUT,        SSC_ARRAY,      "system_lifetime_recapitalize",		      "Recapitalization boolean",	                                   "",                  "",                        "Recapitalization",          "?=0",					   "",                              "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_recapitalization",	                  "Recapitalization operating expense",	                           "$",                 "",                        "Recapitalization",          "*",					   "LENGTH_EQUAL=cf_length",        "" },
                                                                                  															       
/* Dispatch */                                                                    															       
	{ SSC_INPUT,        SSC_NUMBER,     "system_use_lifetime_output",		      "Lifetime hourly system outputs",	                               "0/1",                         "0=hourly first year,1=hourly lifetime",                      "Time of Delivery",             "*",						   "INTEGER,MIN=0",                 "" },

	// dispatch update TODO - remove SO output label below after consildated with CSP
	{ SSC_INPUT, SSC_NUMBER, "ppa_multiplier_model", "PPA multiplier model", "0/1", "0=diurnal,1=timestep", "Time of Delivery", "?=0", "INTEGER,MIN=0", "" },
	{ SSC_INPUT, SSC_ARRAY, "dispatch_factors_ts", "Dispatch payment factor array", "", "", "Time of Delivery", "ppa_multiplier_model=1", "", "" },

	{ SSC_OUTPUT, SSC_ARRAY, "ppa_multipliers", "TOD factors", "", "", "Time of Delivery", "*", "", "" },



	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor1",		                  "TOD factor for period 1",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor2",		                  "TOD factor for period 2",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor3",		                  "TOD factor for period 3",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor4",		                  "TOD factor for period 4",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor5",		                  "TOD factor for period 5",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor6",		                  "TOD factor for period 6",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor7",		                  "TOD factor for period 7",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor8",		                  "TOD factor for period 8",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dispatch_factor9",		                  "TOD factor for period 9",	                                   "",   "",                          "Time of Delivery",             "ppa_multiplier_model=0",						   "",                 "" },
	{ SSC_INPUT,        SSC_MATRIX,     "dispatch_sched_weekday",                 "Diurnal weekday TOD periods",                                   "1..9", "12 x 24 matrix",    "Time of Delivery", "ppa_multiplier_model=0", "", "" },
	{ SSC_INPUT,        SSC_MATRIX,     "dispatch_sched_weekend",                 "Diurnal weekend TOD periods",                                   "1..9", "12 x 24 matrix",    "Time of Delivery", "ppa_multiplier_model=0", "", "" },
                                                                                  																   
                                                                                  																   
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_jan",                     "Energy produced by the system in January",                      "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_jan",                        "Revenue from the system in January",                            "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_feb",                     "Energy produced by the system in February",                     "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_feb",                        "Revenue from the system in February",                           "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_mar",                     "Energy produced by the system in March",                        "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_mar",                        "Revenue from the system in March",                              "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_apr",                     "Energy produced by the system in April",                        "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_apr",                        "Revenue from the system in April",                              "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_may",                     "Energy produced by the system in May",                          "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_may",                        "Revenue from the system in May",                                "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_jun",                     "Energy produced by the system in June",                         "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_jun",                        "Revenue from the system in June",                               "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_jul",                     "Energy produced by the system in July",                         "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_jul",                        "Revenue from the system in July",                               "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_aug",                     "Energy produced by the system in August",                       "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_aug",                        "Revenue from the system in August",                             "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_sep",                     "Energy produced by the system in September",                    "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_sep",                        "Revenue from the system in September",                          "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_oct",                     "Energy produced by the system in October",                      "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_oct",                        "Revenue from the system in October",                            "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_nov",                     "Energy produced by the system in November",                     "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_nov",                        "Revenue from the system in November",                           "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_energy_net_dec",                     "Energy produced by the system in December",                     "kWh", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
    { SSC_OUTPUT,       SSC_ARRAY,       "cf_revenue_dec",                        "Revenue from the system in December",                           "$", "", "Cash Flow Revenue", "*", "LENGTH_EQUAL=cf_length", "" },
                                                                                  
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch1",	              "Energy produced by the system in TOD period 1",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch1",		                  "Revenue from the system in TOD period 1",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch2",	                  "Energy produced by the system in TOD period 2",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch2",		                  "Revenue from the system in TOD period 2",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch3",	                  "Energy produced by the system in TOD period 3",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch3",		                  "Revenue from the system in TOD period 3",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch4",	                  "Energy produced by the system in TOD period 4",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch4",		                  "Revenue from the system in TOD period 4",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch5",	                  "Energy produced by the system in TOD period 5",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch5",		                  "Revenue from the system in TOD period 5",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch6",	                  "Energy produced by the system in TOD period 6",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch6",		                  "Revenue from the system in TOD period 6",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch7",	                  "Energy produced by the system in TOD period 7",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch7",		                  "Revenue from the system in TOD period 7",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch8",	                  "Energy produced by the system in TOD period 8",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch8",		                  "Revenue from the system in TOD period 8",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_dispatch9",	                  "Energy produced by the system in TOD period 9",	               "kWh",   "",  "Cash Flow Revenue",             "ppa_multiplier_model=0",						   "LENGTH_EQUAL=cf_length",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_dispatch9",		                  "Revenue from the system in TOD period 9",	                   "$",   "",      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "LENGTH_EQUAL=cf_length",                 "" },
                                                                                  
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch1",            "First year revenue from the system in TOD period 1",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT, SSC_NUMBER, "firstyear_revenue_dispatch2", "First year revenue from the system in TOD period 2", "$", "", "Cash Flow Revenue", "ppa_multiplier_model=0", "", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "firstyear_revenue_dispatch3", "First year revenue from the system in TOD period 3", "$", "", "Cash Flow Revenue", "ppa_multiplier_model=0", "", "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch4",            "First year revenue from the system in TOD period 4",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch5",            "First year revenue from the system in TOD period 5",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch6",            "First year revenue from the system in TOD period 6",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch7",            "First year revenue from the system in TOD period 7",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch8",            "First year revenue from the system in TOD period 8",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_revenue_dispatch9",            "First year revenue from the system in TOD period 9",            "$",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
                                                                                                                                                   
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch1",             "First year energy from the system in TOD period 1",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch2",             "First year energy from the system in TOD period 2",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch3",             "First year energy from the system in TOD period 3",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch4",             "First year energy from the system in TOD period 4",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch5",             "First year energy from the system in TOD period 5",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch6",             "First year energy from the system in TOD period 6",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch7",             "First year energy from the system in TOD period 7",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch8",             "First year energy from the system in TOD period 8",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_dispatch9",             "First year energy from the system in TOD period 9",             "kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
                                                                                  
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price1",                "First year energy price for TOD period 1",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price2",                "First year energy price for TOD period 2",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price3",                "First year energy price for TOD period 3",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price4",                "First year energy price for TOD period 4",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price5",                "First year energy price for TOD period 5",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price6",                "First year energy price for TOD period 6",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price7",                "First year energy price for TOD period 7",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price8",                "First year energy price for TOD period 8",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "firstyear_energy_price9",                "First year energy price for TOD period 9",                      "cents/kWh",             "",                      "Cash Flow Revenue",      "ppa_multiplier_model=0",                       "",                                  "" },
                                                                                  
// first year monthly output for each TOD period                                  
//	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear",		      "First year revenue from the system by month",	"",   "",      "Cash Flow Revenue",             "*",				   "",                 "" },
//	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear",	      "First year energy from the system by month",	"",   "",          "Cash Flow Revenue",             "*",				   "",                 "" },
                                                                                 
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD1",      "First year revenue from the system by month for TOD period 1",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD1",   "First year energy from the system by month for TOD period 1",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD2",      "First year revenue from the system by month for TOD period 2",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD2",   "First year energy from the system by month for TOD period 2",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD3",      "First year revenue from the system by month for TOD period 3",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD3",   "First year energy from the system by month for TOD period 3",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD4",      "First year revenue from the system by month for TOD period 4",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD4",   "First year energy from the system by month for TOD period 4",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD5",      "First year revenue from the system by month for TOD period 5",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD5",   "First year energy from the system by month for TOD period 5",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD6",      "First year revenue from the system by month for TOD period 6",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD6",   "First year energy from the system by month for TOD period 6",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD7",      "First year revenue from the system by month for TOD period 7",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD7",   "First year energy from the system by month for TOD period 7",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD8",      "First year revenue from the system by month for TOD period 8",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD8",   "First year energy from the system by month for TOD period 8",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_revenue_monthly_firstyear_TOD9",      "First year revenue from the system by month for TOD period 9",  "$",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
	{ SSC_OUTPUT,        SSC_ARRAY,     "cf_energy_net_monthly_firstyear_TOD9",   "First year energy from the system by month for TOD period 9",   "kWh",   "",                      "Cash Flow Revenue",             "ppa_multiplier_model=0",				   "",                 "" },
                                                                                  
/* inputs in DHF model not currently in M 11/15/10 */                             
	{ SSC_INPUT,         SSC_NUMBER,    "total_installed_cost",                   "Installed cost",                                                "$",     "",					  "System Costs",			 "*",                         "",                             "" },
	{ SSC_INPUT,         SSC_NUMBER,    "reserves_interest",                      "Interest on reserves",				                           "%",	 "",					  "Reserve Accounts",             "?=1.75",                     "MIN=0,MAX=100",      			"" },
                                                                                  
/* DHF replacement reserve on top of regular o and m */                             
	{ SSC_INPUT,        SSC_NUMBER,     "equip1_reserve_cost",                    "Major equipment reserve 1 cost",	                               "$/W",	        "",				  "Reserve Accounts",             "?=0.25",               "MIN=0",                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip1_reserve_freq",                    "Major equipment reserve 1 frequency",	                       "years",	 "",			  "Reserve Accounts",             "?=12",               "INTEGER,MIN=0",                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip2_reserve_cost",                    "Major equipment reserve 2 cost",	                               "$/W",	 "",				  "Reserve Accounts",             "?=0",               "MIN=0",                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip2_reserve_freq",                    "Major equipment reserve 2 frequency",	                       "years",	 "",			  "Reserve Accounts",             "?=15",               "INTEGER,MIN=0",                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip3_reserve_cost",                    "Major equipment reserve 3 cost",	                               "$/W",	 "",				  "Reserve Accounts",             "?=0",               "MIN=0",                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip3_reserve_freq",                    "Major equipment reserve 3 frequency",	                       "years",	 "",			  "Reserve Accounts",             "?=20",               "INTEGER,MIN=0",                         "" },
                                                                                  
/* major equipment depreciation schedules - can extend to three different schedu  les */
	{ SSC_INPUT,        SSC_NUMBER,     "equip_reserve_depr_sta",                 "Major equipment reserve state depreciation",	                   "",	 "0=5yr MACRS,1=15yr MACRS,2=5yr SL,3=15yr SL, 4=20yr SL,5=39yr SL,6=Custom",  "Depreciation", "?=0",   "INTEGER,MIN=0,MAX=6",  "" },
	{ SSC_INPUT,        SSC_NUMBER,     "equip_reserve_depr_fed",                 "Major equipment reserve federal depreciation",	               "",	 "0=5yr MACRS,1=15yr MACRS,2=5yr SL,3=15yr SL, 4=20yr SL,5=39yr SL,6=Custom",  "Depreciation", "?=0",   "INTEGER,MIN=0,MAX=6",  "" },
                                                                                  
/* DHF salvage value */	                                                          
	{ SSC_INPUT,        SSC_NUMBER,     "salvage_percentage",                     "Net pre-tax cash salvage value",	                               "%",	 "",					  "Salvage Value",             "?=10",                     "MIN=0,MAX=100",      			"" },
/* DHF market specific inputs - leveraged partnership flip */                     
	{ SSC_INPUT,        SSC_NUMBER,		"ppa_soln_mode",                          "PPA solution mode",                                             "0/1",   "0=solve ppa,1=specify ppa", "Solution Mode",         "?=0",                     "INTEGER,MIN=0,MAX=1",            "" },
//	{ SSC_INPUT, SSC_NUMBER, "ppa_soln_tolerance", "PPA solution tolerance", "", "", "Solution Mode", "?=1e-3", "", "" },

	{ SSC_INPUT, SSC_NUMBER, "ppa_soln_tolerance", "PPA solution tolerance", "", "", "Solution Mode", "?=1e-5", "", "" },
	{ SSC_INPUT, SSC_NUMBER, "ppa_soln_min", "PPA solution minimum ppa", "cents/kWh", "", "Solution Mode", "?=0", "", "" },
	{ SSC_INPUT,        SSC_NUMBER,		"ppa_soln_max",                           "PPA solution maximum ppa",                                      "cents/kWh",   "", "Solution Mode",         "?=100",                     "",            "" },
	{ SSC_INPUT,        SSC_NUMBER,		"ppa_soln_max_iterations",                "PPA solution maximum number of iterations",                     "",   "", "Solution Mode",         "?=100",                     "INTEGER,MIN=1",            "" },
                                                                                  
	{ SSC_INPUT,        SSC_NUMBER,     "ppa_price_input",			              "PPA price in first year",			                               "$/kWh",	 "",			  "PPA Price",			 "?=10",         "",      			"" },
	{ SSC_INPUT, SSC_NUMBER, "ppa_escalation", "PPA escalation rate", "%/year", "", "PPA Price", "?=0", "", "" },

/* DHF construction period */                                                     
	{ SSC_INPUT,       SSC_NUMBER,      "construction_financing_cost",	          "Construction financing total",	                                "$",	 "",					  "Construction Financing",			 "*",                         "",                             "" },
                                                                                  
/* DHF term financing */                                                          
	{ SSC_INPUT,        SSC_NUMBER,     "term_tenor",                             "Term financing period",				                            "years", "",				      "Project Term Debt",             "?=10",					"INTEGER,MIN=0",      			"" },
	{ SSC_INPUT,        SSC_NUMBER,     "term_int_rate",                          "Term financing interest rate",		                            "%",	 "",					  "Project Term Debt",             "?=8.5",                   "MIN=0,MAX=100",      			"" },
	{ SSC_INPUT,        SSC_NUMBER,     "dscr",						              "Debt service coverage ratio",		                            "",	     "",				      "Project Term Debt",             "?=1.5",					"MIN=0",      			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "dscr_reserve_months",		              "Debt service reserve account",		                            "months P&I","",			      "Project Term Debt",             "?=6",					    "MIN=0",      			        "" },
	/* Debt fraction input option */
	{ SSC_INPUT, SSC_NUMBER, "debt_percent", "Debt percent", "%", "", "Project Term Debt", "?=50", "MIN=0,MAX=100", "" },
	{ SSC_INPUT, SSC_NUMBER, "debt_option", "Debt option", "0/1", "0=debt percent,1=dscr", "Project Term Debt", "?=1", "INTEGER,MIN=0,MAX=1", "" },

	{ SSC_INPUT, SSC_NUMBER, "payment_option", "Debt repayment option", "0/1", "0=Equal payments (standard amortization),1=Fixed principal declining interest", "Project Term Debt", "?=0", "INTEGER,MIN=0,MAX=1", "" },



/* DHF Capital Cost */                                                            
	{ SSC_INPUT,        SSC_NUMBER,     "cost_debt_closing",		              "Debt closing cost",                                              "$",                 "",       "Other Capital Costs",        "?=250000",         "MIN=0",              "" },
	{ SSC_INPUT,        SSC_NUMBER,     "cost_debt_fee",		                  "Debt closing fee (% of total debt amount)",                      "%",                 "",       "Other Capital Costs",        "?=1.5",            "MIN=0",              "" },
	{ SSC_INPUT, SSC_NUMBER, "months_working_reserve", "Working capital reserve months of operating costs", "months", "", "Other Capital Costs", "?=6", "MIN=0", "" },
	{ SSC_INPUT, SSC_NUMBER, "months_receivables_reserve", "Receivables reserve months of PPA revenue", "months", "", "Other Capital Costs", "?=0", "MIN=0", "" },
	{ SSC_INPUT, SSC_NUMBER, "cost_other_financing", "Other financing cost", "$", "", "Other Capital Costs", "?=150000", "MIN=0", "" },
/* DHF Equity Structure */                                                        
	{ SSC_INPUT,        SSC_NUMBER,     "flip_target_percent",			          "After-tax IRR target",		"%",	 "",					  "IRR Targets",             "?=11",					  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "flip_target_year",		                  "IRR target year",				"",		 "",					  "IRR Targets",             "?=11",					  "MIN=1",     			        "" },
/* DHF depreciation allocation */                                                 
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_macrs_5_percent",		      "5-yr MACRS depreciation federal and state allocation",	"%", "",	  "Depreciation",             "?=89",					  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_macrs_15_percent",		      "15-yr MACRS depreciation federal and state allocation",	"%", "",  "Depreciation",             "?=1.5",					  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_sl_5_percent",		          "5-yr straight line depreciation federal and state allocation",	"%", "",  "Depreciation",             "?=0",						  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_sl_15_percent",		          "15-yr straight line depreciation federal and state allocation","%", "",  "Depreciation",             "?=3",						  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_sl_20_percent",		          "20-yr straight line depreciation federal and state allocation","%", "",  "Depreciation",             "?=3",						  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_sl_39_percent",		          "39-yr straight line depreciation federal and state allocation","%", "",  "Depreciation",             "?=0.5",					  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,     "depr_alloc_custom_percent",	          "Custom depreciation federal and state allocation","%", "",  "Depreciation",             "?=0",					  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_ARRAY,      "depr_custom_schedule",		              "Custom depreciation schedule",	"%",   "",                      "Depreciation",             "*",						   "",                              "" },
/* DHF bonus depreciation */                                                      
	{ SSC_INPUT,        SSC_NUMBER,     "depr_bonus_sta",			              "State bonus depreciation",			"%",	 "",					  "Depreciation",             "?=0",						  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_macrs_5",                 "State bonus depreciation 5-yr MACRS","0/1", "",                      "Depreciation",			 "?=1",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_macrs_15",                "State bonus depreciation 15-yr MACRS","0/1","",                     "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_sl_5",                    "State bonus depreciation 5-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_sl_15",                   "State bonus depreciation 15-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_sl_20",                   "State bonus depreciation 20-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_sl_39",                   "State bonus depreciation 39-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_sta_custom",                  "State bonus depreciation custom","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
                                                                                  
	{ SSC_INPUT,        SSC_NUMBER,     "depr_bonus_fed",			              "Federal bonus depreciation",			"%",	 "",					  "Depreciation",             "?=0",						  "MIN=0,MAX=100",     			        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_macrs_5",                 "Federal bonus depreciation 5-yr MACRS","0/1", "",                      "Depreciation",			 "?=1",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_macrs_15",                "Federal bonus depreciation 15-yr MACRS","0/1","",                     "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_sl_5",                    "Federal bonus depreciation 5-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_sl_15",                   "Federal bonus depreciation 15-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_sl_20",                   "Federal bonus depreciation 20-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_sl_39",                   "Federal bonus depreciation 39-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_bonus_fed_custom",                  "Federal bonus depreciation custom","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
/* DHF ITC depreciation */                                                        
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_macrs_5",                   "State ITC depreciation 5-yr MACRS","0/1", "",                      "Depreciation",			 "?=1",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_macrs_15",                  "State ITC depreciation 15-yr MACRS","0/1","",                     "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_sl_5",                      "State ITC depreciation 5-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_sl_15",                     "State ITC depreciation 15-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_sl_20",                     "State ITC depreciation 20-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_sl_39",                     "State ITC depreciation 39-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_sta_custom",                    "State ITC depreciation custom","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
                                                                                  
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_macrs_5",                   "Federal ITC depreciation 5-yr MACRS","0/1", "",                      "Depreciation",			 "?=1",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_macrs_15",                  "Federal ITC depreciation 15-yr MACRS","0/1","",                     "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_sl_5",                      "Federal ITC depreciation 5-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_sl_15",                     "Federal ITC depreciation 15-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_sl_20",                     "Federal ITC depreciation 20-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_sl_39",                     "Federal ITC depreciation 39-yr straight line","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
	{ SSC_INPUT,        SSC_NUMBER,		"depr_itc_fed_custom",                    "Federal ITC depreciation custom","0/1","",                  "Depreciation",			 "?=0",                       "BOOLEAN",                        "" },
                                                                                  
/* PBI for debt service TODO - other yearly incentives */                         
	{ SSC_INPUT,        SSC_NUMBER,     "pbi_fed_for_ds",                         "Federal PBI available for debt service",     "0/1",      "",                      "Cash Incentives",      "?=0",                       "BOOLEAN",                                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "pbi_sta_for_ds",                         "State PBI available for debt service",     "0/1",      "",                      "Cash Incentives",      "?=0",                       "BOOLEAN",                                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "pbi_uti_for_ds",                         "Utility PBI available for debt service",     "0/1",      "",                      "Cash Incentives",      "?=0",                       "BOOLEAN",                                         "" },
	{ SSC_INPUT,        SSC_NUMBER,     "pbi_oth_for_ds",                         "Other PBI available for debt service",     "0/1",      "",                      "Cash Incentives",      "?=0",                       "BOOLEAN",                                         "" },
                                                                                  
/* intermediate outputs */                                                        
	{ SSC_OUTPUT,       SSC_NUMBER,     "cost_debt_upfront",                      "Debt up-front fee",          "$",   "",					  "Intermediate Costs",			 "?=0",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cost_financing",                         "Financing cost",          "$",   "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cost_prefinancing",                      "Total installed cost",          "$",   "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cost_installed",                         "Net capital cost",                   "$",     "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cost_installedperwatt",                  "Net capital cost per watt",          "$/W",   "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "nominal_discount_rate",                  "Nominal discount rate",            "%",     "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "prop_tax_assessed_value",                "Assessed value of property for tax purposes","$", "",				  "Intermediate Costs",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "salvage_value",			              "Net pre-tax cash salvage value",	"$",	 "",					  "Intermediate Costs",			 "*",                         "",                             "" },
	                                                                              
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_alloc_none_percent",		          "Non-depreciable federal and state allocation",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_alloc_none",		                  "Non-depreciable federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_alloc_total",		                  "Total depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
                                                                                  
// state itc table                                                                
/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_macrs_5",		      "5-yr MACRS state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_macrs_5",		              "5-yr MACRS depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_macrs_5",		  "5-yr MACRS state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_macrs_5",		  "5-yr MACRS state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_macrs_5",		  "5-yr MACRS state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_macrs_5",		              "5-yr MACRS depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_macrs_5",		  "5-yr MACRS state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_macrs_5",	  "5-yr MACRS depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_macrs_5",		  "5-yr MACRS depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_macrs_5",		  "5-yr MACRS depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_macrs_5",		  "5-yr MACRS depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_macrs_5",  "5-yr MACRS state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_macrs_5",  "5-yr MACRS state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_macrs_5",		  "5-yr MACRS state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_macrs_5",	  "5-yr MACRS state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_macrs_5",		              "5-yr MACRS state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
                                                                                  
/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_macrs_15",		      "15-yr MACRS state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_macrs_15",		              "15-yr MACRS depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_macrs_15",		  "15-yr MACRS state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_macrs_15",		  "15-yr MACRS state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_macrs_15",		  "15-yr MACRS state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_macrs_15",		          "15-yr MACRS depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_macrs_15",	  "15-yr MACRS state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_macrs_15",	  "15-yr MACRS depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_macrs_15",	  "15-yr MACRS depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_macrs_15",	  "15-yr MACRS depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_macrs_15",		  "15-yr MACRS depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_macrs_15", "15-yr MACRS state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_macrs_15", "15-yr MACRS state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_macrs_15",		  "15-yr MACRS state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_macrs_15",  "15-yr MACRS state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_macrs_15",		              "15-yr MACRS state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_sl_5",		          "5-yr straight line state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_5",		                  "5-yr straight line depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_sl_5",		      "5-yr straight line state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_sl_5",		      "5-yr straight line state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_sl_5",		      "5-yr straight line state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_sl_5",		              "5-yr straight line depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_sl_5",		  "5-yr straight line state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_sl_5",		  "5-yr straight line depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_sl_5",		  "5-yr straight line depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_sl_5",		  "5-yr straight line depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_sl_5",		      "5-yr straight line depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_sl_5",	  "5-yr straight line state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_sl_5",	  "5-yr straight line state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_sl_5",		      "5-yr straight line state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_sl_5",	  "5-yr straight line state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_sl_5",		                  "5-yr straight line state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_sl_15",		      "15-yr straight line state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_15",		                  "15-yr straight line depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_sl_15",		      "15-yr straight line state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_sl_15",		      "15-yr straight line state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_sl_15",		      "15-yr straight line state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_sl_15",		              "15-yr straight line depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_sl_15",		  "15-yr straight line state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_sl_15",		  "15-yr straight line depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_sl_15",		  "15-yr straight line depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_sl_15",		  "15-yr straight line depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_sl_15",		      "15-yr straight line depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_sl_15",	  "15-yr straight line state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_sl_15",	  "15-yr straight line state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_sl_15",		      "15-yr straight line state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_sl_15",	  "15-yr straight line state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_sl_15",		              "15-yr straight line state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_sl_20",		      "20-yr straight line state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_20",		                  "20-yr straight line depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_sl_20",		      "20-yr straight line state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_sl_20",		      "20-yr straight line state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_sl_20",		      "20-yr straight line state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_sl_20",		              "20-yr straight line depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_sl_20",		  "20-yr straight line state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_sl_20",		  "20-yr straight line depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_sl_20",		  "20-yr straight line depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_sl_20",		  "20-yr straight line depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_sl_20",		      "20-yr straight line depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_sl_20",	  "20-yr straight line state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_sl_20",	  "20-yr straight line state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_sl_20",		      "20-yr straight line state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_sl_20",	  "20-yr straight line state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_sl_20",		              "20-yr straight line state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_sl_39",		      "39-yr straight line state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_39",		                  "39-yr straight line depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_sl_39",		      "39-yr straight line state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_sl_39",		      "39-yr straight line state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_sl_39",		      "39-yr straight line state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_sl_39",		              "39-yr straight line depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_sl_39",		  "39-yr straight line state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_sl_39",		  "39-yr straight line depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_sl_39",		  "39-yr straight line depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_sl_39",		  "39-yr straight line depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_sl_39",		      "39-yr straight line depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_sl_39",	  "39-yr straight line state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_sl_39",	  "39-yr straight line state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_sl_39",		      "39-yr straight line state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_sl_39",	  "39-yr straight line state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_sl_39",		              "39-yr straight line state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_custom",		      "Custom straight line state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_custom",		              "Custom straight line depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_custom",		      "Custom straight line state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_custom",		      "Custom straight line state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_custom",		      "Custom straight line state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_custom",		              "Custom straight line depreciation state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_custom",		  "Custom straight line state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_custom",	  "Custom straight line depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_custom",		  "Custom straight line depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_custom",		  "Custom straight line depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_custom",		  "Custom straight line depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_custom",	  "Custom straight line state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_custom",	  "Custom straight line state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_custom",		      "Custom straight line state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_custom",	  "Custom straight line state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_custom",		              "Custom straight line state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_total",		      "Total state percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_total",		                  "Total depreciation federal and state allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_ibi_reduc_total",		      "Total state IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_cbi_reduc_total",		      "Total state CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_prior_itc_total",		      "Total state depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_qual_total",		              "Total state ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_qual_total",		  "Total state percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_percent_amount_total",		  "Total depreciation ITC basis from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_percent_total",		  "Total depreciation ITC basis disallowance from state percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_fixed_amount_total",		  "Total depreciation ITC basis from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_sta_fixed_total",		      "Total depreciation ITC basis disallowance from state fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_sta_reduction_total",	  "Total state basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_itc_fed_reduction_total",	  "Total state basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_after_itc_total",		      "Total state depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_first_year_bonus_total",	  "Total state first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_stabas_total",		              "Total state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_percent_total",		          "State ITC percent total",	"$", "",	  "Tax Credits",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_sta_fixed_total",		              "State ITC fixed total",	"$", "",	  "Tax Credits",             "*",					  "",     			        "" },

// federal itc table
/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_macrs_5",		      "5-yr MACRS federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_macrs_5",		              "5-yr MACRS depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_macrs_5",		  "5-yr MACRS federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_macrs_5",		  "5-yr MACRS federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_macrs_5",		  "5-yr MACRS federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_macrs_5",		              "5-yr MACRS depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_macrs_5",		  "5-yr MACRS federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_macrs_5",	  "5-yr MACRS depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_macrs_5",		  "5-yr MACRS depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_macrs_5",		  "5-yr MACRS depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_macrs_5",		  "5-yr MACRS depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_macrs_5",  "5-yr MACRS federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_macrs_5",  "5-yr MACRS federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_macrs_5",		  "5-yr MACRS federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_macrs_5",	  "5-yr MACRS federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_macrs_5",		              "5-yr MACRS federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_macrs_15",		      "15-yr MACRS federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_macrs_15",		              "15-yr MACRS depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_macrs_15",		  "15-yr MACRS federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_macrs_15",		  "15-yr MACRS federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_macrs_15",		  "15-yr MACRS federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_macrs_15",		          "15-yr MACRS depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_macrs_15",	  "15-yr MACRS federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_macrs_15",	  "15-yr MACRS depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_macrs_15",	  "15-yr MACRS depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_macrs_15",	  "15-yr MACRS depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_macrs_15",		  "15-yr MACRS depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_macrs_15", "15-yr MACRS federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_macrs_15", "15-yr MACRS federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_macrs_15",		  "15-yr MACRS federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_macrs_15",  "15-yr MACRS federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_macrs_15",		              "15-yr MACRS federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_sl_5",		          "5-yr straight line federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_5",		                  "5-yr straight line depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_sl_5",		      "5-yr straight line federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_sl_5",		      "5-yr straight line federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_sl_5",		      "5-yr straight line federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_sl_5",		              "5-yr straight line depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_sl_5",		  "5-yr straight line federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_sl_5",		  "5-yr straight line depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_sl_5",		  "5-yr straight line depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_sl_5",		  "5-yr straight line depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_sl_5",		      "5-yr straight line depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_sl_5",	  "5-yr straight line federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_sl_5",	  "5-yr straight line federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_sl_5",		      "5-yr straight line federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_sl_5",	  "5-yr straight line federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_sl_5",		                  "5-yr straight line federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_sl_15",		      "15-yr straight line federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_15",		                  "15-yr straight line depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_sl_15",		      "15-yr straight line federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_sl_15",		      "15-yr straight line federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_sl_15",		      "15-yr straight line federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_sl_15",		              "15-yr straight line depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_sl_15",		  "15-yr straight line federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_sl_15",		  "15-yr straight line depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_sl_15",		  "15-yr straight line depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_sl_15",		  "15-yr straight line depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_sl_15",		      "15-yr straight line depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_sl_15",	  "15-yr straight line federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_sl_15",    "15-yr straight line federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_sl_15",            "15-yr straight line federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_sl_15",     "15-yr straight line federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_sl_15",                      "15-yr straight line federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_sl_20",              "20-yr straight line federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_20",                       "20-yr straight line depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_sl_20",		      "20-yr straight line federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_sl_20",		      "20-yr straight line federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_sl_20",		      "20-yr straight line federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_sl_20",		              "20-yr straight line depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_sl_20",		  "20-yr straight line federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_sl_20",		  "20-yr straight line depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_sl_20",		  "20-yr straight line depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_sl_20",		  "20-yr straight line depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_sl_20",		      "20-yr straight line depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_sl_20",	  "20-yr straight line federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_sl_20",    "20-yr straight line federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_sl_20",            "20-yr straight line federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_sl_20",     "20-yr straight line federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_sl_20",                      "20-yr straight line federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_sl_39",              "39-yr straight line federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_sl_39",                       "39-yr straight line depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_sl_39",            "39-yr straight line federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_sl_39",            "39-yr straight line federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_sl_39",            "39-yr straight line federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_sl_39",                     "39-yr straight line depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_sl_39",		  "39-yr straight line federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_sl_39",		  "39-yr straight line depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_sl_39",		  "39-yr straight line depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_sl_39",		  "39-yr straight line depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_sl_39",		      "39-yr straight line depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_sl_39",	  "39-yr straight line federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_sl_39",	  "39-yr straight line federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_sl_39",		      "39-yr straight line federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_sl_39",     "39-yr straight line federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_sl_39",                      "39-yr straight line federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_custom",  		      "Custom straight line federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_custom",		              "Custom straight line depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_custom",		      "Custom straight line federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_custom",		      "Custom straight line federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_custom",		      "Custom straight line federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_custom",		              "Custom straight line depreciation federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_custom",		  "Custom straight line federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_custom",	  "Custom straight line depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_custom",		  "Custom straight line depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_custom",		  "Custom straight line depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_custom",		  "Custom straight line depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_custom",	  "Custom straight line federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_custom",	  "Custom straight line federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_custom",		      "Custom straight line federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_custom",	  "Custom straight line federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_custom",		              "Custom straight line federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/*1*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_total",		      "Total federal percent of total depreciable basis",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_alloc_total",		                  "Total depreciation federal and federal allocation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*2*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_ibi_reduc_total",		      "Total federal IBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*3*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_cbi_reduc_total",		      "Total federal CBI reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*4*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_prior_itc_total",		      "Total federal depreciation basis prior ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_qual_total",		              "Total federal ITC adj qualifying costs",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*5*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_qual_total",		  "Total federal percent of qualifying costs",	"%", "",	  "Depreciation",             "*",					  "",     			        "" },
/*6*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_percent_amount_total",		  "Total depreciation ITC basis from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_percent_total",		  "Total depreciation ITC basis disallowance from federal percentage",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*7*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_fixed_amount_total",		  "Total depreciation ITC basis from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_disallow_fed_fixed_total",		      "Total depreciation ITC basis disallowance from federal fixed amount",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*8*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_sta_reduction_total",	  "Total federal basis state ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*9*/ { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_itc_fed_reduction_total",	  "Total federal basis federal ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*10*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_after_itc_total",		      "Total federal depreciation basis after ITC reduction",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
/*11*/{ SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_first_year_bonus_total",	  "Total federal first year bonus depreciation",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "depr_fedbas_total",		              "Total federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

      { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_percent_total",		          "Federal ITC percent total",	"$", "",	  "Tax Credits",             "*",					  "",     			        "" },
	  { SSC_OUTPUT,     SSC_NUMBER,     "itc_fed_fixed_total",		              "Federal ITC fixed total",	"$", "",	  "Tax Credits",             "*",					  "",     			        "" },

/* depreciation bases method - added with version 4.1    0=5-yrMacrs, 1=proportional */
	{ SSC_INPUT,        SSC_NUMBER,      "depr_stabas_method",                    "Method of state depreciation reduction",     "",      "0=5yr MACRS,1=Proportional",                      "Depreciation",      "?=0",                       "INTEGER,MIN=0,MAX=1",                                         "" },
	{ SSC_INPUT,        SSC_NUMBER,      "depr_fedbas_method",                    "Method of federal depreciation reduction",     "",      "0=5yr MACRS,1=Proportional",                      "Depreciation",      "?=0",                       "INTEGER,MIN=0,MAX=1",                                         "" },

/* State depreciation table */
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_stabas_total",		              "Total state depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/* Federal depreciation table */
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_macrs_5",		              "5-yr MACRS federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_macrs_15",		              "15-yr MACRS federal depreciation basis",	"$", "",  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_sl_5",		                  "5-yr straight line federal depreciation basis",	"$", "",  "Depreciation",             "*",						  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_sl_15",		              "15-yr straight line federal depreciation basis","$", "",  "Depreciation",             "*",						  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_sl_20",		              "20-yr straight line federal depreciation basis","$", "",  "Depreciation",             "*",						  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_sl_39",		              "39-yr straight line federal depreciation basis","$", "",  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_custom",		              "Custom federal depreciation basis","$", "",  "Depreciation",             "*",					  "",     			        "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "depr_fedbas_total",		              "Total federal depreciation basis",	"$", "",	  "Depreciation",             "*",					  "",     			        "" },

/* State taxes */
	/* intermediate outputs for validation */
	{ SSC_OUTPUT,       SSC_NUMBER,     "cash_for_debt_service",                  "Cash available for debt service (CAFDS)",   "$",     "",					  "Debt Sizing",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "pv_cafds",                               "Present value of CAFDS","$", "",				  "Debt Sizing",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "size_of_debt",			                  "Size of debt",	"$",	 "",					  "Debt Sizing",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "size_of_equity",			              "Equity",	"$",	 "",					  "Debt Sizing",			 "*",                         "",                             "" },

/* model outputs */
	{ SSC_OUTPUT,       SSC_NUMBER,     "cf_length",                              "Number of periods in cashflow",      "",             "",                      "Cash Flow",      "*",                       "INTEGER",                                  "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ppa_price",			                  "PPA price in first year",			"cents/kWh",	"",				   "PPA Price",			  "*",                         "",      					   "" },
/* Production - input as energy_net above */

/* Partial Income Statement: Project */	
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_energy_net",                          "Energy",                     "kWh",      "",                      "Cash Flow Revenues",             "*",                      "LENGTH_EQUAL=cf_length",                             "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_ppa_price",                           "PPA price",                     "cents/kWh",      "",                      "Cash Flow Revenues",             "*",                      "LENGTH_EQUAL=cf_length",                             "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_energy_value",                        "PPA revenue",                     "$",      "",                      "Cash Flow Revenues",             "*",                      "LENGTH_EQUAL=cf_length",                             "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_fixed_expense",                    "O&M fixed expense",                  "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_production_expense",               "O&M production-based expense",       "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_capacity_expense",                 "O&M capacity-based expense",         "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_fuel_expense",                     "O&M fuel expense",                   "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_opt_fuel_1_expense",               "O&M biomass feedstock expense",                   "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_om_opt_fuel_2_expense",               "O&M coal feedstock expense",                   "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_property_tax_assessed_value",         "Property tax net assessed value", "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_property_tax_expense",                "Property tax expense",               "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_insurance_expense",                   "Insurance expense",                  "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_operating_expenses",                  "Total operating expenses",            "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_net_salvage_value",                   "Salvage value",            "$",            "",                      "Cash Flow Revenues",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_total_revenue",                       "Total revenue",            "$",            "",                      "Cash Flow Revenues",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_ebitda",                              "EBITDA",       "$",            "",                      "Cash Flow Expenses",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_reserve_debtservice",                 "Reserves debt service balance",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_reserve_om", "Reserves working capital balance ", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_reserve_receivables", "Reserves receivables balance", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_reserve_equip1", "Reserves major equipment 1 balance", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_reserve_equip2",                      "Reserves major equipment 2 balance",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_reserve_equip3",                      "Reserves major equipment 3 balance",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_reserve_total",                       "Reserves total reserves balance",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_reserve_interest",                    "Interest earned on reserves",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_funding_debtservice",                 "Reserves debt service funding",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_funding_om", "Reserves working capital funding", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_funding_receivables", "Reserves receivables funding", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_funding_equip1", "Reserves major equipment 1 funding", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_funding_equip2",                      "Reserves major equipment 2 funding",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_funding_equip3",                      "Reserves major equipment 3 funding",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_disbursement_debtservice",            "Reserves debt service disbursement ",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_disbursement_om", "Reserves working capital disbursement", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_disbursement_receivables", "Reserves receivables disbursement", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_disbursement_equip1", "Reserves major equipment 1 disbursement", "$", "", "Cash Flow Reserves", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_disbursement_equip2",                 "Reserves major equipment 2 disbursement",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_disbursement_equip3",                 "Reserves major equipment 3 disbursement",       "$",            "",                      "Cash Flow Reserves",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
		
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_cash_for_ds",                         "Cash available for debt service (CAFDS)",                       "$",            "",                      "Cash Flow Debt Sizing",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pv_interest_factor",                  "Present value interest factor for CAFDS",             "",            "",                      "Cash Flow Debt Repayment",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pv_cash_for_ds",                      "Present value of CAFDS",                       "$",            "",                      "Cash Flow Debt Sizing",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_debt_size",                           "Size of debt",                       "$",            "",                      "Cash Flow Debt Sizing",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_debt_balance",                        "Debt balance",                       "$",            "",                      "Cash Flow Debt Repayment",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_debt_payment_interest",               "Debt interest payment",                   "$",            "",                      "Cash Flow Debt Repayment",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_debt_payment_principal",              "Debt principal payment",                  "$",            "",                      "Cash Flow Debt Repayment",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_debt_payment_total",                  "Debt total payment",             "$",            "",                      "Cash Flow Debt Repayment",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
     
	// Project cash flow
	
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_operating_activities",        "Cash flow from operating activities",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_NUMBER,     "purchase_of_property",	                  "Purchase of property",	"$",	 "",					  "Cash Flow Pre Tax",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_dsra",                        "Reserve (increase)/decrease debt service ",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_project_wcra", "Reserve (increase)/decrease working capital", "$", "", "Cash Flow Pre Tax", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_project_receivablesra", "Reserve (increase)/decrease receivables", "$", "", "Cash Flow Pre Tax", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_project_me1ra", "Reserve (increase)/decrease major equipment 1", "$", "", "Cash Flow Pre Tax", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_me2ra",                       "Reserve (increase)/decrease major equipment 2",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_me3ra",                       "Reserve (increase)/decrease major equipment 3",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_ra",                          "Reserve (increase)/decrease total reserve account",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_me1cs",                       "Reserve capital spending major equipment 1",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_me2cs",                       "Reserve capital spending major equipment 2",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_me3cs",                       "Reserve capital spending major equipment 3",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_mecs",                        "Reserve capital spending major equipment total",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_investing_activities",        "Cash flow from investing activities",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_NUMBER,     "issuance_of_equity",	                  "Issuance of equity",	"$",	 "",					  "Cash Flow Pre Tax",			 "*",                         "",                             "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_financing_activities",        "Cash flow from financing activities",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pretax_cashflow",                     "Pre-tax cash flow",  "$", "",                      "Cash Flow Pre Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

// Project returns
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_pretax",               "Pre-tax project returns",  "$", "",                      "Cash Flow Pre Tax Returns",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_pretax_irr",           "Pre-tax project cumulative IRR",  "%", "",                      "Cash Flow Pre Tax Returns",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_pretax_npv",           "Pre-tax project cumulative NPV",  "$", "",                      "Cash Flow Pre Tax Returns",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_aftertax_cash",        "After-tax project returns cash total",  "$", "",                      "Cash Flow After Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_aftertax",             "After-tax project returns",  "$", "",                      "Cash Flow After Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_aftertax_irr",         "After-tax project cumulative IRR",  "%", "",                      "Cash Flow After Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_aftertax_max_irr",     "After-tax project maximum IRR",  "%", "",                      "Cash Flow After Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_project_return_aftertax_npv",         "After-tax project cumulative NPV",  "$", "",                      "Cash Flow After Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
		
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_total_fed",                          "Federal CBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_total_sta",                          "State CBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_total_oth",                          "Other CBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_total_uti",                          "Utility CBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_total",                              "Total CBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_statax_total",                       "State taxable CBI income",   "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "cbi_fedtax_total",                       "Federal taxable CBI income", "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },

	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_total_fed",                          "Federal IBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_total_sta",                          "State IBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_total_oth",                          "Other IBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_total_uti",                          "Utility IBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_total",                              "Total IBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_statax_total",                       "State taxable IBI income",   "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ibi_fedtax_total",                       "Federal taxable IBI income", "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },

	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_total_fed",                       "Federal PBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_total_sta",                       "State PBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_total_oth",                       "Other PBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_total_uti",                       "Utility PBI income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_total",                           "Total PBI income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_statax_total",                    "State taxable PBI income",   "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_pbi_fedtax_total",                    "Federal taxable PBI income", "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	{ SSC_OUTPUT,       SSC_NUMBER,     "itc_total_fed",                          "Federal ITC income",         "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "itc_total_sta",                          "State ITC income",           "$",            "",                      "Cash Flow Incentives",      "*",                     "",                                      "" },
	{ SSC_OUTPUT,        SSC_NUMBER,    "itc_total",							  "Total ITC income",                 "$",            "",                      "Cash Flow Incentives",      "*",                     "",                "" },
																													            
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_ptc_fed",                             "Federal PTC income",                 "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_ptc_sta",                             "State PTC income",                   "$",            "",                      "Cash Flow Incentives",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

/* state depreciation and tax */
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_macrs_5",                     "State depreciation from 5-yr MACRS",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_macrs_15",                    "State depreciation from 15-yr MACRS",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_sl_5",                        "State depreciation from 5-yr straight line",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_sl_15",                       "State depreciation from 15-yr straight line",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_sl_20",                       "State depreciation from 20-yr straight line",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_sl_39",                       "State depreciation from 39-yr straight line",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_custom",                      "State depreciation from custom",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_me1",                         "State depreciation from major equipment 1",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_me2",                         "State depreciation from major equipment 2",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_me3",                         "State depreciation from major equipment 3",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_stadepr_total",                       "Total state tax depreciation",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_statax_income_prior_incentives",      "State taxable income without incentives",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_statax_taxable_incentives",           "State taxable incentives",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_statax_income_with_incentives",       "State taxable income",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_state_tax_frac",				      "State tax rate",                   "%",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_statax",				              "State tax benefit/(liability)",                   "$",            "",                      "Cash Flow State Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

/* federal depreciation and tax */
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_macrs_5",                     "Federal depreciation from 5-yr MACRS",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_macrs_15",                    "Federal depreciation from 15-yr MACRS",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_sl_5",                        "Federal depreciation from 5-yr straight line",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_sl_15",                       "Federal depreciation from 15-yr straight line",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_sl_20",                       "Federal depreciation from 20-yr straight line",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_sl_39",                       "Federal depreciation from 39-yr straight line",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_custom",                      "Federal depreciation from custom",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_me1",                         "Federal depreciation from major equipment 1",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_me2",                         "Federal depreciation from major equipment 2",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_me3",                         "Federal depreciation from major equipment 3",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_feddepr_total",                       "Total federal tax depreciation",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_fedtax_income_prior_incentives",      "Federal taxable income without incentives",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_fedtax_taxable_incentives",           "Federal taxable incentives",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_fedtax_income_with_incentives",       "Federal taxable income",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_federal_tax_frac",				      "Federal tax rate",                   "%",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_effective_tax_frac",				  "Effective tax rate",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },
	{ SSC_OUTPUT,       SSC_ARRAY,      "cf_fedtax",				              "Federal tax benefit/(liability)",                   "$",            "",                      "Cash Flow Federal Income Tax",      "*",                     "LENGTH_EQUAL=cf_length",                "" },

	// metrics table 
//	{ SSC_OUTPUT,       SSC_NUMBER,     "first_year_energy_net",                  "Annual energy",                             "kWh",                 "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "debt_fraction",                          "Debt percent",                             "%", "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "flip_target_year",                       "Target year to meet IRR",                   "",                    "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "flip_target_irr",                        "IRR target",                                "%",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "flip_actual_year",                       "Year target IRR was achieved",              "year",                    "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "flip_actual_irr",                        "IRR in target year",                        "%",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoe_real",                              "Levelized cost (real)",                               "cents/kWh",               "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoe_nom",                               "Levelized cost (nominal)",                            "cents/kWh",               "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lppa_real",                              "Levelized PPA price (real)",                         "cents/kWh",               "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lppa_nom",                               "Levelized PPA price (nominal)",                      "cents/kWh",               "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ppa",                                    "PPA price (Year 1)",                        "cents/kWh",               "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "ppa_escalation",                         "PPA price escalation",                      "%/year",              "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "project_return_aftertax_irr",            "Internal rate of return (after-tax)",       "%",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "project_return_aftertax_npv",            "Net present value (after-tax)",             "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "npv_ppa_revenue",                        "Present value of PPA revenue",              "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "npv_energy_nom",                         "Present value of annual energy (nominal)",     "kWh",                 "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "npv_energy_real",                        "Present value of annual energy (real)",     "kWh",                 "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "present_value_oandm",                    "Present value of O&M",				       "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "present_value_oandm_nonfuel",            "Present value of non-fuel O&M",         "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "present_value_fuel",                     "Present value of fuel O&M",             "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "present_value_insandproptax",            "Present value of insurance and prop tax",   "$",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoptc_fed_real",                        "Levelized federal PTC (real)",              "cents/kWh",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoptc_fed_nom",                         "Levelized federal PTC (nominal)",           "cents/kWh",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoptc_sta_real",                        "Levelized state PTC (real)",                "cents/kWh",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "lcoptc_sta_nom",                         "Levelized state PTC (nominal)",             "cents/kWh",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "wacc",                                   "Weighted average cost of capital (WACC)",   "$",                   "", "Metrics", "*", "", "" },
//	{ SSC_OUTPUT,       SSC_NUMBER,     "effective_tax_rate",                     "Effective tax rate",                        "%",                   "", "Metrics", "*", "", "" },
	{ SSC_OUTPUT,       SSC_NUMBER,     "analysis_period_irr",                    "IRR at end of analysis period",             "%",                   "", "Metrics", "*", "", "" },

	{ SSC_OUTPUT, SSC_ARRAY, "cf_annual_costs", "Annual costs", "$", "", "LCOE calculations", "*", "LENGTH_EQUAL=cf_length", "" },
	{ SSC_OUTPUT, SSC_NUMBER, "npv_annual_costs", "Present value of annual costs", "$", "", "LCOE calculations", "*", "", "" },

	{ SSC_OUTPUT, SSC_NUMBER, "adjusted_installed_cost", "Initial cost less cash incentives", "$", "", "", "*", "", "" },

	{ SSC_OUTPUT, SSC_NUMBER, "min_dscr", "Minimum DSCR", "", "", "DSCR", "", "" },
	{ SSC_OUTPUT, SSC_ARRAY, "cf_pretax_dscr", "DSCR (pre-tax)", "", "", "DSCR", "*", "LENGTH_EQUAL=cf_length", "" },


var_info_invalid };

extern var_info
	vtab_standard_financial[],
	vtab_oandm[],
	vtab_tax_credits[],
	vtab_payment_incentives[],
	vtab_battery_replacement_cost[];

enum {
	CF_energy_net,
	CF_energy_value,
	CF_ppa_price,

	CF_om_fixed_expense,
	CF_om_production_expense,
	CF_om_capacity_expense,
	CF_om_fuel_expense,

	CF_om_opt_fuel_2_expense,
	CF_om_opt_fuel_1_expense,

	CF_federal_tax_frac,
	CF_state_tax_frac,
	CF_effective_tax_frac,

	CF_property_tax_assessed_value,
	CF_property_tax_expense,
	CF_insurance_expense,
	CF_operating_expenses,
	CF_net_salvage_value,
	CF_total_revenue,
	CF_ebitda,

	CF_reserve_debtservice,
	CF_funding_debtservice,
	CF_disbursement_debtservice,
	CF_reserve_om,
	CF_funding_om,
	CF_disbursement_om,
	CF_reserve_receivables,
	CF_funding_receivables,
	CF_disbursement_receivables,
	CF_reserve_equip1,
	CF_funding_equip1,
	CF_disbursement_equip1,
	CF_reserve_equip2,
	CF_funding_equip2,
	CF_disbursement_equip2,
	CF_reserve_equip3,
	CF_funding_equip3,
	CF_disbursement_equip3,
	CF_reserve_total,
	CF_reserve_interest,


	// Project cash flow
	CF_project_operating_activities,
	CF_project_dsra,
	CF_project_wcra,
	CF_project_receivablesra,
	CF_project_me1ra,
	CF_project_me2ra,
	CF_project_me3ra,
	CF_project_ra,
	CF_project_me1cs,
	CF_project_me2cs,
	CF_project_me3cs,
	CF_project_mecs,
	CF_project_investing_activities,
	CF_project_financing_activities,
	CF_pretax_cashflow,


	// Project returns
	CF_project_return_pretax,
	CF_project_return_pretax_irr,
	CF_project_return_pretax_npv,
	CF_project_return_aftertax_cash,
	CF_project_return_aftertax_itc,
	CF_project_return_aftertax_ptc,
	CF_project_return_aftertax_tax,
	CF_project_return_aftertax,
	CF_project_return_aftertax_irr,
	CF_project_return_aftertax_max_irr,
	CF_project_return_aftertax_npv,


	CF_pv_interest_factor,
	CF_cash_for_ds,
	CF_pv_cash_for_ds,
	CF_debt_size,

	CF_debt_balance,
	CF_debt_payment_interest,
	CF_debt_payment_principal,
	CF_debt_payment_total,
	
	CF_pbi_fed,
	CF_pbi_sta,
	CF_pbi_uti,
	CF_pbi_oth,
	CF_pbi_total,
	CF_pbi_statax_total,
	CF_pbi_fedtax_total,

	CF_ptc_fed,
	CF_ptc_sta,
	CF_aftertax_ptc,
	
	CF_macrs_5_frac,
	CF_macrs_15_frac,
	CF_sl_5_frac,
	CF_sl_15_frac,
	CF_sl_20_frac,
	CF_sl_39_frac,
	CF_custom_frac,

	CF_stadepr_macrs_5,
	CF_stadepr_macrs_15,
	CF_stadepr_sl_5,
	CF_stadepr_sl_15,
	CF_stadepr_sl_20,
	CF_stadepr_sl_39,
	CF_stadepr_custom,
	CF_stadepr_me1,
	CF_stadepr_me2,
	CF_stadepr_me3,
	CF_stadepr_total,
	CF_statax_income_prior_incentives,
	CF_statax_taxable_incentives,
	CF_statax_income_with_incentives,
	CF_statax,

	CF_feddepr_macrs_5,
	CF_feddepr_macrs_15,
	CF_feddepr_sl_5,
	CF_feddepr_sl_15,
	CF_feddepr_sl_20,
	CF_feddepr_sl_39,
	CF_feddepr_custom,
	CF_feddepr_me1,
	CF_feddepr_me2,
	CF_feddepr_me3,
	CF_feddepr_total,
	CF_fedtax_income_prior_incentives,
	CF_fedtax_taxable_incentives,
	CF_fedtax_income_with_incentives,
	CF_fedtax,

	CF_me1depr_total,
	CF_me2depr_total,
	CF_me3depr_total,

	CF_sta_depr_sched,
	CF_sta_depreciation,
	CF_sta_incentive_income_less_deductions,
	CF_sta_taxable_income_less_deductions,
//	CF_sta_tax_savings,
	
	CF_fed_depr_sched,
	CF_fed_depreciation,
	CF_fed_incentive_income_less_deductions,
	CF_fed_taxable_income_less_deductions,
//	CF_fed_tax_savings,

	CF_degradation,

	CF_Recapitalization,
	CF_Recapitalization_boolean,

	CF_return_on_equity_input,
	CF_return_on_equity_dollars,
	CF_return_on_equity,
	CF_lcog_costs,

	CF_Annual_Costs,
	CF_pretax_dscr,

	CF_battery_replacement_cost_schedule,
	CF_battery_replacement_cost,
// Host
	CF_agreement_cost,
	CF_after_tax_net_equity_cost_flow,
	CF_after_tax_cash_flow,
	CF_payback_with_expenses,
	CF_cumulative_payback_with_expenses,
	CF_nte,
	CF_host_energy_value,

	CF_max };



class cm_host_developer : public compute_module
{
private:
	util::matrix_t<double> cf;
	dispatch_calculations m_disp_calcs;
	hourly_energy_calculation hourly_energy_calcs;


public:
	cm_host_developer()
	{
		add_var_info( vtab_standard_financial );
		add_var_info( vtab_oandm );
		add_var_info( vtab_tax_credits );
		add_var_info( vtab_payment_incentives );
//		add_var_info(vtab_advanced_financing_cost);
		add_var_info( _cm_vtab_host_developer );
		add_var_info(vtab_battery_replacement_cost);
	}

	void exec( ) throw( general_error )
	{
		int i = 0;

		// cash flow initialization
		int nyears = as_integer("analysis_period");
		cf.resize_fill(CF_max, nyears + 1, 0.0);

		// assign inputs
		double inflation_rate = as_double("inflation_rate")*0.01;
		double ppa_escalation = as_double("ppa_escalation")*0.01;
		double disc_real = as_double("real_discount_rate")*0.01;
		double host_disc_real = as_double("host_real_discount_rate")*0.01;
		size_t count;
		ssc_number_t* arrp;
		arrp = as_array("federal_tax_rate", &count);
		if (count > 0)
		{
			if (count == 1) // single value input
			{
				for (i = 0; i < nyears; i++)
					cf.at(CF_federal_tax_frac, i + 1) = arrp[0] * 0.01;
			}
			else // schedule
			{
				for (i = 0; i < nyears && i < (int)count; i++)
					cf.at(CF_federal_tax_frac, i + 1) = arrp[i] * 0.01;
			}
		}
		arrp = as_array("state_tax_rate", &count);
		if (count > 0)
		{
			if (count == 1) // single value input
			{
				for (i = 0; i < nyears; i++)
					cf.at(CF_state_tax_frac, i + 1) = arrp[0] * 0.01;
			}
			else // schedule
			{
				for (i = 0; i < nyears && i < (int)count; i++)
					cf.at(CF_state_tax_frac, i + 1) = arrp[i] * 0.01;
			}
		}
		for (i = 0; i <= nyears; i++)
			cf.at(CF_effective_tax_frac, i) = cf.at(CF_state_tax_frac, i) +
			(1.0 - cf.at(CF_state_tax_frac, i))*cf.at(CF_federal_tax_frac, i);



		double nom_discount_rate = (1 + inflation_rate)*(1 + disc_real) - 1;
		double host_nom_discount_rate = (1 + inflation_rate)*(1 + host_disc_real) - 1;

		// In conjunction with SAM - take installed costs and salestax costs (for deducting if necessary)
		double cost_prefinancing = as_double("total_installed_cost");

		// use DHF named range names for variables whenever possible
		double nameplate = as_double("system_capacity");
		double year1_fuel_use = as_double("annual_fuel_usage"); // kWht
	
		double assessed_frac = as_double("prop_tax_cost_assessed_percent")*0.01;
		double salvage_value_frac = as_double("salvage_percentage")*0.01;
		double salvage_value = salvage_value_frac * cost_prefinancing;

		double cost_debt_closing = as_double("cost_debt_closing");
		double cost_debt_fee_frac = as_double("cost_debt_fee")*0.01;
		double cost_other_financing = as_double("cost_other_financing");
		double cost_debt_upfront;


		double constr_total_financing = as_double("construction_financing_cost");

		int ppa_mode = as_integer("ppa_soln_mode");

		bool constant_dscr_mode = (as_integer("debt_option")==1);
		bool constant_principal = (as_integer("payment_option") == 1);;
		//		log(util::format("debt option=%d and constant dscr mode=%s.",
//			as_integer("debt_option"), (constant_dscr_mode ? "true":"false")),
//			SSC_WARNING);


		// general financial expenses and incentives - stdlib?
		// precompute expenses from annual schedules or value+escalation
		escal_or_annual( CF_om_fixed_expense, nyears, "om_fixed", inflation_rate, 1.0, false, as_double("om_fixed_escal")*0.01 );
		escal_or_annual( CF_om_production_expense, nyears, "om_production", inflation_rate, 0.001, false, as_double("om_production_escal")*0.01 );  
		escal_or_annual( CF_om_capacity_expense, nyears, "om_capacity", inflation_rate, 1.0, false, as_double("om_capacity_escal")*0.01 );  
		escal_or_annual( CF_om_fuel_expense, nyears, "om_fuel_cost", inflation_rate, as_double("system_heat_rate")*0.001, false, as_double("om_fuel_cost_escal")*0.01 );
		
		escal_or_annual( CF_om_opt_fuel_1_expense, nyears, "om_opt_fuel_1_cost", inflation_rate, 1.0, false, as_double("om_opt_fuel_1_cost_escal")*0.01 );  
		escal_or_annual( CF_om_opt_fuel_2_expense, nyears, "om_opt_fuel_2_cost", inflation_rate, 1.0, false, as_double("om_opt_fuel_2_cost_escal")*0.01 );  

		double om_opt_fuel_1_usage = as_double("om_opt_fuel_1_usage");
		double om_opt_fuel_2_usage = as_double("om_opt_fuel_2_usage");

		// battery cost - replacement from lifetime analysis
		if ((as_integer("en_batt") == 1) && (as_integer("batt_replacement_option") > 0))
		{
			ssc_number_t *batt_rep = 0;
			if (as_integer("batt_replacement_option")==1)
				batt_rep = as_array("batt_bank_replacement", &count); // replacements per year calculated
			else // user specified
				batt_rep = as_array("batt_replacement_schedule", &count); // replacements per year user-defined
			double batt_cap = as_double("batt_computed_bank_capacity");
			// updated 10/17/15 per 10/14/15 meeting
//			escal_or_annual(CF_battery_replacement_cost_schedule, nyears, "batt_replacement_cost", inflation_rate, batt_cap, false, as_double("batt_replacement_cost_escal")*0.01);
			double batt_repl_cost = as_double("batt_replacement_cost");
			double batt_repl_cost_escal = as_double("batt_replacement_cost_escal")*0.01;

			for (i = 0; i<nyears; i++)
				cf.at(CF_battery_replacement_cost_schedule, i + 1) = batt_repl_cost * batt_cap * pow(1 + batt_repl_cost_escal + inflation_rate, i);

			for (i = 0; i < nyears && i<(int)count; i++)
				cf.at(CF_battery_replacement_cost, i + 1) = batt_rep[i] * 
					cf.at(CF_battery_replacement_cost_schedule, i + 1);
		}
		
		// initialize energy and revenue
		// initialize energy
		// differs from samsim - accumulate hourly energy
		//double first_year_energy = as_double("energy_net");
		double first_year_energy = 0.0;


		// degradation
		// degradation starts in year 2 for single value degradation - no degradation in year 1 - degradation =1.0
		// lifetime degradation applied in technology compute modules
		if (as_integer("system_use_lifetime_output") == 1)
		{
			for (i = 1; i <= nyears; i++) cf.at(CF_degradation, i) = 1.0;
		}
		else
		{
			size_t count_degrad = 0;
			ssc_number_t *degrad = 0;
			degrad = as_array("degradation", &count_degrad);

			if (count_degrad == 1)
			{
				for (i = 1; i <= nyears; i++) cf.at(CF_degradation, i) = pow((1.0 - degrad[0] / 100.0), i - 1);
			}
			else if (count_degrad > 0)
			{
				for (i = 0; i < nyears && i < (int)count_degrad; i++) cf.at(CF_degradation, i + 1) = (1.0 - degrad[i] / 100.0);
			}
		}



		hourly_energy_calcs.calculate(this);


		// dispatch
		if (as_integer("system_use_lifetime_output") == 1)
		{
			// hourly_enet includes all curtailment, availability
			for (size_t y = 1; y <= (size_t)nyears; y++)
			{
				for (size_t h = 0; h<8760; h++)
				{
					cf.at(CF_energy_net, y) += hourly_energy_calcs.hourly_energy()[(y - 1) * 8760 + h] * cf.at(CF_degradation, y);
				}
			}
		}
		else
		{
			for (i = 0; i<8760; i++) first_year_energy += hourly_energy_calcs.hourly_energy()[i]; // sum up hourly kWh to get total annual kWh first year production includes first year curtailment, availability 
			cf.at(CF_energy_net, 1) = first_year_energy;
			for (i = 1; i <= nyears; i++)
				cf.at(CF_energy_net, i) = first_year_energy * cf.at(CF_degradation, i);
		}

		first_year_energy = cf.at(CF_energy_net, 1);



		std::vector<double> degrade_cf;
		for (i = 0; i <= nyears; i++)
		{
			degrade_cf.push_back(cf.at(CF_degradation, i));
		}
		m_disp_calcs.init(this, degrade_cf, hourly_energy_calcs.hourly_energy());
		// end of energy and dispatch initialization



		for (i=1;i<=nyears;i++)
		{
			cf.at(CF_om_production_expense,i) *= cf.at(CF_energy_net,i);
			cf.at(CF_om_capacity_expense,i) *= nameplate;
			cf.at(CF_om_fuel_expense,i) *= year1_fuel_use;

			cf.at(CF_om_opt_fuel_1_expense,i) *= om_opt_fuel_1_usage;
			cf.at(CF_om_opt_fuel_2_expense,i) *= om_opt_fuel_2_usage;
		}


		double ppa = as_double("ppa_price_input")*100.0; // either initial guess for ppa_mode=1 or final ppa for ppa_mode=0
		if (ppa_mode == 0) ppa = 0; // initial guess for target irr mode

		double property_tax_assessed_value = cost_prefinancing * as_double("prop_tax_cost_assessed_percent") * 0.01;
		double property_tax_decline_percentage = as_double("prop_tax_assessed_decline");
		double property_tax_rate = as_double("property_tax_rate")*0.01;
		double insurance_rate = as_double("insurance_rate")*0.01;
		double months_working_reserve_frac = as_double("months_working_reserve") / 12.0;
		double months_receivables_reserve_frac = as_double("months_receivables_reserve") / 12.0;
		double equip1_reserve_cost = as_double("equip1_reserve_cost");
		int equip1_reserve_freq = as_integer("equip1_reserve_freq");
		double equip2_reserve_cost = as_double("equip2_reserve_cost");
		int equip2_reserve_freq = as_integer("equip2_reserve_freq");
		double equip3_reserve_cost = as_double("equip3_reserve_cost");
		int equip3_reserve_freq = as_integer("equip3_reserve_freq");

		//  calculate debt for constant dscr mode
		int term_tenor = as_integer("term_tenor");
		int loan_moratorium = as_integer("loan_moratorium");
		double term_int_rate = as_double("term_int_rate")*0.01;
		double dscr = as_double("dscr");
		double dscr_reserve_months = as_double("dscr_reserve_months");
		double cash_for_debt_service=0;
		double pv_cafds=0;
		double size_of_debt=0;


		// pre calculate reserves
		int i_equip1=1;
		int i_equip2=1;
		int i_equip3=1;

		for (i=1; i<=nyears; i++)
		{			
			// reserves calculations
			// major equipment 1 reserve
			if ( (i <= (i_equip1 * equip1_reserve_freq)) && ((i_equip1 * equip1_reserve_freq) <= nyears) ) // note will not enter if equip_reequip1_reserve_freq=0
			{
				cf.at(CF_funding_equip1,i) = equip1_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i_equip1 * equip1_reserve_freq-1 ) / equip1_reserve_freq;
			}
			if (i == (i_equip1 * equip1_reserve_freq))
			{
				cf.at(CF_disbursement_equip1,i) =  -equip1_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i-1 );
				i_equip1++;
			}
			cf.at(CF_reserve_equip1,i) = cf.at(CF_funding_equip1,i) + cf.at(CF_disbursement_equip1,i) + cf.at(CF_reserve_equip1,i-1);
			// major equipment 2 reserve
			if ( (i <= (i_equip2 * equip2_reserve_freq)) && ((i_equip2 * equip2_reserve_freq) <= nyears) ) // note will not enter if equip_reequip2_reserve_freq=0
			{
				cf.at(CF_funding_equip2,i) = equip2_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i_equip2 * equip2_reserve_freq-1 ) / equip2_reserve_freq;
			}
			if (i == (i_equip2 * equip2_reserve_freq))
			{
				cf.at(CF_disbursement_equip2,i) =  -equip2_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i-1 );
				i_equip2++;
			}
			cf.at(CF_reserve_equip2,i) = cf.at(CF_funding_equip2,i) + cf.at(CF_disbursement_equip2,i) + cf.at(CF_reserve_equip2,i-1);;
			// major equipment 3 reserve
			if ( (i <= (i_equip3 * equip3_reserve_freq)) && ((i_equip3 * equip3_reserve_freq) <= nyears) ) // note will not enter if equip_reequip3_reserve_freq=0
			{
				cf.at(CF_funding_equip3,i) = equip3_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i_equip3 * equip3_reserve_freq-1 ) / equip3_reserve_freq;
			}
			if (i == (i_equip3 * equip3_reserve_freq))
			{
				cf.at(CF_disbursement_equip3,i) =  -equip3_reserve_cost * nameplate*1000 * pow( 1 + inflation_rate, i-1 );
				i_equip3++;
			}
			cf.at(CF_reserve_equip3,i) = cf.at(CF_funding_equip3,i) + cf.at(CF_disbursement_equip3,i) + cf.at(CF_reserve_equip3,i-1);
		}

		depreciation_sched_5_year_macrs_half_year(CF_macrs_5_frac,nyears);
		depreciation_sched_15_year_macrs_half_year(CF_macrs_15_frac,nyears);
		depreciation_sched_5_year_straight_line_half_year(CF_sl_5_frac,nyears);
		depreciation_sched_15_year_straight_line_half_year(CF_sl_15_frac,nyears);
		depreciation_sched_20_year_straight_line_half_year(CF_sl_20_frac,nyears);
		depreciation_sched_39_year_straight_line_half_year(CF_sl_39_frac,nyears);
		depreciation_sched_custom(CF_custom_frac,nyears,"depr_custom_schedule");

		int feddepr_me1=CF_macrs_5_frac + as_integer("equip_reserve_depr_fed");
		int feddepr_me2=CF_macrs_5_frac + as_integer("equip_reserve_depr_fed");
		int feddepr_me3=CF_macrs_5_frac + as_integer("equip_reserve_depr_fed");

		int stadepr_me1=CF_macrs_5_frac + as_integer("equip_reserve_depr_sta");
		int stadepr_me2=CF_macrs_5_frac + as_integer("equip_reserve_depr_sta");
		int stadepr_me3=CF_macrs_5_frac + as_integer("equip_reserve_depr_sta");


		for (i=1;i<=nyears;i++)
		{
			if ((equip1_reserve_freq != 0) && (i%equip1_reserve_freq == 0))
			{
				major_equipment_depreciation(CF_disbursement_equip1,feddepr_me1,i,nyears,CF_feddepr_me1);
				major_equipment_depreciation(CF_disbursement_equip1,stadepr_me1,i,nyears,CF_stadepr_me1);
			}
			if ((equip2_reserve_freq != 0) && (i%equip2_reserve_freq == 0))
			{
				major_equipment_depreciation(CF_disbursement_equip2,feddepr_me2,i,nyears,CF_feddepr_me2);
				major_equipment_depreciation(CF_disbursement_equip2,stadepr_me2,i,nyears,CF_stadepr_me2);
			}
			if ((equip3_reserve_freq != 0) && (i%equip3_reserve_freq == 0))
			{
				major_equipment_depreciation(CF_disbursement_equip3,feddepr_me3,i,nyears,CF_feddepr_me3);
				major_equipment_depreciation(CF_disbursement_equip3,stadepr_me3,i,nyears,CF_stadepr_me3);
			}
		}

		double recapitalization_cost = as_double("system_recapitalization_cost");
		double recapitalization_escalation = 0.01*as_double("system_recapitalization_escalation");
		if (as_integer("system_use_recapitalization"))
		{
			size_t recap_boolean_count;
			ssc_number_t *recap_boolean = 0;
			recap_boolean = as_array("system_lifetime_recapitalize", &recap_boolean_count);

			if (recap_boolean_count > 0)
			{
				for (i=0;i<nyears && i<(int)recap_boolean_count;i++) cf.at(CF_Recapitalization_boolean,i+1) = recap_boolean[i];
			}
		}

		// return on equity based on workbook and emails from Sara Turner for SAM for India
		size_t roe_count;
		ssc_number_t *roe_input = 0;
		roe_input = as_array("roe_input", &roe_count);
		if (roe_count > 0)
		{
			if (roe_count == 1) // single value input
			{
				for (i = 0; i < nyears; i++)
					cf.at(CF_return_on_equity_input, i + 1) = roe_input[0]/100.0;
			}
			else // schedule
			{
				for (i = 0; i < nyears && i < (int)roe_count; i++) 
					cf.at(CF_return_on_equity_input, i + 1) = roe_input[i]/100.0;
			}
		}



		for (i=1; i<=nyears; i++)
		{			
		// Project partial income statement			

			double decline_percent = 100 - (i-1)*property_tax_decline_percentage;
			cf.at(CF_property_tax_assessed_value,i) = (decline_percent > 0) ? property_tax_assessed_value * decline_percent * 0.01:0.0;
			cf.at(CF_property_tax_expense,i) = cf.at(CF_property_tax_assessed_value,i) * property_tax_rate;
			cf.at(CF_insurance_expense,i) = cost_prefinancing * insurance_rate * pow( 1 + inflation_rate, i-1 );

			if (as_integer("system_use_recapitalization"))
			{
				cf.at(CF_Recapitalization,i) = cf.at(CF_Recapitalization_boolean,i) * recapitalization_cost
					 *  pow((1 + inflation_rate + recapitalization_escalation ), i-1 );
			}

			cf.at(CF_operating_expenses,i) = 
				+ cf.at(CF_om_fixed_expense,i)
				+ cf.at(CF_om_production_expense,i)
				+ cf.at(CF_om_capacity_expense,i)
				+ cf.at(CF_om_fuel_expense,i)
				+ cf.at(CF_om_opt_fuel_1_expense,i)
				+ cf.at(CF_om_opt_fuel_2_expense,i)
				+ cf.at(CF_property_tax_expense,i)
				+ cf.at(CF_insurance_expense,i)
				+ cf.at(CF_battery_replacement_cost,i)
				+ cf.at(CF_Recapitalization,i);
		}

		// salvage value
		cf.at(CF_net_salvage_value,nyears) = salvage_value;

		// o and m reserve
		if (nyears>0) 
		{
			cf.at(CF_reserve_om,0) = months_working_reserve_frac * cf.at(CF_operating_expenses,1) ;
			cf.at(CF_funding_om,0) = cf.at(CF_reserve_om,0);
			for (i=1; i<nyears; i++)
			{
				cf.at(CF_reserve_om,i) = months_working_reserve_frac * cf.at(CF_operating_expenses,i+1) ;
				cf.at(CF_funding_om,i) = cf.at(CF_reserve_om,i) - cf.at(CF_reserve_om,i-1);
			}
			cf.at(CF_disbursement_om, nyears) = -cf.at(CF_reserve_om,nyears-1);
		}

		for (i=0;i<=nyears; i++)
		{
			cf.at(CF_project_wcra,i) = -cf.at(CF_funding_om,i) - cf.at(CF_disbursement_om,i);
			cf.at(CF_project_me1ra,i) = -cf.at(CF_funding_equip1,i) - cf.at(CF_disbursement_equip1,i);
			cf.at(CF_project_me2ra,i) = -cf.at(CF_funding_equip2,i) - cf.at(CF_disbursement_equip2,i);
			cf.at(CF_project_me3ra,i) = -cf.at(CF_funding_equip3,i) - cf.at(CF_disbursement_equip3,i);
		} 

		// interest on reserves
		double reserves_interest = as_double("reserves_interest")*0.01;

		double issuance_of_equity;

		// ibi fixed
		double ibi_fed_amount = as_double("ibi_fed_amount");
		double ibi_sta_amount = as_double("ibi_sta_amount");
		double ibi_uti_amount = as_double("ibi_uti_amount");
		double ibi_oth_amount = as_double("ibi_oth_amount");

		// ibi percent
		double ibi_fed_per = as_double("ibi_fed_percent")*0.01*cost_prefinancing;
		if (ibi_fed_per > as_double("ibi_fed_percent_maxvalue")) ibi_fed_per = as_double("ibi_fed_percent_maxvalue"); 
		double ibi_sta_per = as_double("ibi_sta_percent")*0.01*cost_prefinancing;
		if (ibi_sta_per > as_double("ibi_sta_percent_maxvalue")) ibi_sta_per = as_double("ibi_sta_percent_maxvalue"); 
		double ibi_uti_per = as_double("ibi_uti_percent")*0.01*cost_prefinancing;
		if (ibi_uti_per > as_double("ibi_uti_percent_maxvalue")) ibi_uti_per = as_double("ibi_uti_percent_maxvalue"); 
		double ibi_oth_per = as_double("ibi_oth_percent")*0.01*cost_prefinancing;
		if (ibi_oth_per > as_double("ibi_oth_percent_maxvalue")) ibi_oth_per = as_double("ibi_oth_percent_maxvalue"); 

		// itc fixed
		double itc_fed_amount = as_double("itc_fed_amount");
		double itc_sta_amount = as_double("itc_sta_amount");

		// itc percent - max value used for comparison to qualifying costs
		double itc_fed_frac = as_double("itc_fed_percent")*0.01;
		double itc_fed_per;
		double itc_sta_frac = as_double("itc_sta_percent")*0.01;
		double itc_sta_per;

		// cbi
		double cbi_fed_amount = 1000.0*nameplate*as_double("cbi_fed_amount");
		if (cbi_fed_amount > as_double("cbi_fed_maxvalue")) cbi_fed_amount = as_double("cbi_fed_maxvalue"); 
		double cbi_sta_amount = 1000.0*nameplate*as_double("cbi_sta_amount");
		if (cbi_sta_amount > as_double("cbi_sta_maxvalue")) cbi_sta_amount = as_double("cbi_sta_maxvalue"); 
		double cbi_uti_amount = 1000.0*nameplate*as_double("cbi_uti_amount");
		if (cbi_uti_amount > as_double("cbi_uti_maxvalue")) cbi_uti_amount = as_double("cbi_uti_maxvalue"); 
		double cbi_oth_amount = 1000.0*nameplate*as_double("cbi_oth_amount");
		if (cbi_oth_amount > as_double("cbi_oth_maxvalue")) cbi_oth_amount = as_double("cbi_oth_maxvalue"); 
		
		// precompute pbi
		compute_production_incentive( CF_pbi_fed, nyears, "pbi_fed_amount", "pbi_fed_term", "pbi_fed_escal" );
		compute_production_incentive( CF_pbi_sta, nyears, "pbi_sta_amount", "pbi_sta_term", "pbi_sta_escal" );
		compute_production_incentive( CF_pbi_uti, nyears, "pbi_uti_amount", "pbi_uti_term", "pbi_uti_escal" );
		compute_production_incentive( CF_pbi_oth, nyears, "pbi_oth_amount", "pbi_oth_term", "pbi_oth_escal" );

		// precompute ptc
		compute_production_incentive_IRS_2010_37( CF_ptc_sta, nyears, "ptc_sta_amount", "ptc_sta_term", "ptc_sta_escal" );
		compute_production_incentive_IRS_2010_37( CF_ptc_fed, nyears, "ptc_fed_amount", "ptc_fed_term", "ptc_fed_escal" );

		for (i=0;i<=nyears; i++)
		{
			cf.at(CF_pbi_total,i) = cf.at(CF_pbi_fed,i) + cf.at(CF_pbi_sta,i) + cf.at(CF_pbi_uti,i) + cf.at(CF_pbi_oth,i);
			cf.at(CF_aftertax_ptc,i) = cf.at(CF_ptc_fed,i) + cf.at(CF_ptc_sta,i);
		}

		double cbi_total = cbi_fed_amount + cbi_sta_amount +cbi_uti_amount + cbi_oth_amount;
		double ibi_total = ibi_fed_amount + ibi_sta_amount +ibi_uti_amount + ibi_oth_amount + ibi_fed_per + ibi_sta_per +ibi_uti_per + ibi_oth_per;
		double itc_fed_total;
		double itc_sta_total;
		double itc_total;

		double cbi_statax_total = 
			( as_boolean("cbi_fed_tax_sta") ? cbi_fed_amount : 0 ) +
			( as_boolean("cbi_sta_tax_sta") ? cbi_sta_amount : 0 ) +
			( as_boolean("cbi_uti_tax_sta") ? cbi_uti_amount : 0 ) +
			( as_boolean("cbi_oth_tax_sta") ? cbi_oth_amount : 0 );
		double ibi_statax_total = 
			( as_boolean("ibi_fed_amount_tax_sta") ? ibi_fed_amount : 0 ) +
			( as_boolean("ibi_fed_percent_tax_sta") ? ibi_fed_per : 0 ) +
			( as_boolean("ibi_sta_amount_tax_sta") ? ibi_sta_amount : 0 ) +
			( as_boolean("ibi_sta_percent_tax_sta") ? ibi_sta_per : 0 ) +
			( as_boolean("ibi_uti_amount_tax_sta") ? ibi_uti_amount : 0 ) +
			( as_boolean("ibi_uti_percent_tax_sta") ? ibi_uti_per : 0 ) +
			( as_boolean("ibi_oth_amount_tax_sta") ? ibi_oth_amount : 0 ) +
			( as_boolean("ibi_oth_percent_tax_sta") ? ibi_oth_per : 0 );

		double cbi_fedtax_total = 
			( as_boolean("cbi_fed_tax_fed") ? cbi_fed_amount : 0 ) +
			( as_boolean("cbi_sta_tax_fed") ? cbi_sta_amount : 0 ) +
			( as_boolean("cbi_uti_tax_fed") ? cbi_uti_amount : 0 ) +
			( as_boolean("cbi_oth_tax_fed") ? cbi_oth_amount : 0 );
		double ibi_fedtax_total = 
			( as_boolean("ibi_fed_amount_tax_fed") ? ibi_fed_amount : 0 ) +
			( as_boolean("ibi_fed_percent_tax_fed") ? ibi_fed_per : 0 ) +
			( as_boolean("ibi_sta_amount_tax_fed") ? ibi_sta_amount : 0 ) +
			( as_boolean("ibi_sta_percent_tax_fed") ? ibi_sta_per : 0 ) +
			( as_boolean("ibi_uti_amount_tax_fed") ? ibi_uti_amount : 0 ) +
			( as_boolean("ibi_uti_percent_tax_fed") ? ibi_uti_per : 0 ) +
			( as_boolean("ibi_oth_amount_tax_fed") ? ibi_oth_amount : 0 ) +
			( as_boolean("ibi_oth_percent_tax_fed") ? ibi_oth_per : 0 );


		for (i=1;i<=nyears;i++)
		{
			cf.at(CF_pbi_statax_total,i) =
				(( as_boolean("pbi_fed_tax_sta") && (!as_boolean("pbi_fed_for_ds"))) ? cf.at(CF_pbi_fed,i) : 0 ) +
				(( as_boolean("pbi_sta_tax_sta") && (!as_boolean("pbi_sta_for_ds"))) ? cf.at(CF_pbi_sta,i) : 0 ) +
				(( as_boolean("pbi_uti_tax_sta") && (!as_boolean("pbi_uti_for_ds"))) ? cf.at(CF_pbi_uti,i) : 0 ) +
				(( as_boolean("pbi_oth_tax_sta") && (!as_boolean("pbi_oth_for_ds"))) ? cf.at(CF_pbi_oth,i) : 0 ) ;

			cf.at(CF_pbi_fedtax_total,i) =
				(( as_boolean("pbi_fed_tax_fed") && (!as_boolean("pbi_fed_for_ds"))) ? cf.at(CF_pbi_fed,i) : 0 ) +
				(( as_boolean("pbi_sta_tax_fed") && (!as_boolean("pbi_sta_for_ds"))) ? cf.at(CF_pbi_sta,i) : 0 ) +
				(( as_boolean("pbi_uti_tax_fed") && (!as_boolean("pbi_uti_for_ds"))) ? cf.at(CF_pbi_uti,i) : 0 ) +
				(( as_boolean("pbi_oth_tax_fed") && (!as_boolean("pbi_oth_for_ds"))) ? cf.at(CF_pbi_oth,i) : 0 ) ;
		}
		// 5/1/11
		for (i=1;i<=nyears;i++)
		{
			cf.at(CF_statax_taxable_incentives,i) = cf.at(CF_pbi_statax_total,i);
			cf.at(CF_fedtax_taxable_incentives,i) = cf.at(CF_pbi_fedtax_total,i);
		}
		cf.at(CF_statax_taxable_incentives,1) += cbi_statax_total + ibi_statax_total;
		cf.at(CF_fedtax_taxable_incentives,1) += cbi_fedtax_total + ibi_fedtax_total;

		double cost_financing;

		double cost_installed;

		double depr_alloc_macrs_5_frac = as_double("depr_alloc_macrs_5_percent") * 0.01;
		double depr_alloc_macrs_15_frac = as_double("depr_alloc_macrs_15_percent") * 0.01;
		double depr_alloc_sl_5_frac = as_double("depr_alloc_sl_5_percent") * 0.01;
		double depr_alloc_sl_15_frac = as_double("depr_alloc_sl_15_percent") * 0.01;
		double depr_alloc_sl_20_frac = as_double("depr_alloc_sl_20_percent") * 0.01;
		double depr_alloc_sl_39_frac = as_double("depr_alloc_sl_39_percent") * 0.01;
		double depr_alloc_custom_frac = as_double("depr_alloc_custom_percent") * 0.01;
		double depr_alloc_total_frac = depr_alloc_macrs_5_frac + depr_alloc_macrs_15_frac +	
			depr_alloc_sl_5_frac + depr_alloc_sl_15_frac +	depr_alloc_sl_20_frac +	depr_alloc_sl_39_frac + depr_alloc_custom_frac;
		// TODO - place check that depr_alloc_total_frac <= 1 and <>0
		double depr_alloc_none_frac = 1.0 - depr_alloc_total_frac;
		// TODO - place check that depr_alloc_none_frac >= 0

		
		// redistribute fractions to only depreciable allocations
		if (depr_alloc_total_frac > 0) // and <=1
		{
			depr_alloc_macrs_5_frac /= depr_alloc_total_frac;
			depr_alloc_macrs_15_frac /= depr_alloc_total_frac;
			depr_alloc_sl_5_frac /= depr_alloc_total_frac;
			depr_alloc_sl_15_frac /= depr_alloc_total_frac;
			depr_alloc_sl_20_frac /= depr_alloc_total_frac;
			depr_alloc_sl_39_frac /= depr_alloc_total_frac;
			depr_alloc_custom_frac /= depr_alloc_total_frac;
		}
		
		double depr_stabas_macrs_5_frac;
		double depr_stabas_macrs_15_frac;
		double depr_stabas_sl_5_frac;
		double depr_stabas_sl_15_frac;
		double depr_stabas_sl_20_frac;
		double depr_stabas_sl_39_frac;
		double depr_stabas_custom_frac;

		if (as_integer("depr_stabas_method")==0) 
		{
			depr_stabas_macrs_5_frac = 1.0;
			depr_stabas_macrs_15_frac = 0.0;
			depr_stabas_sl_5_frac = 0.0;
			depr_stabas_sl_15_frac = 0.0;
			depr_stabas_sl_20_frac = 0.0;
			depr_stabas_sl_39_frac = 0.0;
			depr_stabas_custom_frac = 0.0;
		}
		else
		{
			depr_stabas_macrs_5_frac = depr_alloc_macrs_5_frac;
			depr_stabas_macrs_15_frac = depr_alloc_macrs_15_frac;
			depr_stabas_sl_5_frac = depr_alloc_sl_5_frac;
			depr_stabas_sl_15_frac = depr_alloc_sl_15_frac;
			depr_stabas_sl_20_frac = depr_alloc_sl_20_frac;
			depr_stabas_sl_39_frac = depr_alloc_sl_39_frac;
			depr_stabas_custom_frac = depr_alloc_custom_frac;
		}

		double depr_fedbas_macrs_5_frac;
		double depr_fedbas_macrs_15_frac;
		double depr_fedbas_sl_5_frac;
		double depr_fedbas_sl_15_frac;
		double depr_fedbas_sl_20_frac;
		double depr_fedbas_sl_39_frac;
		double depr_fedbas_custom_frac;

		if (as_integer("depr_fedbas_method")==0) 
		{
			depr_fedbas_macrs_5_frac = 1.0;
			depr_fedbas_macrs_15_frac = 0.0;
			depr_fedbas_sl_5_frac = 0.0;
			depr_fedbas_sl_15_frac = 0.0;
			depr_fedbas_sl_20_frac = 0.0;
			depr_fedbas_sl_39_frac = 0.0;
			depr_fedbas_custom_frac = 0.0;
		}
		else
		{
			depr_fedbas_macrs_5_frac = depr_alloc_macrs_5_frac;
			depr_fedbas_macrs_15_frac = depr_alloc_macrs_15_frac;
			depr_fedbas_sl_5_frac = depr_alloc_sl_5_frac;
			depr_fedbas_sl_15_frac = depr_alloc_sl_15_frac;
			depr_fedbas_sl_20_frac = depr_alloc_sl_20_frac;
			depr_fedbas_sl_39_frac = depr_alloc_sl_39_frac;
			depr_fedbas_custom_frac = depr_alloc_custom_frac;
		}

		double depr_alloc_macrs_5;
		double depr_alloc_macrs_15;
		double depr_alloc_sl_5;
		double depr_alloc_sl_15;
		double depr_alloc_sl_20;
		double depr_alloc_sl_39;
		double depr_alloc_custom;
		double depr_alloc_none;
		double depr_alloc_total;

		double itc_sta_qual_macrs_5_frac = ( as_boolean("depr_itc_sta_macrs_5")  ? 1: 0 ) ;
		double itc_sta_qual_macrs_15_frac = ( as_boolean("depr_itc_sta_macrs_15")  ? 1: 0 ) ;
		double itc_sta_qual_sl_5_frac = ( as_boolean("depr_itc_sta_sl_5")  ? 1: 0 ) ;
		double itc_sta_qual_sl_15_frac = ( as_boolean("depr_itc_sta_sl_15")   ? 1: 0 ) ;
		double itc_sta_qual_sl_20_frac = ( as_boolean("depr_itc_sta_sl_20")  ? 1: 0 ) ;
		double itc_sta_qual_sl_39_frac = ( as_boolean("depr_itc_sta_sl_39")  ? 1: 0 ) ;
		double itc_sta_qual_custom_frac = ( as_boolean("depr_itc_sta_custom")  ? 1: 0 ) ;

		double itc_sta_qual_total;

		double itc_sta_qual_macrs_5;
		double itc_sta_qual_macrs_15;
		double itc_sta_qual_sl_5;
		double itc_sta_qual_sl_15;
		double itc_sta_qual_sl_20;
		double itc_sta_qual_sl_39;
		double itc_sta_qual_custom;

		double itc_sta_percent_maxvalue = as_double("itc_sta_percent_maxvalue");

		double itc_sta_disallow_factor = 0.5;

		double itc_disallow_sta_percent_macrs_5;
		double itc_disallow_sta_percent_macrs_15;
		double itc_disallow_sta_percent_sl_5;
		double itc_disallow_sta_percent_sl_15;
		double itc_disallow_sta_percent_sl_20;
		double itc_disallow_sta_percent_sl_39;
		double itc_disallow_sta_percent_custom;

		double itc_disallow_sta_fixed_macrs_5 = (itc_sta_disallow_factor*itc_sta_qual_macrs_5_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_macrs_15 = (itc_sta_disallow_factor*itc_sta_qual_macrs_15_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_sl_5 = (itc_sta_disallow_factor*itc_sta_qual_sl_5_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_sl_15 = (itc_sta_disallow_factor*itc_sta_qual_sl_15_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_sl_20 = (itc_sta_disallow_factor*itc_sta_qual_sl_20_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_sl_39 = (itc_sta_disallow_factor*itc_sta_qual_sl_39_frac * itc_sta_amount);
		double itc_disallow_sta_fixed_custom = (itc_sta_disallow_factor*itc_sta_qual_custom_frac * itc_sta_amount);

		double itc_fed_qual_macrs_5_frac = ( as_boolean("depr_itc_fed_macrs_5")  ?1: 0 ) ;
		double itc_fed_qual_macrs_15_frac = ( as_boolean("depr_itc_fed_macrs_15")  ? 1: 0 ) ;
		double itc_fed_qual_sl_5_frac = ( as_boolean("depr_itc_fed_sl_5")  ? 1: 0 ) ;
		double itc_fed_qual_sl_15_frac = ( as_boolean("depr_itc_fed_sl_15")   ? 1: 0 ) ;
		double itc_fed_qual_sl_20_frac = ( as_boolean("depr_itc_fed_sl_20")  ? 1: 0 ) ;
		double itc_fed_qual_sl_39_frac = ( as_boolean("depr_itc_fed_sl_39")  ? 1: 0 ) ;
		double itc_fed_qual_custom_frac = ( as_boolean("depr_itc_fed_custom")  ? 1: 0 ) ;

		double itc_fed_qual_total;

		double itc_fed_qual_macrs_5;
		double itc_fed_qual_macrs_15;
		double itc_fed_qual_sl_5;
		double itc_fed_qual_sl_15;
		double itc_fed_qual_sl_20;
		double itc_fed_qual_sl_39;
		double itc_fed_qual_custom;

		double itc_fed_percent_maxvalue = as_double("itc_fed_percent_maxvalue");

		double itc_fed_disallow_factor = 0.5;

		double itc_disallow_fed_percent_macrs_5;
		double itc_disallow_fed_percent_macrs_15;
		double itc_disallow_fed_percent_sl_5;
		double itc_disallow_fed_percent_sl_15;
		double itc_disallow_fed_percent_sl_20;
		double itc_disallow_fed_percent_sl_39;
		double itc_disallow_fed_percent_custom;

		double itc_disallow_fed_fixed_macrs_5 = (itc_fed_disallow_factor*itc_fed_qual_macrs_5_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_macrs_15 = (itc_fed_disallow_factor*itc_fed_qual_macrs_15_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_sl_5 = (itc_fed_disallow_factor*itc_fed_qual_sl_5_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_sl_15 = (itc_fed_disallow_factor*itc_fed_qual_sl_15_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_sl_20 = (itc_fed_disallow_factor*itc_fed_qual_sl_20_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_sl_39 = (itc_fed_disallow_factor*itc_fed_qual_sl_39_frac * itc_fed_amount);
		double itc_disallow_fed_fixed_custom = (itc_fed_disallow_factor*itc_fed_qual_custom_frac * itc_fed_amount);



// Depreciation
// State depreciation
		double depr_sta_reduction_ibi =  (
			( as_boolean("ibi_fed_amount_deprbas_sta")  ? ibi_fed_amount : 0 ) +
			( as_boolean("ibi_fed_percent_deprbas_sta")  ? ibi_fed_per : 0 ) +
			( as_boolean("ibi_sta_amount_deprbas_sta")  ? ibi_sta_amount : 0 ) +
			( as_boolean("ibi_sta_percent_deprbas_sta")  ? ibi_sta_per : 0 ) +
			( as_boolean("ibi_uti_amount_deprbas_sta")  ? ibi_uti_amount : 0 ) +
			( as_boolean("ibi_uti_percent_deprbas_sta")  ? ibi_uti_per : 0 ) +
			( as_boolean("ibi_oth_amount_deprbas_sta")  ? ibi_oth_amount : 0 ) +
			( as_boolean("ibi_oth_percent_deprbas_sta")  ? ibi_oth_per : 0 )
			);

		double depr_sta_reduction_cbi =  (
			( as_boolean("cbi_fed_deprbas_sta")  ? cbi_fed_amount : 0 ) +
			( as_boolean("cbi_sta_deprbas_sta")  ? cbi_sta_amount : 0 ) +
			( as_boolean("cbi_uti_deprbas_sta")  ? cbi_uti_amount : 0 ) +
			( as_boolean("cbi_oth_deprbas_sta")  ? cbi_oth_amount : 0 ) 
			);
		
		double depr_sta_reduction = depr_sta_reduction_ibi + depr_sta_reduction_cbi;

		double depr_stabas_macrs_5;
		double depr_stabas_macrs_15;
		double depr_stabas_sl_5;
		double depr_stabas_sl_15;
		double depr_stabas_sl_20;
		double depr_stabas_sl_39;
		double depr_stabas_custom;

		// ITC reduction
		double itc_fed_percent_deprbas_sta = as_boolean("itc_fed_percent_deprbas_sta") ? 1.0 : 0.0;
		double itc_fed_amount_deprbas_sta = as_boolean("itc_fed_amount_deprbas_sta") ? 1.0 : 0.0;
		double itc_sta_percent_deprbas_sta = as_boolean("itc_sta_percent_deprbas_sta") ? 1.0 : 0.0;
		double itc_sta_amount_deprbas_sta = as_boolean("itc_sta_amount_deprbas_sta") ? 1.0 : 0.0;


		// Bonus depreciation
		double depr_stabas_macrs_5_bonus_frac = ( as_boolean("depr_bonus_sta_macrs_5") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_macrs_15_bonus_frac = ( as_boolean("depr_bonus_sta_macrs_15") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_sl_5_bonus_frac = ( as_boolean("depr_bonus_sta_sl_5") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_sl_15_bonus_frac = ( as_boolean("depr_bonus_sta_sl_15") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_sl_20_bonus_frac = ( as_boolean("depr_bonus_sta_sl_20") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_sl_39_bonus_frac = ( as_boolean("depr_bonus_sta_sl_39") ? as_double("depr_bonus_sta")*0.01 : 0 );
		double depr_stabas_custom_bonus_frac = ( as_boolean("depr_bonus_sta_custom") ? as_double("depr_bonus_sta")*0.01 : 0 );

		double depr_stabas_macrs_5_bonus;
		double depr_stabas_macrs_15_bonus;
		double depr_stabas_sl_5_bonus;
		double depr_stabas_sl_15_bonus;
		double depr_stabas_sl_20_bonus;
		double depr_stabas_sl_39_bonus;
		double depr_stabas_custom_bonus;

		double depr_stabas_total;

		// Federal depreciation
			double depr_fed_reduction_ibi =  (
			( as_boolean("ibi_fed_amount_deprbas_fed")  ? ibi_fed_amount : 0 ) +
			( as_boolean("ibi_fed_percent_deprbas_fed")  ? ibi_fed_per : 0 ) +
			( as_boolean("ibi_sta_amount_deprbas_fed")  ? ibi_sta_amount : 0 ) +
			( as_boolean("ibi_sta_percent_deprbas_fed")  ? ibi_sta_per : 0 ) +
			( as_boolean("ibi_uti_amount_deprbas_fed")  ? ibi_uti_amount : 0 ) +
			( as_boolean("ibi_uti_percent_deprbas_fed")  ? ibi_uti_per : 0 ) +
			( as_boolean("ibi_oth_amount_deprbas_fed")  ? ibi_oth_amount : 0 ) +
			( as_boolean("ibi_oth_percent_deprbas_fed")  ? ibi_oth_per : 0 )
			);

		double depr_fed_reduction_cbi =  (
			( as_boolean("cbi_fed_deprbas_fed")  ? cbi_fed_amount : 0 ) +
			( as_boolean("cbi_sta_deprbas_fed")  ? cbi_sta_amount : 0 ) +
			( as_boolean("cbi_uti_deprbas_fed")  ? cbi_uti_amount : 0 ) +
			( as_boolean("cbi_oth_deprbas_fed")  ? cbi_oth_amount : 0 ) 
			);
		
		double depr_fed_reduction = depr_fed_reduction_ibi + depr_fed_reduction_cbi;

		double depr_fedbas_macrs_5;
		double depr_fedbas_macrs_15;
		double depr_fedbas_sl_5;
		double depr_fedbas_sl_15;
		double depr_fedbas_sl_20;
		double depr_fedbas_sl_39;
		double depr_fedbas_custom;

		// ITC reduction
		double itc_fed_percent_deprbas_fed = as_boolean("itc_fed_percent_deprbas_fed") ? 1.0 : 0.0;
		double itc_fed_amount_deprbas_fed = as_boolean("itc_fed_amount_deprbas_fed") ? 1.0 : 0.0;
		double itc_sta_percent_deprbas_fed = as_boolean("itc_sta_percent_deprbas_fed") ? 1.0 : 0.0;
		double itc_sta_amount_deprbas_fed = as_boolean("itc_sta_amount_deprbas_fed") ? 1.0 : 0.0;

		// Bonus depreciation
		double depr_fedbas_macrs_5_bonus_frac = ( as_boolean("depr_bonus_fed_macrs_5") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_macrs_15_bonus_frac = ( as_boolean("depr_bonus_fed_macrs_15") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_sl_5_bonus_frac = ( as_boolean("depr_bonus_fed_sl_5") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_sl_15_bonus_frac = ( as_boolean("depr_bonus_fed_sl_15") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_sl_20_bonus_frac = ( as_boolean("depr_bonus_fed_sl_20") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_sl_39_bonus_frac = ( as_boolean("depr_bonus_fed_sl_39") ? as_double("depr_bonus_fed")*0.01 : 0 );
		double depr_fedbas_custom_bonus_frac = ( as_boolean("depr_bonus_fed_custom") ? as_double("depr_bonus_fed")*0.01 : 0 );

		double depr_fedbas_macrs_5_bonus;
		double depr_fedbas_macrs_15_bonus;
		double depr_fedbas_sl_5_bonus;
		double depr_fedbas_sl_15_bonus;
		double depr_fedbas_sl_20_bonus;
		double depr_fedbas_sl_39_bonus;
		double depr_fedbas_custom_bonus;

		double depr_fedbas_total;


		double pbi_fed_for_ds_frac = as_boolean("pbi_fed_for_ds") ? 1.0 : 0.0;
		double pbi_sta_for_ds_frac = as_boolean("pbi_sta_for_ds") ? 1.0 : 0.0;
		double pbi_uti_for_ds_frac = as_boolean("pbi_uti_for_ds") ? 1.0 : 0.0;
		double pbi_oth_for_ds_frac = as_boolean("pbi_oth_for_ds") ? 1.0 : 0.0;

		
		//		if (ppa_mode == 0) // iterate to meet flip target by varying ppa price
		double ppa_soln_tolerance = as_double("ppa_soln_tolerance");
		int ppa_soln_max_iteations = as_integer("ppa_soln_max_iterations");
		double flip_target_percent = as_double("flip_target_percent") ;
		int flip_target_year = as_integer("flip_target_year");
		// check for accessing off of the end of cashflow matrix
		if (flip_target_year > nyears) flip_target_year = nyears;
		int flip_year=-1;
		double purchase_of_property;
		bool solved=true;
		double ppa_min=as_double("ppa_soln_min");
		double ppa_max=as_double("ppa_soln_max");
		int its=0;
		double irr_weighting_factor = DBL_MAX;
		bool irr_is_minimally_met = false;
		bool irr_greater_than_target = false;
		double w0=1.0;
		double w1=1.0;
		double x0=ppa_min;
		double x1=ppa_max;
		double ppa_coarse_interval=10; // 10 cents/kWh
		bool ppa_interval_found=false;
		bool ppa_too_large=false;
		bool ppa_interval_reset=true;
		// 12/14/12 - address issue from Eric Lantz - ppa solution when target mode and ppa < 0
		double ppa_old=ppa;


		// debt fraction input
		if (!constant_dscr_mode)
		{
			double debt_frac = as_double("debt_percent")*0.01;

			cost_installed = 
				cost_prefinancing 
				+ constr_total_financing
				+ cost_debt_closing 
				+ cost_other_financing
				+ cf.at(CF_reserve_debtservice, 0) // initially zero - based on p&i
				+ cf.at(CF_reserve_om, 0)
				- ibi_fed_amount 
				- ibi_sta_amount
				- ibi_uti_amount
				- ibi_oth_amount
				- ibi_fed_per
				- ibi_sta_per
				- ibi_uti_per
				- ibi_oth_per
				- cbi_fed_amount
				- cbi_sta_amount
				- cbi_uti_amount
				- cbi_oth_amount;
			cost_installed += debt_frac *cost_installed*cost_debt_fee_frac; // approximate up front fee
			double loan_amount = debt_frac * cost_installed;

			int i_repeat = 0;
			double old_ds_reserve = 0, new_ds_reserve = 0;

			// first year principal payment based on loan moratorium
			ssc_number_t first_principal_payment = 0;

			do
			{
				// first iteration - calculate debt reserve account based on initial installed cost
				old_ds_reserve = new_ds_reserve;
				// debt service reserve
				if (loan_moratorium < 1)
				{
					if (constant_principal)
					{
						if ((term_tenor - loan_moratorium) > 0)
							first_principal_payment = (ssc_number_t)loan_amount / (ssc_number_t)(term_tenor - loan_moratorium);
					}
					else
					{
						first_principal_payment = (ssc_number_t)-ppmt(term_int_rate,       // Rate
							1,           // Period
							(term_tenor - loan_moratorium),   // Number periods
							loan_amount, // Present Value
							0,           // future Value
							0);         // cash flow at end of period
					}
				}
				else
					first_principal_payment = 0;
				cf.at(CF_debt_payment_principal, 1) = first_principal_payment;
				cf.at(CF_debt_payment_interest, 1) = loan_amount * term_int_rate;
				cf.at(CF_reserve_debtservice, 0) = dscr_reserve_months / 12.0 * (cf.at(CF_debt_payment_principal, 1) + cf.at(CF_debt_payment_interest, 1));
				cf.at(CF_funding_debtservice, 0) = cf.at(CF_reserve_debtservice, 0);
				new_ds_reserve = cf.at(CF_reserve_debtservice, 0);

				// update installed cost with approximate debt reserve account for year 0
				cost_installed =
					cost_prefinancing
					+ constr_total_financing
					+ cost_debt_closing
					+ cost_other_financing
					+ cf.at(CF_reserve_debtservice, 0) // initially zero - based on p&i
					+ cf.at(CF_reserve_om, 0)
					- ibi_fed_amount
					- ibi_sta_amount
					- ibi_uti_amount
					- ibi_oth_amount
					- ibi_fed_per
					- ibi_sta_per
					- ibi_uti_per
					- ibi_oth_per
					- cbi_fed_amount
					- cbi_sta_amount
					- cbi_uti_amount
					- cbi_oth_amount;
				cost_debt_upfront = debt_frac * cost_installed * cost_debt_fee_frac; // for cash flow output
				cost_installed += debt_frac *cost_installed*cost_debt_fee_frac;
				loan_amount = debt_frac * cost_installed;
				i_repeat++;
			} while ((fabs(new_ds_reserve - old_ds_reserve) > 1e-3) && (i_repeat < 10));

			if (term_tenor == 0) loan_amount = 0;
//			log(util::format("loan amount =%lg, debt fraction=%lg, adj installed cost=%lg", loan_amount, debt_frac, adjusted_installed_cost), SSC_WARNING);
			for ( i = 1; i <= nyears; i++)
			{
				if (i == 1)
				{
					first_principal_payment = 0;
					cf.at(CF_debt_balance, i - 1) = loan_amount;
					cf.at(CF_debt_payment_interest, i) = loan_amount * term_int_rate;
					if (i > loan_moratorium)
					{
						if (constant_principal)
						{
							if ((term_tenor - loan_moratorium) > 0)	
								first_principal_payment = (ssc_number_t)loan_amount / (ssc_number_t)(term_tenor - loan_moratorium);
						}
						else
						{
							first_principal_payment = (ssc_number_t)-ppmt(term_int_rate,       // Rate
								i,           // Period
								(term_tenor - loan_moratorium),   // Number periods
								loan_amount, // Present Value
								0,           // future Value
								0);         // cash flow at end of period
						}
					}
					cf.at(CF_debt_payment_principal, 1) = first_principal_payment;
					cf.at(CF_debt_balance, i) = cf.at(CF_debt_balance, i - 1) - cf.at(CF_debt_payment_principal, i);
// update reserve account
					cf.at(CF_debt_payment_interest, i) = loan_amount * term_int_rate;
					cf.at(CF_reserve_debtservice, i-1) = dscr_reserve_months / 12.0 * (cf.at(CF_debt_payment_principal, i) + cf.at(CF_debt_payment_interest, i));
					cf.at(CF_funding_debtservice, i-1) = cf.at(CF_reserve_debtservice, i-1);
				}
				else // i > 1 
				{
					if (i <= term_tenor)
					{
						cf.at(CF_debt_payment_interest, i) = term_int_rate * cf.at(CF_debt_balance, i - 1);
						if (i > loan_moratorium)
						{
							if (constant_principal)
							{
								if ((term_tenor - loan_moratorium) > 0) 
									cf.at(CF_debt_payment_principal, i) = loan_amount / (term_tenor - loan_moratorium);
							}
							else
							{
								if (term_int_rate != 0.0)
								{
									cf.at(CF_debt_payment_principal, i) = term_int_rate * loan_amount / (1 - pow((1 + term_int_rate), -(term_tenor - loan_moratorium)))
										- cf.at(CF_debt_payment_interest, i);
								}
								else
								{
									cf.at(CF_debt_payment_principal, i) = loan_amount / (term_tenor - loan_moratorium) - cf.at(CF_debt_payment_interest, i);
								}
							}
						}
						else
						{
								cf.at(CF_debt_payment_principal, i) = 0;
						}

						// debt service reserve
						cf.at(CF_reserve_debtservice, i - 1) = dscr_reserve_months / 12.0 *		(cf.at(CF_debt_payment_principal, i) + cf.at(CF_debt_payment_interest, i));
						cf.at(CF_funding_debtservice, i - 1) = cf.at(CF_reserve_debtservice, i - 1);
						cf.at(CF_funding_debtservice, i - 1) -= cf.at(CF_reserve_debtservice, i	 - 2);
						if (i == term_tenor) cf.at(CF_disbursement_debtservice, i) = 0 - cf.at		(CF_reserve_debtservice, i - 1);

					}
					cf.at(CF_debt_balance, i) = cf.at(CF_debt_balance, i - 1) - cf.at(CF_debt_payment_principal, i);

				}

				cf.at(CF_debt_payment_total, i) = cf.at(CF_debt_payment_principal, i) + cf.at(CF_debt_payment_interest, i);
				cf.at(CF_debt_size, i) = cf.at(CF_debt_payment_principal, i);
//				log(util::format("year=%d, debt balance =%lg, debt interest=%lg, debt principal=%lg, total debt payment=%lg, debt size=%lg", i, cf.at(CF_debt_balance, i), cf.at(CF_debt_payment_interest, i), cf.at(CF_debt_payment_principal, i), cf.at(CF_debt_payment_total, i), cf.at(CF_debt_size, i)), SSC_WARNING);

				size_of_debt += cf.at(CF_debt_size, i);
			}
			cf.at(CF_debt_balance, 0) = loan_amount;
//			log(util::format("size of debt=%lg.", size_of_debt), SSC_WARNING);

		}

//		log(util::format("before loop  - size of debt =%lg .",	size_of_debt),	SSC_WARNING);




/***************** begin iterative solution *********************************************************************/

	do
	{

		flip_year=-1;
		cash_for_debt_service=0;
		pv_cafds=0;
		if (constant_dscr_mode)	size_of_debt=0;
		if (ppa_interval_found)	ppa = (w0*x1+w1*x0)/(w0 + w1);

		// debt pre calculation
		for (i=1; i<=nyears; i++)
		{			
		// Project partial income statement			
			// energy_value = DHF Total PPA Revenue (cents/kWh)
			cf.at(CF_ppa_price,i) = ppa * pow( 1 + ppa_escalation, i-1 ); // ppa_mode==1
//			cf.at(CF_energy_value,i) = cf.at(CF_energy_net,i) * cf.at(CF_ppa_price,i) /100.0;
			// dispatch
			cf.at(CF_energy_value, i) = cf.at(CF_ppa_price, i) / 100.0 *(
				m_disp_calcs.tod_energy_value(i));

//			log(util::format("year %d : energy value =%lg", i, m_disp_calcs.tod_energy_value(i)), SSC_WARNING);
			// total revenue
			cf.at(CF_total_revenue,i) = cf.at(CF_energy_value,i) +
				pbi_fed_for_ds_frac * cf.at(CF_pbi_fed,i) +
				pbi_sta_for_ds_frac * cf.at(CF_pbi_sta,i) +
				pbi_uti_for_ds_frac * cf.at(CF_pbi_uti,i) +
				pbi_oth_for_ds_frac * cf.at(CF_pbi_oth,i) +
				cf.at(CF_net_salvage_value,i);

			cf.at(CF_ebitda,i) = cf.at(CF_total_revenue,i) - cf.at(CF_operating_expenses,i);
		
	
		} // end of debt precalculation.

		// receivables precalculation need future energy value so outside previous loop
		if (nyears>0)
		{
			cf.at(CF_reserve_receivables, 0) = months_receivables_reserve_frac * cf.at(CF_energy_value, 1);
			cf.at(CF_funding_receivables, 0) = cf.at(CF_reserve_receivables, 0);
			for (i = 1; i<nyears; i++)
			{
				cf.at(CF_reserve_receivables, i) = months_receivables_reserve_frac * cf.at(CF_energy_value, i + 1);
				cf.at(CF_funding_receivables, i) = cf.at(CF_reserve_receivables, i) - cf.at(CF_reserve_receivables, i - 1);
			}
			cf.at(CF_disbursement_receivables, nyears) = -cf.at(CF_reserve_receivables, nyears - 1);
		}
		for (i = 0; i <= nyears; i++)
		{
			cf.at(CF_project_receivablesra, i) = -cf.at(CF_funding_receivables, i) - cf.at(CF_disbursement_receivables, i);
			// include receivables.
			if (i <= term_tenor)
			{
				cf.at(CF_cash_for_ds, i) = cf.at(CF_ebitda, i) - cf.at(CF_funding_equip1, i) - cf.at(CF_funding_equip2, i) - cf.at(CF_funding_equip3, i) - cf.at(CF_funding_receivables, i);
				cash_for_debt_service += cf.at(CF_cash_for_ds, i);
				if (i <= 1)
					cf.at(CF_pv_interest_factor, i) = 1.0 / (1.0 + term_int_rate);
				else
					cf.at(CF_pv_interest_factor, i) = cf.at(CF_pv_interest_factor, i - 1) / (1.0 + term_int_rate);
				cf.at(CF_pv_cash_for_ds, i) = cf.at(CF_pv_interest_factor, i) * cf.at(CF_cash_for_ds, i);
				pv_cafds += cf.at(CF_pv_cash_for_ds, i);
				if (constant_dscr_mode)
				{
					if (dscr != 0) cf.at(CF_debt_size, i) = cf.at(CF_pv_cash_for_ds, i) / dscr;
					size_of_debt += cf.at(CF_debt_size, i);
				}
			}
		}

		/*
		// DSCR calculations
		for (i = 0; i <= nyears; i++)
		{
		if (cf.at(CF_debt_payment_total, i) == 0.0) cf.at(CF_pretax_dscr, i) = 0; //cf.at(CF_pretax_dscr, i) = std::numeric_limits<double>::quiet_NaN();
		else cf.at(CF_pretax_dscr, i) = cf.at(CF_cash_for_ds, i) / cf.at(CF_debt_payment_total, i);
		}

		*/
		if (constant_dscr_mode)
		{
			cf.at(CF_debt_balance, 0) = size_of_debt;

			for (i = 1; ((i <= nyears) && (i <= term_tenor)); i++)
			{
				cf.at(CF_debt_payment_interest, i) = cf.at(CF_debt_balance, i - 1) * term_int_rate;
					if (dscr != 0)
						cf.at(CF_debt_payment_total, i) = cf.at(CF_cash_for_ds, i) / dscr;
				else
					cf.at(CF_debt_payment_total, i) = cf.at(CF_debt_payment_interest, i);
				cf.at(CF_debt_payment_principal, i) = cf.at(CF_debt_payment_total, i) - cf.at(CF_debt_payment_interest, i);
				cf.at(CF_debt_balance, i) = cf.at(CF_debt_balance, i - 1) - cf.at(CF_debt_payment_principal, i);
			}


			// debt service reserve
			for (i = 1; ((i <= nyears) && (i <= term_tenor)); i++)
			{
				cf.at(CF_reserve_debtservice, i - 1) = dscr_reserve_months / 12.0 * (cf.at(CF_debt_payment_principal, i) + cf.at(CF_debt_payment_interest, i));
				cf.at(CF_funding_debtservice, i - 1) = cf.at(CF_reserve_debtservice, i - 1);
				if (i > 1) cf.at(CF_funding_debtservice, i - 1) -= cf.at(CF_reserve_debtservice, i - 2);
				if (i == term_tenor) cf.at(CF_disbursement_debtservice, i) = 0 - cf.at(CF_reserve_debtservice, i - 1);
			}
		}

		// total reserves
		for (i=0; i<=nyears; i++)
			cf.at(CF_reserve_total,i) = 
				cf.at(CF_reserve_debtservice,i) +
				cf.at(CF_reserve_om, i) +
				cf.at(CF_reserve_receivables, i) +
				cf.at(CF_reserve_equip1, i) +
				cf.at(CF_reserve_equip2,i) +
				cf.at(CF_reserve_equip3,i);
		for (i=1; i<=nyears; i++)
			cf.at(CF_reserve_interest,i) = reserves_interest * cf.at(CF_reserve_total,i-1);

//		if (constant_dscr_mode)
//		{
			cost_financing =
				cost_debt_closing +
				cost_debt_fee_frac * size_of_debt +
				cost_other_financing +
				cf.at(CF_reserve_debtservice, 0) +
				constr_total_financing +
				cf.at(CF_reserve_om, 0) +
				cf.at(CF_reserve_receivables, 0);

			cost_debt_upfront = cost_debt_fee_frac * size_of_debt; // cpg added this to make cash flow consistent with single_owner.xlsx

			cost_installed = cost_prefinancing + cost_financing
				- ibi_fed_amount
				- ibi_sta_amount
				- ibi_uti_amount
				- ibi_oth_amount
				- ibi_fed_per
				- ibi_sta_per
				- ibi_uti_per
				- ibi_oth_per
				- cbi_fed_amount
				- cbi_sta_amount
				- cbi_uti_amount
				- cbi_oth_amount;
			
//		}
		depr_alloc_total = depr_alloc_total_frac * cost_installed;
		depr_alloc_macrs_5 = depr_alloc_macrs_5_frac * depr_alloc_total;
		depr_alloc_macrs_15 = depr_alloc_macrs_15_frac * depr_alloc_total;
		depr_alloc_sl_5 = depr_alloc_sl_5_frac * depr_alloc_total;
		depr_alloc_sl_15 = depr_alloc_sl_15_frac * depr_alloc_total;
		depr_alloc_sl_20 = depr_alloc_sl_20_frac * depr_alloc_total;
		depr_alloc_sl_39 = depr_alloc_sl_39_frac * depr_alloc_total;
		depr_alloc_custom = depr_alloc_custom_frac * depr_alloc_total;
		depr_alloc_none = depr_alloc_none_frac * depr_alloc_total;

		itc_sta_qual_macrs_5 = itc_sta_qual_macrs_5_frac * ( depr_alloc_macrs_5 - depr_stabas_macrs_5_frac * depr_sta_reduction);
		itc_sta_qual_macrs_15 = itc_sta_qual_macrs_15_frac * ( depr_alloc_macrs_15 - depr_stabas_macrs_15_frac * depr_sta_reduction);
		itc_sta_qual_sl_5 = itc_sta_qual_sl_5_frac * ( depr_alloc_sl_5 - depr_stabas_sl_5_frac * depr_sta_reduction);
		itc_sta_qual_sl_15 = itc_sta_qual_sl_15_frac * ( depr_alloc_sl_15 - depr_stabas_sl_15_frac * depr_sta_reduction);
		itc_sta_qual_sl_20 = itc_sta_qual_sl_20_frac * ( depr_alloc_sl_20 - depr_stabas_sl_20_frac * depr_sta_reduction);
		itc_sta_qual_sl_39 = itc_sta_qual_sl_39_frac * ( depr_alloc_sl_39 - depr_stabas_sl_39_frac * depr_sta_reduction);
		itc_sta_qual_custom = itc_sta_qual_custom_frac * ( depr_alloc_custom - depr_stabas_custom_frac * depr_sta_reduction);

		itc_sta_qual_total = itc_sta_qual_macrs_5 + itc_sta_qual_macrs_15 + itc_sta_qual_sl_5 +itc_sta_qual_sl_15 +itc_sta_qual_sl_20 + itc_sta_qual_sl_39 + itc_sta_qual_custom;

		itc_sta_per = min(itc_sta_percent_maxvalue,itc_sta_frac*itc_sta_qual_total);

		if (itc_sta_qual_total > 0)
		{
			itc_disallow_sta_percent_macrs_5 = itc_sta_qual_macrs_5_frac * (itc_sta_disallow_factor * itc_sta_qual_macrs_5 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_macrs_15 = itc_sta_qual_macrs_15_frac * (itc_sta_disallow_factor * itc_sta_qual_macrs_15 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_sl_5 = itc_sta_qual_sl_5_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_5 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_sl_15 = itc_sta_qual_sl_15_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_15 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_sl_20 = itc_sta_qual_sl_20_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_20 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_sl_39 = itc_sta_qual_sl_39_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_39 / itc_sta_qual_total * itc_sta_per);
			itc_disallow_sta_percent_custom = itc_sta_qual_custom_frac * (itc_sta_disallow_factor * itc_sta_qual_custom / itc_sta_qual_total * itc_sta_per);

			itc_disallow_sta_fixed_macrs_5 = itc_sta_qual_macrs_5_frac * (itc_sta_disallow_factor * itc_sta_qual_macrs_5 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_macrs_15 = itc_sta_qual_macrs_15_frac * (itc_sta_disallow_factor * itc_sta_qual_macrs_15 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_sl_5 = itc_sta_qual_sl_5_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_5 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_sl_15 = itc_sta_qual_sl_15_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_15 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_sl_20 = itc_sta_qual_sl_20_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_20 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_sl_39 = itc_sta_qual_sl_39_frac * (itc_sta_disallow_factor * itc_sta_qual_sl_39 / itc_sta_qual_total * itc_sta_amount);
			itc_disallow_sta_fixed_custom = itc_sta_qual_custom_frac * (itc_sta_disallow_factor * itc_sta_qual_custom / itc_sta_qual_total * itc_sta_amount);
		}
		else
		{
			itc_disallow_sta_percent_macrs_5 = 0;
			itc_disallow_sta_percent_macrs_15 = 0;
			itc_disallow_sta_percent_sl_5 = 0;
			itc_disallow_sta_percent_sl_15 = 0;
			itc_disallow_sta_percent_sl_20 = 0;
			itc_disallow_sta_percent_sl_39 = 0;
			itc_disallow_sta_percent_custom = 0;

			itc_disallow_sta_fixed_macrs_5 = 0;
			itc_disallow_sta_fixed_macrs_15 = 0;
			itc_disallow_sta_fixed_sl_5 = 0;
			itc_disallow_sta_fixed_sl_15 = 0;
			itc_disallow_sta_fixed_sl_20 = 0;
			itc_disallow_sta_fixed_sl_39 = 0;
			itc_disallow_sta_fixed_custom = 0;
		}

		itc_fed_qual_macrs_5 = itc_fed_qual_macrs_5_frac * ( depr_alloc_macrs_5 - depr_fedbas_macrs_5_frac * depr_fed_reduction);
		itc_fed_qual_macrs_15 = itc_fed_qual_macrs_15_frac * ( depr_alloc_macrs_15 - depr_fedbas_macrs_15_frac * depr_fed_reduction);
		itc_fed_qual_sl_5 = itc_fed_qual_sl_5_frac * ( depr_alloc_sl_5 - depr_fedbas_sl_5_frac * depr_fed_reduction);
		itc_fed_qual_sl_15 = itc_fed_qual_sl_15_frac * ( depr_alloc_sl_15 - depr_fedbas_sl_15_frac * depr_fed_reduction);
		itc_fed_qual_sl_20 = itc_fed_qual_sl_20_frac * ( depr_alloc_sl_20 - depr_fedbas_sl_20_frac * depr_fed_reduction);
		itc_fed_qual_sl_39 = itc_fed_qual_sl_39_frac * ( depr_alloc_sl_39 - depr_fedbas_sl_39_frac * depr_fed_reduction);
		itc_fed_qual_custom = itc_fed_qual_custom_frac * ( depr_alloc_custom - depr_fedbas_custom_frac * depr_fed_reduction);

		itc_fed_qual_total = itc_fed_qual_macrs_5 + itc_fed_qual_macrs_15 + itc_fed_qual_sl_5 +itc_fed_qual_sl_15 +itc_fed_qual_sl_20 + itc_fed_qual_sl_39 + itc_fed_qual_custom;

		itc_fed_per = min(itc_fed_percent_maxvalue,itc_fed_frac*itc_fed_qual_total);

		if (itc_fed_qual_total > 0)
		{
			itc_disallow_fed_percent_macrs_5 = itc_fed_qual_macrs_5_frac * (itc_fed_disallow_factor * itc_fed_qual_macrs_5 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_macrs_15 = itc_fed_qual_macrs_15_frac * (itc_fed_disallow_factor * itc_fed_qual_macrs_15 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_sl_5 = itc_fed_qual_sl_5_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_5 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_sl_15 = itc_fed_qual_sl_15_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_15 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_sl_20 = itc_fed_qual_sl_20_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_20 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_sl_39 = itc_fed_qual_sl_39_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_39 / itc_fed_qual_total * itc_fed_per);
			itc_disallow_fed_percent_custom = itc_fed_qual_custom_frac * (itc_fed_disallow_factor * itc_fed_qual_custom / itc_fed_qual_total * itc_fed_per);

			itc_disallow_fed_fixed_macrs_5 = itc_fed_qual_macrs_5_frac * (itc_fed_disallow_factor * itc_fed_qual_macrs_5 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_macrs_15 = itc_fed_qual_macrs_15_frac * (itc_fed_disallow_factor * itc_fed_qual_macrs_15 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_sl_5 = itc_fed_qual_sl_5_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_5 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_sl_15 = itc_fed_qual_sl_15_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_15 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_sl_20 = itc_fed_qual_sl_20_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_20 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_sl_39 = itc_fed_qual_sl_39_frac * (itc_fed_disallow_factor * itc_fed_qual_sl_39 / itc_fed_qual_total * itc_fed_amount);
			itc_disallow_fed_fixed_custom = itc_fed_qual_custom_frac * (itc_fed_disallow_factor * itc_fed_qual_custom / itc_fed_qual_total * itc_fed_amount);
		}
		else
		{
			itc_disallow_fed_percent_macrs_5 = 0;
			itc_disallow_fed_percent_macrs_15 = 0;
			itc_disallow_fed_percent_sl_5 = 0;
			itc_disallow_fed_percent_sl_15 = 0;
			itc_disallow_fed_percent_sl_20 = 0;
			itc_disallow_fed_percent_sl_39 = 0;
			itc_disallow_fed_percent_custom = 0;

			itc_disallow_fed_fixed_macrs_5 = 0;
			itc_disallow_fed_fixed_macrs_15 = 0;
			itc_disallow_fed_fixed_sl_5 = 0;
			itc_disallow_fed_fixed_sl_15 = 0;
			itc_disallow_fed_fixed_sl_20 = 0;
			itc_disallow_fed_fixed_sl_39 = 0;
			itc_disallow_fed_fixed_custom = 0;
		}

		itc_fed_total = itc_fed_amount + itc_fed_per;
		itc_sta_total = itc_sta_amount + itc_sta_per;
		itc_total = itc_fed_total + itc_sta_total;

// Depreciation
// State depreciation
		depr_stabas_macrs_5 = depr_alloc_macrs_5 - depr_stabas_macrs_5_frac * depr_sta_reduction;
		depr_stabas_macrs_15 = depr_alloc_macrs_15 - depr_stabas_macrs_15_frac * depr_sta_reduction;
		depr_stabas_sl_5 = depr_alloc_sl_5 - depr_stabas_sl_5_frac * depr_sta_reduction;
		depr_stabas_sl_15 = depr_alloc_sl_15 - depr_stabas_sl_15_frac * depr_sta_reduction;
		depr_stabas_sl_20 = depr_alloc_sl_20 - depr_stabas_sl_20_frac * depr_sta_reduction;
		depr_stabas_sl_39 = depr_alloc_sl_39 - depr_stabas_sl_39_frac * depr_sta_reduction;
		depr_stabas_custom = depr_alloc_custom - depr_stabas_custom_frac * depr_sta_reduction;

		// ITC reduction
		depr_stabas_macrs_5 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_macrs_5 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_macrs_5 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_macrs_5 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_macrs_5 );

		depr_stabas_macrs_15 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_macrs_15 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_macrs_15 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_macrs_15 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_macrs_15 );

		depr_stabas_sl_5 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_5 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_5 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_5 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_5 );

		depr_stabas_sl_15 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_15 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_15 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_15 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_15 );

		depr_stabas_sl_20 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_20 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_20 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_20 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_20 );

		depr_stabas_sl_39 -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_39 +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_39 +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_39 +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_39 );

		depr_stabas_custom -= (itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_custom +
								itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_custom +
								itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_custom +
								itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_custom );

		// Bonus depreciation
		depr_stabas_macrs_5_bonus = depr_stabas_macrs_5_bonus_frac * depr_stabas_macrs_5;
		depr_stabas_macrs_15_bonus = depr_stabas_macrs_15_bonus_frac * depr_stabas_macrs_15;
		depr_stabas_sl_5_bonus = depr_stabas_sl_5_bonus_frac * depr_stabas_sl_5;
		depr_stabas_sl_15_bonus = depr_stabas_sl_15_bonus_frac * depr_stabas_sl_15;
		depr_stabas_sl_20_bonus = depr_stabas_sl_20_bonus_frac * depr_stabas_sl_20;
		depr_stabas_sl_39_bonus = depr_stabas_sl_39_bonus_frac * depr_stabas_sl_39;
		depr_stabas_custom_bonus = depr_stabas_custom_bonus_frac * depr_stabas_custom;

		depr_stabas_macrs_5 -= depr_stabas_macrs_5_bonus;
		depr_stabas_macrs_15 -= depr_stabas_macrs_15_bonus;
		depr_stabas_sl_5 -= depr_stabas_sl_5_bonus;
		depr_stabas_sl_15 -= depr_stabas_sl_15_bonus;
		depr_stabas_sl_20 -= depr_stabas_sl_20_bonus;
		depr_stabas_sl_39 -= depr_stabas_sl_39_bonus;
		depr_stabas_custom -= depr_stabas_custom_bonus;
		
		depr_stabas_total = depr_stabas_macrs_5 + depr_stabas_macrs_15 + depr_stabas_sl_5 + depr_stabas_sl_15 + depr_stabas_sl_20 + depr_stabas_sl_39 + depr_stabas_custom;

		// Federal depreciation
		depr_fedbas_macrs_5 = depr_alloc_macrs_5 - depr_fedbas_macrs_5_frac * depr_fed_reduction;
		depr_fedbas_macrs_15 = depr_alloc_macrs_15 - depr_fedbas_macrs_15_frac * depr_fed_reduction;
		depr_fedbas_sl_5 = depr_alloc_sl_5 - depr_fedbas_sl_5_frac * depr_fed_reduction;
		depr_fedbas_sl_15 = depr_alloc_sl_15 - depr_fedbas_sl_15_frac * depr_fed_reduction;
		depr_fedbas_sl_20 = depr_alloc_sl_20 - depr_fedbas_sl_20_frac * depr_fed_reduction;
		depr_fedbas_sl_39 = depr_alloc_sl_39 - depr_fedbas_sl_39_frac * depr_fed_reduction;
		depr_fedbas_custom = depr_alloc_custom - depr_fedbas_custom_frac * depr_fed_reduction;

		// ITC reduction
		depr_fedbas_macrs_5 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_macrs_5 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_macrs_5 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_macrs_5 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_macrs_5 );

		depr_fedbas_macrs_15 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_macrs_15 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_macrs_15 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_macrs_15 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_macrs_15 );

		depr_fedbas_sl_5 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_5 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_5 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_5 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_5 );

		depr_fedbas_sl_15 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_15 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_15 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_15 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_15 );

		depr_fedbas_sl_20 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_20 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_20 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_20 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_20 );

		depr_fedbas_sl_39 -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_39 +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_39 +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_39 +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_39 );

		depr_fedbas_custom -= (itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_custom +
								itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_custom +
								itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_custom +
								itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_custom );

		// Bonus depreciation
		depr_fedbas_macrs_5_bonus = depr_fedbas_macrs_5_bonus_frac * depr_fedbas_macrs_5;
		depr_fedbas_macrs_15_bonus = depr_fedbas_macrs_15_bonus_frac * depr_fedbas_macrs_15;
		depr_fedbas_sl_5_bonus = depr_fedbas_sl_5_bonus_frac * depr_fedbas_sl_5;
		depr_fedbas_sl_15_bonus = depr_fedbas_sl_15_bonus_frac * depr_fedbas_sl_15;
		depr_fedbas_sl_20_bonus = depr_fedbas_sl_20_bonus_frac * depr_fedbas_sl_20;
		depr_fedbas_sl_39_bonus = depr_fedbas_sl_39_bonus_frac * depr_fedbas_sl_39;
		depr_fedbas_custom_bonus = depr_fedbas_custom_bonus_frac * depr_fedbas_custom;

		depr_fedbas_macrs_5 -= depr_fedbas_macrs_5_bonus;
		depr_fedbas_macrs_15 -= depr_fedbas_macrs_15_bonus;
		depr_fedbas_sl_5 -= depr_fedbas_sl_5_bonus;
		depr_fedbas_sl_15 -= depr_fedbas_sl_15_bonus;
		depr_fedbas_sl_20 -= depr_fedbas_sl_20_bonus;
		depr_fedbas_sl_39 -= depr_fedbas_sl_39_bonus;
		depr_fedbas_custom -= depr_fedbas_custom_bonus;
		
		depr_fedbas_total = depr_fedbas_macrs_5 + depr_fedbas_macrs_15 + depr_fedbas_sl_5 + depr_fedbas_sl_15 + depr_fedbas_sl_20 + depr_fedbas_sl_39 + depr_fedbas_custom;

		purchase_of_property = -cost_installed + cf.at(CF_reserve_debtservice, 0) + cf.at(CF_reserve_om, 0) + cf.at(CF_reserve_receivables, 0);
//		issuance_of_equity = cost_installed - (size_of_debt + ibi_total + cbi_total);
		issuance_of_equity = cost_installed - size_of_debt;

		for (i=0; i<=nyears; i++)
		{
//			cf.at(CF_return_on_equity_dollars, i) = issuance_of_equity * cf.at(CF_return_on_equity_input, i);
			cf.at(CF_return_on_equity_dollars, i) = (issuance_of_equity - cf.at(CF_reserve_receivables,0)) * cf.at(CF_return_on_equity_input, i);
			if (cf.at(CF_energy_net, i) != 0)
				cf.at(CF_return_on_equity, i) = cf.at(CF_return_on_equity_dollars, i) / cf.at(CF_energy_net, i);
			//			cf.at(CF_project_operating_activities,i) = cf.at(CF_ebitda,i) + cf.at(CF_pbi_total,i) + cf.at(CF_reserve_interest,i) - cf.at(CF_debt_payment_interest,i);
			cf.at(CF_project_operating_activities,i) = cf.at(CF_ebitda,i) + cf.at(CF_reserve_interest,i) - cf.at(CF_debt_payment_interest,i) +
				(1.0 - pbi_fed_for_ds_frac) * cf.at(CF_pbi_fed,i) +
				(1-0 - pbi_sta_for_ds_frac) * cf.at(CF_pbi_sta,i) +
				(1-0 - pbi_uti_for_ds_frac) * cf.at(CF_pbi_uti,i) +
				(1-0 - pbi_oth_for_ds_frac) * cf.at(CF_pbi_oth,i);
			cf.at(CF_project_dsra,i) = -cf.at(CF_funding_debtservice,i) - cf.at(CF_disbursement_debtservice,i);
			cf.at(CF_project_ra,i) =
				cf.at(CF_project_dsra,i) +
				cf.at(CF_project_wcra, i) +
				cf.at(CF_project_receivablesra, i) +
				cf.at(CF_project_me1ra, i) +
				cf.at(CF_project_me2ra,i) +
				cf.at(CF_project_me3ra,i);
			cf.at(CF_project_me1cs,i) = cf.at(CF_disbursement_equip1,i);
			cf.at(CF_project_me2cs,i) = cf.at(CF_disbursement_equip2,i);
			cf.at(CF_project_me3cs,i) = cf.at(CF_disbursement_equip3,i);
			cf.at(CF_project_mecs,i) =
				cf.at(CF_project_me1cs,i) +
				cf.at(CF_project_me2cs,i) +
				cf.at(CF_project_me3cs,i);
			cf.at(CF_project_investing_activities,i) = cf.at(CF_project_ra,i) + cf.at(CF_project_mecs,i);
			if (i==0) cf.at(CF_project_investing_activities,i) += purchase_of_property;

			cf.at(CF_project_financing_activities,i) = -cf.at(CF_debt_payment_principal,i);
			if (i==0) cf.at(CF_project_financing_activities,i) += issuance_of_equity + size_of_debt + ibi_total + cbi_total;

			cf.at(CF_pretax_cashflow,i) = cf.at(CF_project_operating_activities,i) + cf.at(CF_project_investing_activities,i) + cf.at(CF_project_financing_activities,i);

			cf.at(CF_project_return_pretax,i) = cf.at(CF_pretax_cashflow,i);
			if (i==0) cf.at(CF_project_return_pretax,i) -= (issuance_of_equity); 

			cf.at(CF_project_return_pretax_irr,i) = irr(CF_project_return_pretax,i)*100.0;
			cf.at(CF_project_return_pretax_npv,i) = npv(CF_project_return_pretax,i,nom_discount_rate) +  cf.at(CF_project_return_pretax,0) ;

			cf.at(CF_project_return_aftertax_cash,i) = cf.at(CF_project_return_pretax,i);
		}


		cf.at(CF_project_return_aftertax,0) = cf.at(CF_project_return_aftertax_cash,0);
		cf.at(CF_project_return_aftertax_irr,0) = irr(CF_project_return_aftertax_tax,0)*100.0;
		cf.at(CF_project_return_aftertax_max_irr,0) = cf.at(CF_project_return_aftertax_irr,0);
		cf.at(CF_project_return_aftertax_npv,0) = cf.at(CF_project_return_aftertax,0) ;


		for (i=1;i<=nyears;i++)
		{
			cf.at(CF_stadepr_macrs_5,i) = cf.at(CF_macrs_5_frac,i) * depr_stabas_macrs_5;
			cf.at(CF_stadepr_macrs_15,i) = cf.at(CF_macrs_15_frac,i) * depr_stabas_macrs_15;
			cf.at(CF_stadepr_sl_5,i) = cf.at(CF_sl_5_frac,i) * depr_stabas_sl_5;
			cf.at(CF_stadepr_sl_15,i) = cf.at(CF_sl_15_frac,i) * depr_stabas_sl_15;
			cf.at(CF_stadepr_sl_20,i) = cf.at(CF_sl_20_frac,i) * depr_stabas_sl_20;
			cf.at(CF_stadepr_sl_39,i) = cf.at(CF_sl_39_frac,i) * depr_stabas_sl_39;
			cf.at(CF_stadepr_custom,i) = cf.at(CF_custom_frac,i) * depr_stabas_custom;


			cf.at(CF_stadepr_total,i)=
				cf.at(CF_stadepr_macrs_5,i)+
				cf.at(CF_stadepr_macrs_15,i)+
				cf.at(CF_stadepr_sl_5,i)+
				cf.at(CF_stadepr_sl_15,i)+
				cf.at(CF_stadepr_sl_20,i)+
				cf.at(CF_stadepr_sl_39,i)+
				cf.at(CF_stadepr_custom,i)+
				cf.at(CF_stadepr_me1,i)+
				cf.at(CF_stadepr_me2,i)+
				cf.at(CF_stadepr_me3,i);

			if (i==1) cf.at(CF_stadepr_total,i) += ( depr_stabas_macrs_5_bonus +depr_stabas_macrs_15_bonus + depr_stabas_sl_5_bonus + depr_stabas_sl_15_bonus + depr_stabas_sl_20_bonus + depr_stabas_sl_39_bonus + depr_stabas_custom_bonus);
			cf.at(CF_statax_income_prior_incentives,i)=
				cf.at(CF_ebitda,i) + 
				cf.at(CF_reserve_interest,i) -
				cf.at(CF_debt_payment_interest,i) -
				cf.at(CF_stadepr_total,i);



			// pbi in ebitda - so remove if non-taxable
			// 5/1/11
			cf.at(CF_statax_income_with_incentives,i) = cf.at(CF_statax_income_prior_incentives,i) + cf.at(CF_statax_taxable_incentives,i);
			cf.at(CF_statax, i) = -cf.at(CF_state_tax_frac, i) * cf.at(CF_statax_income_with_incentives, i);

// federal 
			cf.at(CF_feddepr_macrs_5,i) = cf.at(CF_macrs_5_frac,i) * depr_fedbas_macrs_5;
			cf.at(CF_feddepr_macrs_15,i) = cf.at(CF_macrs_15_frac,i) * depr_fedbas_macrs_15;
			cf.at(CF_feddepr_sl_5,i) = cf.at(CF_sl_5_frac,i) * depr_fedbas_sl_5;
			cf.at(CF_feddepr_sl_15,i) = cf.at(CF_sl_15_frac,i) * depr_fedbas_sl_15;
			cf.at(CF_feddepr_sl_20,i) = cf.at(CF_sl_20_frac,i) * depr_fedbas_sl_20;
			cf.at(CF_feddepr_sl_39,i) = cf.at(CF_sl_39_frac,i) * depr_fedbas_sl_39;
			cf.at(CF_feddepr_custom,i) = cf.at(CF_custom_frac,i) * depr_fedbas_custom;
			cf.at(CF_feddepr_total,i)=
				cf.at(CF_feddepr_macrs_5,i)+
				cf.at(CF_feddepr_macrs_15,i)+
				cf.at(CF_feddepr_sl_5,i)+
				cf.at(CF_feddepr_sl_15,i)+
				cf.at(CF_feddepr_sl_20,i)+
				cf.at(CF_feddepr_sl_39,i)+
				cf.at(CF_feddepr_custom,i)+
				cf.at(CF_feddepr_me1,i)+
				cf.at(CF_feddepr_me2,i)+
				cf.at(CF_feddepr_me3,i);
			if (i==1) cf.at(CF_feddepr_total,i) += ( depr_fedbas_macrs_5_bonus +depr_fedbas_macrs_15_bonus + depr_fedbas_sl_5_bonus + depr_fedbas_sl_15_bonus + depr_fedbas_sl_20_bonus + depr_fedbas_sl_39_bonus + depr_fedbas_custom_bonus);
			cf.at(CF_fedtax_income_prior_incentives,i)=
				cf.at(CF_ebitda,i) + 
				cf.at(CF_reserve_interest,i) -
				cf.at(CF_debt_payment_interest,i) -
				cf.at(CF_feddepr_total,i) +
				cf.at(CF_statax,i) +
				cf.at(CF_ptc_sta,i);
			if (i==1) cf.at(CF_fedtax_income_prior_incentives,i) += itc_sta_total;


			// pbi in ebitda - so remove if non-taxable
			// 5/1/11
			cf.at(CF_fedtax_income_with_incentives,i) = cf.at(CF_fedtax_income_prior_incentives,i) + cf.at(CF_fedtax_taxable_incentives,i);
			cf.at(CF_fedtax, i) = -cf.at(CF_federal_tax_frac, i) * cf.at(CF_fedtax_income_with_incentives, i);

			cf.at(CF_project_return_aftertax,i) = 
				cf.at(CF_project_return_aftertax_cash,i) +
				cf.at(CF_ptc_fed,i) + cf.at(CF_ptc_sta,i) +
				cf.at(CF_statax,i) + cf.at(CF_fedtax,i);
			if (i==1) cf.at(CF_project_return_aftertax,i) += itc_total;

			cf.at(CF_project_return_aftertax_irr,i) = irr(CF_project_return_aftertax,i)*100.0;
			cf.at(CF_project_return_aftertax_max_irr,i) = max(cf.at(CF_project_return_aftertax_max_irr,i-1),cf.at(CF_project_return_aftertax_irr,i));
			cf.at(CF_project_return_aftertax_npv,i) = npv(CF_project_return_aftertax,i,nom_discount_rate) +  cf.at(CF_project_return_aftertax,0) ;

			if (flip_year <=0) 
			{
				double residual = fabs(cf.at(CF_project_return_aftertax_irr, i) - flip_target_percent) / 100.0; // solver checks fractions and not percentages
				if ( ( cf.at(CF_project_return_aftertax_max_irr,i-1) < flip_target_percent ) &&  (   residual  < ppa_soln_tolerance ) 	) 
				{
					flip_year = i;
					cf.at(CF_project_return_aftertax_max_irr,i)=flip_target_percent; //within tolerance so pre-flip and post-flip percentages applied correctly
				}
				else if ((cf.at(CF_project_return_aftertax_max_irr, i - 1) < flip_target_percent) && (cf.at(CF_project_return_aftertax_max_irr, i) >= flip_target_percent)) flip_year = i;
			}


		}
		cf.at(CF_project_return_aftertax_npv,0) = cf.at(CF_project_return_aftertax,0) ;

		// 12/14/12 - address issue from Eric Lantz - ppa solution when target mode and ppa < 0
		ppa_old = ppa;

		if (ppa_mode == 0)
		{
		// 12/14/12 - address issue from Eric Lantz - ppa solution when target mode and ppa < 0
			double resid_denom = max(flip_target_percent,1);
		// 12/14/12 - address issue from Eric Lantz - ppa solution when target mode and ppa < 0
			double ppa_denom = max(x0, x1);
			if (ppa_denom <= ppa_soln_tolerance) ppa_denom = 1;
			double residual = cf.at(CF_project_return_aftertax_irr, flip_target_year) - flip_target_percent;
			solved = (( fabs( residual )/resid_denom < ppa_soln_tolerance ) || ( fabs(x0-x1)/ppa_denom < ppa_soln_tolerance) );
//			solved = (( fabs( residual ) < ppa_soln_tolerance ) );
				double flip_frac = flip_target_percent/100.0;
				double itnpv_target = npv(CF_project_return_aftertax,flip_target_year,flip_frac) +  cf.at(CF_project_return_aftertax,0) ;
//				double itnpv_target_delta = npv(CF_project_return_aftertax,flip_target_year,flip_frac+0.001) +  cf.at(CF_project_return_aftertax,0) ;
			//	double itnpv_actual = npv(CF_project_return_aftertax,flip_target_year,cf.at(CF_project_return_aftertax_irr, flip_target_year)) +  cf.at(CF_project_return_aftertax,0) ;
			//	double itnpv_actual_delta = npv(CF_project_return_aftertax,flip_target_year,cf.at(CF_project_return_aftertax_irr, flip_target_year)+0.001) +  cf.at(CF_project_return_aftertax,0) ;
			if (!solved)
			{
//				double flip_frac = flip_target_percent/100.0;
//				double itnpv_target = npv(CF_project_return_aftertax,flip_target_year,flip_frac) +  cf.at(CF_project_return_aftertax,0) ;
				irr_weighting_factor = fabs(itnpv_target);
				irr_is_minimally_met = ((irr_weighting_factor < ppa_soln_tolerance));
				irr_greater_than_target = (( itnpv_target >= 0.0) || irr_is_minimally_met );
				if (ppa_interval_found)
				{// reset interval
				
					if (irr_greater_than_target) // too large
					{
			// set endpoint of weighted interval x0<x1
						x1 = ppa;
						w1 = irr_weighting_factor;
					}
					else // too small
					{
			// set endpoint of weighted interval x0<x1
						x0 = ppa;
						w0 = irr_weighting_factor;
					}

				}
				else
				{ // find solution interval [x0,x1]
					if (ppa_interval_reset) 
					{
							if (irr_greater_than_target) ppa_too_large=true;
							ppa_interval_reset=false;
					}
					if (ppa_too_large) // too large
					{
						if (irr_greater_than_target)
						{
							x0 = ppa;
							w0 = irr_weighting_factor;
							ppa = x0-ppa_coarse_interval;
						}
						else
						{
						  x1 = x0;
						  w1 = w0;
						  x0 = ppa;
						  w0 = irr_weighting_factor;
						  ppa_interval_found=true;
						}
					}
					else
					{
						if (!irr_greater_than_target)
						{
							x1 = ppa;
							w1 = irr_weighting_factor;
							ppa = x1+ppa_coarse_interval;
						}
						else
						{
						  x0 = x1;
						  w0 = w1;
						  x1 = ppa;
						  w1 = irr_weighting_factor;
						  ppa_interval_found=true;
						}
					}
					// for initial guess of zero
					if (fabs(x0-x1)<ppa_soln_tolerance) x0 = x1-2*ppa_soln_tolerance;
				}
					//std::stringstream outm;
					//outm << "iteration=" << its  << ", irr=" << cf.at(CF_project_return_aftertax_irr, flip_target_year)  << ", npvtarget=" << itnpv_target  << ", npvtarget_delta=" << itnpv_target_delta  
					//	//  << ", npvactual=" << itnpv_actual  << ", npvactual_delta=" << itnpv_target_delta  
					//	<< ", residual=" << residual << ", ppa=" << ppa << ", x0=" << x0 << ", x1=" << x1 <<  ",w0=" << w0 << ", w1=" << w1 << ", ppamax-ppamin=" << x1-x0;
					//log( outm.str() );
			}
		}
		its++;

	}	// target tax investor return in target year
	while (!solved && !irr_is_minimally_met  && (its < ppa_soln_max_iteations) && (ppa >= 0) );


		// 12/14/12 - address issue from Eric Lantz - ppa solution when target mode and ppa < 0
	if (ppa < 0) ppa = ppa_old;	

/***************** end iterative solution *********************************************************************/

//	log(util::format("after loop  - size of debt =%lg .", size_of_debt), SSC_WARNING);



	assign("flip_target_year", var_data((ssc_number_t) flip_target_year ));
	assign("flip_target_irr", var_data((ssc_number_t)  flip_target_percent ));

	// Paul 1/27/15 - update for ppa specified and IRR year requested
	if (ppa_mode == 1) flip_year = flip_target_year;

	double actual_flip_irr = std::numeric_limits<double>::quiet_NaN();
	if (flip_year > -1)
	{
		actual_flip_irr = cf.at(CF_project_return_aftertax_irr, flip_target_year);
		assign("flip_actual_year", var_data((ssc_number_t)flip_year));
	}
	else
	{
		assign("flip_actual_year", var_data((ssc_number_t)actual_flip_irr));
	}
	assign("flip_actual_irr", var_data((ssc_number_t)actual_flip_irr));




	// ----------------------------------------------------------------------------
	// Host calculations based on thirdpartyownership with PPA agreement

	// output from utility rate already nyears+1 - no offset
	arrp = as_array("annual_energy_value", &count);
	if ((int)count != nyears + 1)
		throw exec_error("host developer", util::format("energy value input wrong length (%d) should be (%d)", count, nyears + 1));
	for (i = 0; i < (int)count; i++)
		cf.at(CF_host_energy_value, i) = (double)arrp[i];
	// calculated ppa in cents/kWh - cash flow line items in $
	for ( i = 1; i <= nyears; i++)
		cf.at(CF_agreement_cost, i) = (ppa/100.0) * cf.at(CF_energy_net, i) * pow(1 + ppa_escalation, i - 1);

	for (i = 1; i <= nyears; i++)
	{
		cf.at(CF_after_tax_net_equity_cost_flow, i) =
			-cf.at(CF_agreement_cost, i);

		cf.at(CF_after_tax_cash_flow, i) =
			cf.at(CF_after_tax_net_equity_cost_flow, i)
			+ cf.at(CF_host_energy_value, i);

		cf.at(CF_payback_with_expenses, i) =
			cf.at(CF_after_tax_cash_flow, i);

		cf.at(CF_cumulative_payback_with_expenses, i) =
			cf.at(CF_cumulative_payback_with_expenses, i - 1)
			+ cf.at(CF_payback_with_expenses, i);
	}



	double host_npv_energy_real = npv(CF_energy_net, nyears, host_disc_real);
	double host_lcoe_real = -(cf.at(CF_after_tax_net_equity_cost_flow, 0) + npv(CF_after_tax_net_equity_cost_flow, nyears, host_nom_discount_rate)) * 100;
	if (host_npv_energy_real == 0.0)
		host_lcoe_real = std::numeric_limits<double>::quiet_NaN();
	else
		host_lcoe_real /= host_npv_energy_real;

	double host_npv_energy_nom = npv(CF_energy_net, nyears, host_nom_discount_rate);
	double host_lcoe_nom = -(cf.at(CF_after_tax_net_equity_cost_flow, 0) + npv(CF_after_tax_net_equity_cost_flow, nyears, host_nom_discount_rate)) * 100;
	if (host_npv_energy_nom == 0.0)
		host_lcoe_nom = std::numeric_limits<double>::quiet_NaN();
	else
		host_lcoe_nom /= host_npv_energy_nom;


	// NTE
	ssc_number_t *ub_w_sys = 0;
	ub_w_sys = as_array("elec_cost_with_system", &count);
	if ((int)count != nyears + 1)
		throw exec_error("host developer", util::format("utility bill with system input wrong length (%d) should be (%d)", count, nyears + 1));
	ssc_number_t *ub_wo_sys = 0;
	ub_wo_sys = as_array("elec_cost_without_system", &count);
	if ((int)count != nyears + 1)
		throw exec_error("host developer", util::format("utility bill without system input wrong length (%d) should be (%d)", count, nyears + 1));

	for (i = 0; i < (int)count; i++)
		cf.at(CF_nte, i) = (double)(ub_wo_sys[i] - ub_w_sys[i]) *100.0;// $ to cents
	double lnte_real = npv(CF_nte, nyears, host_nom_discount_rate);

	for (i = 0; i < (int)count; i++)
		if (cf.at(CF_energy_net, i) > 0) cf.at(CF_nte, i) /= cf.at(CF_energy_net, i);

	double lnte_nom = lnte_real;
	if (host_npv_energy_real == 0.0)
		lnte_real = std::numeric_limits<double>::quiet_NaN();
	else
		lnte_real /= host_npv_energy_real;
	if (host_npv_energy_nom == 0.0)
		lnte_nom = std::numeric_limits<double>::quiet_NaN();
	else
		lnte_nom /= host_npv_energy_nom;

	assign("lnte_real", var_data((ssc_number_t)lnte_real));
	assign("lnte_nom", var_data((ssc_number_t)lnte_nom));
	save_cf(CF_nte, nyears, "cf_nte");
	assign("year1_nte", var_data((ssc_number_t)cf.at(CF_nte, 1)));


	double host_net_present_value = cf.at(CF_after_tax_cash_flow, 0) + npv(CF_after_tax_cash_flow, nyears, host_nom_discount_rate);

	assign("cf_length", var_data((ssc_number_t)nyears + 1));

	//LCOE's commented out so as not to confuse the user. They don't compare to other LCOEs.
	//		assign( "lcoe_real", var_data((ssc_number_t)lcoe_real) );
	//		assign( "lcoe_nom", var_data((ssc_number_t)lcoe_nom) );
	assign("npv", var_data((ssc_number_t)host_net_present_value));

	assign("host_nominal_discount_rate", var_data((ssc_number_t)(host_nom_discount_rate*100.0)));


	save_cf(CF_agreement_cost, nyears, "cf_agreement_cost");
	save_cf(CF_after_tax_net_equity_cost_flow, nyears, "cf_after_tax_net_equity_cost_flow");
	save_cf(CF_after_tax_cash_flow, nyears, "cf_after_tax_cash_flow");
	save_cf(CF_payback_with_expenses, nyears, "cf_payback_with_expenses");
	save_cf(CF_cumulative_payback_with_expenses, nyears, "cf_cumulative_payback_with_expenses");


	// end Host calculations
	//-------------------------------------------------------------------------------------

	
	// LPPA - change form total revenue to PPA revenue 7/19/15 consistent with DHF v4.4
	// fixed price PPA - LPPA independent of salvage value per 7/16/15 meeting
	double npv_ppa_revenue = npv(CF_energy_value, nyears, nom_discount_rate);
//	double npv_ppa_revenue = npv(CF_total_revenue, nyears, nom_discount_rate);
	double npv_energy_nom = npv(CF_energy_net,nyears,nom_discount_rate);
	double lppa_nom = 0;
	if (npv_energy_nom != 0) lppa_nom = npv_ppa_revenue / npv_energy_nom * 100.0;
	double lppa_real = 0;
	double npv_energy_real = npv(CF_energy_net,nyears,disc_real);
	if (npv_energy_real != 0) lppa_real = npv_ppa_revenue / npv_energy_real * 100.0;

	// update LCOE calculations 
	double lcoe_nom = lppa_nom;
	double lcoe_real = lppa_real;

	// from single_owner.xlsm
	cf.at(CF_Annual_Costs, 0) = -issuance_of_equity;
	for (i = 1; i <= nyears; i++)
	{
		cf.at(CF_Annual_Costs, i) =
			cf.at(CF_pbi_total, i)
			+ cf.at(CF_statax, i)
			+ cf.at(CF_fedtax, i)
			- cf.at(CF_debt_payment_interest, i)
			- cf.at(CF_debt_payment_principal, i)
			- cf.at(CF_operating_expenses, i)
			// incentives (cbi and ibi in installed cost and itc in year 1 below
			// TODO - check PBI
			+ cf.at(CF_ptc_fed, i)
			+ cf.at(CF_ptc_sta, i)
			// reserve accounts
			- cf.at(CF_funding_equip1, i)
			- cf.at(CF_funding_equip2, i)
			- cf.at(CF_funding_equip3, i)
			- cf.at(CF_funding_om, i)
			- cf.at(CF_funding_receivables, i)
			- cf.at(CF_funding_debtservice, i)
			+ cf.at(CF_reserve_interest, i)
			- cf.at(CF_disbursement_debtservice, i) // note sign is negative for positive disbursement
			- cf.at(CF_disbursement_om, i) // note sign is negative for positive disbursement
			+ cf.at(CF_net_salvage_value, i); // benefit to cost reduction so that project revenue based on PPA revenue and not total revenue per 7/16/15 meeting
	}
	// year 1 add total ITC (net benefit) so that project return = project revenue - project cost
	if (nyears >= 1) cf.at(CF_Annual_Costs, 1) += itc_total;

	double npv_annual_costs = -(npv(CF_Annual_Costs, nyears, nom_discount_rate)
		+ cf.at(CF_Annual_Costs, 0));
	if (npv_energy_nom != 0) lcoe_nom = npv_annual_costs / npv_energy_nom * 100.0;
	if (npv_energy_real != 0) lcoe_real = npv_annual_costs / npv_energy_real * 100.0;

	assign("npv_annual_costs", var_data((ssc_number_t)npv_annual_costs));
	save_cf(CF_Annual_Costs, nyears, "cf_annual_costs");


	// DSCR calculations
	for (i = 0; i <= nyears; i++)
	{
		if (cf.at(CF_debt_payment_total, i) == 0.0) cf.at(CF_pretax_dscr, i) = 0; //cf.at(CF_pretax_dscr, i) = std::numeric_limits<double>::quiet_NaN();
		else cf.at(CF_pretax_dscr, i) = cf.at(CF_cash_for_ds, i) / cf.at(CF_debt_payment_total, i);
	}
	double min_dscr = min_cashflow_value(CF_pretax_dscr, nyears);
	assign("min_dscr", var_data((ssc_number_t)min_dscr));
	save_cf(CF_pretax_dscr, nyears, "cf_pretax_dscr");



	double npv_fed_ptc = npv(CF_ptc_fed,nyears,nom_discount_rate);
	double npv_sta_ptc = npv(CF_ptc_sta,nyears,nom_discount_rate);

//	double effective_tax_rate = state_tax_rate + (1.0-state_tax_rate)*federal_tax_rate;
	npv_fed_ptc /= (1.0 - cf.at(CF_effective_tax_frac, 1));
	npv_sta_ptc /= (1.0 - cf.at(CF_effective_tax_frac, 1));


	double lcoptc_fed_nom=0.0;
	if (npv_energy_nom != 0) lcoptc_fed_nom = npv_fed_ptc / npv_energy_nom * 100.0;
	double lcoptc_fed_real=0.0;
	if (npv_energy_real != 0) lcoptc_fed_real = npv_fed_ptc / npv_energy_real * 100.0;

	double lcoptc_sta_nom=0.0;
	if (npv_energy_nom != 0) lcoptc_sta_nom = npv_sta_ptc / npv_energy_nom * 100.0;
	double lcoptc_sta_real=0.0;
	if (npv_energy_real != 0) lcoptc_sta_real = npv_sta_ptc / npv_energy_real * 100.0;

	assign("lcoptc_fed_nom", var_data((ssc_number_t) lcoptc_fed_nom));
	assign("lcoptc_fed_real", var_data((ssc_number_t) lcoptc_fed_real));
	assign("lcoptc_sta_nom", var_data((ssc_number_t) lcoptc_sta_nom));
	assign("lcoptc_sta_real", var_data((ssc_number_t) lcoptc_sta_real));

	double analysis_period_irr = 0.0;
	analysis_period_irr = cf.at(CF_project_return_aftertax_irr, nyears)/100.0; //fraction for calculations

	double debt_fraction = 0.0;
//	double size_of_equity = cost_installed - ibi_total - cbi_total - size_of_debt;
	double size_of_equity = cost_installed - size_of_debt;
	//cpg same as issuance_of_equity 
	//	if (cost_installed > 0) debt_fraction = size_of_debt / cost_installed;
	if ((size_of_debt + size_of_equity) > 0)
		debt_fraction = size_of_debt / (size_of_debt + size_of_equity);



	double wacc = 0.0;
	wacc = (1.0 - debt_fraction)*analysis_period_irr + debt_fraction*term_int_rate*(1.0 - cf.at(CF_effective_tax_frac, 1));

	// percentages
	debt_fraction *= 100.0;
	wacc *= 100.0;
//	effective_tax_rate *= 100.0;
	analysis_period_irr *= 100.0;


	assign("debt_fraction", var_data((ssc_number_t) debt_fraction ));
	assign("wacc", var_data( (ssc_number_t) wacc));
	assign("effective_tax_rate", var_data((ssc_number_t)(cf.at(CF_effective_tax_frac, 1)*100.0)));
	assign("analysis_period_irr", var_data( (ssc_number_t) analysis_period_irr));



	assign("npv_ppa_revenue", var_data( (ssc_number_t) npv_ppa_revenue));
	assign("npv_energy_nom", var_data( (ssc_number_t) npv_energy_nom));
	assign("npv_energy_real", var_data( (ssc_number_t) npv_energy_real));

		assign( "cf_length", var_data( (ssc_number_t) nyears+1 ));

		assign( "salvage_value", var_data((ssc_number_t)salvage_value));

		assign( "prop_tax_assessed_value", var_data((ssc_number_t)( assessed_frac * cost_prefinancing )));

		assign("adjusted_installed_cost", var_data((ssc_number_t)(cost_installed - cbi_total - ibi_total)));
		assign("cost_installed", var_data((ssc_number_t)cost_installed));

		assign( "cost_prefinancing", var_data((ssc_number_t) cost_prefinancing ) );
		//assign( "cost_prefinancingperwatt", var_data((ssc_number_t)( cost_prefinancing / nameplate / 1000.0 ) ));

		assign( "nominal_discount_rate", var_data((ssc_number_t)nom_discount_rate ) );


		assign( "depr_fedbas_macrs_5", var_data((ssc_number_t) depr_fedbas_macrs_5 ) );
		assign( "depr_fedbas_macrs_15", var_data((ssc_number_t) depr_fedbas_macrs_15 ) );
		assign( "depr_fedbas_sl_5", var_data((ssc_number_t) depr_fedbas_sl_5 ) );
		assign( "depr_fedbas_sl_15", var_data((ssc_number_t) depr_fedbas_sl_15 ) );
		assign( "depr_fedbas_sl_20", var_data((ssc_number_t) depr_fedbas_sl_20 ) );
		assign( "depr_fedbas_sl_39", var_data((ssc_number_t) depr_fedbas_sl_39 ) );
		assign( "depr_fedbas_custom", var_data((ssc_number_t) depr_fedbas_custom ) );
		assign( "depr_fedbas_total", var_data((ssc_number_t) depr_fedbas_total ) );


		assign("cost_financing", var_data((ssc_number_t) cost_financing));
		assign("cost_debt_upfront", var_data((ssc_number_t) cost_debt_upfront));


		assign( "size_of_equity", var_data((ssc_number_t) size_of_equity) );
		assign( "cost_installedperwatt", var_data((ssc_number_t)( cost_installed / nameplate / 1000.0 ) ));

		// metric costs
		//advanced_financing_cost adv(this);
		//adv.compute_cost(cost_installed, size_of_equity, size_of_debt, cbi_total, ibi_total);



 		assign( "itc_fed_qual_macrs_5", var_data((ssc_number_t) itc_fed_qual_macrs_5 ) );
		assign( "itc_fed_qual_macrs_15", var_data((ssc_number_t) itc_fed_qual_macrs_15 ) );
		assign( "itc_fed_qual_sl_5", var_data((ssc_number_t) itc_fed_qual_sl_5 ) );
		assign( "itc_fed_qual_sl_15", var_data((ssc_number_t) itc_fed_qual_sl_15 ) );
		assign( "itc_fed_qual_sl_20", var_data((ssc_number_t) itc_fed_qual_sl_20 ) );
		assign( "itc_fed_qual_sl_39", var_data((ssc_number_t) itc_fed_qual_sl_39 ) );
		assign( "itc_fed_qual_custom", var_data((ssc_number_t) itc_fed_qual_custom ) );

		assign( "itc_disallow_fed_percent_macrs_5", var_data((ssc_number_t) itc_disallow_fed_percent_macrs_5 ) );
		assign( "itc_disallow_fed_percent_macrs_15", var_data((ssc_number_t) itc_disallow_fed_percent_macrs_15 ) );
		assign( "itc_disallow_fed_percent_sl_5", var_data((ssc_number_t) itc_disallow_fed_percent_sl_5 ) );
		assign( "itc_disallow_fed_percent_sl_15", var_data((ssc_number_t) itc_disallow_fed_percent_sl_15 ) );
		assign( "itc_disallow_fed_percent_sl_20", var_data((ssc_number_t) itc_disallow_fed_percent_sl_20 ) );
		assign( "itc_disallow_fed_percent_sl_39", var_data((ssc_number_t) itc_disallow_fed_percent_sl_39 ) );
		assign( "itc_disallow_fed_percent_custom", var_data((ssc_number_t) itc_disallow_fed_percent_custom ) );

		assign( "itc_disallow_fed_fixed_macrs_5", var_data((ssc_number_t) itc_disallow_fed_fixed_macrs_5 ) );
		assign( "itc_disallow_fed_fixed_macrs_15", var_data((ssc_number_t) itc_disallow_fed_fixed_macrs_15 ) );
		assign( "itc_disallow_fed_fixed_sl_5", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_5 ) );
		assign( "itc_disallow_fed_fixed_sl_15", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_15 ) );
		assign( "itc_disallow_fed_fixed_sl_20", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_20 ) );
		assign( "itc_disallow_fed_fixed_sl_39", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_39 ) );
		assign( "itc_disallow_fed_fixed_custom", var_data((ssc_number_t) itc_disallow_fed_fixed_custom ) );

		assign( "itc_fed_qual_total", var_data((ssc_number_t) itc_fed_qual_total ) );
		assign( "itc_fed_percent_total", var_data((ssc_number_t) itc_fed_per ) );
		assign( "itc_fed_fixed_total", var_data((ssc_number_t) itc_fed_amount ) );
	
	

		// output variable and cashflow line item assignments

		assign("issuance_of_equity", var_data((ssc_number_t) issuance_of_equity));
		assign("purchase_of_property", var_data((ssc_number_t) purchase_of_property));
		assign("cash_for_debt_service", var_data((ssc_number_t) cash_for_debt_service));
		assign("pv_cafds", var_data((ssc_number_t) pv_cafds));
		assign("size_of_debt", var_data((ssc_number_t) size_of_debt));
		
		assign("ppa_price", var_data((ssc_number_t) ppa));
		assign("target_return_flip_year", var_data((ssc_number_t) flip_year));


		assign("ibi_total_fed", var_data((ssc_number_t) (ibi_fed_amount+ibi_fed_per)));
		assign("ibi_total_sta", var_data((ssc_number_t) (ibi_sta_amount+ibi_sta_per)));
		assign("ibi_total_oth", var_data((ssc_number_t) (ibi_oth_amount+ibi_oth_per)));
		assign("ibi_total_uti", var_data((ssc_number_t) (ibi_uti_amount+ibi_uti_per)));
		assign("ibi_total", var_data((ssc_number_t) ibi_total));
	assign("ibi_fedtax_total", var_data((ssc_number_t) ibi_fedtax_total));
	assign("ibi_statax_total", var_data((ssc_number_t) ibi_statax_total));
		assign("cbi_total", var_data((ssc_number_t) cbi_total));
	assign("cbi_fedtax_total", var_data((ssc_number_t) cbi_fedtax_total));
	assign("cbi_statax_total", var_data((ssc_number_t) cbi_statax_total));
		assign("cbi_total_fed", var_data((ssc_number_t) cbi_fed_amount));
		assign("cbi_total_sta", var_data((ssc_number_t) cbi_sta_amount));
		assign("cbi_total_oth", var_data((ssc_number_t) cbi_oth_amount));
		assign("cbi_total_uti", var_data((ssc_number_t) cbi_uti_amount));
		assign("itc_total_fed", var_data((ssc_number_t) itc_fed_total));
		assign("itc_total_sta", var_data((ssc_number_t) itc_sta_total));
		assign("itc_total", var_data((ssc_number_t) itc_total));

//		assign("first_year_energy_net", var_data((ssc_number_t) cf.at(CF_energy_net,1)));

		assign("lcoe_nom", var_data((ssc_number_t)lcoe_nom));
		assign("lcoe_real", var_data((ssc_number_t)lcoe_real));
		assign("lppa_nom", var_data((ssc_number_t)lppa_nom));
		assign("lppa_real", var_data((ssc_number_t)lppa_real));
		assign("ppa_price", var_data((ssc_number_t)ppa));
		assign("ppa_escalation", var_data((ssc_number_t) (ppa_escalation *100.0) ));
		assign("ppa", var_data((ssc_number_t) ppa));


		assign("issuance_of_equity", var_data((ssc_number_t) issuance_of_equity));
		

		assign("project_return_aftertax_irr", var_data((ssc_number_t)  (irr(CF_project_return_aftertax,nyears)*100.0)));
		assign("project_return_aftertax_npv", var_data((ssc_number_t)  (npv(CF_project_return_aftertax,nyears,nom_discount_rate) +  cf.at(CF_project_return_aftertax,0)) ));


		// cash flow line items
		save_cf(CF_federal_tax_frac, nyears, "cf_federal_tax_frac");
		save_cf(CF_state_tax_frac, nyears, "cf_state_tax_frac");
		save_cf(CF_effective_tax_frac, nyears, "cf_effective_tax_frac");

		
		save_cf( CF_statax_taxable_incentives, nyears, "cf_statax_taxable_incentives" );
		save_cf( CF_statax_income_with_incentives, nyears, "cf_statax_income_with_incentives" );
		save_cf( CF_statax, nyears, "cf_statax" );
		save_cf( CF_fedtax_taxable_incentives, nyears, "cf_fedtax_taxable_incentives" );
		save_cf( CF_fedtax_income_with_incentives, nyears, "cf_fedtax_income_with_incentives" );
		save_cf( CF_fedtax, nyears, "cf_fedtax" );

		save_cf( CF_stadepr_macrs_5, nyears, "cf_stadepr_macrs_5" );
		save_cf( CF_stadepr_macrs_15, nyears, "cf_stadepr_macrs_15" );
		save_cf( CF_stadepr_sl_5, nyears, "cf_stadepr_sl_5" );
		save_cf( CF_stadepr_sl_15, nyears, "cf_stadepr_sl_15" );
		save_cf( CF_stadepr_sl_20, nyears, "cf_stadepr_sl_20" );
		save_cf( CF_stadepr_sl_39, nyears, "cf_stadepr_sl_39" );
		save_cf( CF_stadepr_custom, nyears, "cf_stadepr_custom" );
		save_cf( CF_stadepr_me1, nyears, "cf_stadepr_me1" );
		save_cf( CF_stadepr_me2, nyears, "cf_stadepr_me2" );
		save_cf( CF_stadepr_me3, nyears, "cf_stadepr_me3" );
		save_cf( CF_stadepr_total, nyears, "cf_stadepr_total" );
		save_cf( CF_statax_income_prior_incentives, nyears, "cf_statax_income_prior_incentives" );

		save_cf( CF_feddepr_macrs_5, nyears, "cf_feddepr_macrs_5" );
		save_cf( CF_feddepr_macrs_15, nyears, "cf_feddepr_macrs_15" );
		save_cf( CF_feddepr_sl_5, nyears, "cf_feddepr_sl_5" );
		save_cf( CF_feddepr_sl_15, nyears, "cf_feddepr_sl_15" );
		save_cf( CF_feddepr_sl_20, nyears, "cf_feddepr_sl_20" );
		save_cf( CF_feddepr_sl_39, nyears, "cf_feddepr_sl_39" );
		save_cf( CF_feddepr_custom, nyears, "cf_feddepr_custom" );
		save_cf( CF_feddepr_me1, nyears, "cf_feddepr_me1" );
		save_cf( CF_feddepr_me2, nyears, "cf_feddepr_me2" );
		save_cf( CF_feddepr_me3, nyears, "cf_feddepr_me3" );
		save_cf( CF_feddepr_total, nyears, "cf_feddepr_total" );
		save_cf( CF_fedtax_income_prior_incentives, nyears, "cf_fedtax_income_prior_incentives" );

	save_cf( CF_pbi_fed, nyears, "cf_pbi_total_fed");
	save_cf( CF_pbi_sta, nyears, "cf_pbi_total_sta");
	save_cf( CF_pbi_oth, nyears, "cf_pbi_total_oth");
	save_cf( CF_pbi_uti, nyears, "cf_pbi_total_uti");
	save_cf( CF_pbi_total, nyears, "cf_pbi_total" );
	save_cf( CF_pbi_statax_total, nyears, "cf_pbi_statax_total" );
	save_cf( CF_pbi_fedtax_total, nyears, "cf_pbi_fedtax_total" );

		save_cf( CF_ptc_fed, nyears, "cf_ptc_fed" );
		save_cf( CF_ptc_sta, nyears, "cf_ptc_sta" );

		save_cf( CF_project_return_aftertax_cash, nyears, "cf_project_return_aftertax_cash" );
		save_cf( CF_project_return_aftertax, nyears, "cf_project_return_aftertax" );
		save_cf( CF_project_return_aftertax_irr, nyears, "cf_project_return_aftertax_irr" );
		save_cf( CF_project_return_aftertax_max_irr, nyears, "cf_project_return_aftertax_max_irr" );
		save_cf( CF_project_return_aftertax_npv, nyears, "cf_project_return_aftertax_npv" );
		save_cf( CF_project_return_pretax, nyears, "cf_project_return_pretax" );
		save_cf( CF_project_return_pretax_irr, nyears, "cf_project_return_pretax_irr" );
		save_cf( CF_project_return_pretax_npv, nyears, "cf_project_return_pretax_npv" );

		save_cf( CF_project_financing_activities, nyears, "cf_project_financing_activities" );
		save_cf( CF_pretax_cashflow, nyears, "cf_pretax_cashflow" );

		save_cf( CF_project_dsra, nyears, "cf_project_dsra" );
		save_cf(CF_project_wcra, nyears, "cf_project_wcra");
		save_cf(CF_project_receivablesra, nyears, "cf_project_receivablesra");
		save_cf(CF_project_me1ra, nyears, "cf_project_me1ra");
		save_cf( CF_project_me2ra, nyears, "cf_project_me2ra" );
		save_cf( CF_project_me3ra, nyears, "cf_project_me3ra" );
		save_cf( CF_project_ra, nyears, "cf_project_ra" );
		save_cf( CF_project_me1cs, nyears, "cf_project_me1cs" );
		save_cf( CF_project_me2cs, nyears, "cf_project_me2cs" );
		save_cf( CF_project_me3cs, nyears, "cf_project_me3cs" );
		save_cf( CF_project_mecs, nyears, "cf_project_mecs" );
		save_cf( CF_project_investing_activities, nyears, "cf_project_investing_activities" );

		save_cf( CF_pv_interest_factor, nyears, "cf_pv_interest_factor" );
		save_cf( CF_cash_for_ds, nyears, "cf_cash_for_ds" );
		save_cf( CF_pv_cash_for_ds, nyears, "cf_pv_cash_for_ds" );
		save_cf( CF_debt_size, nyears, "cf_debt_size" );			
		save_cf( CF_project_operating_activities, nyears, "cf_project_operating_activities" );

		save_cf( CF_debt_payment_total, nyears, "cf_debt_payment_total" );
		save_cf( CF_debt_payment_interest, nyears, "cf_debt_payment_interest" );
		save_cf( CF_debt_payment_principal, nyears, "cf_debt_payment_principal" );
		save_cf( CF_debt_balance, nyears, "cf_debt_balance" );

		save_cf( CF_energy_value, nyears, "cf_energy_value" );
		save_cf( CF_ppa_price, nyears, "cf_ppa_price" );
		save_cf( CF_om_fixed_expense, nyears, "cf_om_fixed_expense" );
		save_cf( CF_om_production_expense, nyears, "cf_om_production_expense" );
		save_cf( CF_om_capacity_expense, nyears, "cf_om_capacity_expense" );
		save_cf( CF_om_fuel_expense, nyears, "cf_om_fuel_expense" );
		save_cf( CF_om_opt_fuel_1_expense, nyears, "cf_om_opt_fuel_1_expense" );
		save_cf( CF_om_opt_fuel_2_expense, nyears, "cf_om_opt_fuel_2_expense" );
		save_cf( CF_property_tax_assessed_value, nyears, "cf_property_tax_assessed_value" );
		save_cf( CF_property_tax_expense, nyears, "cf_property_tax_expense" );
		save_cf( CF_insurance_expense, nyears, "cf_insurance_expense" );
		save_cf(CF_battery_replacement_cost, nyears, "cf_battery_replacement_cost");
		save_cf(CF_battery_replacement_cost_schedule, nyears, "cf_battery_replacement_cost_schedule");
		save_cf( CF_operating_expenses, nyears, "cf_operating_expenses" );
		save_cf( CF_ebitda, nyears, "cf_ebitda" );
		save_cf( CF_net_salvage_value, nyears, "cf_net_salvage_value" );
		save_cf( CF_total_revenue, nyears, "cf_total_revenue" );
		save_cf( CF_energy_net, nyears, "cf_energy_net" );
		save_cf( CF_reserve_debtservice, nyears, "cf_reserve_debtservice" );
		save_cf(CF_reserve_om, nyears, "cf_reserve_om");
		save_cf(CF_reserve_receivables, nyears, "cf_reserve_receivables");
		save_cf(CF_reserve_equip1, nyears, "cf_reserve_equip1");
		save_cf( CF_reserve_equip2, nyears, "cf_reserve_equip2" );
		save_cf( CF_reserve_equip3, nyears, "cf_reserve_equip3" );

		save_cf( CF_funding_debtservice, nyears, "cf_funding_debtservice" );
		save_cf(CF_funding_om, nyears, "cf_funding_om");
		save_cf(CF_funding_receivables, nyears, "cf_funding_receivables");
		save_cf(CF_funding_equip1, nyears, "cf_funding_equip1");
		save_cf( CF_funding_equip2, nyears, "cf_funding_equip2" );
		save_cf( CF_funding_equip3, nyears, "cf_funding_equip3" );

		save_cf( CF_disbursement_debtservice, nyears, "cf_disbursement_debtservice" );
		save_cf(CF_disbursement_om, nyears, "cf_disbursement_om");
		save_cf(CF_disbursement_receivables, nyears, "cf_disbursement_receivables");
		save_cf(CF_disbursement_equip1, nyears, "cf_disbursement_equip1");
		save_cf( CF_disbursement_equip2, nyears, "cf_disbursement_equip2" );
		save_cf( CF_disbursement_equip3, nyears, "cf_disbursement_equip3" );

		save_cf( CF_reserve_total, nyears, "cf_reserve_total" );
		save_cf( CF_reserve_interest, nyears, "cf_reserve_interest" );

		save_cf(CF_Recapitalization, nyears, "cf_recapitalization");

		save_cf(CF_return_on_equity_input, nyears, "cf_return_on_equity_input");
		save_cf(CF_return_on_equity_dollars, nyears, "cf_return_on_equity_dollars");
		save_cf(CF_return_on_equity, nyears, "cf_return_on_equity");


		for (i = 0; i <= nyears; i++)
		{
			cf.at(CF_lcog_costs, i) = cf.at(CF_om_capacity_expense, i)
				+ cf.at(CF_feddepr_total, i) 
				+ cf.at(CF_debt_payment_interest, i)
				+ cf.at(CF_reserve_interest, i)
				+ cf.at(CF_return_on_equity_dollars, i);
		}
		save_cf(CF_lcog_costs, nyears, "cf_lcog_costs");

		double lcog_om = npv(CF_om_capacity_expense, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_om = lcog_om * 100.0 / npv_energy_nom;
		assign("lcog_om", var_data((ssc_number_t)lcog_om));

		double lcog_depr = npv(CF_feddepr_total, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_depr = lcog_depr * 100.0 / npv_energy_nom;
		assign("lcog_depr", var_data((ssc_number_t)lcog_depr));

		double lcog_loan_int = npv(CF_debt_payment_interest, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_loan_int = lcog_loan_int * 100.0 / npv_energy_nom;
		assign("lcog_loan_int", var_data((ssc_number_t)lcog_loan_int));

		double lcog_wc_int = npv(CF_reserve_interest, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_wc_int = lcog_wc_int * 100.0 / npv_energy_nom;
		assign("lcog_wc_int", var_data((ssc_number_t)lcog_wc_int));

		double lcog_roe = npv(CF_return_on_equity_dollars, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_roe = lcog_roe * 100.0 / npv_energy_nom;
		assign("lcog_roe", var_data((ssc_number_t)lcog_roe));

		double lcog_nom = npv(CF_lcog_costs, nyears, nom_discount_rate);
		if (npv_energy_nom != 0) lcog_nom = lcog_nom * 100.0 / npv_energy_nom;
		assign("lcog", var_data((ssc_number_t)lcog_nom));




		// dispatch
		std::vector<double> ppa_cf;
		for (i = 0; i <= nyears; i++)
		{
			ppa_cf.push_back(cf.at(CF_ppa_price, i));
		}
		m_disp_calcs.compute_outputs(ppa_cf);


		// State ITC/depreciation table
		assign("depr_stabas_percent_macrs_5", var_data((ssc_number_t)  (depr_stabas_macrs_5_frac*100.0)));
		assign( "depr_alloc_macrs_5", var_data((ssc_number_t) depr_alloc_macrs_5 ) );
		double depr_stabas_ibi_reduc_macrs_5 = depr_stabas_macrs_5_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_macrs_5 = depr_stabas_macrs_5_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_macrs_5", var_data((ssc_number_t) depr_stabas_ibi_reduc_macrs_5 ) );
		assign( "depr_stabas_cbi_reduc_macrs_5", var_data((ssc_number_t) depr_stabas_cbi_reduc_macrs_5 ) );
 		assign( "depr_stabas_prior_itc_macrs_5", var_data((ssc_number_t) ( depr_alloc_macrs_5 - depr_stabas_ibi_reduc_macrs_5 - depr_stabas_cbi_reduc_macrs_5)) );
 		assign( "itc_sta_qual_macrs_5", var_data((ssc_number_t) itc_sta_qual_macrs_5 ) );
		double depr_stabas_percent_qual_macrs_5 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_macrs_5 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_macrs_5", var_data((ssc_number_t) depr_stabas_percent_qual_macrs_5) );
 		assign( "depr_stabas_percent_amount_macrs_5", var_data((ssc_number_t) (depr_stabas_percent_qual_macrs_5/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_macrs_5", var_data((ssc_number_t) itc_disallow_sta_percent_macrs_5 ) );
 		assign( "depr_stabas_fixed_amount_macrs_5", var_data((ssc_number_t) (depr_stabas_percent_qual_macrs_5/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_macrs_5", var_data((ssc_number_t) itc_disallow_sta_fixed_macrs_5 ) );
		double depr_stabas_itc_sta_reduction_macrs_5 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_macrs_5 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_macrs_5;
		double depr_stabas_itc_fed_reduction_macrs_5 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_macrs_5 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_macrs_5;
		assign( "depr_stabas_itc_sta_reduction_macrs_5", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_macrs_5 ) );
		assign( "depr_stabas_itc_fed_reduction_macrs_5", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_macrs_5 ) );
		assign( "depr_stabas_after_itc_macrs_5", var_data((ssc_number_t) (depr_stabas_macrs_5 + depr_stabas_macrs_5_bonus) ) );
		assign( "depr_stabas_first_year_bonus_macrs_5", var_data((ssc_number_t) depr_stabas_macrs_5_bonus ) );
		assign( "depr_stabas_macrs_5", var_data((ssc_number_t) depr_stabas_macrs_5 ) );

		assign("depr_stabas_percent_macrs_15", var_data((ssc_number_t)  (depr_stabas_macrs_15_frac*100.0)));
		assign( "depr_alloc_macrs_15", var_data((ssc_number_t) depr_alloc_macrs_15 ) );
		double depr_stabas_ibi_reduc_macrs_15 = depr_stabas_macrs_15_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_macrs_15 = depr_stabas_macrs_15_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_macrs_15", var_data((ssc_number_t) depr_stabas_ibi_reduc_macrs_15 ) );
		assign( "depr_stabas_cbi_reduc_macrs_15", var_data((ssc_number_t) depr_stabas_cbi_reduc_macrs_15 ) );
 		assign( "depr_stabas_prior_itc_macrs_15", var_data((ssc_number_t) ( depr_alloc_macrs_15 - depr_stabas_ibi_reduc_macrs_15 - depr_stabas_cbi_reduc_macrs_15)) );
 		assign( "itc_sta_qual_macrs_15", var_data((ssc_number_t) itc_sta_qual_macrs_15 ) );
		double depr_stabas_percent_qual_macrs_15 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_macrs_15 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_macrs_15", var_data((ssc_number_t) depr_stabas_percent_qual_macrs_15) );
 		assign( "depr_stabas_percent_amount_macrs_15", var_data((ssc_number_t) (depr_stabas_percent_qual_macrs_15/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_macrs_15", var_data((ssc_number_t) itc_disallow_sta_percent_macrs_15 ) );
 		assign( "depr_stabas_fixed_amount_macrs_15", var_data((ssc_number_t) (depr_stabas_percent_qual_macrs_15/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_macrs_15", var_data((ssc_number_t) itc_disallow_sta_fixed_macrs_15 ) );
		double depr_stabas_itc_sta_reduction_macrs_15 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_macrs_15 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_macrs_15;
		double depr_stabas_itc_fed_reduction_macrs_15 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_macrs_15 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_macrs_15;
		assign( "depr_stabas_itc_sta_reduction_macrs_15", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_macrs_15 ) );
		assign( "depr_stabas_itc_fed_reduction_macrs_15", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_macrs_15 ) );
		assign( "depr_stabas_after_itc_macrs_15", var_data((ssc_number_t) (depr_stabas_macrs_15 + depr_stabas_macrs_15_bonus) ) );
		assign( "depr_stabas_first_year_bonus_macrs_15", var_data((ssc_number_t) depr_stabas_macrs_15_bonus ) );
		assign( "depr_stabas_macrs_15", var_data((ssc_number_t) depr_stabas_macrs_15 ) );

		assign("depr_stabas_percent_sl_5", var_data((ssc_number_t)  (depr_stabas_sl_5_frac*100.0)));
		assign( "depr_alloc_sl_5", var_data((ssc_number_t) depr_alloc_sl_5 ) );
		double depr_stabas_ibi_reduc_sl_5 = depr_stabas_sl_5_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_sl_5 = depr_stabas_sl_5_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_sl_5", var_data((ssc_number_t) depr_stabas_ibi_reduc_sl_5 ) );
		assign( "depr_stabas_cbi_reduc_sl_5", var_data((ssc_number_t) depr_stabas_cbi_reduc_sl_5 ) );
 		assign( "depr_stabas_prior_itc_sl_5", var_data((ssc_number_t) ( depr_alloc_sl_5 - depr_stabas_ibi_reduc_sl_5 - depr_stabas_cbi_reduc_sl_5)) );
 		assign( "itc_sta_qual_sl_5", var_data((ssc_number_t) itc_sta_qual_sl_5 ) );
		double depr_stabas_percent_qual_sl_5 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_sl_5 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_sl_5", var_data((ssc_number_t) depr_stabas_percent_qual_sl_5) );
 		assign( "depr_stabas_percent_amount_sl_5", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_5/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_sl_5", var_data((ssc_number_t) itc_disallow_sta_percent_sl_5 ) );
 		assign( "depr_stabas_fixed_amount_sl_5", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_5/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_sl_5", var_data((ssc_number_t) itc_disallow_sta_fixed_sl_5 ) );
		double depr_stabas_itc_sta_reduction_sl_5 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_5 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_5;
		double depr_stabas_itc_fed_reduction_sl_5 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_5 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_5;
		assign( "depr_stabas_itc_sta_reduction_sl_5", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_sl_5 ) );
		assign( "depr_stabas_itc_fed_reduction_sl_5", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_sl_5 ) );
		assign( "depr_stabas_after_itc_sl_5", var_data((ssc_number_t) (depr_stabas_sl_5 + depr_stabas_sl_5_bonus) ) );
		assign( "depr_stabas_first_year_bonus_sl_5", var_data((ssc_number_t) depr_stabas_sl_5_bonus ) );
		assign( "depr_stabas_sl_5", var_data((ssc_number_t) depr_stabas_sl_5 ) );

		assign("depr_stabas_percent_sl_15", var_data((ssc_number_t)  (depr_stabas_sl_15_frac*100.0)));
		assign( "depr_alloc_sl_15", var_data((ssc_number_t) depr_alloc_sl_15 ) );
		double depr_stabas_ibi_reduc_sl_15 = depr_stabas_sl_15_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_sl_15 = depr_stabas_sl_15_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_sl_15", var_data((ssc_number_t) depr_stabas_ibi_reduc_sl_15 ) );
		assign( "depr_stabas_cbi_reduc_sl_15", var_data((ssc_number_t) depr_stabas_cbi_reduc_sl_15 ) );
 		assign( "depr_stabas_prior_itc_sl_15", var_data((ssc_number_t) ( depr_alloc_sl_15 - depr_stabas_ibi_reduc_sl_15 - depr_stabas_cbi_reduc_sl_15)) );
 		assign( "itc_sta_qual_sl_15", var_data((ssc_number_t) itc_sta_qual_sl_15 ) );
		double depr_stabas_percent_qual_sl_15 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_sl_15 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_sl_15", var_data((ssc_number_t) depr_stabas_percent_qual_sl_15) );
 		assign( "depr_stabas_percent_amount_sl_15", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_15/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_sl_15", var_data((ssc_number_t) itc_disallow_sta_percent_sl_15 ) );
 		assign( "depr_stabas_fixed_amount_sl_15", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_15/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_sl_15", var_data((ssc_number_t) itc_disallow_sta_fixed_sl_15 ) );
		double depr_stabas_itc_sta_reduction_sl_15 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_15 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_15;
		double depr_stabas_itc_fed_reduction_sl_15 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_15 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_15;
		assign( "depr_stabas_itc_sta_reduction_sl_15", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_sl_15 ) );
		assign( "depr_stabas_itc_fed_reduction_sl_15", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_sl_15 ) );
		assign( "depr_stabas_after_itc_sl_15", var_data((ssc_number_t) (depr_stabas_sl_15 + depr_stabas_sl_15_bonus) ) );
		assign( "depr_stabas_first_year_bonus_sl_15", var_data((ssc_number_t) depr_stabas_sl_15_bonus ) );
		assign( "depr_stabas_sl_15", var_data((ssc_number_t) depr_stabas_sl_15 ) );

		assign("depr_stabas_percent_sl_20", var_data((ssc_number_t)  (depr_stabas_sl_20_frac*100.0)));
		assign( "depr_alloc_sl_20", var_data((ssc_number_t) depr_alloc_sl_20 ) );
		double depr_stabas_ibi_reduc_sl_20 = depr_stabas_sl_20_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_sl_20 = depr_stabas_sl_20_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_sl_20", var_data((ssc_number_t) depr_stabas_ibi_reduc_sl_20 ) );
		assign( "depr_stabas_cbi_reduc_sl_20", var_data((ssc_number_t) depr_stabas_cbi_reduc_sl_20 ) );
 		assign( "depr_stabas_prior_itc_sl_20", var_data((ssc_number_t) ( depr_alloc_sl_20 - depr_stabas_ibi_reduc_sl_20 - depr_stabas_cbi_reduc_sl_20)) );
 		assign( "itc_sta_qual_sl_20", var_data((ssc_number_t) itc_sta_qual_sl_20 ) );
		double depr_stabas_percent_qual_sl_20 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_sl_20 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_sl_20", var_data((ssc_number_t) depr_stabas_percent_qual_sl_20) );
 		assign( "depr_stabas_percent_amount_sl_20", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_20/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_sl_20", var_data((ssc_number_t) itc_disallow_sta_percent_sl_20 ) );
 		assign( "depr_stabas_fixed_amount_sl_20", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_20/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_sl_20", var_data((ssc_number_t) itc_disallow_sta_fixed_sl_20 ) );
		double depr_stabas_itc_sta_reduction_sl_20 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_20 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_20;
		double depr_stabas_itc_fed_reduction_sl_20 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_20 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_20;
		assign( "depr_stabas_itc_sta_reduction_sl_20", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_sl_20 ) );
		assign( "depr_stabas_itc_fed_reduction_sl_20", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_sl_20 ) );
		assign( "depr_stabas_after_itc_sl_20", var_data((ssc_number_t) (depr_stabas_sl_20 + depr_stabas_sl_20_bonus) ) );
		assign( "depr_stabas_first_year_bonus_sl_20", var_data((ssc_number_t) depr_stabas_sl_20_bonus ) );
		assign( "depr_stabas_sl_20", var_data((ssc_number_t) depr_stabas_sl_20 ) );

		assign("depr_stabas_percent_sl_39", var_data((ssc_number_t)  (depr_stabas_sl_39_frac*100.0)));
		assign( "depr_alloc_sl_39", var_data((ssc_number_t) depr_alloc_sl_39 ) );
		double depr_stabas_ibi_reduc_sl_39 = depr_stabas_sl_39_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_sl_39 = depr_stabas_sl_39_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_sl_39", var_data((ssc_number_t) depr_stabas_ibi_reduc_sl_39 ) );
		assign( "depr_stabas_cbi_reduc_sl_39", var_data((ssc_number_t) depr_stabas_cbi_reduc_sl_39 ) );
 		assign( "depr_stabas_prior_itc_sl_39", var_data((ssc_number_t) ( depr_alloc_sl_39 - depr_stabas_ibi_reduc_sl_39 - depr_stabas_cbi_reduc_sl_39)) );
 		assign( "itc_sta_qual_sl_39", var_data((ssc_number_t) itc_sta_qual_sl_39 ) );
		double depr_stabas_percent_qual_sl_39 = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_sl_39 /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_sl_39", var_data((ssc_number_t) depr_stabas_percent_qual_sl_39) );
 		assign( "depr_stabas_percent_amount_sl_39", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_39/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_sl_39", var_data((ssc_number_t) itc_disallow_sta_percent_sl_39 ) );
 		assign( "depr_stabas_fixed_amount_sl_39", var_data((ssc_number_t) (depr_stabas_percent_qual_sl_39/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_sl_39", var_data((ssc_number_t) itc_disallow_sta_fixed_sl_39 ) );
		double depr_stabas_itc_sta_reduction_sl_39 = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_sl_39 + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_sl_39;
		double depr_stabas_itc_fed_reduction_sl_39 = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_sl_39 + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_sl_39;
		assign( "depr_stabas_itc_sta_reduction_sl_39", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_sl_39 ) );
		assign( "depr_stabas_itc_fed_reduction_sl_39", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_sl_39 ) );
		assign( "depr_stabas_after_itc_sl_39", var_data((ssc_number_t) (depr_stabas_sl_39 + depr_stabas_sl_39_bonus) ) );
		assign( "depr_stabas_first_year_bonus_sl_39", var_data((ssc_number_t) depr_stabas_sl_39_bonus ) );
		assign( "depr_stabas_sl_39", var_data((ssc_number_t) depr_stabas_sl_39 ) );

		assign("depr_stabas_percent_custom", var_data((ssc_number_t)  (depr_stabas_custom_frac*100.0)));
		assign( "depr_alloc_custom", var_data((ssc_number_t) depr_alloc_custom ) );
		double depr_stabas_ibi_reduc_custom = depr_stabas_custom_frac * depr_sta_reduction_ibi;
		double depr_stabas_cbi_reduc_custom = depr_stabas_custom_frac * depr_sta_reduction_cbi;
		assign( "depr_stabas_ibi_reduc_custom", var_data((ssc_number_t) depr_stabas_ibi_reduc_custom ) );
		assign( "depr_stabas_cbi_reduc_custom", var_data((ssc_number_t) depr_stabas_cbi_reduc_custom ) );
 		assign( "depr_stabas_prior_itc_custom", var_data((ssc_number_t) ( depr_alloc_custom - depr_stabas_ibi_reduc_custom - depr_stabas_cbi_reduc_custom)) );
 		assign( "itc_sta_qual_custom", var_data((ssc_number_t) itc_sta_qual_custom ) );
		double depr_stabas_percent_qual_custom = (itc_sta_qual_total > 0)? 100.0 * itc_sta_qual_custom /  itc_sta_qual_total:0.0;
 		assign( "depr_stabas_percent_qual_custom", var_data((ssc_number_t) depr_stabas_percent_qual_custom) );
 		assign( "depr_stabas_percent_amount_custom", var_data((ssc_number_t) (depr_stabas_percent_qual_custom/100.0 * itc_sta_per)) );
		assign( "itc_disallow_sta_percent_custom", var_data((ssc_number_t) itc_disallow_sta_percent_custom ) );
 		assign( "depr_stabas_fixed_amount_custom", var_data((ssc_number_t) (depr_stabas_percent_qual_custom/100.0 * itc_sta_amount)) );
		assign( "itc_disallow_sta_fixed_custom", var_data((ssc_number_t) itc_disallow_sta_fixed_custom ) );
		double depr_stabas_itc_sta_reduction_custom = itc_sta_percent_deprbas_sta * itc_disallow_sta_percent_custom + itc_sta_amount_deprbas_sta * itc_disallow_sta_fixed_custom;
		double depr_stabas_itc_fed_reduction_custom = itc_fed_percent_deprbas_sta * itc_disallow_fed_percent_custom + itc_fed_amount_deprbas_sta * itc_disallow_fed_fixed_custom;
		assign( "depr_stabas_itc_sta_reduction_custom", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_custom ) );
		assign( "depr_stabas_itc_fed_reduction_custom", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_custom ) );
		assign( "depr_stabas_after_itc_custom", var_data((ssc_number_t) (depr_stabas_custom + depr_stabas_custom_bonus) ) );
		assign( "depr_stabas_first_year_bonus_custom", var_data((ssc_number_t) depr_stabas_custom_bonus ) );
		assign( "depr_stabas_custom", var_data((ssc_number_t) depr_stabas_custom ) );

		assign("depr_stabas_percent_total", var_data((ssc_number_t)  (100.0*(depr_stabas_macrs_5_frac+depr_stabas_macrs_15_frac+depr_stabas_sl_5_frac+depr_stabas_sl_15_frac+depr_stabas_sl_20_frac+depr_stabas_sl_39_frac+depr_stabas_custom_frac))));
		assign( "depr_alloc_total", var_data((ssc_number_t) depr_alloc_total ) );
		assign( "depr_stabas_ibi_reduc_total", var_data((ssc_number_t) depr_sta_reduction_ibi ) );
		assign( "depr_stabas_cbi_reduc_total", var_data((ssc_number_t) depr_sta_reduction_cbi ) );
 		assign( "depr_stabas_prior_itc_total", var_data((ssc_number_t) ( depr_alloc_total - depr_sta_reduction_ibi - depr_sta_reduction_cbi)) );
 		assign( "itc_sta_qual_total", var_data((ssc_number_t) itc_sta_qual_total ) );
 		assign( "depr_stabas_percent_qual_total", var_data((ssc_number_t) 100.0) );
 		assign( "depr_stabas_percent_amount_total", var_data((ssc_number_t) itc_sta_per) );
		assign( "itc_disallow_sta_percent_total", var_data((ssc_number_t) (itc_disallow_sta_percent_macrs_5 + itc_disallow_sta_percent_macrs_15 + itc_disallow_sta_percent_sl_5 + itc_disallow_sta_percent_sl_15 + itc_disallow_sta_percent_sl_20 + itc_disallow_sta_percent_sl_39 + itc_disallow_sta_percent_custom) ) );
 		assign( "depr_stabas_fixed_amount_total", var_data((ssc_number_t) itc_sta_amount) );
		assign( "itc_disallow_sta_fixed_total", var_data((ssc_number_t) (itc_disallow_sta_fixed_macrs_5 + itc_disallow_sta_fixed_macrs_15 + itc_disallow_sta_fixed_sl_5 + itc_disallow_sta_fixed_sl_15 + itc_disallow_sta_fixed_sl_20 + itc_disallow_sta_fixed_sl_39 + itc_disallow_sta_fixed_custom) ) );
		double depr_stabas_itc_sta_reduction_total = depr_stabas_itc_sta_reduction_macrs_5 + depr_stabas_itc_sta_reduction_macrs_15 + depr_stabas_itc_sta_reduction_sl_5 + depr_stabas_itc_sta_reduction_sl_15 + depr_stabas_itc_sta_reduction_sl_20 + depr_stabas_itc_sta_reduction_sl_39 + depr_stabas_itc_sta_reduction_custom;
		assign( "depr_stabas_itc_sta_reduction_total", var_data((ssc_number_t) depr_stabas_itc_sta_reduction_total ) );
		double depr_stabas_itc_fed_reduction_total = depr_stabas_itc_fed_reduction_macrs_5 + depr_stabas_itc_fed_reduction_macrs_15 + depr_stabas_itc_fed_reduction_sl_5 + depr_stabas_itc_fed_reduction_sl_15 + depr_stabas_itc_fed_reduction_sl_20 + depr_stabas_itc_fed_reduction_sl_39 + depr_stabas_itc_fed_reduction_custom;
		assign( "depr_stabas_itc_fed_reduction_total", var_data((ssc_number_t) depr_stabas_itc_fed_reduction_total ) );
		double depr_stabas_first_year_bonus_total = depr_stabas_macrs_5_bonus+depr_stabas_macrs_15_bonus+depr_stabas_sl_5_bonus+depr_stabas_sl_15_bonus+depr_stabas_sl_20_bonus+depr_stabas_sl_39_bonus+depr_stabas_custom_bonus;
		assign( "depr_stabas_after_itc_total", var_data((ssc_number_t) (depr_stabas_total + depr_stabas_first_year_bonus_total) ) );
		assign( "depr_stabas_first_year_bonus_total", var_data((ssc_number_t) depr_stabas_first_year_bonus_total ) );
		assign( "depr_stabas_total", var_data((ssc_number_t) depr_stabas_total ) );

	
		assign( "itc_sta_percent_total", var_data((ssc_number_t) itc_sta_per ) );
		assign( "itc_sta_fixed_total", var_data((ssc_number_t) itc_sta_amount ) );



		// Federal ITC/depreciation table
		assign("depr_fedbas_percent_macrs_5", var_data((ssc_number_t)  (depr_fedbas_macrs_5_frac*100.0)));
		assign( "depr_alloc_macrs_5", var_data((ssc_number_t) depr_alloc_macrs_5 ) );
		double depr_fedbas_ibi_reduc_macrs_5 = depr_fedbas_macrs_5_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_macrs_5 = depr_fedbas_macrs_5_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_macrs_5", var_data((ssc_number_t) depr_fedbas_ibi_reduc_macrs_5 ) );
		assign( "depr_fedbas_cbi_reduc_macrs_5", var_data((ssc_number_t) depr_fedbas_cbi_reduc_macrs_5 ) );
 		assign( "depr_fedbas_prior_itc_macrs_5", var_data((ssc_number_t) ( depr_alloc_macrs_5 - depr_fedbas_ibi_reduc_macrs_5 - depr_fedbas_cbi_reduc_macrs_5)) );
 		assign( "itc_fed_qual_macrs_5", var_data((ssc_number_t) itc_fed_qual_macrs_5 ) );
		double depr_fedbas_percent_qual_macrs_5 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_macrs_5 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_macrs_5", var_data((ssc_number_t) depr_fedbas_percent_qual_macrs_5) );
 		assign( "depr_fedbas_percent_amount_macrs_5", var_data((ssc_number_t) (depr_fedbas_percent_qual_macrs_5/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_macrs_5", var_data((ssc_number_t) itc_disallow_fed_percent_macrs_5 ) );
 		assign( "depr_fedbas_fixed_amount_macrs_5", var_data((ssc_number_t) (depr_fedbas_percent_qual_macrs_5/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_macrs_5", var_data((ssc_number_t) itc_disallow_fed_fixed_macrs_5 ) );
		double depr_fedbas_itc_sta_reduction_macrs_5 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_macrs_5 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_macrs_5;
		double depr_fedbas_itc_fed_reduction_macrs_5 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_macrs_5 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_macrs_5;
		assign( "depr_fedbas_itc_sta_reduction_macrs_5", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_macrs_5 ) );
		assign( "depr_fedbas_itc_fed_reduction_macrs_5", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_macrs_5 ) );
		assign( "depr_fedbas_after_itc_macrs_5", var_data((ssc_number_t) (depr_fedbas_macrs_5 + depr_fedbas_macrs_5_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_macrs_5", var_data((ssc_number_t) depr_fedbas_macrs_5_bonus ) );
		assign( "depr_fedbas_macrs_5", var_data((ssc_number_t) depr_fedbas_macrs_5 ) );

		assign("depr_fedbas_percent_macrs_15", var_data((ssc_number_t)  (depr_fedbas_macrs_15_frac*100.0)));
		assign( "depr_alloc_macrs_15", var_data((ssc_number_t) depr_alloc_macrs_15 ) );
		double depr_fedbas_ibi_reduc_macrs_15 = depr_fedbas_macrs_15_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_macrs_15 = depr_fedbas_macrs_15_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_macrs_15", var_data((ssc_number_t) depr_fedbas_ibi_reduc_macrs_15 ) );
		assign( "depr_fedbas_cbi_reduc_macrs_15", var_data((ssc_number_t) depr_fedbas_cbi_reduc_macrs_15 ) );
 		assign( "depr_fedbas_prior_itc_macrs_15", var_data((ssc_number_t) ( depr_alloc_macrs_15 - depr_fedbas_ibi_reduc_macrs_15 - depr_fedbas_cbi_reduc_macrs_15)) );
 		assign( "itc_fed_qual_macrs_15", var_data((ssc_number_t) itc_fed_qual_macrs_15 ) );
		double depr_fedbas_percent_qual_macrs_15 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_macrs_15 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_macrs_15", var_data((ssc_number_t) depr_fedbas_percent_qual_macrs_15) );
 		assign( "depr_fedbas_percent_amount_macrs_15", var_data((ssc_number_t) (depr_fedbas_percent_qual_macrs_15/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_macrs_15", var_data((ssc_number_t) itc_disallow_fed_percent_macrs_15 ) );
 		assign( "depr_fedbas_fixed_amount_macrs_15", var_data((ssc_number_t) (depr_fedbas_percent_qual_macrs_15/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_macrs_15", var_data((ssc_number_t) itc_disallow_fed_fixed_macrs_15 ) );
		double depr_fedbas_itc_sta_reduction_macrs_15 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_macrs_15 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_macrs_15;
		double depr_fedbas_itc_fed_reduction_macrs_15 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_macrs_15 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_macrs_15;
		assign( "depr_fedbas_itc_sta_reduction_macrs_15", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_macrs_15 ) );
		assign( "depr_fedbas_itc_fed_reduction_macrs_15", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_macrs_15 ) );
		assign( "depr_fedbas_after_itc_macrs_15", var_data((ssc_number_t) (depr_fedbas_macrs_15 + depr_fedbas_macrs_15_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_macrs_15", var_data((ssc_number_t) depr_fedbas_macrs_15_bonus ) );
		assign( "depr_fedbas_macrs_15", var_data((ssc_number_t) depr_fedbas_macrs_15 ) );

		assign("depr_fedbas_percent_sl_5", var_data((ssc_number_t)  (depr_fedbas_sl_5_frac*100.0)));
		assign( "depr_alloc_sl_5", var_data((ssc_number_t) depr_alloc_sl_5 ) );
		double depr_fedbas_ibi_reduc_sl_5 = depr_fedbas_sl_5_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_sl_5 = depr_fedbas_sl_5_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_sl_5", var_data((ssc_number_t) depr_fedbas_ibi_reduc_sl_5 ) );
		assign( "depr_fedbas_cbi_reduc_sl_5", var_data((ssc_number_t) depr_fedbas_cbi_reduc_sl_5 ) );
 		assign( "depr_fedbas_prior_itc_sl_5", var_data((ssc_number_t) ( depr_alloc_sl_5 - depr_fedbas_ibi_reduc_sl_5 - depr_fedbas_cbi_reduc_sl_5)) );
 		assign( "itc_fed_qual_sl_5", var_data((ssc_number_t) itc_fed_qual_sl_5 ) );
		double depr_fedbas_percent_qual_sl_5 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_sl_5 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_sl_5", var_data((ssc_number_t) depr_fedbas_percent_qual_sl_5) );
 		assign( "depr_fedbas_percent_amount_sl_5", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_5/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_sl_5", var_data((ssc_number_t) itc_disallow_fed_percent_sl_5 ) );
 		assign( "depr_fedbas_fixed_amount_sl_5", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_5/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_sl_5", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_5 ) );
		double depr_fedbas_itc_sta_reduction_sl_5 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_5 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_5;
		double depr_fedbas_itc_fed_reduction_sl_5 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_5 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_5;
		assign( "depr_fedbas_itc_sta_reduction_sl_5", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_sl_5 ) );
		assign( "depr_fedbas_itc_fed_reduction_sl_5", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_sl_5 ) );
		assign( "depr_fedbas_after_itc_sl_5", var_data((ssc_number_t) (depr_fedbas_sl_5 + depr_fedbas_sl_5_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_sl_5", var_data((ssc_number_t) depr_fedbas_sl_5_bonus ) );
		assign( "depr_fedbas_sl_5", var_data((ssc_number_t) depr_fedbas_sl_5 ) );

		assign("depr_fedbas_percent_sl_15", var_data((ssc_number_t)  (depr_fedbas_sl_15_frac*100.0)));
		assign( "depr_alloc_sl_15", var_data((ssc_number_t) depr_alloc_sl_15 ) );
		double depr_fedbas_ibi_reduc_sl_15 = depr_fedbas_sl_15_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_sl_15 = depr_fedbas_sl_15_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_sl_15", var_data((ssc_number_t) depr_fedbas_ibi_reduc_sl_15 ) );
		assign( "depr_fedbas_cbi_reduc_sl_15", var_data((ssc_number_t) depr_fedbas_cbi_reduc_sl_15 ) );
 		assign( "depr_fedbas_prior_itc_sl_15", var_data((ssc_number_t) ( depr_alloc_sl_15 - depr_fedbas_ibi_reduc_sl_15 - depr_fedbas_cbi_reduc_sl_15)) );
 		assign( "itc_fed_qual_sl_15", var_data((ssc_number_t) itc_fed_qual_sl_15 ) );
		double depr_fedbas_percent_qual_sl_15 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_sl_15 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_sl_15", var_data((ssc_number_t) depr_fedbas_percent_qual_sl_15) );
 		assign( "depr_fedbas_percent_amount_sl_15", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_15/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_sl_15", var_data((ssc_number_t) itc_disallow_fed_percent_sl_15 ) );
 		assign( "depr_fedbas_fixed_amount_sl_15", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_15/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_sl_15", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_15 ) );
		double depr_fedbas_itc_sta_reduction_sl_15 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_15 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_15;
		double depr_fedbas_itc_fed_reduction_sl_15 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_15 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_15;
		assign( "depr_fedbas_itc_sta_reduction_sl_15", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_sl_15 ) );
		assign( "depr_fedbas_itc_fed_reduction_sl_15", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_sl_15 ) );
		assign( "depr_fedbas_after_itc_sl_15", var_data((ssc_number_t) (depr_fedbas_sl_15 + depr_fedbas_sl_15_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_sl_15", var_data((ssc_number_t) depr_fedbas_sl_15_bonus ) );
		assign( "depr_fedbas_sl_15", var_data((ssc_number_t) depr_fedbas_sl_15 ) );

		assign("depr_fedbas_percent_sl_20", var_data((ssc_number_t)  (depr_fedbas_sl_20_frac*100.0)));
		assign( "depr_alloc_sl_20", var_data((ssc_number_t) depr_alloc_sl_20 ) );
		double depr_fedbas_ibi_reduc_sl_20 = depr_fedbas_sl_20_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_sl_20 = depr_fedbas_sl_20_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_sl_20", var_data((ssc_number_t) depr_fedbas_ibi_reduc_sl_20 ) );
		assign( "depr_fedbas_cbi_reduc_sl_20", var_data((ssc_number_t) depr_fedbas_cbi_reduc_sl_20 ) );
 		assign( "depr_fedbas_prior_itc_sl_20", var_data((ssc_number_t) ( depr_alloc_sl_20 - depr_fedbas_ibi_reduc_sl_20 - depr_fedbas_cbi_reduc_sl_20)) );
 		assign( "itc_fed_qual_sl_20", var_data((ssc_number_t) itc_fed_qual_sl_20 ) );
		double depr_fedbas_percent_qual_sl_20 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_sl_20 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_sl_20", var_data((ssc_number_t) depr_fedbas_percent_qual_sl_20) );
 		assign( "depr_fedbas_percent_amount_sl_20", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_20/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_sl_20", var_data((ssc_number_t) itc_disallow_fed_percent_sl_20 ) );
 		assign( "depr_fedbas_fixed_amount_sl_20", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_20/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_sl_20", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_20 ) );
		double depr_fedbas_itc_sta_reduction_sl_20 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_20 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_20;
		double depr_fedbas_itc_fed_reduction_sl_20 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_20 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_20;
		assign( "depr_fedbas_itc_sta_reduction_sl_20", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_sl_20 ) );
		assign( "depr_fedbas_itc_fed_reduction_sl_20", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_sl_20 ) );
		assign( "depr_fedbas_after_itc_sl_20", var_data((ssc_number_t) (depr_fedbas_sl_20 + depr_fedbas_sl_20_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_sl_20", var_data((ssc_number_t) depr_fedbas_sl_20_bonus ) );
		assign( "depr_fedbas_sl_20", var_data((ssc_number_t) depr_fedbas_sl_20 ) );

		assign("depr_fedbas_percent_sl_39", var_data((ssc_number_t)  (depr_fedbas_sl_39_frac*100.0)));
		assign( "depr_alloc_sl_39", var_data((ssc_number_t) depr_alloc_sl_39 ) );
		double depr_fedbas_ibi_reduc_sl_39 = depr_fedbas_sl_39_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_sl_39 = depr_fedbas_sl_39_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_sl_39", var_data((ssc_number_t) depr_fedbas_ibi_reduc_sl_39 ) );
		assign( "depr_fedbas_cbi_reduc_sl_39", var_data((ssc_number_t) depr_fedbas_cbi_reduc_sl_39 ) );
 		assign( "depr_fedbas_prior_itc_sl_39", var_data((ssc_number_t) ( depr_alloc_sl_39 - depr_fedbas_ibi_reduc_sl_39 - depr_fedbas_cbi_reduc_sl_39)) );
 		assign( "itc_fed_qual_sl_39", var_data((ssc_number_t) itc_fed_qual_sl_39 ) );
		double depr_fedbas_percent_qual_sl_39 = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_sl_39 /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_sl_39", var_data((ssc_number_t) depr_fedbas_percent_qual_sl_39) );
 		assign( "depr_fedbas_percent_amount_sl_39", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_39/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_sl_39", var_data((ssc_number_t) itc_disallow_fed_percent_sl_39 ) );
 		assign( "depr_fedbas_fixed_amount_sl_39", var_data((ssc_number_t) (depr_fedbas_percent_qual_sl_39/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_sl_39", var_data((ssc_number_t) itc_disallow_fed_fixed_sl_39 ) );
		double depr_fedbas_itc_sta_reduction_sl_39 = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_sl_39 + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_sl_39;
		double depr_fedbas_itc_fed_reduction_sl_39 = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_sl_39 + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_sl_39;
		assign( "depr_fedbas_itc_sta_reduction_sl_39", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_sl_39 ) );
		assign( "depr_fedbas_itc_fed_reduction_sl_39", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_sl_39 ) );
		assign( "depr_fedbas_after_itc_sl_39", var_data((ssc_number_t) (depr_fedbas_sl_39 + depr_fedbas_sl_39_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_sl_39", var_data((ssc_number_t) depr_fedbas_sl_39_bonus ) );
		assign( "depr_fedbas_sl_39", var_data((ssc_number_t) depr_fedbas_sl_39 ) );

		assign("depr_fedbas_percent_custom", var_data((ssc_number_t)  (depr_fedbas_custom_frac*100.0)));
		assign( "depr_alloc_custom", var_data((ssc_number_t) depr_alloc_custom ) );
		double depr_fedbas_ibi_reduc_custom = depr_fedbas_custom_frac * depr_fed_reduction_ibi;
		double depr_fedbas_cbi_reduc_custom = depr_fedbas_custom_frac * depr_fed_reduction_cbi;
		assign( "depr_fedbas_ibi_reduc_custom", var_data((ssc_number_t) depr_fedbas_ibi_reduc_custom ) );
		assign( "depr_fedbas_cbi_reduc_custom", var_data((ssc_number_t) depr_fedbas_cbi_reduc_custom ) );
 		assign( "depr_fedbas_prior_itc_custom", var_data((ssc_number_t) ( depr_alloc_custom - depr_fedbas_ibi_reduc_custom - depr_fedbas_cbi_reduc_custom)) );
 		assign( "itc_fed_qual_custom", var_data((ssc_number_t) itc_fed_qual_custom ) );
		double depr_fedbas_percent_qual_custom = (itc_fed_qual_total > 0)? 100.0 * itc_fed_qual_custom /  itc_fed_qual_total:0.0;
 		assign( "depr_fedbas_percent_qual_custom", var_data((ssc_number_t) depr_fedbas_percent_qual_custom) );
 		assign( "depr_fedbas_percent_amount_custom", var_data((ssc_number_t) (depr_fedbas_percent_qual_custom/100.0 * itc_fed_per)) );
		assign( "itc_disallow_fed_percent_custom", var_data((ssc_number_t) itc_disallow_fed_percent_custom ) );
 		assign( "depr_fedbas_fixed_amount_custom", var_data((ssc_number_t) (depr_fedbas_percent_qual_custom/100.0 * itc_fed_amount)) );
		assign( "itc_disallow_fed_fixed_custom", var_data((ssc_number_t) itc_disallow_fed_fixed_custom ) );
		double depr_fedbas_itc_sta_reduction_custom = itc_sta_percent_deprbas_fed * itc_disallow_sta_percent_custom + itc_sta_amount_deprbas_fed * itc_disallow_sta_fixed_custom;
		double depr_fedbas_itc_fed_reduction_custom = itc_fed_percent_deprbas_fed * itc_disallow_fed_percent_custom + itc_fed_amount_deprbas_fed * itc_disallow_fed_fixed_custom;
		assign( "depr_fedbas_itc_sta_reduction_custom", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_custom ) );
		assign( "depr_fedbas_itc_fed_reduction_custom", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_custom ) );
		assign( "depr_fedbas_after_itc_custom", var_data((ssc_number_t) (depr_fedbas_custom + depr_fedbas_custom_bonus) ) );
		assign( "depr_fedbas_first_year_bonus_custom", var_data((ssc_number_t) depr_fedbas_custom_bonus ) );
		assign( "depr_fedbas_custom", var_data((ssc_number_t) depr_fedbas_custom ) );


		assign("depr_fedbas_percent_total", var_data((ssc_number_t)  (100.0*(depr_fedbas_macrs_5_frac+depr_fedbas_macrs_15_frac+depr_fedbas_sl_5_frac+depr_fedbas_sl_15_frac+depr_fedbas_sl_20_frac+depr_fedbas_sl_39_frac+depr_fedbas_custom_frac))));
		assign( "depr_alloc_total", var_data((ssc_number_t) depr_alloc_total ) );
		assign( "depr_fedbas_ibi_reduc_total", var_data((ssc_number_t) depr_sta_reduction_ibi ) );
		assign( "depr_fedbas_cbi_reduc_total", var_data((ssc_number_t) depr_sta_reduction_cbi ) );
 		assign( "depr_fedbas_prior_itc_total", var_data((ssc_number_t) ( depr_alloc_total - depr_sta_reduction_ibi - depr_sta_reduction_cbi)) );
 		assign( "itc_sta_qual_total", var_data((ssc_number_t) itc_sta_qual_total ) );
 		assign( "depr_fedbas_percent_qual_total", var_data((ssc_number_t) 100.0) );
 		assign( "depr_fedbas_percent_amount_total", var_data((ssc_number_t) itc_fed_per) );
		assign( "itc_disallow_fed_percent_total", var_data((ssc_number_t) (itc_disallow_fed_percent_macrs_5 + itc_disallow_fed_percent_macrs_15 + itc_disallow_fed_percent_sl_5 + itc_disallow_fed_percent_sl_15 + itc_disallow_fed_percent_sl_20 + itc_disallow_fed_percent_sl_39 + itc_disallow_fed_percent_custom) ) );
 		assign( "depr_fedbas_fixed_amount_total", var_data((ssc_number_t) itc_fed_amount) );
		assign( "itc_disallow_fed_fixed_total", var_data((ssc_number_t) (itc_disallow_fed_fixed_macrs_5 + itc_disallow_fed_fixed_macrs_15 + itc_disallow_fed_fixed_sl_5 + itc_disallow_fed_fixed_sl_15 + itc_disallow_fed_fixed_sl_20 + itc_disallow_fed_fixed_sl_39 + itc_disallow_fed_fixed_custom) ) );
		double depr_fedbas_itc_sta_reduction_total = depr_fedbas_itc_sta_reduction_macrs_5 + depr_fedbas_itc_sta_reduction_macrs_15 + depr_fedbas_itc_sta_reduction_sl_5 + depr_fedbas_itc_sta_reduction_sl_15 + depr_fedbas_itc_sta_reduction_sl_20 + depr_fedbas_itc_sta_reduction_sl_39 + depr_fedbas_itc_sta_reduction_custom;
		assign( "depr_fedbas_itc_sta_reduction_total", var_data((ssc_number_t) depr_fedbas_itc_sta_reduction_total ) );
		double depr_fedbas_itc_fed_reduction_total = depr_fedbas_itc_fed_reduction_macrs_5 + depr_fedbas_itc_fed_reduction_macrs_15 + depr_fedbas_itc_fed_reduction_sl_5 + depr_fedbas_itc_fed_reduction_sl_15 + depr_fedbas_itc_fed_reduction_sl_20 + depr_fedbas_itc_fed_reduction_sl_39 + depr_fedbas_itc_fed_reduction_custom;
		assign( "depr_fedbas_itc_fed_reduction_total", var_data((ssc_number_t) depr_fedbas_itc_fed_reduction_total ) );
		double depr_fedbas_first_year_bonus_total = depr_fedbas_macrs_5_bonus+depr_fedbas_macrs_15_bonus+depr_fedbas_sl_5_bonus+depr_fedbas_sl_15_bonus+depr_fedbas_sl_20_bonus+depr_fedbas_sl_39_bonus+depr_fedbas_custom_bonus;
		assign( "depr_fedbas_after_itc_total", var_data((ssc_number_t) (depr_fedbas_total + depr_fedbas_first_year_bonus_total) ) );
		assign( "depr_fedbas_first_year_bonus_total", var_data((ssc_number_t) depr_fedbas_first_year_bonus_total ) );
		assign( "depr_fedbas_total", var_data((ssc_number_t) depr_fedbas_total ) );

		assign( "depr_alloc_none_percent", var_data((ssc_number_t) (depr_alloc_none_frac*100.0) ) );
		assign( "depr_alloc_none", var_data((ssc_number_t) depr_alloc_none ) );
		assign( "depr_alloc_total", var_data((ssc_number_t) depr_alloc_total ) );
		// Project cash flow

	// for cost stacked bars
		//npv(CF_energy_value, nyears, nom_discount_rate)
		// present value of o and m value - note - present value is distributive - sum of pv = pv of sum
		double pvAnnualOandM = npv(CF_om_fixed_expense, nyears, nom_discount_rate);
		double pvFixedOandM = npv(CF_om_capacity_expense, nyears, nom_discount_rate);
		double pvVariableOandM = npv(CF_om_production_expense, nyears, nom_discount_rate);
		double pvFuelOandM = npv(CF_om_fuel_expense, nyears, nom_discount_rate);
		double pvOptFuel1OandM = npv(CF_om_opt_fuel_1_expense, nyears, nom_discount_rate);
		double pvOptFuel2OandM = npv(CF_om_opt_fuel_2_expense, nyears, nom_discount_rate);
	//	double pvWaterOandM = NetPresentValue(sv[svNominalDiscountRate], cf[cfAnnualWaterCost], analysis_period);

		assign( "present_value_oandm",  var_data((ssc_number_t)(pvAnnualOandM + pvFixedOandM + pvVariableOandM + pvFuelOandM))); // + pvWaterOandM);

		assign( "present_value_oandm_nonfuel", var_data((ssc_number_t)(pvAnnualOandM + pvFixedOandM + pvVariableOandM)));
		assign( "present_value_fuel", var_data((ssc_number_t)(pvFuelOandM + pvOptFuel1OandM + pvOptFuel2OandM)));

		// present value of insurance and property tax
		double pvInsurance = npv(CF_insurance_expense, nyears, nom_discount_rate);
		double pvPropertyTax = npv(CF_property_tax_expense, nyears, nom_discount_rate);

		assign( "present_value_insandproptax", var_data((ssc_number_t)(pvInsurance + pvPropertyTax)));
	}



	// end exec


	// std lib
	void major_equipment_depreciation( int cf_equipment_expenditure, int cf_depr_sched, int expenditure_year, int analysis_period, int cf_equipment_depreciation )
	{
		// depreciate equipment cost in expenditure_year according to depr_sched schedule subject to cutoff by analysis_period
		if ( (expenditure_year > 0 ) && (expenditure_year <= analysis_period))
		{
			// sign convention from DHF v3 model
			double depreciable_basis = -cf.at(cf_equipment_expenditure, expenditure_year);
			for (int i=expenditure_year; i<=analysis_period; i++)
			{
				cf.at(cf_equipment_depreciation,i) += depreciable_basis * cf.at(cf_depr_sched,i-expenditure_year+1);
			}

		}
	}

	// std lib
	void depreciation_sched_5_year_macrs_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			switch(i)
			{
			case 1: factor = 0.2000; break;
			case 2: factor = 0.3200; break;
			case 3: factor = 0.1920; break;
			case 4: factor = 0.1152; break;
			case 5: factor = 0.1152; break;
			case 6: factor = 0.0576; break;
			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}
	// std lib
	void depreciation_sched_15_year_macrs_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			switch(i)
			{
			case 1: factor = 0.0500; break;
			case 2: factor = 0.0950; break;
			case 3: factor = 0.0855; break;
			case 4: factor = 0.0770; break;
			case 5: factor = 0.0693; break;
			case 6: factor = 0.0623; break;
			case 7: factor = 0.0590; break;
			case 8: factor = 0.0590; break;
			case 9: factor = 0.0591; break;
			case 10: factor = 0.0590; break;
			case 11: factor = 0.0591; break;
			case 12: factor = 0.0590; break;
			case 13: factor = 0.0591; break;
			case 14: factor = 0.0590; break;
			case 15: factor = 0.0591; break;
			case 16: factor = 0.0295; break;
			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}
	// std lib
	void depreciation_sched_5_year_straight_line_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			switch(i)
			{
			case 1: factor = 0.1000; break;
			case 2: factor = 0.2000; break;
			case 3: factor = 0.2000; break;
			case 4: factor = 0.2000; break;
			case 5: factor = 0.2000; break;
			case 6: factor = 0.1000; break;
			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}
	// std lib
	void depreciation_sched_15_year_straight_line_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			switch(i)
			{
			case 1: factor = 0.0333; break;
			case 2: factor = 0.0667; break;
			case 3: factor = 0.0667; break;
			case 4: factor = 0.0667; break;
			case 5: factor = 0.0667; break;
			case 6: factor = 0.0667; break;
			case 7: factor = 0.0667; break;
			case 8: factor = 0.0666; break;
			case 9: factor = 0.0667; break;
			case 10: factor = 0.0666; break;
			case 11: factor = 0.0667; break;
			case 12: factor = 0.0666; break;
			case 13: factor = 0.0667; break;
			case 14: factor = 0.0666; break;
			case 15: factor = 0.0667; break;
			case 16: factor = 0.0333; break;

			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}
	// std lib
	void depreciation_sched_20_year_straight_line_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			switch(i)
			{
			case 1: factor = 0.0250; break;
			case 2: factor = 0.0500; break;
			case 3: factor = 0.0500; break;
			case 4: factor = 0.0500; break;
			case 5: factor = 0.0500; break;
			case 6: factor = 0.0500; break;
			case 7: factor = 0.0500; break;
			case 8: factor = 0.0500; break;
			case 9: factor = 0.0500; break;
			case 10: factor = 0.0500; break;
			case 11: factor = 0.0500; break;
			case 12: factor = 0.0500; break;
			case 13: factor = 0.0500; break;
			case 14: factor = 0.0500; break;
			case 15: factor = 0.0500; break;
			case 16: factor = 0.0500; break;
			case 17: factor = 0.0500; break;
			case 18: factor = 0.0500; break;
			case 19: factor = 0.0500; break;
			case 20: factor = 0.0500; break;
			case 21: factor = 0.0250; break;
			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}
	// std lib
	void depreciation_sched_39_year_straight_line_half_year( int cf_line, int nyears )
	{
		for (int i=1; i<=nyears; i++)
		{
			double factor = 0.0;
			double base=2.56410256410256e-2;
			switch(i)
			{
			case 1: factor = 0.5*base; break;
			case 2: factor = base; break;
			case 3: factor = base; break;
			case 4: factor = base; break;
			case 5: factor = base; break;
			case 6: factor = base; break;
			case 7: factor = base; break;
			case 8: factor = base; break;
			case 9: factor = base; break;
			case 10: factor = base; break;
			case 11: factor = base; break;
			case 12: factor = base; break;
			case 13: factor = base; break;
			case 14: factor = base; break;
			case 15: factor = base; break;
			case 16: factor = base; break;
			case 17: factor = base; break;
			case 18: factor = base; break;
			case 19: factor = base; break;
			case 20: factor = base; break;
			case 21: factor = base; break;
			case 22: factor = base; break;
			case 23: factor = base; break;
			case 24: factor = base; break;
			case 25: factor = base; break;
			case 26: factor = base; break;
			case 27: factor = base; break;
			case 28: factor = base; break;
			case 29: factor = base; break;
			case 30: factor = base; break;
			case 31: factor = base; break;
			case 32: factor = base; break;
			case 33: factor = base; break;
			case 34: factor = base; break;
			case 35: factor = base; break;
			case 36: factor = base; break;
			case 37: factor = base; break;
			case 38: factor = base; break;
			case 39: factor = base; break;
			case 40: factor = 0.5*base; break;
			default: factor = 0.0; break;
			}
			cf.at(cf_line, i) = factor;
		}
	}


	void depreciation_sched_custom(int cf_line, int nyears, const std::string &custom)
	{
		// computes custom percentage schedule 100%
		// if customValue is array then annual schedule of percent
		// if customValue is a single value then that value is used up to 100%
		int i;
		size_t count = 0;
		ssc_number_t *parr = as_array(custom, &count);
		for (i = 1; i<=nyears; i++)
		{
			cf.at(cf_line,i) = 0;
		}

		if (count ==1) // single value
		{
			cf.at(cf_line,1) = parr[0]/100.0;
		}
		else // annual schedule
		{// note schedules begin at year 1 (index 0)
			int scheduleDuration = ((int)count > nyears)? nyears : (int)count;
			for (i = 1; i<=scheduleDuration; i++)
			{
				cf.at(cf_line,i) = parr[i-1] / 100.0; // percentage to factor
			}
		}
	}



	// std lib
	void save_cf(int cf_line, int nyears, const std::string &name)
	{
		ssc_number_t *arrp = allocate( name, nyears+1 );
		for (int i=0;i<=nyears;i++)
			arrp[i] = (ssc_number_t)cf.at(cf_line, i);
	}

	void escal_or_annual( int cf_line, int nyears, const std::string &variable, 
			double inflation_rate, double scale, bool as_rate=true, double escal = 0.0)
	{
		size_t count;
		ssc_number_t *arrp = as_array(variable, &count);

		if (as_rate)
		{
			if (count == 1)
			{
				escal = inflation_rate + scale*arrp[0];
				for (int i=0; i < nyears; i++)
					cf.at(cf_line, i+1) = pow( 1+escal, i );
			}
			else
			{
				for (int i=0; i < nyears && i < (int)count; i++)
					cf.at(cf_line, i+1) = 1 + arrp[i]*scale;
			}
		}
		else
		{
			if (count == 1)
			{
				for (int i=0;i<nyears;i++)
					cf.at(cf_line, i+1) = arrp[0]*scale*pow( 1+escal+inflation_rate, i );
			}
			else
			{
				for (int i=0;i<nyears && i<(int)count;i++)
					cf.at(cf_line, i+1) = arrp[i]*scale;
			}
		}
	}

		void compute_production_incentive( int cf_line, int nyears, const std::string &s_val, const std::string &s_term, const std::string &s_escal )
	{
		size_t len = 0;
		ssc_number_t *parr = as_array(s_val, &len);
		int term = as_integer(s_term);
		double escal = as_double(s_escal)/100.0;

		if (len == 1)
		{
			for (int i=1;i<=nyears;i++)
				cf.at(cf_line, i) = (i <= term) ? parr[0] * cf.at(CF_energy_net,i) * pow(1 + escal, i-1) : 0.0;
		}
		else
		{
			for (int i=1;i<=nyears && i <= (int)len;i++)
				cf.at(cf_line, i) = parr[i-1]*cf.at(CF_energy_net,i);
		}
	}

		void compute_production_incentive_IRS_2010_37( int cf_line, int nyears, const std::string &s_val, const std::string &s_term, const std::string &s_escal )
	{
		// rounding based on IRS document and emails from John and Matt from DHF Financials 2/24/2011 and DHF model v4.4
		size_t len = 0;
		ssc_number_t *parr = as_array(s_val, &len);
		int term = as_integer(s_term);
		double escal = as_double(s_escal)/100.0;

		if (len == 1)
		{
			for (int i=1;i<=nyears;i++)
				cf.at(cf_line, i) = (i <= term) ? cf.at(CF_energy_net,i) / 1000.0 * round_dhf(1000.0 * parr[0] * pow(1 + escal, i-1)) : 0.0;
		}
		else
		{
			for (int i=1;i<=nyears && i <= (int)len;i++)
				cf.at(cf_line, i) = parr[i-1]*cf.at(CF_energy_net,i);
		}
	}

	void single_or_schedule( int cf_line, int nyears, double scale, const std::string &name )
	{
		size_t len = 0;
		ssc_number_t *p = as_array(name, &len);
		for (int i=1;i<=(int)len && i <= nyears;i++)
			cf.at(cf_line, i) = scale*p[i-1];
	}
	
	void single_or_schedule_check_max( int cf_line, int nyears, double scale, const std::string &name, const std::string &maxvar )
	{
		double max = as_double(maxvar);
		size_t len = 0;
		ssc_number_t *p = as_array(name, &len);
		for (int i=1;i<=(int)len && i <= nyears;i++)
			cf.at(cf_line, i) = min( scale*p[i-1], max );
	}

	double npv( int cf_line, int nyears, double rate ) throw ( general_error )
	{		
		//if (rate == -1.0) throw general_error("cannot calculate NPV with discount rate equal to -1.0");
		double rr = 1.0;
		if (rate != -1.0) rr = 1.0/(1.0+rate);
		double result = 0;
		for (int i=nyears;i>0;i--)
			result = rr * result + cf.at(cf_line,i);

		return result*rr;
	}

/* ported from http://code.google.com/p/irr-newtonraphson-calculator/ */
	bool is_valid_iter_bound(double estimated_return_rate)
	{
		return estimated_return_rate != -1 && (estimated_return_rate < std::numeric_limits<int>::max()) && (estimated_return_rate > std::numeric_limits<int>::min());
	}

	double irr_poly_sum(double estimated_return_rate, int cf_line, int count)
	{
		double sum_of_polynomial = 0;
		if (is_valid_iter_bound(estimated_return_rate))
		{
			for (int j = 0; j <= count ; j++)
			{
				double val = (pow((1 + estimated_return_rate), j));
				if (val != 0.0)
					sum_of_polynomial += cf.at(cf_line,j)/val;
				else
					break;
			}
		}
		return sum_of_polynomial;
	}

	double irr_derivative_sum(double estimated_return_rate,int cf_line, int count)
	{
		double sum_of_derivative = 0;
		if (is_valid_iter_bound(estimated_return_rate))
			for (int i = 1; i <= count ; i++)
			{
				sum_of_derivative += cf.at(cf_line,i)*(i)/pow((1 + estimated_return_rate), i+1);
			}
		return sum_of_derivative*-1;
	}

	double irr_scale_factor( int cf_unscaled, int count)
	{
		// scale to max value for better irr convergence
		if (count<1) return 1.0;
		int i=0;
		double max=fabs(cf.at(cf_unscaled,0));
		for (i=0;i<=count;i++) 
			if (fabs(cf.at(cf_unscaled,i))> max) max =fabs(cf.at(cf_unscaled,i));
		return (max>0 ? max:1);
	}

	bool is_valid_irr( int cf_line, int count, double residual, double tolerance, int number_of_iterations, int max_iterations, double calculated_irr, double scale_factor )
	{
		double npv_of_irr = npv(cf_line,count,calculated_irr)+cf.at(cf_line,0);
		double npv_of_irr_plus_delta = npv(cf_line,count,calculated_irr+0.001)+cf.at(cf_line,0);
		bool is_valid = ( (number_of_iterations<max_iterations) && (fabs(residual)<tolerance) && (npv_of_irr>npv_of_irr_plus_delta) && (fabs(npv_of_irr/scale_factor)<tolerance) );
				//if (!is_valid)
				//{
				//std::stringstream outm;
				//outm <<  "cf_line=" << cf_line << "count=" << count << "residual=" << residual << "number_of_iterations=" << number_of_iterations << "calculated_irr=" << calculated_irr
				//	<< "npv of irr=" << npv_of_irr << "npv of irr plus delta=" << npv_of_irr_plus_delta;
				//log( outm.str() );
				//}
		return is_valid;
	}

	double irr( int cf_line, int count, double initial_guess=-2, double tolerance=1e-6, int max_iterations=100 )
	{
		int number_of_iterations=0;
//		double calculated_irr = 0;
		double calculated_irr = std::numeric_limits<double>::quiet_NaN();
//		double calculated_irr = -999;


		if (count < 1) 
			return calculated_irr;

		// only possible for first value negative
		if ( (cf.at(cf_line,0) <= 0))
		{
			// initial guess from http://zainco.blogspot.com/2008/08/internal-rate-of-return-using-newton.html
			if ((initial_guess < -1) && (count > 1))// second order
			{
				if (cf.at(cf_line,0) !=0) 
				{
					double b = 2.0+ cf.at(cf_line,1)/cf.at(cf_line,0);
					double c = 1.0+cf.at(cf_line,1)/cf.at(cf_line,0)+cf.at(cf_line,2)/cf.at(cf_line,0);
					initial_guess = -0.5*b - 0.5*sqrt(b*b-4.0*c);
					if ((initial_guess <= 0) || (initial_guess >= 1)) initial_guess = -0.5*b + 0.5*sqrt(b*b-4.0*c);
				}
			}
			else if (initial_guess < 0) // first order
			{
				if (cf.at(cf_line,0) !=0) initial_guess = -(1.0 + cf.at(cf_line,1)/cf.at(cf_line,0));
			}

			double scale_factor = irr_scale_factor(cf_line,count);
			double residual=DBL_MAX;

			calculated_irr = irr_calc(cf_line,count,initial_guess,tolerance,max_iterations,scale_factor,number_of_iterations,residual);

			if (!is_valid_irr(cf_line,count,residual,tolerance,number_of_iterations,max_iterations,calculated_irr,scale_factor)) // try 0.1 as initial guess
			{
				initial_guess=0.1;
				number_of_iterations=0;
				residual=0;
				calculated_irr = irr_calc(cf_line,count,initial_guess,tolerance,max_iterations,scale_factor,number_of_iterations,residual);
			}

			if (!is_valid_irr(cf_line,count,residual,tolerance,number_of_iterations,max_iterations,calculated_irr,scale_factor)) // try -0.1 as initial guess
			{
				initial_guess=-0.1;
				number_of_iterations=0;
				residual=0;
				calculated_irr = irr_calc(cf_line,count,initial_guess,tolerance,max_iterations,scale_factor,number_of_iterations,residual);
			}
			if (!is_valid_irr(cf_line,count,residual,tolerance,number_of_iterations,max_iterations,calculated_irr,scale_factor)) // try 0 as initial guess
			{
				initial_guess=0;
				number_of_iterations=0;
				residual=0;
				calculated_irr = irr_calc(cf_line,count,initial_guess,tolerance,max_iterations,scale_factor,number_of_iterations,residual);
			}

			if (!is_valid_irr(cf_line,count,residual,tolerance,number_of_iterations,max_iterations,calculated_irr,scale_factor)) // try 0.1 as initial guess
			{
//				calculated_irr = 0.0; // did not converge
				calculated_irr = std::numeric_limits<double>::quiet_NaN(); // did not converge
//				double calculated_irr = -999;
			}

		}
		return calculated_irr;
	}


	double irr_calc( int cf_line, int count, double initial_guess, double tolerance, int max_iterations, double scale_factor, int &number_of_iterations, double &residual )
	{
//		double calculated_irr = 0;
		double calculated_irr = std::numeric_limits<double>::quiet_NaN();
//		double calculated_irr = -999;
		double deriv_sum = irr_derivative_sum(initial_guess, cf_line, count);
		if (deriv_sum != 0.0)
			calculated_irr = initial_guess - irr_poly_sum(initial_guess,cf_line,count)/deriv_sum;
		else
			return initial_guess;

		number_of_iterations++;


		residual = irr_poly_sum(calculated_irr,cf_line,count) / scale_factor;

		while (!(fabs(residual) <= tolerance) && (number_of_iterations < max_iterations))
		{
			deriv_sum = irr_derivative_sum(initial_guess,cf_line,count);
			if (deriv_sum != 0.0)
				calculated_irr = calculated_irr - irr_poly_sum(calculated_irr,cf_line,count)/deriv_sum;
			else
				break;

			number_of_iterations++;
			residual = irr_poly_sum(calculated_irr,cf_line,count) / scale_factor;
		}
		return calculated_irr;
	}


	double min(double a, double b)
	{ // handle NaN
		if ((a != a) || (b != b))
			return 0;
		else
			return (a < b) ? a : b;
	}

	double max(double a, double b)
	{ // handle NaN
		if ((a != a) || (b != b))
			return 0;
		else
			return (a > b) ? a : b;
	}

	double min_cashflow_value(int cf_line, int nyears)
	{
		// check for NaN
		bool is_nan = true;
		for (int i = 1; i <= nyears; i++)
			is_nan &= std::isnan(cf.at(cf_line, i));
		if (is_nan) return std::numeric_limits<double>::quiet_NaN();

		double min_value = DBL_MAX;
		for (int i = 1; i <= nyears; i++)
			if ((cf.at(cf_line, i)<min_value) && (cf.at(cf_line, i) != 0)) min_value = cf.at(cf_line, i);
		return min_value;
	}


};




DEFINE_MODULE_ENTRY( host_developer, "Host Developer Financial Model_", 1 );

