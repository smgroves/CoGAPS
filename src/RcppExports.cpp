// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// cogaps
Rcpp::List cogaps(Rcpp::NumericMatrix DMatrix, Rcpp::NumericMatrix SMatrix, unsigned nFactor, double alphaA, double alphaP, unsigned nEquil, unsigned nEquilCool, unsigned nSample, double maxGibbsMassA, double maxGibbsMassP, Rcpp::NumericMatrix fixedPatterns, char whichMatrixFixed, int seed, bool messages, bool singleCellRNASeq, unsigned numOutputs, unsigned numSnapshots);
RcppExport SEXP _CoGAPS_cogaps(SEXP DMatrixSEXP, SEXP SMatrixSEXP, SEXP nFactorSEXP, SEXP alphaASEXP, SEXP alphaPSEXP, SEXP nEquilSEXP, SEXP nEquilCoolSEXP, SEXP nSampleSEXP, SEXP maxGibbsMassASEXP, SEXP maxGibbsMassPSEXP, SEXP fixedPatternsSEXP, SEXP whichMatrixFixedSEXP, SEXP seedSEXP, SEXP messagesSEXP, SEXP singleCellRNASeqSEXP, SEXP numOutputsSEXP, SEXP numSnapshotsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type DMatrix(DMatrixSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type SMatrix(SMatrixSEXP);
    Rcpp::traits::input_parameter< unsigned >::type nFactor(nFactorSEXP);
    Rcpp::traits::input_parameter< double >::type alphaA(alphaASEXP);
    Rcpp::traits::input_parameter< double >::type alphaP(alphaPSEXP);
    Rcpp::traits::input_parameter< unsigned >::type nEquil(nEquilSEXP);
    Rcpp::traits::input_parameter< unsigned >::type nEquilCool(nEquilCoolSEXP);
    Rcpp::traits::input_parameter< unsigned >::type nSample(nSampleSEXP);
    Rcpp::traits::input_parameter< double >::type maxGibbsMassA(maxGibbsMassASEXP);
    Rcpp::traits::input_parameter< double >::type maxGibbsMassP(maxGibbsMassPSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type fixedPatterns(fixedPatternsSEXP);
    Rcpp::traits::input_parameter< char >::type whichMatrixFixed(whichMatrixFixedSEXP);
    Rcpp::traits::input_parameter< int >::type seed(seedSEXP);
    Rcpp::traits::input_parameter< bool >::type messages(messagesSEXP);
    Rcpp::traits::input_parameter< bool >::type singleCellRNASeq(singleCellRNASeqSEXP);
    Rcpp::traits::input_parameter< unsigned >::type numOutputs(numOutputsSEXP);
    Rcpp::traits::input_parameter< unsigned >::type numSnapshots(numSnapshotsSEXP);
    rcpp_result_gen = Rcpp::wrap(cogaps(DMatrix, SMatrix, nFactor, alphaA, alphaP, nEquil, nEquilCool, nSample, maxGibbsMassA, maxGibbsMassP, fixedPatterns, whichMatrixFixed, seed, messages, singleCellRNASeq, numOutputs, numSnapshots));
    return rcpp_result_gen;
END_RCPP
}
// run_catch_unit_tests
int run_catch_unit_tests();
RcppExport SEXP _CoGAPS_run_catch_unit_tests() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(run_catch_unit_tests());
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_CoGAPS_cogaps", (DL_FUNC) &_CoGAPS_cogaps, 17},
    {"_CoGAPS_run_catch_unit_tests", (DL_FUNC) &_CoGAPS_run_catch_unit_tests, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_CoGAPS(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
