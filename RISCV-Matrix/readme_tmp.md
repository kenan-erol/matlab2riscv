|Function|Vectorizable?/Worth vectorizing?|Vectorized?|Default Mode|Notes|
|--|--|--|--|--|
|createM|No|*n/a*|*n/a*|*n/a*|
|duplicateM|No|*n/a*|*n/a*|*n/a*|
|full_duplicateM|Yes|Yes|Ref|*n/a*|
|identityM|No|*n/a*|*n/a*|*n/a*|
|zerosM|No|*n/a*|*n/a*|*n/a*|
|onesM|Yes|Yes|Ref|*n/a*|
|randomSeed|No|*n/a*|*n/a*|*n/a*|
|resetSeed|No|*n/a*|*n/a*|*n/a*|
|randM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random floats|
|randiM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random ints|
|randnM|Possibly|No|*n/a*|Possible if there's a way to initialize a vector of random floats|
|reindexM|Possibly|No|*n/a*|*n/a*|
|destroyM|No|*n/a*|*n/a*|*n/a*|
|writeM|Yes|Yes|Ref|LMUL settings need checking|
|indexM|No|*n/a*|*n/a*|*n/a*|
|printM|No|*n/a*|*n/a*|*n/a*|
|fprintM|No|*n/a*|*n/a*|*n/a*|
|getnDimM|No|*n/a*|*n/a*|*n/a*|
|getDimsM|No|*n/a*|*n/a*|*n/a*|
|getdataM|No|*n/a*|*n/a*|*n/a*|
|gettypeM|No|*n/a*|*n/a*|*n/a*|
|getsizeM|No|*n/a*|*n/a*|*n/a*|
|getreadonlyM|No|*n/a*|*n/a*|*n/a*|
|reshapeM |No|*n/a*|*n/a*|*n/a*|
|can_matrices_be_compared|No|*n/a*|*n/a*|*n/a*|
|almostEqual_ii|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_id|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_ic|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_di|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_dd|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_dc|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_ci|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_cd|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|almostEqual_cc|Yes|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li><li>Vectorization of equalM_internal_vec is a starting point.</li></ul>|
|i_to_i|Yes|Yes|Ref|LMUL settings need checking|
|i_to_d|Yes|Yes|Ref|LMUL settings need checking|
|i_to_c|Yes|Yes|Ref|LMUL settings need checking|
|d_to_i|Yes|Yes|Ref|LMUL settings need checking|
|d_to_d|Yes|Yes|Ref|LMUL settings need checking|
|d_to_c|Yes|Yes|Ref|LMUL settings need checking|
|c_to_i|Yes|Yes|Ref|LMUL settings need checking|
|c_to_d|Yes|Yes|Ref|LMUL settings need checking|
|c_to_c|Yes|Yes|Ref|LMUL settings need checking|
|int_comp_asc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|int_comp_desc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|double_comp_asc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|double_comp_desc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|complex_comp_asc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|complex_comp_desc|Maybe|No|*n/a*|<ul><li>Currently written to compare two individual elements</li><li>Could be vectorized if it took in vector arguments.</li><li>Honestly probably a good idea to do.</li><li>Would greatly simplify all the elementwise comparisons. </li></ul>|
|containsOverflow|Yes|No|*n/a*|*n/a*|
|long get_time|No|*n/a*|*n/a*|*n/a*|
|elapsed_time|No|*n/a*|*n/a*|*n/a*|
|long get_cycles|No|*n/a*|*n/a*|*n/a*|
|elapsed_cycles|No|*n/a*|*n/a*|*n/a*|
|transposeM|Yes|No|*n/a*|*n/a*|
|ctransposeM|Yes|No|*n/a*|*n/a*|
|determinantM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|invertM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|eigM|Maybe|No|*n/a*|Depends on vectorization of LAPACKE|
|trueM|Yes|Yes|Vec|Needs testing against Octave|
|andM|Yes|Yes|Ref|*n/a*|
|orM|Yes|Yes|Ref|*n/a*|
|notM|Yes|Yes|Ref|*n/a*|
|isEqualM|Yes|Yes|Ref|*n/a*|
|equalM|Yes|Yes|Ref|LMUL settings need checking|
|areEqualM|Maybe|No|*n/a*|*n/a*|
|gtM|Yes|Yes|Ref|LMUL settings need checking|
|geM|Yes|Yes|Ref|<ul><li>Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes</li><li>LMUL settings need checking</li></ul>|
|ltM|Yes|Yes|Ref|LMUL settings need checking|
|leM|Yes|Yes|Ref|<ul><li>Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes</li><li>LMUL settings need checking</li></ul>|
|neM|Yes|Yes|Ref|<ul><li>Needs to be updated so that the "or equal to" parts use almostEqual for doubles/complexes</li><li>LMUL settings need checking</li></ul>|
|pairwise_maxM|Yes|Yes|Ref|<ul><li>Reference version is slow because it calls several other functions to avoid different behaviors for COMPARISON_MODE. Reference version needs improvement</li><li>LMUL settings need checking</li></ul>|
|pairwise_minM|Yes|Yes|Ref|<ul><li>Reference version is slow because it calls several other functions to avoid different behaviors for COMPARISON_MODE. Reference version needs improvement</li><li>LMUL settings need checking</li></ul>|
|argM|Yes|Yes|Vec|Needs testing against Octave|
|absM|Yes|Yes|Vec|*n/a*|
|floorM|Yes|Yes|Vec|LMUL settings need checking|
|ceilM|Yes|Yes|Vec|LMUL settings need checking|
|roundM|Yes|Yes|Vec|LMUL settings need checking|
|scaleM|Yes|Yes|Vec|*n/a*|
|scalarpowerM|Yes|No|*n/a*|*n/a*|
|sinM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|sindM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|cosM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|cosdM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|tanM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|tandM|Yes|No|*n/a*|Depends on ability (or lack thereof) of doing trig functions in RVV|
|plusM|Yes|Yes|Vec|*n/a*|
|minusM|Yes|Yes|Vec|*n/a*|
|timesM|Yes|Yes|Vec|LMUL settings need checking|
|rdivideM|Yes|Yes|Vec|LMUL settings need checking|
|ldivideM|Yes|Yes|Vec|LMUL settings need checking|
|powerM|Yes|No|*n/a*|This function differs from Octave by some amount of rounding error.|
|mtimesM|Maybe|<ul><li>Naive version vectorized</li><li>BLAS version not vectorized</li></ul>|BLAS implementation|<ul><li>Depends on vectorization of BLAS</li><li>Vectorized implementation differs from BLAS/naive implementation by some rounding error</li></ul>|
|mrdivideM|Maybe|No|*n/a*|Depends on vectorization of BLAS (for mtimesM) & LAPACKE (invertM)|
|mldivideM|Maybe|No|*n/a*|Depends on vectorization of BLAS (for mtimesM) & LAPACKE (invertM)|
|mpowerM|Yes|No|*n/a*|Unable to raise non-diagonalizable matrices to non-integer powers|
|sort|Maybe|No|*n/a*|*n/a*|
|sortM|Maybe|No|*n/a*|*n/a*|
|mean|Yes|No|*n/a*|*n/a*|
|meanM|Yes|No|*n/a*|*n/a*|
|medianM|Maybe|No|*n/a*|*n/a*|
|minM|Maybe|No|*n/a*|*n/a*|
|maxM|Maybe|No|*n/a*|*n/a*|
|minV|Maybe|No|*n/a*|*n/a*|
|maxV|Maybe|No|*n/a*|*n/a*|
|varM|Maybe|No|*n/a*|*n/a*|
|popvarM|Maybe|No|*n/a*|*n/a*|
|stdM|Maybe|No|*n/a*|*n/a*|
|popstdM|Maybe|No|*n/a*|*n/a*|
|int_quantile|Maybe|No|*n/a*|*n/a*|
|double_quantile|Maybe|No|*n/a*|*n/a*|
|complex_quantile|Maybe|No|*n/a*|*n/a*|
|quantileM_vect|Maybe|No|*n/a*|*n/a*|
|quantileM|Maybe|No|*n/a*|*n/a*|
|betapdfM|Maybe|No|*n/a*|*n/a*|
|exppdfM|Maybe|No|*n/a*|*n/a*|
|chi2pdfM|Maybe|No|*n/a*|*n/a*|
|gampdfM|Maybe|No|*n/a*|*n/a*|
|lognpdfM|Maybe|No|*n/a*|*n/a*|
|normpdfM|Maybe|No|*n/a*|*n/a*|
|unidpdfM|Maybe|No|*n/a*|*n/a*|
|gammadiv|Maybe|No|*n/a*|<ul><li>Currently written to operate on a single element</li><li>Maybe could be vectorized if it took in vector arguments.</li></ul>|
|tcdf |Maybe|No|*n/a*|<ul><li>Currently written to operate on a single element</li><li>Maybe could be vectorized if it took in vector arguments.</li></ul>|
|ttestM|Maybe|No|*n/a*|*n/a*|
|ttestM_xy|Maybe|No|*n/a*|*n/a*|
|normcdf |Maybe|No|*n/a*|<ul><li>Currently written to operate on a single element</li><li>Maybe could be vectorized if it took in vector arguments.</li></ul>|
|ztestM|Maybe|No|*n/a*|*n/a*|
|chi2cdf|Maybe|No|*n/a*|<ul><li>Currently written to operate on a single element</li><li>Maybe could be vectorized if it took in vector arguments.</li></ul>|
|vartestM|Maybe|No|*n/a*|*n/a*|
|normfitM|Maybe|No|*n/a*|*n/a*|
|unifitM|Maybe|No|*n/a*|*n/a*|
|hamming|Maybe|No|*n/a*|*n/a*|
|periodichamming|Maybe|No|*n/a*|*n/a*|
|hanning|Maybe|No|*n/a*|*n/a*|
|periodichanning|Maybe|No|*n/a*|*n/a*|
|dftM|Maybe|No|*n/a*|Needs testing against Octave|
|idftM|Maybe|No|*n/a*|Needs testing against Octave|
|fftM|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|ifftM|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|fftV|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|ifftV|Maybe|No|*n/a*|Depends on vectorization of fftw library|
|stftV|Maybe|No|*n/a*|Needs testing against Octave|
|filterM|Maybe|No|*n/a*|Needs testing against Octave|
|xcorrM|Maybe|No|*n/a*|*n/a*|