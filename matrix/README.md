# Matrix Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

The matrix library provides a comprehensive set of functions for creating, manipulating, and analyzing matrices in C. It includes basic operations like addition, subtraction, and multiplication, as well as advanced features such as decompositions, transformations, and special matrix generation (e.g., identity, Hilbert, and Pascal matrices). The library also supports various matrix checks (e.g., square, symmetric, orthogonal) and utility functions for row and column operations. This versatile tool is designed to facilitate complex mathematical computations with matrices, making it ideal for scientific and engineering applications.

## Compilation

To compile the matrix library along with your main program, use the following GCC command:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./matrix/matrix.c 
```

If you need other libraries, you can add them by including their `.c` files.

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Matrix library in your project, include the `matrix.h` header file in your source code:

```c
#include "matrix/matrix.h"
```

## Function Descriptions

---

### `Matrix* matrix_create(size_t rows, size_t cols)`

**Purpose**:  
Allocates a new zero-initialised matrix with the given dimensions.

**Parameters**:  
- `rows`: Number of rows.  
- `cols`: Number of columns.

**Return Value**:  
Pointer to the newly created `Matrix`, or `NULL` on allocation failure or if `rows`/`cols` is 0.

**Usage Case**:  
First step for any matrix computation. All elements start at 0.0; use `matrix_set` or `matrix_fill` to populate them.

---

### `void matrix_deallocate(Matrix* matrix)`

**Purpose**:  
Frees all memory owned by the matrix (the data array and the struct itself). Safe to call with `NULL`.

**Parameters**:  
- `matrix`: Matrix to destroy. May be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Always call when a matrix is no longer needed to prevent memory leaks.

---

### `Matrix* matrix_copy(const Matrix* matrix)`

**Purpose**:  
Creates a deep, independent copy of `matrix` with the same dimensions and element values.

**Parameters**:  
- `matrix`: Source matrix to copy. Must not be `NULL`.

**Return Value**:  
Newly allocated `Matrix*` the caller must free with `matrix_deallocate`, or `NULL` on failure.

**Usage Case**:  
Snapshot a matrix before a destructive operation (e.g., in-place factorisation), or pass a copy to another subsystem without transferring ownership.

---

### `Matrix* matrix_from_array(const double* data, size_t rows, size_t cols)`

**Purpose**:  
Creates a matrix and fills it row-by-row from a flat 1-D array of `rows × cols` doubles.

**Parameters**:  
- `data`: Pointer to the source data (row-major order). Must not be `NULL`.  
- `rows`: Number of rows.  
- `cols`: Number of columns.

**Return Value**:  
Pointer to the newly created matrix, or `NULL` on failure.

**Usage Case**:  
Initialise a matrix from a C array literal without writing individual `matrix_set` calls.

---

### `Matrix* matrix_random(size_t row, size_t col, size_t start, size_t end)`

**Purpose**:  
Fills a newly created `row × col` matrix with uniformly distributed random integers in the range `[start, end)` (the full `size_t` range is supported). It uses its own generator, so it does not reseed or disturb the program's `rand()` sequence; it is not suitable for cryptographic use (see the `secrets` module).

**Parameters**:  
- `row`: Number of rows.  
- `col`: Number of columns.  
- `start`: Minimum value (inclusive).  
- `end`: Maximum value (exclusive).

**Return Value**:  
Pointer to the randomly filled matrix, or `NULL` on allocation failure.

**Usage Case**:  
Generate random test data or initialise Monte-Carlo simulations.

---

### `void matrix_print(Matrix* matrix)`

**Purpose**:  
Prints the matrix to `stdout` in a human-readable row-by-row format with 5 decimal places per element.

**Parameters**:  
- `matrix`: Matrix to print. May be `NULL` (prints nothing).

**Return Value**:  
None.

**Usage Case**:  
Quick console inspection during development or debugging.

---

### `Matrix* matrix_create_identity(size_t n)`

**Purpose**:  
Creates an `n × n` identity matrix (diagonal elements = 1, all others = 0).

**Parameters**:  
- `n`: Size of the square matrix.

**Return Value**:  
Pointer to the identity matrix, or `NULL` on failure.

**Usage Case**:  
Starting point for matrix inversion algorithms or as a neutral element in matrix multiplication.

---

### `Matrix* matrix_ones(size_t rows, size_t cols)`

**Purpose**:  
Creates a new `rows × cols` matrix in which every element is `1.0` — the C analogue of NumPy's `numpy.ones(shape)`. Since this library stores a 2-D matrix of `double`, NumPy's `dtype` / `order` / `device` / `like` parameters do not apply: the element type is always `double` and storage is always row-major.

**Parameters**:  
- `rows`: Number of rows (must be greater than 0).
- `cols`: Number of columns (must be greater than 0).

**Return Value**:  
Pointer to the new matrix of ones, or `NULL` if either dimension is 0 or memory allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
A bias/all-ones initializer, a multiplicative identity for the Hadamard product, or the direct C equivalent of `np.ones((m, n))`. A 1-D NumPy shape like `np.ones(5)` maps to a row vector `matrix_ones(1, 5)` (or a column vector `matrix_ones(5, 1)`).

---

### `Matrix* matrix_zeros(size_t rows, size_t cols)`

**Purpose**:  
Creates a new `rows × cols` matrix in which every element is `0.0` — the C analogue of NumPy's `numpy.zeros(shape)`. As with `matrix_ones`, NumPy's `dtype` / `order` / `device` / `like` parameters do not apply: the element type is always `double` and storage is always row-major.

**Parameters**:  
- `rows`: Number of rows (must be greater than 0).
- `cols`: Number of columns (must be greater than 0).

**Return Value**:  
Pointer to the new matrix of zeros, or `NULL` if either dimension is 0 or memory allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
A zero-initialized accumulator (e.g. building up a sum of matrices), an additive identity, or the direct C equivalent of `np.zeros((m, n))`. A 1-D NumPy shape like `np.zeros(5)` maps to a row vector `matrix_zeros(1, 5)` (or a column vector `matrix_zeros(5, 1)`).

---

### `Matrix* matrix_eye(size_t rows, size_t cols, int k)`

**Purpose**:  
Creates a `rows × cols` matrix with `1.0` on the `k`-th diagonal and `0.0` elsewhere — the C analogue of NumPy's `numpy.eye(N, M, k)`. Unlike `matrix_create_identity` (square, main diagonal only), this may be **rectangular** and the diagonal may be **offset**: `k = 0` is the main diagonal, `k > 0` selects a super-diagonal (above the main), and `k < 0` a sub-diagonal (below).

**Parameters**:  
- `rows`: Number of rows (must be greater than 0).
- `cols`: Number of columns (must be greater than 0).
- `k`: Diagonal offset (`0` = main, `>0` = above, `<0` = below).

**Return Value**:  
Pointer to the new matrix, or `NULL` if either dimension is 0 or allocation fails. An out-of-range `k` simply yields an all-zero matrix (matching NumPy). Free it with `matrix_deallocate`.

**Usage Case**:  
A rectangular or shifted identity — e.g. a shift operator (`matrix_eye(n, n, 1)`), or an identity block embedded in a larger matrix. `matrix_eye(n, n, 0)` is equivalent to `matrix_create_identity(n)`.

---

### `Matrix* matrix_diag(const Matrix* vector)`

**Purpose**:  
Builds a square diagonal matrix from a vector — the constructor form of NumPy's `numpy.diag(v)`. Given a `1 × n` row or `n × 1` column vector, returns an `n × n` matrix with the vector's values on the main diagonal and `0.0` elsewhere.

**Parameters**:  
- `vector`: The source vector (`1 × n` or `n × 1`).

**Return Value**:  
A new `n × n` diagonal matrix, or `NULL` if `vector` is `NULL`, is not a row/column vector, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Turning a vector of scale factors into a scaling matrix, or building `diag(eigenvalues)`. This is only the constructor direction; to *extract* a diagonal use `matrix_get_main_diagonal_as_column`.

---

### `bool matrix_meshgrid(const Matrix* x, const Matrix* y, Matrix** X, Matrix** Y)`

**Purpose**:  
Builds coordinate grids from two 1-D vectors — the C analogue of NumPy's `numpy.meshgrid(x, y)` (default `indexing='xy'`). Given a length-`nx` vector `x` and a length-`ny` vector `y`, produces two `ny × nx` matrices: `X` has `x` repeated down every row, and `Y` has `y` repeated across every column.

**Parameters**:  
- `x`: The x-coordinate vector.
- `y`: The y-coordinate vector.
- `X`: Out-parameter receiving the `ny × nx` matrix of x-coordinates.
- `Y`: Out-parameter receiving the `ny × nx` matrix of y-coordinates.

**Return Value**:  
`true` on success; `false` if any argument is `NULL`, a vector is empty, or allocation fails. On success free both `*X` and `*Y` with `matrix_deallocate`.

**Usage Case**:  
Evaluating a function `f(x, y)` over a grid, generating coordinates for plotting or image generation.

---

### `Matrix* matrix_hilbert(size_t n)`

**Purpose**:  
Generates an `n × n` Hilbert matrix where `H[i][j] = 1 / (i + j + 1)`.

**Parameters**:  
- `n`: Size of the matrix.

**Return Value**:  
Pointer to the Hilbert matrix, or `NULL` on failure.

**Usage Case**:  
Classic ill-conditioned test matrix for numerical analysis benchmarks.

---

### `Matrix* matrix_inverse_hilbert(size_t n)`

**Purpose**:  
Computes the exact integer inverse of the n×n Hilbert matrix using the closed-form formula.

**Parameters**:  
- `n`: Size of the matrix.

**Return Value**:  
Pointer to the inverse Hilbert matrix, or `NULL` on failure.

**Usage Case**:  
Companion to `matrix_hilbert` for numerical conditioning experiments.

---

### `Matrix* matrix_pascal(size_t n)`

**Purpose**:  
Generates an `n × n` Pascal matrix where each element is a binomial coefficient.

**Parameters**:  
- `n`: Size of the matrix.

**Return Value**:  
Pointer to the Pascal matrix, or `NULL` on failure.

**Usage Case**:  
Combinatorics, polynomial arithmetic, and numerical analysis.

---

### `Matrix* matrix_helmert(size_t n, bool full)`

**Purpose**:  
Generates a Helmert matrix — an orthogonal matrix used in multivariate statistics — exactly as `scipy.linalg.helmert`.

**Parameters**:  
- `n`: Size of the matrix.  
- `full`: If `true`, returns the full `n × n` Helmert matrix; otherwise the reduced `(n-1) × n` form, which drops the first (constant `1/√n`) row.

**Return Value**:  
Pointer to the Helmert matrix, or `NULL` if `n` is 0 (or 1 for the reduced form) or allocation fails.

**Usage Case**:  
Contrast coding in ANOVA, principal component preprocessing.

---

### `Matrix* matrix_walsh(size_t n)`

**Purpose**:  
Generates an `n × n` Walsh–Hadamard matrix (entries ±1, `n` must be a power of 2).

**Parameters**:  
- `n`: Size of the matrix (must be a power of 2).

**Return Value**:  
Pointer to the Walsh matrix, or `NULL` if `n` is not a power of 2 or allocation fails.

**Usage Case**:  
Signal processing, error-correcting codes, and fast Walsh–Hadamard transforms.

---

### `Matrix* matrix_vandermonde(const Matrix* matrix, size_t n)`

**Purpose**:  
Generates a Vandermonde matrix where each row is a geometric progression of the corresponding input element: `V[i][j] = matrix[i]^j`.

**Parameters**:  
- `matrix`: 1×m input row matrix containing the base values.  
- `n`: Size of the result: an `n × n` matrix built from the first `n` input elements.

**Return Value**:  
Pointer to the Vandermonde matrix, or `NULL` if `n` is 0 or larger than the number of input elements, or allocation fails.

**Usage Case**:  
Polynomial interpolation, least-squares curve fitting.

---

### `Matrix* matrix_companion(const Matrix* coefficients, size_t n)`

**Purpose**:  
Constructs the companion matrix of a polynomial given its coefficients, like `scipy.linalg.companion`: the first row is `-a[1..n-1] / a[0]` and the sub-diagonal is all ones, so the eigenvalues of the result are the roots of the polynomial.

**Parameters**:  
- `coefficients`: row matrix of polynomial coefficients (highest degree first).  
- `n`: Number of coefficients to use (= polynomial degree + 1); at least 2 and at most the number of elements in `coefficients`.

**Return Value**:  
Pointer to the `(n-1) × (n-1)` companion matrix, or `NULL` if `n` is out of range, the leading coefficient is 0, or allocation fails.

**Usage Case**:  
Finding polynomial roots by computing matrix eigenvalues.

---

### `Matrix* matrix_circulant(const Matrix* firstRow)`

**Purpose**:  
Generates a circulant matrix where each row is a cyclic right-shift of the previous row.

**Parameters**:  
- `firstRow`: 1×n row matrix defining the first row.

**Return Value**:  
Pointer to the `n × n` circulant matrix, or `NULL` on failure.

**Usage Case**:  
Discrete convolution, cyclic shift operators, and fast Fourier transform-based algorithms.

---

### `Matrix* matrix_hankel(const Matrix* firstRow, const Matrix* lastCol)`

**Purpose**:  
Generates a Hankel matrix where element `[i][j]` equals `firstRow[0][i+j]` (anti-diagonal-constant).

**Parameters**:  
- `firstRow`: 1×n row matrix defining the first row.  
- `lastCol`: m×1 column matrix defining the last column.

**Return Value**:  
Pointer to the Hankel matrix, or `NULL` on failure.

**Usage Case**:  
System identification, signal processing, moment problems.

---

### `Matrix* matrix_toeplitz(const Matrix* firstRow, const Matrix* firstCol)`

**Purpose**:  
Generates a Toeplitz matrix where each descending diagonal is constant, defined by the first row and first column.

**Parameters**:  
- `firstRow`: 1×n row matrix.  
- `firstCol`: m×1 column matrix. The `[0][0]` element must be the same in both.

**Return Value**:  
Pointer to the `m × n` Toeplitz matrix, or `NULL` on failure.

**Usage Case**:  
Convolution operators, time-series modelling, and linear prediction.

---

### `Matrix* matrix_leslie(Matrix* f, size_t f_size, Matrix* s, size_t s_size)`

**Purpose**:  
Generates a Leslie matrix used in discrete population dynamics. The first row holds fecundity coefficients; the sub-diagonal holds survival coefficients.

**Parameters**:  
- `f`: 1×f_size fecundity matrix.  
- `f_size`: Number of age classes (determines matrix size).  
- `s`: 1×s_size survival matrix (`s_size` must equal `f_size - 1`).  
- `s_size`: Number of survival coefficients.

**Return Value**:  
Pointer to the `f_size × f_size` Leslie matrix, or `NULL` on failure.

**Usage Case**:  
Age-structured population models in ecology and demography.

---

### `Matrix* matrix_fiedler(const Matrix* matrix)`

**Purpose**:  
Creates a Fiedler matrix where `F[i][j] = |matrix[0][i] - matrix[0][j]|`.

**Parameters**:  
- `matrix`: 1×n row matrix whose elements define the pairwise differences.

**Return Value**:  
Pointer to the `n × n` Fiedler matrix, or `NULL` on failure.

**Usage Case**:  
Graph theory (algebraic connectivity), spectral analysis.

---

### `Matrix* matrix_block_diag(size_t count, ...)`

**Purpose**:  
Creates a block-diagonal matrix by placing `count` matrices along the diagonal of a larger zero matrix.

**Parameters**:  
- `count`: Number of block matrices to include.  
- `...`: `count` pointers of type `Matrix*`, each becoming one diagonal block.

**Return Value**:  
Pointer to the assembled block-diagonal matrix, or `NULL` on failure.

**Usage Case**:  
Building structured system matrices in control theory, block-structured linear systems.

---

### `double matrix_get(const Matrix* matrix, size_t row, size_t col)`

**Purpose**:  
Returns the scalar value at position `(row, col)` in the matrix.

**Parameters**:  
- `matrix`: Source matrix.  
- `row`: Zero-based row index.  
- `col`: Zero-based column index.

**Return Value**:  
The element value, or `0.0` on out-of-bounds access or `NULL` input.

**Usage Case**:  
Read individual elements for inspection, output formatting, or conditional logic.

---

### `bool matrix_set(Matrix* matrix, size_t row, size_t col, double value)`

**Purpose**:  
Assigns `value` to position `(row, col)` in the matrix.

**Parameters**:  
- `matrix`: Destination matrix.  
- `row`: Zero-based row index.  
- `col`: Zero-based column index.  
- `value`: The scalar to assign.

**Return Value**:  
`true` on success; `false` if indices are out of range or `matrix` is `NULL`.

**Usage Case**:  
Populate individual elements after creating a matrix with `matrix_create`.

---

### `bool matrix_fill(Matrix* matrix, double value)`

**Purpose**:  
Sets every element of the matrix to the same scalar `value`.

**Parameters**:  
- `matrix`: Matrix to fill.  
- `value`: Value to assign to all elements.

**Return Value**:  
`true` on success; `false` on `NULL` input.

**Usage Case**:  
Quickly initialise a matrix to zero, one, or any constant before further computation.

---

### `double* matrix_to_array(const Matrix* matrix)`

**Purpose**:  
Copies the matrix data into a newly allocated flat 1-D array in row-major order.

**Parameters**:  
- `matrix`: Source matrix.

**Return Value**:  
Heap-allocated `double[]` of `rows × cols` elements the caller must `free()`, or `NULL` on failure.

**Usage Case**:  
Pass matrix data to a C library function that expects a raw array (e.g., BLAS, FFTW).

---

### `size_t matrix_size(const Matrix* matrix)`

**Purpose**:  
Returns the total number of elements in the matrix (`rows × cols`).

**Parameters**:  
- `matrix`: Matrix to query. May be `NULL` (returns `0`).

**Return Value**:  
Element count, or `0` on `NULL` input.

**Usage Case**:  
Loop bound when iterating over the raw data pointer.

---

### `Matrix* matrix_get_row(const Matrix* matrix, size_t row)`

**Purpose**:  
Extracts the specified row as a new 1×cols matrix.

**Parameters**:  
- `matrix`: Source matrix.  
- `row`: Zero-based row index.

**Return Value**:  
Newly allocated 1×cols `Matrix*`, or `NULL` on out-of-bounds or allocation failure.

**Usage Case**:  
Isolate a row for dot-product computations or sub-problem decomposition.

---

### `Matrix* matrix_get_col(const Matrix* matrix, size_t col)`

**Purpose**:  
Extracts the specified column as a new rows×1 matrix.

**Parameters**:  
- `matrix`: Source matrix.  
- `col`: Zero-based column index.

**Return Value**:  
Newly allocated rows×1 `Matrix*`, or `NULL` on out-of-bounds or allocation failure.

**Usage Case**:  
Isolate a column vector for Gram–Schmidt orthogonalisation or regression.

---

### `void matrix_swap_rows(Matrix* mat, size_t row1, size_t row2)`

**Purpose**:  
Swaps the contents of two rows in place.

**Parameters**:  
- `mat`: Matrix to modify.  
- `row1`, `row2`: Zero-based indices of the rows to swap.

**Return Value**:  
None.

**Usage Case**:  
Partial pivoting in Gaussian elimination, row permutation in LU decomposition.

---

### `void matrix_swap_cols(Matrix* mat, size_t col1, size_t col2)`

**Purpose**:  
Swaps the contents of two columns in place.

**Parameters**:  
- `mat`: Matrix to modify.  
- `col1`, `col2`: Zero-based indices of the columns to swap.

**Return Value**:  
None.

**Usage Case**:  
Column pivoting in QR decomposition, reordering feature columns.

---

### `void matrix_row_divide(Matrix* matrix, size_t row, double scalar)`

**Purpose**:  
Divides every element of the specified row by `scalar`.

**Parameters**:  
- `matrix`: Matrix to modify.  
- `row`: Zero-based row index.  
- `scalar`: Divisor (must not be zero).

**Return Value**:  
None.

**Usage Case**:  
Row normalisation step in Gauss-Jordan elimination.

---

### `void matrix_row_subtract(Matrix* matrix, size_t targetRow, size_t subtractRow, double scalar)`

**Purpose**:  
Subtracts `scalar × subtractRow` from `targetRow` element-wise (`targetRow -= scalar × subtractRow`).

**Parameters**:  
- `matrix`: Matrix to modify.  
- `targetRow`: Row to be updated.  
- `subtractRow`: Row to scale and subtract.  
- `scalar`: Scale factor applied to `subtractRow`.

**Return Value**:  
None.

**Usage Case**:  
Row reduction step in Gaussian elimination for solving linear systems.

---

### `bool matrix_row_addition(Matrix* matrix, size_t targetRow, size_t sourceRow, double scale)`

**Purpose**:  
Adds `scale × sourceRow` to `targetRow` element-wise (`targetRow += scale × sourceRow`).

**Parameters**:  
- `matrix`: Matrix to modify.  
- `targetRow`: Row to be updated.  
- `sourceRow`: Row to scale and add.  
- `scale`: Scale factor applied to `sourceRow`.

**Return Value**:  
`true` on success; `false` on `NULL` input or out-of-range indices.

**Usage Case**:  
Elementary row operations in Gauss-Jordan elimination, linear combination of rows.

---

### `bool matrix_col_addition(Matrix* matrix, size_t targetCol, size_t sourceCol, double scale)`

**Purpose**:  
Adds `scale × sourceCol` to `targetCol` element-wise (`targetCol += scale × sourceCol`).

**Parameters**:  
- `matrix`: Matrix to modify.  
- `targetCol`: Column to be updated.  
- `sourceCol`: Column to scale and add.  
- `scale`: Scale factor applied to `sourceCol`.

**Return Value**:  
`true` on success; `false` on `NULL` input or out-of-range indices.

**Usage Case**:  
Elementary column operations, column normalisation.

---

### `bool matrix_apply_to_row(Matrix* matrix, size_t row, MatrixFunc func)`

**Purpose**:  
Applies the scalar function `func` to every element of the specified row in place.

**Parameters**:  
- `matrix`: Matrix to modify.  
- `row`: Zero-based row index.  
- `func`: A `double (*)(double)` callback.

**Return Value**:  
`true` on success; `false` on `NULL` input or out-of-range index.

**Usage Case**:  
Apply activation functions (ReLU, sigmoid), element-wise transforms on a single row.

---

### `bool matrix_apply_to_col(Matrix* matrix, size_t col, MatrixFunc func)`

**Purpose**:  
Applies the scalar function `func` to every element of the specified column in place.

**Parameters**:  
- `matrix`: Matrix to modify.  
- `col`: Zero-based column index.  
- `func`: A `double (*)(double)` callback.

**Return Value**:  
`true` on success; `false` on `NULL` input or out-of-range index.

**Usage Case**:  
Column-wise normalisation, element-wise transforms on a single column.

---

### `Matrix* matrix_add(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns a new matrix equal to the element-wise sum `matrix1 + matrix2`. Both matrices must have identical dimensions.

**Parameters**:  
- `matrix1`: First operand.  
- `matrix2`: Second operand.

**Return Value**:  
Newly allocated sum matrix, or `NULL` on dimension mismatch or allocation failure.

**Usage Case**:  
Superimpose two matrices, accumulate gradient updates in neural network training.

---

### `Matrix* matrix_subtract(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns a new matrix equal to the element-wise difference `matrix1 - matrix2`.

**Parameters**:  
- `matrix1`: Minuend.  
- `matrix2`: Subtrahend.

**Return Value**:  
Newly allocated difference matrix, or `NULL` on dimension mismatch or allocation failure.

**Usage Case**:  
Compute residuals, centre data, or implement gradient descent update steps.

---

### `Matrix* matrix_multiply(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Computes the standard matrix product `matrix1 × matrix2`. `matrix1.cols` must equal `matrix2.rows`.

**Parameters**:  
- `matrix1`: Left operand (m × k).  
- `matrix2`: Right operand (k × n).

**Return Value**:  
Newly allocated m × n product matrix, or `NULL` on size mismatch or allocation failure.

**Usage Case**:  
Core operation in linear algebra, neural network forward passes, coordinate transforms.

---

### `Matrix* matrix_kronecker_product(const Matrix* A, const Matrix* B)`

**Purpose**:  
Computes the Kronecker (tensor) product of two matrices, yielding a matrix of size `(A.rows × B.rows) × (A.cols × B.cols)`.

**Parameters**:  
- `A`: Left matrix.  
- `B`: Right matrix.

**Return Value**:  
Newly allocated Kronecker product matrix, or `NULL` on failure.

**Usage Case**:  
Quantum computing simulation, composite system state representation, block matrix construction.

---

### `Matrix* matrix_hadamard_product(const Matrix* A, const Matrix* B)`

**Purpose**:  
Computes the element-wise (Hadamard) product: `C[i][j] = A[i][j] * B[i][j]`. Both matrices must have identical dimensions. This is distinct from `matrix_multiply` (the dot-product matrix product) and `matrix_kronecker_product`.

**Parameters**:  
- `A`: First matrix.  
- `B`: Second matrix (same dimensions as `A`).

**Return Value**:  
Newly allocated matrix holding the element-wise product, or `NULL` if either input is `NULL`, the dimensions differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Neural-network gating/masking, signal processing, applying a per-element weight map.

---

### `Matrix* matrix_power(const Matrix* matrix, int power)`

**Purpose**:  
Raises a square matrix to an integer power. Handles zero power (returns identity) and negative powers (inverts first).

**Parameters**:  
- `matrix`: Square matrix to exponentiate.  
- `power`: Integer exponent (may be negative).

**Return Value**:  
Newly allocated result matrix, or `NULL` on failure.

**Usage Case**:  
Markov chain transition probabilities, discrete-time dynamical systems.

---

### `Matrix* matrix_map(const Matrix* matrix, MatrixFunc func)`

**Purpose**:  
Returns a new matrix where every element `e` is replaced by `func(e)`.

**Parameters**:  
- `matrix`: Source matrix.  
- `func`: A `double (*)(double)` callback applied element-wise.

**Return Value**:  
Newly allocated transformed matrix, or `NULL` on `NULL` input or allocation failure.

**Usage Case**:  
Apply `sin`, `exp`, `fabs`, or any custom activation function to every matrix element without modifying the original.

---

### `bool matrix_scalar_multiply(Matrix* matrix, double scalar)`

**Purpose**:  
Multiplies every element of `matrix` by `scalar` in place.

**Parameters**:  
- `matrix`: Matrix to scale.  
- `scalar`: Multiplier.

**Return Value**:  
`true` on success; `false` on `NULL` input.

**Usage Case**:  
Scale a matrix by a constant (e.g., normalise by dividing by the norm: `matrix_scalar_multiply(m, 1.0/norm)`).

---

### `Matrix* matrix_abs(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise absolute value of `matrix` — the C analogue of NumPy's `numpy.abs` / `numpy.absolute`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `|x|`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Turn a matrix of signed residuals or errors into magnitudes before summing or comparing.

---

### `Matrix* matrix_negate(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise negation of `matrix` (`-x`) — the C analogue of NumPy's `numpy.negative`. Equivalent to scaling by `-1`, but returns a new matrix instead of mutating in place like `matrix_scalar_multiply`.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `-x`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Flip the sign of a matrix to move a term to the other side of an equation, or to build `A - B` as `A + negate(B)`.

---

### `Matrix* matrix_scalar_add(const Matrix* matrix, double scalar)`

**Purpose**:  
Returns a new matrix with `scalar` added to every element — the C analogue of NumPy's `numpy.add(M, c)` (i.e. `M + c`). This is the additive counterpart of `matrix_scalar_multiply`; unlike that in-place function, it returns a new matrix.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `scalar`: Value added to every element (use a negative value to subtract a constant).

**Return Value**:  
A new matrix where every element is `x + scalar`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Shift a matrix by a constant bias, e.g. add `1.0` to every probability or re-centre data.

---

### `Matrix* matrix_divide(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns a new matrix with the element-wise (Hadamard) division `matrix1 / matrix2` — the C analogue of NumPy's `numpy.divide(A, B)`. The two matrices must have identical dimensions (no broadcasting); neither input is modified.

**Parameters**:  
- `matrix1`: Numerator matrix (not modified).
- `matrix2`: Denominator matrix, same dimensions as `matrix1` (not modified).

**Return Value**:  
A new matrix where every element is `a / b`, or `NULL` if either matrix is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Element-wise ratios — e.g. normalising one matrix by another, or computing per-element rates. Division by zero is not an error: as in NumPy's default mode it follows IEEE-754 and yields `+inf`, `-inf`, or `nan` (for `0.0 / 0.0`).

---

### `Matrix* matrix_pow(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise power of two equally-sized matrices — the C analogue of NumPy's `numpy.power(A, B)` (`A ** B`): each element is `a ^ b`. **Distinct from `matrix_power`**, which raises a square matrix to an integer power. Neither input is modified.

**Parameters**:  
- `matrix1`: Base matrix (not modified).
- `matrix2`: Exponent matrix, same dimensions as `matrix1` (not modified).

**Return Value**:  
A new matrix where each element is `a ^ b`, or `NULL` if either matrix is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-element exponentiation, e.g. applying different powers across a grid. Like C's `pow`, a negative base with a non-integer exponent yields `nan`.

---

### `Matrix* matrix_mod(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise modulo of two equally-sized matrices — the C analogue of NumPy's `numpy.mod` (equivalently `numpy.remainder`). Computed as `a − b·floor(a/b)`, so the result takes the sign of the **divisor** (Python semantics), **unlike C's `fmod`** which takes the sign of the dividend. Neither input is modified.

**Parameters**:  
- `matrix1`: Dividend matrix (not modified).
- `matrix2`: Divisor matrix, same dimensions as `matrix1` (not modified).

**Return Value**:  
A new matrix of element-wise remainders, or `NULL` if either matrix is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Wrapping indices/angles into a range. For example `mod(-3, 5) = 2` (whereas `fmod(-3, 5) = -3`). Modulo by zero yields `nan`, like NumPy.

---

### `Matrix* matrix_arctan2(const Matrix* y, const Matrix* x)`

**Purpose**:  
Returns the element-wise quadrant-aware arctangent — the C analogue of NumPy's `numpy.arctan2(y, x)`. Each element is `atan2(y, x)`, the angle (in radians, in `(-π, π]`) of the point `(x, y)`, using the signs of both arguments to pick the correct quadrant. Same dimensions required; neither input is modified.

**Parameters**:  
- `y`: Numerator (sine-component) matrix.
- `x`: Denominator (cosine-component) matrix, same dimensions as `y`.

**Return Value**:  
A new matrix of angles, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Recovering a full-circle angle from `(x, y)` coordinates — e.g. converting Cartesian to polar.

---

### `Matrix* matrix_hypot(const Matrix* x, const Matrix* y)`

**Purpose**:  
Returns the element-wise Euclidean hypotenuse `√(x² + y²)` — the C analogue of NumPy's `numpy.hypot`, computed without undue overflow. Same dimensions required; neither input is modified.

**Parameters**:  
- `x`, `y`: Matrices of the same dimensions.

**Return Value**:  
A new matrix of hypotenuses, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Vector magnitudes / distances — e.g. the radial coordinate when converting Cartesian to polar.

---

### `Matrix* matrix_copysign(const Matrix* x, const Matrix* y)`

**Purpose**:  
Returns the element-wise magnitude of `x` with the sign of `y` — the C analogue of NumPy's `numpy.copysign`. Same dimensions required; neither input is modified.

**Parameters**:  
- `x`: Magnitude source matrix.
- `y`: Sign source matrix, same dimensions as `x`.

**Return Value**:  
A new matrix of sign-adjusted values, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Restoring a sign after taking a magnitude, or transferring sign between quantities.

---

### `Matrix* matrix_fmod(const Matrix* x, const Matrix* y)`

**Purpose**:  
Returns the element-wise C-style floating-point remainder — the C analogue of NumPy's `numpy.fmod`. The result has the sign of the **dividend** `x` (`x − trunc(x/y)·y`), **unlike `matrix_mod`** (NumPy's `mod`/`remainder`), whose result takes the sign of the **divisor**. Same dimensions required; neither input is modified.

**Parameters**:  
- `x`: Dividend matrix.
- `y`: Divisor matrix, same dimensions as `x`.

**Return Value**:  
A new matrix of remainders, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
C/IEEE-style remainder where the result should follow the dividend's sign. For example `fmod(-3, 5) = -3`, whereas `matrix_mod(-3, 5) = 2`.

---

### `Matrix* matrix_sqrt(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise square root of `matrix` — the C analogue of NumPy's `numpy.sqrt`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `sqrt(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Element-wise roots — e.g. converting variances to standard deviations. A negative element yields `nan`, mirroring real `np.sqrt`.

---

### `Matrix* matrix_sign(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise sign of `matrix` — the C analogue of NumPy's `numpy.sign`. Each element becomes `-1` (negative), `0` (zero, including `-0.0`), or `+1` (positive). The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of element-wise signs, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Extract the direction of each value (e.g. gradient signs in optimisation). A `nan` element yields `nan`, matching `np.sign`.

---

### `Matrix* matrix_reciprocal(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise reciprocal (`1/x`) of `matrix` — the C analogue of NumPy's `numpy.reciprocal`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `1.0 / x`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Invert rates or scale factors element-wise. Reciprocal of zero is not an error: as in NumPy's default mode it follows IEEE-754 and yields `+inf` (`1/0.0`) or `-inf` (`1/-0.0`).

---

### `Matrix* matrix_clip(const Matrix* matrix, double min_val, double max_val)`

**Purpose**:  
Returns a new matrix with every element of `matrix` clamped to the interval `[min_val, max_val]` — the C analogue of NumPy's `numpy.clip`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `min_val`: Lower bound of the clamp interval.
- `max_val`: Upper bound of the clamp interval.

**Return Value**:  
A new clamped matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Bound values into a valid range (e.g. clamp probabilities to `[0, 1]`, or limit pixel values). The computation mirrors NumPy's `minimum(max, maximum(x, min))` with NaN propagation, so a `nan` element yields `nan`, and if `min_val > max_val` every element collapses to `max_val`.

---

### `Matrix* matrix_round(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with every element rounded to the nearest integer — the C analogue of NumPy's `numpy.round` (with the default `decimals=0`). The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of rounded values, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Snap values to whole numbers. Like NumPy, ties round to the nearest **even** integer (banker's rounding): `0.5 → 0`, `1.5 → 2`, `2.5 → 2`, `3.5 → 4`, `-2.5 → -2`. (This differs from C's `round`, which rounds halves away from zero.)

---

### `Matrix* matrix_floor(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise floor of `matrix` — the C analogue of NumPy's `numpy.floor`. Each element becomes the largest integer not greater than it. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of floored values, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Round down — e.g. `2.7 → 2`, `-2.3 → -3` (toward negative infinity).

---

### `Matrix* matrix_ceil(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the element-wise ceiling of `matrix` — the C analogue of NumPy's `numpy.ceil`. Each element becomes the smallest integer not less than it. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of ceiled values, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Round up — e.g. `2.3 → 3`, `-2.7 → -2` (toward positive infinity).

---

### `Matrix* matrix_square(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise square (`x²`) of `matrix` — the C analogue of NumPy's `numpy.square`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `x·x`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Sum-of-squares / energy terms; cheaper and clearer than `matrix_pow(M, twos)`.

---

### `Matrix* matrix_exp(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise exponential (`eˣ`) of `matrix` — the C analogue of NumPy's `numpy.exp`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `eˣ`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Softmax numerators, exponential decay/growth, probability kernels.

---

### `Matrix* matrix_log(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise natural logarithm (`ln x`) of `matrix` — the C analogue of NumPy's `numpy.log`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `ln(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Log-likelihoods, log-scaling. Like NumPy, `log(0)` is `-inf` and `log(x)` for `x < 0` is `nan`.

---

### `Matrix* matrix_log10(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise base-10 logarithm of `matrix` — the C analogue of NumPy's `numpy.log10`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `log₁₀(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Decibel/order-of-magnitude scaling. Like NumPy, `log10(0)` is `-inf` and negatives give `nan`.

---

### `Matrix* matrix_sin(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise sine of `matrix` (radians) — the C analogue of NumPy's `numpy.sin`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `sin(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Waveform generation, rotations, periodic features.

---

### `Matrix* matrix_cos(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise cosine of `matrix` (radians) — the C analogue of NumPy's `numpy.cos`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `cos(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Rotation matrices, signal processing, periodic features.

---

### `Matrix* matrix_tan(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise tangent of `matrix` (radians) — the C analogue of NumPy's `numpy.tan`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `tan(x)`, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Slopes/angles, projective geometry.

---

### `Matrix* matrix_trunc(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise truncation of `matrix` toward zero — the C analogue of NumPy's `numpy.trunc`. Discards the fractional part (`2.7 → 2`, `-2.7 → -2`), unlike `matrix_floor`/`matrix_ceil` which round toward `−∞`/`+∞`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of truncated values, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Taking the integer part of signed values without the directional bias of floor/ceil.

---

### `Matrix* matrix_cbrt(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise cube root of `matrix` — the C analogue of NumPy's `numpy.cbrt`. Unlike `matrix_sqrt`, negative inputs are valid (`cbrt(-27) = -3`). The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of element-wise cube roots, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Inverting cubes, scaling by volume; works for negative values where `sqrt` would give `nan`.

---

### `Matrix* matrix_log2(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise base-2 logarithm — the C analogue of NumPy's `numpy.log2`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `log₂(x)`, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`. Like NumPy, `log2(0)` is `-inf` and negatives give `nan`.

**Usage Case**:  
Information-theoretic measures (bits/entropy), octave/level scaling.

---

### `Matrix* matrix_exp2(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise base-2 exponential (`2ˣ`) — the C analogue of NumPy's `numpy.exp2`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `2ˣ`, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Powers-of-two scaling; the inverse of `matrix_log2`.

---

### `Matrix* matrix_expm1(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise `eˣ − 1` — the C analogue of NumPy's `numpy.expm1`, computed accurately for small `x` (where `exp(x) − 1` would lose precision). The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `eˣ − 1`, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Small-rate growth/interest, numerically stable softplus-like terms.

---

### `Matrix* matrix_log1p(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise `ln(1 + x)` — the C analogue of NumPy's `numpy.log1p`, computed accurately for small `x`. The inverse of `matrix_expm1`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix where every element is `ln(1 + x)`, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Log-returns and likelihoods where `x` is close to 0.

---

### `Matrix* matrix_arcsin(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise inverse sine (radians) — the C analogue of NumPy's `numpy.arcsin`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of arcsines, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`. An element outside `[-1, 1]` yields `nan`, like NumPy.

**Usage Case**:  
Recovering angles from sine ratios.

---

### `Matrix* matrix_arccos(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise inverse cosine (radians) — the C analogue of NumPy's `numpy.arccos`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of arccosines, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`. An element outside `[-1, 1]` yields `nan`, like NumPy.

**Usage Case**:  
Angle between vectors from a normalised dot product.

---

### `Matrix* matrix_arctan(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise inverse tangent (radians) — the C analogue of NumPy's `numpy.arctan`. For the quadrant-aware two-argument form use `matrix_arctan2`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of arctangents, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Slope-to-angle conversion.

---

### `Matrix* matrix_sinh(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise hyperbolic sine — the C analogue of NumPy's `numpy.sinh`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of hyperbolic sines, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Catenary curves, hyperbolic geometry.

---

### `Matrix* matrix_cosh(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise hyperbolic cosine — the C analogue of NumPy's `numpy.cosh`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of hyperbolic cosines, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Catenary curves, stable softmax-style normalisers.

---

### `Matrix* matrix_tanh(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise hyperbolic tangent — the C analogue of NumPy's `numpy.tanh`. Maps values into `(-1, 1)`; the classic neural-network activation. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix of hyperbolic tangents, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Activation function in neural networks; squashing values to a bounded range.

---

### `Matrix* matrix_deg2rad(const Matrix* matrix)`

**Purpose**:  
Converts degrees to radians element-wise (`x · π/180`) — the C analogue of NumPy's `numpy.deg2rad`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix in degrees (not modified).

**Return Value**:  
A new matrix in radians, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Preparing degree-valued angles for the trig functions, which expect radians.

---

### `Matrix* matrix_rad2deg(const Matrix* matrix)`

**Purpose**:  
Converts radians to degrees element-wise (`x · 180/π`) — the C analogue of NumPy's `numpy.rad2deg`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix in radians (not modified).

**Return Value**:  
A new matrix in degrees, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Presenting computed angles in human-friendly degrees.

---

### `Matrix* matrix_signbit(const Matrix* matrix)`

**Purpose**:  
Returns a 1/0 mask of the negatively-signed positions — the C analogue of NumPy's `numpy.signbit`. Each element is `1.0` where the sign bit is set (negative values, **including `-0.0`**), otherwise `0.0`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` on `NULL` input/allocation failure. Free it with `matrix_deallocate`.

**Usage Case**:  
Detecting negative entries (distinguishing `-0.0` from `+0.0`, which `matrix_less(M, 0)` cannot).

---

### `Matrix* matrix_isnan(const Matrix* matrix)`

**Purpose**:  
Returns a 1/0 mask of the `NaN` positions — the C analogue of NumPy's `numpy.isnan`. Each element is `1.0` where the input is `NaN`, otherwise `0.0`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Detecting/counting missing data (combine with `matrix_sum` to count NaNs, or `matrix_where` to replace them).

---

### `Matrix* matrix_isinf(const Matrix* matrix)`

**Purpose**:  
Returns a 1/0 mask of the infinite positions — the C analogue of NumPy's `numpy.isinf`. Each element is `1.0` where the input is `±inf`, otherwise `0.0`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Flagging overflow/divergence in a computation.

---

### `Matrix* matrix_isfinite(const Matrix* matrix)`

**Purpose**:  
Returns a 1/0 mask of the finite positions — the C analogue of NumPy's `numpy.isfinite`. Each element is `1.0` where the input is finite (neither `NaN` nor `±inf`), otherwise `0.0`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Validating that a result is clean before using it; the complement of `isnan | isinf`.

---

### `Matrix* matrix_logical_not(const Matrix* matrix)`

**Purpose**:  
Returns the element-wise logical NOT — the C analogue of NumPy's `numpy.logical_not`. Each element is `1.0` where the input is zero (false), otherwise `0.0`. Any non-zero value (including `NaN`) is treated as true. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Inverting a mask — e.g. `logical_not(isnan(M))` is equivalent to `isfinite`-of-non-inf.

---

### `Matrix* matrix_maximum(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns a new matrix containing the element-wise maximum of two equally-sized matrices — the C analogue of NumPy's `numpy.maximum(A, B)`. Neither input is modified.

**Parameters**:  
- `matrix1`: First matrix (not modified).
- `matrix2`: Second matrix, same dimensions as `matrix1` (not modified).

**Return Value**:  
A new matrix where each element is `max(a, b)`, or `NULL` if either matrix is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
A ReLU-style clamp (`matrix_maximum(x, zeros)`), or combining two matrices keeping the larger value at each position. Like NumPy's `maximum` (and unlike C's `fmax`), `nan` propagates: if either operand is `nan` the result is `nan`. This is distinct from the reduction `matrix_max_element`, which collapses the whole matrix to one value.

---

### `Matrix* matrix_minimum(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns a new matrix containing the element-wise minimum of two equally-sized matrices — the C analogue of NumPy's `numpy.minimum(A, B)`. Neither input is modified.

**Parameters**:  
- `matrix1`: First matrix (not modified).
- `matrix2`: Second matrix, same dimensions as `matrix1` (not modified).

**Return Value**:  
A new matrix where each element is `min(a, b)`, or `NULL` if either matrix is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Capping values against a ceiling matrix, or combining two matrices keeping the smaller value at each position. Like NumPy's `minimum`, `nan` propagates.

---

### `Matrix* matrix_greater(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise `A > B` comparison as a 1/0 mask — the C analogue of NumPy's `numpy.greater`. Each element is `1.0` where `a > b`, otherwise `0.0`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare (same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Thresholding (`matrix_greater(M, threshold)`). A comparison involving `NaN` is false (`0.0`), like NumPy.

---

### `Matrix* matrix_greater_equal(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise `A >= B` comparison as a 1/0 mask — the C analogue of NumPy's `numpy.greater_equal`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare (same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Inclusive thresholding / range membership.

---

### `Matrix* matrix_less(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise `A < B` comparison as a 1/0 mask — the C analogue of NumPy's `numpy.less`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare (same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Lower-bound thresholding.

---

### `Matrix* matrix_less_equal(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise `A <= B` comparison as a 1/0 mask — the C analogue of NumPy's `numpy.less_equal`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare (same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Inclusive lower-bound / range checks.

---

### `Matrix* matrix_logical_and(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise logical AND of two matrices — the C analogue of NumPy's `numpy.logical_and`. Each element is `1.0` where both inputs are non-zero (true), otherwise `0.0`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices (treated as boolean: non-zero = true; same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Combining conditions, e.g. `(M > lo) AND (M < hi)` for a band-pass mask.

---

### `Matrix* matrix_logical_or(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns the element-wise logical OR of two matrices — the C analogue of NumPy's `numpy.logical_or`. Each element is `1.0` where either input is non-zero (true), otherwise `0.0`. Same dimensions required; neither input is modified.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices (treated as boolean: non-zero = true; same dimensions, not modified).

**Return Value**:  
A new 1/0 mask matrix, or `NULL` if either is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Unioning conditions, e.g. flagging entries that are either `NaN` or `inf`.

---

### `Matrix* matrix_where(const Matrix* cond, const Matrix* a, const Matrix* b)`

**Purpose**:  
Element-wise conditional select — the C analogue of NumPy's `numpy.where(cond, a, b)`. Each result element is taken from `a` where `cond` is non-zero (true), otherwise from `b`. All three matrices must have identical dimensions; none is modified.

**Parameters**:  
- `cond`: Condition matrix (non-zero = true).
- `a`: Values chosen where `cond` is true.
- `b`: Values chosen where `cond` is false.

**Return Value**:  
A new matrix of selected values, or `NULL` if any input is `NULL`, the dimensions differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Replacing flagged entries — e.g. `matrix_where(matrix_isnan(M), zeros, M)` substitutes `0` for every `NaN`. As in NumPy, any non-zero condition (including `NaN`) selects from `a`.

---

### `Matrix* matrix_outer(const Matrix* a, const Matrix* b)`

**Purpose**:  
Returns the outer product of two vectors — the C analogue of NumPy's `numpy.outer(a, b)`. Both inputs are flattened to 1-D, and the result is a `len(a) × len(b)` matrix where `result[i][j] = a[i] · b[j]`. Neither input is modified.

**Parameters**:  
- `a`: First vector (any shape; flattened).
- `b`: Second vector (any shape; flattened).

**Return Value**:  
A new `len(a) × len(b)` matrix, or `NULL` if either input is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Building rank-1 matrices, e.g. a weight update `outer(error, input)` in a perceptron.

---

### `Matrix* matrix_cross(const Matrix* a, const Matrix* b)`

**Purpose**:  
Returns the cross product of two 3-element vectors — the C analogue of NumPy's `numpy.cross(a, b)` for 3-D vectors: `a × b = (a₁b₂ − a₂b₁, a₂b₀ − a₀b₂, a₀b₁ − a₁b₀)`, as a `1 × 3` row vector. Both inputs must contain exactly 3 elements (flattened). Neither input is modified.

**Parameters**:  
- `a`: First 3-element vector.
- `b`: Second 3-element vector.

**Return Value**:  
A new `1 × 3` matrix holding the cross product, or `NULL` if either input is `NULL`, does not have exactly 3 elements, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Surface normals, torque/angular-momentum calculations in 3-D geometry and graphics.

---

### `Matrix* matrix_convolve(const Matrix* a, const Matrix* v)`

**Purpose**:  
Returns the full discrete linear convolution of two 1-D sequences — the C analogue of NumPy's `numpy.convolve(a, v)` with the default `mode='full'`. Both inputs are flattened; the result is a `1 × (len(a) + len(v) − 1)` row vector with `out[n] = Σₖ a[k]·v[n−k]`. Neither input is modified.

**Parameters**:  
- `a`: First sequence (any shape; flattened).
- `v`: Second sequence (any shape; flattened).

**Return Value**:  
A new `1 × (M + N − 1)` row vector holding the convolution, or `NULL` if either input is `NULL`/empty or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Filtering a signal with a kernel, polynomial multiplication (coefficients convolve).

---

### `Matrix* matrix_transpose(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix that is the transpose of `matrix` (rows and columns swapped).

**Parameters**:  
- `matrix`: Matrix to transpose.

**Return Value**:  
Newly allocated transpose matrix, or `NULL` on failure.

**Usage Case**:  
Compute `A^T A` for least-squares problems, convert between row and column vectors.

---

### `Matrix* matrix_reshape(const Matrix* matrix, size_t new_rows, size_t new_cols)`

**Purpose**:  
Returns a new matrix with the same elements re-laid into a `new_rows × new_cols` shape — the C analogue of NumPy's `numpy.reshape(M, (new_rows, new_cols))` in the default row-major (C) order. The total element count must be unchanged. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `new_rows`: Rows in the result (must be greater than 0).
- `new_cols`: Columns in the result (must be greater than 0).

**Return Value**:  
A new `new_rows × new_cols` matrix holding the same elements in row-major order, or `NULL` if `matrix` is `NULL`, either dimension is 0, the element count differs (`new_rows·new_cols ≠ rows·cols`), or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Turn a flat vector into a 2-D grid, or re-tile data — e.g. reshape a `1 × 6` row into a `2 × 3` matrix.

---

### `Matrix* matrix_flipud(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the row order reversed (flip up-down) — the C analogue of NumPy's `numpy.flipud(M)`. The order within each row is unchanged. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix with reversed row order, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Flip an image/grid vertically, or reverse a column vector top-to-bottom.

---

### `Matrix* matrix_fliplr(const Matrix* matrix)`

**Purpose**:  
Returns a new matrix with the column order reversed (flip left-right) — the C analogue of NumPy's `numpy.fliplr(M)`. The order of the rows is unchanged. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).

**Return Value**:  
A new matrix with reversed column order, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Mirror an image/grid horizontally, or reverse a row vector left-to-right.

---

### `Matrix* matrix_rot90(const Matrix* matrix, int k)`

**Purpose**:  
Returns a new matrix rotated by `90 × k` degrees counterclockwise in the plane — the C analogue of NumPy's `numpy.rot90(M, k)`. `k` is taken modulo 4, so any integer works (negative values rotate clockwise). For odd `k` the dimensions swap (`rows × cols` → `cols × rows`); for even `k` the shape is preserved. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `k`: Number of 90° counterclockwise rotations (any `int`; `k = 0`/`4` is a copy, `k = -1` rotates clockwise).

**Return Value**:  
A new rotated matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Rotate an image/grid; `matrix_rot90(M, 1)` rotates counterclockwise, `matrix_rot90(M, -1)` (or `3`) clockwise.

---

### `Matrix* matrix_roll(const Matrix* matrix, int shift)`

**Purpose**:  
Returns a new matrix with all elements cyclically shifted — the C analogue of NumPy's `numpy.roll(M, shift)` with the default `axis=None`. The matrix is flattened in row-major order, rotated by `shift` positions, and reshaped back. A positive `shift` moves elements toward later indices; `shift` is taken modulo the element count. The input is not modified. For a per-axis roll use `matrix_roll_axis`.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `shift`: Number of positions to roll (any `int`; negative rolls the other way).

**Return Value**:  
A new matrix (same shape) with elements cyclically shifted, or `NULL` if `matrix` is `NULL`/invalid or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Circular buffers, wrap-around shifts of a flattened sequence.

---

### `Matrix* matrix_roll_axis(const Matrix* matrix, int shift, int axis)`

**Purpose**:  
Returns a new matrix cyclically shifted along one axis — the C analogue of NumPy's `numpy.roll(M, shift, axis=...)`. `axis = 0` rolls the rows (down for positive `shift`); `axis = 1` rolls the columns (right). The result has the same shape as the input, which is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `shift`: Number of positions to roll along the axis (any `int`).
- `axis`: `0` to roll rows, `1` to roll columns.

**Return Value**:  
A new matrix (same shape) rolled along the chosen axis, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Shifting rows/columns with wrap-around, e.g. time-series lag along one dimension.

---

### `Matrix* matrix_shift(const Matrix* matrix, int periods, int axis, double fill_value)`

**Purpose**:  
Shifts the data along an axis **without wrapping** (unlike `matrix_roll`), filling the vacated cells — the C analogue of pandas' `DataFrame.shift(periods, axis, fill_value)`. A positive `periods` shifts forward (down for `axis = 0`, right for `axis = 1`); a negative `periods` the other way. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `periods`: Number of positions to shift (any `int`).
- `axis`: `0` to shift rows, `1` to shift columns.
- `fill_value`: Value placed in the vacated cells (use `NAN` for pandas' default).

**Return Value**:  
A new matrix (same shape) with the data shifted and gaps filled, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Computing lags/leads of a time series, where the introduced gap should be a sentinel (`NaN` or `0`) rather than wrapped-around data.

---

### `Matrix* matrix_tril(const Matrix* matrix, int k)`

**Purpose**:  
Returns a new matrix holding the lower-triangular part of `matrix` — the C analogue of NumPy's `numpy.tril(M, k)`. Elements on and below the `k`-th diagonal are kept; everything above is set to `0.0`. `k = 0` is the main diagonal, `k > 0` keeps extra super-diagonals, `k < 0` drops sub-diagonals. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `k`: Diagonal offset above which elements are zeroed.

**Return Value**:  
A new lower-triangular matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Extracting the `L` factor pattern, masking a matrix to its lower triangle.

---

### `Matrix* matrix_triu(const Matrix* matrix, int k)`

**Purpose**:  
Returns a new matrix holding the upper-triangular part of `matrix` — the C analogue of NumPy's `numpy.triu(M, k)`. Elements on and above the `k`-th diagonal are kept; everything below is set to `0.0`. `k = 0` is the main diagonal, `k > 0` drops super-diagonals, `k < 0` keeps extra sub-diagonals. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `k`: Diagonal offset below which elements are zeroed.

**Return Value**:  
A new upper-triangular matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Extracting the `U`/`R` factor pattern, masking a matrix to its upper triangle.

---

### `Matrix* matrix_hstack(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Concatenates two matrices side by side — the C analogue of NumPy's `numpy.hstack((A, B))` for 2-D arrays. The two matrices must have the same number of rows; the result has that many rows and `cols(A) + cols(B)` columns. Neither input is modified.

**Parameters**:  
- `matrix1`: Left matrix (not modified).
- `matrix2`: Right matrix, same number of rows as `matrix1` (not modified).

**Return Value**:  
A new `rows × (cols1 + cols2)` matrix, or `NULL` if either matrix is `NULL`, the row counts differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Appending feature columns, building an augmented matrix `[A | b]` for solving systems. For more than two matrices, chain calls.

---

### `Matrix* matrix_vstack(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Stacks two matrices one above the other — the C analogue of NumPy's `numpy.vstack((A, B))` for 2-D arrays. The two matrices must have the same number of columns; the result has `rows(A) + rows(B)` rows and that many columns. Neither input is modified.

**Parameters**:  
- `matrix1`: Top matrix (not modified).
- `matrix2`: Bottom matrix, same number of columns as `matrix1` (not modified).

**Return Value**:  
A new `(rows1 + rows2) × cols` matrix, or `NULL` if either matrix is `NULL`, the column counts differ, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Appending rows/observations, assembling a dataset from batches. For more than two matrices, chain calls.

---

### `Matrix* matrix_tile(const Matrix* matrix, size_t reps_row, size_t reps_col)`

**Purpose**:  
Repeats the whole matrix in a grid — the C analogue of NumPy's `numpy.tile(M, (reps_row, reps_col))`. Stacks `reps_row` copies vertically and `reps_col` copies horizontally, giving a `(rows·reps_row) × (cols·reps_col)` matrix. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `reps_row`: Vertical repetitions (must be greater than 0).
- `reps_col`: Horizontal repetitions (must be greater than 0).

**Return Value**:  
A new tiled matrix, or `NULL` if `matrix` is `NULL`, either count is 0, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Building a repeating pattern, broadcasting a small block across a larger grid.

---

### `Matrix* matrix_repeat(const Matrix* matrix, size_t repeats)`

**Purpose**:  
Repeats each element — the C analogue of NumPy's `numpy.repeat(M, repeats)` with the default `axis=None`. The matrix is flattened (row-major) and each element is repeated `repeats` times consecutively, returning a `1 × (rows·cols·repeats)` row vector. For per-row/column repetition use `matrix_repeat_axis`.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `repeats`: Times to repeat each element (must be greater than 0).

**Return Value**:  
A new `1 × (rows·cols·repeats)` row vector, or `NULL` if `matrix` is `NULL`/invalid, `repeats` is 0, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Upsampling a flattened signal, expanding run-length data.

---

### `Matrix* matrix_repeat_axis(const Matrix* matrix, size_t repeats, int axis)`

**Purpose**:  
Repeats rows or columns — the C analogue of NumPy's `numpy.repeat(M, repeats, axis=...)`. `axis = 0` repeats each row (`(rows·repeats) × cols`); `axis = 1` repeats each column (`rows × (cols·repeats)`). The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `repeats`: Number of repetitions (must be greater than 0).
- `axis`: `0` to repeat rows, `1` to repeat columns.

**Return Value**:  
A new matrix with rows/columns repeated, or `NULL` if `matrix` is `NULL`, `repeats` is 0, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Upsampling an image along one dimension, expanding grouped rows.

---

### `Matrix* matrix_pad(const Matrix* matrix, size_t pad, double value)`

**Purpose**:  
Adds a constant-valued border — the C analogue of NumPy's `numpy.pad(M, pad, mode='constant', constant_values=value)`. Surrounds the matrix with a border of width `pad` filled with `value` on all four sides, giving a `(rows + 2·pad) × (cols + 2·pad)` matrix. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `pad`: Border width on every side (`0` yields a plain copy).
- `value`: Constant value used to fill the border.

**Return Value**:  
A new padded matrix, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Zero-padding an image before convolution, framing a grid with a sentinel border.

---

### `Matrix* matrix_sort(const Matrix* matrix, int axis)`

**Purpose**:  
Returns a copy sorted along one axis — the C analogue of NumPy's `numpy.sort(M, axis=...)`. `axis = 1` sorts each row, `axis = 0` sorts each column, both ascending. (NumPy's default `axis=-1` corresponds to `axis = 1` for a 2-D array.) The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `axis`: `0` to sort each column, `1` to sort each row.

**Return Value**:  
A new sorted matrix (same shape), or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Ordering each row/column of a dataset; `NaN` values sort to the end, matching NumPy.

---

### `Matrix* matrix_argsort(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the indices that would sort the matrix along one axis — the C analogue of NumPy's `numpy.argsort(M, axis=...)`. For each row (`axis = 1`) or column (`axis = 0`) it returns the per-axis indices that put that row/column in ascending order, with the indices stored as `double` values. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `axis`: `0` for column-wise indices, `1` for row-wise indices.

**Return Value**:  
A new matrix (same shape) of sorting indices, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Ranking, reordering a companion matrix by another's order. The sort is stable (ties keep their original order); `NaN` sorts last.

---

### `Matrix* matrix_diff(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the first discrete difference along one axis — the C analogue of NumPy's `numpy.diff(M, axis=...)` (order `n = 1`). `axis = 1` differences along each row (result loses one column); `axis = 0` differences along each column (result loses one row). NumPy's default `axis=-1` corresponds to `axis = 1` here. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `axis`: `0` to difference down columns, `1` along rows.

**Return Value**:  
A new matrix one shorter along the chosen axis, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, the axis length is less than 2, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Discrete derivatives, spacing between successive samples, detecting changes in a sequence.

---

### `Matrix* matrix_unique(const Matrix* matrix)`

**Purpose**:  
Returns the sorted unique values of a matrix — the C analogue of NumPy's `numpy.unique(M)`. The matrix is flattened, sorted ascending, and duplicates removed, returning a `1 × k` row vector of the `k` distinct values.

**Parameters**:  
- `matrix`: Input matrix (not modified).

**Return Value**:  
A new `1 × k` row vector of sorted unique values, or `NULL` if `matrix` is `NULL`/invalid or a temporary allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Listing the distinct labels/categories in a matrix, or de-duplicating a value set.

---

### `Matrix* matrix_searchsorted(const Matrix* a, const Matrix* v)`

**Purpose**:  
Finds insertion indices into a sorted vector by binary search — the C analogue of NumPy's `numpy.searchsorted(a, v)` with the default `side='left'`. For each element of `v`, returns the leftmost index at which it could be inserted into the sorted vector `a` while keeping it sorted (i.e. the number of elements of `a` strictly less than the value). Both inputs are treated as flattened 1-D vectors; `a` must be sorted ascending.

**Parameters**:  
- `a`: The sorted reference vector.
- `v`: The query values.

**Return Value**:  
A new matrix (same shape as `v`) of insertion indices stored as `double`, or `NULL` if either input is `NULL`/empty or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Bucketing values into sorted bins, table lookups, finding ranks against a reference distribution.

---

### `Matrix** matrix_split(const Matrix* matrix, size_t sections, int axis, size_t* out_count)`

**Purpose**:  
Splits a matrix into equal blocks along an axis — the C analogue of NumPy's `numpy.split` (and `hsplit`/`vsplit`). `axis = 0` splits the rows (like `vsplit`); `axis = 1` splits the columns (like `hsplit`). The axis length must divide evenly by `sections`.

**Parameters**:  
- `matrix`: The matrix to split.
- `sections`: Number of equal pieces (must divide the axis length evenly).
- `axis`: `0` to split rows, `1` to split columns.
- `out_count`: Out-parameter receiving the number of returned pieces (or 0 on failure); may be `NULL`.

**Return Value**:  
A newly allocated array of `sections` matrices, or `NULL` on any error (NULL matrix, zero sections, bad axis, non-divisible length, or allocation failure). **Ownership**: the caller owns both the array and each matrix — release every element with `matrix_deallocate`, then `free` the array.

**Usage Case**:  
Partitioning a dataset into equal folds/batches, or separating stacked blocks.

---

### `Matrix* matrix_inverse(const Matrix* matrix)`

**Purpose**:  
Computes the inverse of a square matrix by Gauss–Jordan elimination with partial pivoting (O(n³)).

**Parameters**:  
- `matrix`: Square matrix to invert.

**Return Value**:  
Newly allocated inverse matrix, or `NULL` if the matrix is not square or is singular, i.e. a pivot is no larger than `n·DBL_EPSILON` times the largest entry. The test is relative, so small-scale matrices such as `0.01·I` invert normally.

**Usage Case**:  
Solve linear systems `Ax = b` via `x = A^{-1} b`, compute pseudo-inverses.

---

### `Matrix* matrix_pinv(const Matrix* matrix)`

**Purpose**:  
Computes the Moore–Penrose pseudo-inverse — the C analogue of NumPy's `numpy.linalg.pinv(M)`, built from the SVD as `A⁺ = V·Σ⁺·Uᵀ`. Works for **any shape**: for an `m × n` input the result is `n × m`. Unlike `matrix_inverse` (square, exact), this handles rectangular and rank-deficient matrices.

**Parameters**:  
- `matrix`: Matrix to pseudo-invert (any shape).

**Return Value**:  
A new `n × m` pseudo-inverse, or `NULL` if `matrix` is `NULL` or the SVD/allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Least-squares solutions (`x = pinv(A)·b`), inverting non-square Jacobians. Singular values at or below `rcond·σ_max`, with `rcond` the larger of `1e-15` and `max(m, n)·eps` (NumPy's defaults), are treated as zero; because the SVD is computed on `A` itself, a zero singular value really comes out at rounding level, so rank-deficient inputs give NumPy's result.

---

### `Matrix* matrix_inverse_gauss_jordan(const Matrix* matrix)`

**Purpose**:  
Computes the inverse of a square matrix using Gauss-Jordan elimination on the augmented matrix `[A | I]`.

**Parameters**:  
- `matrix`: Square matrix to invert.

**Return Value**:  
Newly allocated inverse matrix, or `NULL` if the matrix is singular, not square, or allocation fails.

**Usage Case**:  
Alternative to `matrix_inverse` — numerically explicit and easy to trace step-by-step.

---

### `Matrix* matrix_adjugate(const Matrix* matrix)`

**Purpose**:  
Computes the adjugate (classical adjoint) of a square matrix: the transpose of the cofactor matrix.

**Parameters**:  
- `matrix`: Square matrix.

**Return Value**:  
Newly allocated adjugate matrix, or `NULL` on failure.

**Usage Case**:  
Compute the inverse via `A^{-1} = adj(A) / det(A)`, used in symbolic matrix inversion.

---

### `Matrix* matrix_cofactor(const Matrix* matrix)`

**Purpose**:  
Computes the cofactor matrix where each element `C[i][j] = (-1)^{i+j} × M_{ij}`, with `M_{ij}` the minor obtained by deleting row `i` and column `j`.

**Parameters**:  
- `matrix`: Square matrix.

**Return Value**:  
Newly allocated cofactor matrix, or `NULL` on failure.

**Usage Case**:  
Building block for adjugate and determinant computations; Cramer's rule.

---

### `Matrix* matrix_create_submatrix(const Matrix* matrix, size_t excludeRow, size_t excludeCol)`

**Purpose**:  
Creates a submatrix by deleting one row and one column from `matrix`.

**Parameters**:  
- `matrix`: Source matrix.  
- `excludeRow`: Zero-based index of the row to remove.  
- `excludeCol`: Zero-based index of the column to remove.

**Return Value**:  
Newly allocated `(rows-1) × (cols-1)` matrix, or `NULL` on failure.

**Usage Case**:  
Computing minors for cofactor expansion of the determinant.

---

### `Matrix* matrix_projection(const Matrix* matrix)`

**Purpose**:  
Computes the orthogonal projection matrix onto the column space of `matrix`: `P = A(A^T A)^{-1} A^T`.

**Parameters**:  
- `matrix`: Input matrix `A` (must have full column rank).

**Return Value**:  
Newly allocated square projection matrix, or `NULL` on failure (e.g., `A^T A` is singular).

**Usage Case**:  
Least-squares regression, orthogonal projection in subspace methods.

---

### `Matrix* matrix_get_main_diagonal_as_column(const Matrix* matrix)`

**Purpose**:  
Extracts the main diagonal elements of a square matrix into a new n×1 column vector.

**Parameters**:  
- `matrix`: Square source matrix.

**Return Value**:  
Newly allocated n×1 matrix, or `NULL` on failure.

**Usage Case**:  
Inspect diagonal entries (e.g., eigenvalues of a diagonal matrix), compute the trace manually.

---

### `Matrix* matrix_get_main_diagonal_as_row(const Matrix* matrix)`

**Purpose**:  
Extracts the main diagonal elements of a square matrix into a new 1×n row vector.

**Parameters**:  
- `matrix`: Square source matrix.

**Return Value**:  
Newly allocated 1×n matrix, or `NULL` on failure.

**Usage Case**:  
Feed diagonal entries into a function expecting a row vector.

---

### `Matrix* matrix_get_minor_diagonal_as_row(const Matrix* matrix)`

**Purpose**:  
Extracts the anti-diagonal (minor diagonal) elements into a new 1×n row vector. Element `[0][n-1]` comes first.

**Parameters**:  
- `matrix`: Square source matrix.

**Return Value**:  
Newly allocated 1×n matrix, or `NULL` on failure.

**Usage Case**:  
Inspect the anti-diagonal of a matrix, check for persymmetry.

---

### `Matrix* matrix_get_minor_diagonal_as_column(const Matrix* matrix)`

**Purpose**:  
Extracts the anti-diagonal elements into a new n×1 column vector.

**Parameters**:  
- `matrix`: Square source matrix.

**Return Value**:  
Newly allocated n×1 matrix, or `NULL` on failure.

**Usage Case**:  
Feed anti-diagonal entries into a function expecting a column vector.

---

### `Matrix* matrix_diagonal(const Matrix* matrix, int k)`

**Purpose**:  
Extracts the `k`-th diagonal as a row vector — the C analogue of NumPy's `numpy.diagonal(M, offset=k)`. Returns the elements `M[i][i+k]`. `k = 0` is the main diagonal, `k > 0` a super-diagonal (above the main), `k < 0` a sub-diagonal (below). Unlike the fixed `matrix_get_main_diagonal_*` helpers, this supports any offset.

**Parameters**:  
- `matrix`: Input matrix.
- `k`: The diagonal offset (`0` = main, `>0` = above, `<0` = below).

**Return Value**:  
A new `1 × len` row vector of the diagonal's elements, or `NULL` if `matrix` is `NULL`, the requested diagonal is empty (`k` out of range), or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Reading off-diagonal bands of a banded matrix; inspecting super-/sub-diagonals.

---

### `bool matrix_fill_diagonal(Matrix* matrix, double value)`

**Purpose**:  
Fills the main diagonal **in place** with a constant — the C analogue of NumPy's `numpy.fill_diagonal(M, value)`. Sets `M[i][i] = value` for `i` from 0 to `min(rows, cols) − 1`. This **mutates** the matrix.

**Parameters**:  
- `matrix`: The matrix to modify (mutated in place).
- `value`: The value written to every main-diagonal entry.

**Return Value**:  
`true` on success; `false` if `matrix` is `NULL`.

**Usage Case**:  
Zeroing self-loops in an adjacency matrix, or setting a regularisation term on the diagonal.

---

### `bool matrix_lu_decomposition(const Matrix* matrix, Matrix** L, Matrix** U)`

**Purpose**:  
Performs LU decomposition of a square matrix without pivoting (Doolittle): `matrix = L × U`, where `L` is unit lower triangular and `U` is upper triangular.

**Parameters**:  
- `matrix`: Square matrix to factorise.  
- `L`: Output pointer filled with the lower triangular factor.  
- `U`: Output pointer filled with the upper triangular factor.

**Return Value**:  
`true` on success; `false` if an argument is `NULL`, the matrix is not square, a pivot that must be divided by is zero (at the rounding level of the matrix's entries — then no LU factorization without pivoting exists, e.g. `[[0,1],[1,0]]`), or allocation fails. On failure `*L` and `*U` are set to `NULL`. For solving systems prefer `matrix_solve`, which pivots.

**Usage Case**:  
Solving linear systems, computing determinants, matrix inversion.

---

### `bool matrix_qr_decomposition(const Matrix* A, Matrix** Q, Matrix** R)`

**Purpose**:  
Performs the thin QR decomposition with Householder reflections: `A = Q × R`, where `Q` (`m × n`) has orthonormal columns and `R` (`n × n`) is upper triangular with a non-negative diagonal (the same factors Gram-Schmidt gives for full-rank input, but numerically stable, and without NaN for rank-deficient input).

**Parameters**:  
- `A`: Matrix to decompose (must have at least as many rows as columns).  
- `Q`: Output pointer filled with the orthogonal factor.  
- `R`: Output pointer filled with the upper triangular factor.

**Return Value**:  
`true` on success; `false` on dimension error or allocation failure.

**Usage Case**:  
Least-squares solving, eigenvalue algorithms, numerical stability improvements over LU.

---

### `Matrix* matrix_cholesky_decomposition(const Matrix* matrix)`

**Purpose**:  
Performs Cholesky decomposition of a symmetric positive-definite matrix: returns the upper triangular `U` such that `matrix = U^T × U`.

**Parameters**:  
- `matrix`: Symmetric positive-definite square matrix.

**Return Value**:  
Newly allocated upper triangular Cholesky factor, or `NULL` if the matrix is not positive definite or allocation fails.

**Usage Case**:  
Solving positive-definite systems (e.g., normal equations), sampling from multivariate Gaussian distributions.

---

### `Matrix* matrix_solve(const Matrix* A, const Matrix* b)`

**Purpose**:  
Solves the linear system `A x = b` for `x` using Gaussian elimination with partial pivoting followed by back substitution — the numerically sound, general-purpose approach (no explicit inverse is formed). `b` may have more than one column, in which case every column is solved at once (i.e. it solves `A X = B` for an `n × k` right-hand side, like NumPy's `linalg.solve`).

**Parameters**:  
- `A`: The coefficient matrix. Must be square (`n × n`) and non-singular.  
- `b`: The right-hand side. Must have `n` rows and one or more columns.

**Return Value**:  
A newly allocated `n × k` matrix holding the solution `x` (`X`), or `NULL` if either input is `NULL`, `A` is not square, the dimensions are incompatible, `A` is singular (no unique solution), or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The everyday workhorse — solving systems of equations: circuit analysis, least-squares normal equations, interpolation coefficients, physics simulation steps. Prefer this over `matrix_inverse` + `matrix_multiply`, which is slower and less accurate.

---

### `bool matrix_eig(const Matrix* matrix, Matrix** eigenvalues, Matrix** eigenvectors)`

**Purpose**:  
Computes the eigenvalues and eigenvectors of a **symmetric** matrix — the C analogue of NumPy's `numpy.linalg.eig` for symmetric input (equivalently `numpy.linalg.eigh`), via the cyclic Jacobi algorithm. A real-valued matrix library cannot represent the complex eigenvalues a general matrix may have, so the input must be symmetric (guaranteeing real eigenvalues and orthonormal eigenvectors).

**Parameters**:  
- `matrix`: The symmetric matrix to decompose.
- `eigenvalues`: Out-parameter receiving an `n × 1` column of eigenvalues in **ascending** order (like `numpy.linalg.eigh`).
- `eigenvectors`: Out-parameter receiving an `n × n` matrix whose **columns** are the corresponding orthonormal eigenvectors.

**Return Value**:  
`true` on success; `false` if `matrix` is `NULL`, not square, not symmetric, an out-pointer is `NULL`, or the iteration fails. On success free `*eigenvalues` and `*eigenvectors` with `matrix_deallocate`.

**Usage Case**:  
Principal-axis analysis, vibration modes, PCA of a covariance matrix. Eigenvectors are unique only up to sign, so a column may have the opposite sign to NumPy's; the eigenvalues and `A·v = λ·v` are unaffected.

---

### `bool matrix_svd(const Matrix* matrix, Matrix** U, Matrix** S, Matrix** V)`

**Purpose**:  
Computes the (thin) singular value decomposition — the C analogue of NumPy's `numpy.linalg.svd(M, full_matrices=False)`. Factors an `m × n` matrix (`m ≥ n`) as `A = U·diag(S)·Vᵀ`, computed with one-sided Jacobi rotations on `A` itself (so small singular values stay accurate; `U` always has orthonormal columns).

**Parameters**:  
- `matrix`: The matrix to decompose (must have `rows ≥ cols`).
- `U`: Out-parameter receiving the `m × n` left singular vectors (orthonormal columns).
- `S`: Out-parameter receiving the `n × 1` singular values in **descending** order.
- `V`: Out-parameter receiving the `n × n` right singular vectors.

**Return Value**:  
`true` on success; `false` if `matrix` is `NULL`, has fewer rows than columns, an out-pointer is `NULL`, or the iteration fails. On success free `*U`, `*S`, `*V` with `matrix_deallocate`.

**Usage Case**:  
Low-rank approximation, PCA, computing `matrix_pinv`/`matrix_cond`. This is the economy SVD (`full_matrices=False`); singular values match NumPy exactly, while singular vectors may differ in sign.

---

### `Matrix* matrix_lstsq(const Matrix* A, const Matrix* b)`

**Purpose**:  
Solves the linear least-squares problem `min ‖A·x − b‖` — the C analogue of NumPy's `numpy.linalg.lstsq(A, b)[0]` for an overdetermined or square, full-column-rank system, using the QR decomposition (`A = QR`, then back-substitute `R·x = Qᵀ·b`). `b` may have multiple right-hand-side columns.

**Parameters**:  
- `A`: The `m × n` coefficient matrix (`m ≥ n`).
- `b`: The `m × k` right-hand side(s) (same number of rows as `A`).

**Return Value**:  
A new `n × k` least-squares solution, or `NULL` if either argument is `NULL`, the row counts differ, `A` has fewer rows than columns, or the QR/allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Fitting a line/curve to data, regression. For the minimum-norm solution of an underdetermined or rank-deficient system, use `matrix_pinv` instead (`x = pinv(A)·b`).

---

### `Matrix* matrix_expm(const Matrix* matrix)`

**Purpose**:  
Computes the matrix exponential `eᴬ` — the C analogue of SciPy's `scipy.linalg.expm(A)`, using scaling-and-squaring with a [6/6] Padé approximation. Scales `A` so its norm is small, approximates `e^(A/2ˢ)` by a Padé rational, then squares `s` times. The input must be square and is not modified.

**Parameters**:  
- `matrix`: The square matrix.

**Return Value**:  
A new matrix holding `eᴬ`, or `NULL` if `matrix` is `NULL`, not square, or an intermediate computation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Solving linear ODE systems (`x(t) = e^{At}·x₀`), continuous-time state transitions, Lie-group exponentials.

---

### `Matrix* matrix_polyval(const Matrix* p, const Matrix* x)`

**Purpose**:  
Evaluates a polynomial at one or more points — the C analogue of NumPy's `numpy.polyval(p, x)`. The coefficients `p = [p₀, p₁, …, pₙ]` are highest-degree-first (so the polynomial is `p₀·xⁿ + … + pₙ`), and it is evaluated at every element of `x` via Horner's method. The result has the same shape as `x`.

**Parameters**:  
- `p`: The coefficient vector (flattened, highest degree first).
- `x`: The points at which to evaluate (any shape).

**Return Value**:  
A new matrix (same shape as `x`) of evaluated values, or `NULL` if either input is `NULL`/empty or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Evaluating fitted polynomials, basis functions, or characteristic polynomials at a set of points.

---

### `Matrix* matrix_roots(const Matrix* coeffs)`

**Purpose**:  
Computes the roots of a polynomial — the C analogue of NumPy's `numpy.roots(p)`. Given coefficients `[p₀, …, pₙ]` (highest degree first), returns the `n` roots. Because roots may be complex, the result is an `n × 2` matrix whose columns are the **real** and **imaginary** parts. Found with the Durand–Kerner (Weierstrass) iteration, which solves for all roots simultaneously.

**Parameters**:  
- `coeffs`: The coefficient vector (flattened, highest degree first).

**Return Value**:  
A new `n × 2` matrix of `(real, imag)` root pairs, or `NULL` if `coeffs` is `NULL`, has degree below 1 (a constant has no roots), or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Finding the roots/zeros of a polynomial, eigenvalues via the characteristic polynomial. Real roots come back with a tiny (≈0) imaginary part; the iteration is most reliable for distinct roots.

---

### `double matrix_determinant(const Matrix* matrix)`

**Purpose**:  
Computes the scalar determinant of a square matrix: closed-form for sizes up to 3×3, LU decomposition with partial pivoting (O(n³)) above that.

**Parameters**:  
- `matrix`: Square matrix.

**Return Value**:  
Determinant value, or `0.0` on non-square input or `NULL`.

**Usage Case**:  
Test for invertibility (`det ≠ 0`), compute signed volume of a parallelepiped.

---

### `bool matrix_slogdet(const Matrix* matrix, double* sign, double* logabsdet)`

**Purpose**:  
Computes the sign and natural log of the absolute determinant — the C analogue of NumPy's `numpy.linalg.slogdet(M)`, where `det(M) = sign · exp(logabsdet)`. Computed via Gaussian elimination with partial pivoting, accumulating `log|pivot|`, so it avoids the overflow/underflow a direct determinant can suffer for large matrices.

**Parameters**:  
- `matrix`: The square matrix.
- `sign`: Out-parameter receiving `-1`, `0`, or `+1` (0 if singular).
- `logabsdet`: Out-parameter receiving `log|det|` (`-inf` if singular).

**Return Value**:  
`true` on success; `false` if `matrix` is `NULL`, not square, an out-pointer is `NULL`, or a temporary allocation fails.

**Usage Case**:  
Log-likelihoods of multivariate Gaussians (which need `log|det(Σ)|`), where the determinant itself would overflow.

---

### `double matrix_trace(const Matrix* matrix)`

**Purpose**:  
Returns the sum of the main diagonal elements of a square matrix.

**Parameters**:  
- `matrix`: Square matrix.

**Return Value**:  
Trace (sum of diagonal entries), or `0.0` on non-square or `NULL` input.

**Usage Case**:  
Quick invariant check; trace equals the sum of eigenvalues.

---

### `double matrix_frobenius_norm(const Matrix* matrix)`

**Purpose**:  
Computes the Frobenius norm: the square root of the sum of the squares of all elements.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
Frobenius norm as a `double`, or `0.0` on `NULL` input.

**Usage Case**:  
Measure overall matrix magnitude; regularisation term in optimisation (ridge regression).

---

### `double matrix_l1_norm(const Matrix* matrix)`

**Purpose**:  
Computes the L1 (maximum absolute column sum) norm.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
L1 norm, or `0.0` on `NULL` input.

**Usage Case**:  
Operator norm bound, sparse matrix analysis.

---

### `double matrix_infinity_norm(const Matrix* matrix)`

**Purpose**:  
Computes the infinity (maximum absolute row sum) norm.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
Infinity norm, or `0.0` on `NULL` input.

**Usage Case**:  
Bound on the maximum row-wise perturbation; iterative solver convergence checks.

---

### `double matrix_cond(const Matrix* matrix)`

**Purpose**:  
Computes the 2-norm condition number — the C analogue of NumPy's `numpy.linalg.cond(M)` with the default 2-norm: the ratio of the largest to the smallest singular value, `σ_max / σ_min`. Complements the existing Frobenius / L1 / ∞ norms with the spectral measure. Works for any shape.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The condition number (`+inf` for a singular matrix), or `NaN` if `matrix` is `NULL` or the SVD fails.

**Usage Case**:  
Assessing how sensitive a linear solve is to perturbations — a large condition number warns that `matrix_solve`/`matrix_inverse` results may be inaccurate.

---

### `double matrix_nuclear_norm(const Matrix* matrix)`

**Purpose**:  
Computes the nuclear (trace) norm — the C analogue of NumPy's `numpy.linalg.norm(M, 'nuc')`: the **sum** of the singular values, computed from the SVD. Works for any shape.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The sum of the singular values, or `NaN` if `matrix` is `NULL` or the SVD fails.

**Usage Case**:  
Low-rank regularisation (the nuclear norm is the convex surrogate for matrix rank, used in matrix completion).

---

### `double matrix_spectral_norm(const Matrix* matrix)`

**Purpose**:  
Computes the spectral (2-)norm — the C analogue of NumPy's `numpy.linalg.norm(M, 2)`: the **largest** singular value, computed from the SVD. This is the true matrix 2-norm, completing the norm family alongside the existing Frobenius / L1 / infinity norms. Works for any shape.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The largest singular value, or `NaN` if `matrix` is `NULL` or the SVD fails.

**Usage Case**:  
Bounding the maximum amplification of a linear map; Lipschitz constants; operator-norm analysis.

---

### `double matrix_min_element(const Matrix* matrix)`

**Purpose**:  
Returns the smallest element value across the entire matrix.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
Minimum element value, or `0.0` on `NULL` input.

**Usage Case**:  
Data range inspection, clipping, and normalisation.

---

### `double matrix_max_element(const Matrix* matrix)`

**Purpose**:  
Returns the largest element value across the entire matrix.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
Maximum element value, or `0.0` on `NULL` input.

**Usage Case**:  
Data range inspection, clipping, softmax denominator, and normalisation.

---

### `double matrix_sum(const Matrix* matrix)`

**Purpose**:  
Returns the sum of all elements — the C analogue of NumPy's `numpy.sum(M)`. For a per-axis reduction (column or row sums) use `matrix_sum_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The sum of every element, or `0.0` if `matrix` is `NULL`/invalid. If any element is `nan`, the result is `nan` (like NumPy).

**Usage Case**:  
Totals, dot-product building blocks, and the numerator of a mean.

---

### `double matrix_prod(const Matrix* matrix)`

**Purpose**:  
Returns the product of all elements — the C analogue of NumPy's `numpy.prod(M)`. For a per-axis reduction use `matrix_prod_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The product of every element, or `0.0` if `matrix` is `NULL`/invalid. If any element is `nan`, the result is `nan` (like NumPy).

**Usage Case**:  
Combined scale factors, determinant-of-diagonal shortcuts, probability products.

---

### `double matrix_mean(const Matrix* matrix)`

**Purpose**:  
Returns the arithmetic mean of all elements — the C analogue of NumPy's `numpy.mean(M)`. For a per-axis mean use `matrix_mean_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The mean of every element, or `0.0` if `matrix` is `NULL`/invalid. If any element is `nan`, the result is `nan` (like NumPy).

**Usage Case**:  
Centering data, computing averages, the first moment of a distribution.

---

### `double matrix_ptp(const Matrix* matrix)`

**Purpose**:  
Returns the peak-to-peak range (`max - min`) over all elements — the C analogue of NumPy's `numpy.ptp(M)`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
`max - min` across the matrix, or `0.0` if `matrix` is `NULL`/invalid.

**Usage Case**:  
Measuring spread/amplitude, e.g. for min-max normalisation (`(x - min) / ptp`).

---

### `size_t matrix_argmax(const Matrix* matrix)`

**Purpose**:  
Returns the flattened (row-major, C-order) index of the largest element — the C analogue of NumPy's `numpy.argmax(M)`. On ties, the first occurrence wins, exactly as in NumPy.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The flattened index of the maximum, or `SIZE_MAX` if `matrix` is `NULL`/invalid. Recover row/column as `row = index / cols`, `col = index % cols`.

**Usage Case**:  
Classification (predicted label = argmax of scores), locating the dominant entry.

---

### `size_t matrix_argmin(const Matrix* matrix)`

**Purpose**:  
Returns the flattened (row-major, C-order) index of the smallest element — the C analogue of NumPy's `numpy.argmin(M)`. On ties, the first occurrence wins.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The flattened index of the minimum, or `SIZE_MAX` if `matrix` is `NULL`/invalid. Recover row/column as `row = index / cols`, `col = index % cols`.

**Usage Case**:  
Nearest-neighbour search (closest distance), locating the smallest cost.

---

### `Matrix* matrix_sum_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Sums the matrix along one axis — the C analogue of NumPy's `numpy.sum(M, axis=...)`. `axis = 0` sums down each column (returns a `1 × cols` row vector); `axis = 1` sums across each row (returns a `rows × 1` column vector).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column sums, `1` for row sums.

**Return Value**:  
A new vector of partial sums, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Row/column totals — e.g. summing feature columns or per-sample rows.

---

### `Matrix* matrix_prod_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Multiplies the matrix along one axis — the C analogue of NumPy's `numpy.prod(M, axis=...)`. `axis = 0` gives column products (`1 × cols`); `axis = 1` gives row products (`rows × 1`).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column products, `1` for row products.

**Return Value**:  
A new vector of partial products, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-row or per-column products (e.g. combining independent probabilities).

---

### `Matrix* matrix_mean_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Averages the matrix along one axis — the C analogue of NumPy's `numpy.mean(M, axis=...)`. `axis = 0` gives column means (`1 × cols`); `axis = 1` gives row means (`rows × 1`).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column means, `1` for row means.

**Return Value**:  
A new vector of partial means, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Feature-wise means for standardisation (subtract `matrix_mean_axis(X, 0)` from each row).

---

### `Matrix* matrix_min_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Reduces the matrix to its per-axis minimum — the C analogue of NumPy's `numpy.min(M, axis=...)`. `axis = 0` gives column minima (`1 × cols`); `axis = 1` gives row minima (`rows × 1`). Complements the scalar `matrix_min_element`.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column minima, `1` for row minima.

**Return Value**:  
A new vector of per-axis minima, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-feature minima for min-max scaling.

---

### `Matrix* matrix_max_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Reduces the matrix to its per-axis maximum — the C analogue of NumPy's `numpy.max(M, axis=...)`. `axis = 0` gives column maxima (`1 × cols`); `axis = 1` gives row maxima (`rows × 1`).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column maxima, `1` for row maxima.

**Return Value**:  
A new vector of per-axis maxima, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-feature maxima for min-max scaling, or per-row peak detection.

---

### `Matrix* matrix_argmax_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the per-axis index of the maximum — the C analogue of NumPy's `numpy.argmax(M, axis=...)`. `axis = 0` returns, for each column, the **row** index of its max (`1 × cols`); `axis = 1` returns, for each row, the **column** index of its max (`rows × 1`). Indices are stored as `double`; on ties the first occurrence wins.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column-wise (row) indices, `1` for row-wise (column) indices.

**Return Value**:  
A new vector of indices, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-sample classification (predicted label = `argmax` of each row of scores).

---

### `size_t matrix_count_nonzero(const Matrix* matrix)`

**Purpose**:  
Returns the number of non-zero elements — the C analogue of NumPy's `numpy.count_nonzero(M)`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The count of non-zero elements, or `0` if `matrix` is `NULL`/invalid. As in NumPy, `-0.0` counts as zero while `nan` and the infinities count as non-zero.

**Usage Case**:  
Measuring sparsity, counting active features, or validating how many entries survived a threshold/clip.

---

### `bool matrix_any(const Matrix* matrix)`

**Purpose**:  
Returns `true` if **any** element is non-zero — the C analogue of NumPy's `numpy.any(M)`. Any non-zero value (including `NaN`) counts as true.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
`true` if at least one element is non-zero; `false` if all are zero or `matrix` is `NULL`/invalid.

**Usage Case**:  
Short-circuit checks, e.g. `matrix_any(matrix_isnan(M))` tests whether `M` contains any NaN.

---

### `bool matrix_all(const Matrix* matrix)`

**Purpose**:  
Returns `true` if **all** elements are non-zero — the C analogue of NumPy's `numpy.all(M)`. Any non-zero value (including `NaN`) counts as true.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
`true` if every element is non-zero; `false` if any element is zero or `matrix` is `NULL`/invalid.

**Usage Case**:  
Validating that a whole mask is satisfied, e.g. `matrix_all(matrix_isfinite(M))` checks that `M` is entirely finite.

---

### `Matrix* matrix_cumsum(const Matrix* matrix)`

**Purpose**:  
Returns the cumulative sum of all elements in row-major (C) order — the C analogue of NumPy's `numpy.cumsum(M)` with the default `axis=None`. The matrix is flattened and the running total is returned as a `1 × (rows·cols)` row vector. For a per-axis cumulative sum use `matrix_cumsum_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
A new `1 × (rows·cols)` row vector of running totals, or `NULL` if `matrix` is `NULL`/invalid or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Prefix sums / running totals over a flattened sequence.

---

### `Matrix* matrix_cumsum_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the cumulative sum along one axis — the C analogue of NumPy's `numpy.cumsum(M, axis=...)`. The result has the **same shape** as the input: `axis = 0` accumulates down each column, `axis = 1` accumulates across each row.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` to accumulate down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape as `matrix`) of running totals, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-row or per-column running totals — e.g. cumulative distributions along an axis.

---

### `Matrix* matrix_cumprod(const Matrix* matrix)`

**Purpose**:  
Returns the cumulative product of all elements in row-major (C) order — the C analogue of NumPy's `numpy.cumprod(M)` with the default `axis=None`. The matrix is flattened and the running product is returned as a `1 × (rows·cols)` row vector. For a per-axis cumulative product use `matrix_cumprod_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
A new `1 × (rows·cols)` row vector of running products, or `NULL` if `matrix` is `NULL`/invalid or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Prefix products, compounding factors over a flattened sequence.

---

### `Matrix* matrix_cumprod_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the cumulative product along one axis — the C analogue of NumPy's `numpy.cumprod(M, axis=...)`. Same shape as the input: `axis = 0` accumulates down columns, `axis = 1` across rows.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` to accumulate down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape) of running products, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-row or per-column compounding.

---

### `Matrix* matrix_cummax(const Matrix* matrix)`

**Purpose**:  
Returns the running maximum of all elements in row-major (C) order — the C analogue of NumPy's `numpy.maximum.accumulate(M.flatten())`. The matrix is flattened and the running maximum is returned as a `1 × (rows·cols)` row vector. For a per-axis running maximum use `matrix_cummax_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
A new `1 × (rows·cols)` row vector of running maxima, or `NULL` if `matrix` is `NULL`/invalid or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Running peaks / high-water marks over a sequence (e.g. drawdown analysis).

---

### `Matrix* matrix_cummax_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the running maximum along one axis — the C analogue of NumPy's `numpy.maximum.accumulate(M, axis=...)`. Same shape as the input: `axis = 0` runs down columns, `axis = 1` across rows.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` to accumulate down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape) of running maxima, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-row or per-column running maxima.

---

### `Matrix* matrix_cummin(const Matrix* matrix)`

**Purpose**:  
Returns the running minimum of all elements in row-major (C) order — the C analogue of NumPy's `numpy.minimum.accumulate(M.flatten())`. The matrix is flattened and the running minimum is returned as a `1 × (rows·cols)` row vector. For a per-axis running minimum use `matrix_cummin_axis`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
A new `1 × (rows·cols)` row vector of running minima, or `NULL` if `matrix` is `NULL`/invalid or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Running troughs / low-water marks over a sequence.

---

### `Matrix* matrix_cummin_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the running minimum along one axis — the C analogue of NumPy's `numpy.minimum.accumulate(M, axis=...)`. Same shape as the input: `axis = 0` runs down columns, `axis = 1` across rows.

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` to accumulate down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape) of running minima, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-row or per-column running minima.

---

### `double matrix_var(const Matrix* matrix)`

**Purpose**:  
Returns the variance of all elements — the C analogue of NumPy's `numpy.var(M)`: the population variance with `ddof = 0`, i.e. `mean((x − mean(x))²)`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The population variance, or `0.0` if `matrix` is `NULL`/invalid.

**Usage Case**:  
Measuring spread; uses `ddof = 0` (divide by `N`) like NumPy. Matches the `statistics` library's `statistics_pvariance`.

---

### `double matrix_std(const Matrix* matrix)`

**Purpose**:  
Returns the standard deviation of all elements — the C analogue of NumPy's `numpy.std(M)`: the population standard deviation with `ddof = 0`, i.e. `sqrt(matrix_var(M))`.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The population standard deviation, or `0.0` if `matrix` is `NULL`/invalid.

**Usage Case**:  
Normalising/standardising data; matches the `statistics` library's `statistics_pstdev`.

---

### `double matrix_median(const Matrix* matrix)`

**Purpose**:  
Returns the median of all elements — the C analogue of NumPy's `numpy.median(M)`. The matrix is flattened and sorted; the middle value (or the average of the two middle values for an even count) is returned.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
The median of all elements, or `0.0` if `matrix` is `NULL`/invalid or a temporary allocation fails.

**Usage Case**:  
A robust centre estimate, less sensitive to outliers than the mean. Matches the `statistics` library's `statistics_median`.

---

### `Matrix* matrix_var_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the variance along one axis — the C analogue of NumPy's `numpy.var(M, axis=...)` with the default `ddof = 0` (population variance). `axis = 0` gives column variances (`1 × cols`); `axis = 1` gives row variances (`rows × 1`).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column variances, `1` for row variances.

**Return Value**:  
A new vector of per-axis variances, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-feature spread for standardisation or feature selection.

---

### `Matrix* matrix_std_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the standard deviation along one axis — the C analogue of NumPy's `numpy.std(M, axis=...)` with the default `ddof = 0`: the element-wise square root of `matrix_var_axis`. `axis = 0` gives column std deviations (`1 × cols`); `axis = 1` gives row std deviations (`rows × 1`).

**Parameters**:  
- `matrix`: Input matrix.
- `axis`: `0` for column std deviations, `1` for row std deviations.

**Return Value**:  
A new vector of per-axis standard deviations, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Per-feature std for z-score standardisation: `(X − mean_axis) / std_axis`.

---

### `double matrix_average(const Matrix* matrix, const Matrix* weights)`

**Purpose**:  
Returns the weighted average of all elements — the C analogue of NumPy's `numpy.average(M, weights=W)`: `sum(M · W) / sum(W)`. If `weights` is `NULL`, returns the unweighted mean (like `matrix_mean`).

**Parameters**:  
- `matrix`: Input matrix.
- `weights`: A weight matrix with the same dimensions as `matrix`, or `NULL` for an unweighted mean.

**Return Value**:  
The weighted average, or `0.0` if `matrix` is `NULL`/invalid or the weight dimensions do not match.

**Usage Case**:  
Weighted means — e.g. averaging with confidence/importance weights. If the weights sum to zero the result follows IEEE-754 (`±inf`/`nan`), as in NumPy.

---

### `double matrix_percentile(const Matrix* matrix, double p)`

**Purpose**:  
Computes the `p`-th percentile of the flattened data — the C analogue of NumPy's `numpy.percentile(M, p)` with the default `method='linear'`. The data is sorted and the value at rank `p/100 · (n−1)` is returned, linearly interpolating between the two surrounding samples.

**Parameters**:  
- `matrix`: Input matrix.
- `p`: The percentile in `[0, 100]`.

**Return Value**:  
The `p`-th percentile, `NaN` if `p` is outside `[0, 100]` (or is `NaN`), or `0.0` if `matrix` is `NULL`/invalid or a temporary allocation fails.

**Usage Case**:  
Robust statistics — medians (`p = 50`), quartiles, outlier thresholds (`p = 99`).

---

### `Matrix* matrix_standardize(const Matrix* matrix)`

**Purpose**:  
Standardises each column to zero mean and unit variance — the C analogue of `scipy.stats.zscore(M, axis=0)`. For every column, subtracts the column mean and divides by the column standard deviation (population, `ddof = 0`). The input is not modified.

**Parameters**:  
- `matrix`: Input matrix (not modified).

**Return Value**:  
A new standardised matrix (same shape), or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Feature scaling for machine learning (the StandardScaler transform). A constant column yields `nan`, matching SciPy's `zscore`.

---

### `Matrix* matrix_normalize(const Matrix* matrix)`

**Purpose**:  
Min-max scales each column to the range `[0, 1]` — the data-analysis scaler (`sklearn.preprocessing.MinMaxScaler`). For every column, maps the minimum to 0 and the maximum to 1 via `(x − min) / (max − min)`. The input is not modified.

**Parameters**:  
- `matrix`: Input matrix (not modified).

**Return Value**:  
A new scaled matrix (same shape), or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Bounding features into `[0, 1]` for ML or visualisation. A constant column (`max == min`) maps to all zeros, avoiding a division by zero (as scikit-learn does).

---

### `Matrix* matrix_rolling_mean(const Matrix* matrix, size_t window, int axis)`

**Purpose**:  
Computes the trailing moving average over a sliding window — the C analogue of pandas' `DataFrame.rolling(window).mean()`. Each output element is the mean of the `window` most recent values along the axis. `axis = 0` rolls down each column; `axis = 1` rolls across each row. The first `window − 1` positions along the axis are set to `NaN` (too few preceding values).

**Parameters**:  
- `matrix`: Input matrix (not modified).
- `window`: The window length (`> 0`, at most the axis length).
- `axis`: `0` to roll down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape) of moving averages with `NaN` in the warm-up region, or `NULL` if `matrix` is `NULL`, `window` is 0 or larger than the axis, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Smoothing time series, computing moving averages of price/signal data.

---

### `Matrix* matrix_pct_change(const Matrix* matrix, int axis)`

**Purpose**:  
Computes the fractional change between adjacent elements along an axis — the C analogue of pandas' `DataFrame.pct_change()`. Each element becomes `(current − previous) / previous`. `axis = 0` compares to the row above; `axis = 1` to the column to the left. The first position along the axis is `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `axis`: `0` to compare down columns, `1` across rows.

**Return Value**:  
A new matrix (same shape) of fractional changes, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Returns/growth rates of a time series. Division by a previous value of zero follows IEEE-754 (`±inf`/`nan`), as in pandas.

---

### `Matrix* matrix_fillna(const Matrix* matrix, double value)`

**Purpose**:  
Replaces every `NaN` with a constant — the C analogue of pandas' `DataFrame.fillna(value)`. Each `NaN` element becomes `value`; all others are copied unchanged. The input is not modified.

**Parameters**:  
- `matrix`: Source matrix (not modified).
- `value`: The value substituted for each `NaN`.

**Return Value**:  
A new matrix (same shape) with NaNs replaced, or `NULL` if `matrix` is `NULL` or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Cleaning missing data — e.g. filling the `NaN` warm-up from `matrix_pct_change` or `matrix_rolling_mean` with `0`.

---

### `Matrix* matrix_describe(const Matrix* matrix)`

**Purpose**:  
Produces a per-column summary of descriptive statistics — the C analogue of pandas' `DataFrame.describe()`. Returns an `8 × cols` matrix whose rows are, in order: **count, mean, std, min, 25%, 50% (median), 75%, max**.

**Parameters**:  
- `matrix`: Source matrix (rows = observations, columns = variables).

**Return Value**:  
A new `8 × cols` summary matrix, or `NULL` if `matrix` is `NULL`/invalid or a temporary allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Quick exploratory analysis of a dataset's columns. The std is the **sample** std (`ddof = 1`, divide by `n − 1`), matching pandas — distinct from `matrix_std` (population). Percentiles use linear interpolation; assumes finite data.

---

### `Matrix* matrix_cov(const Matrix* matrix)`

**Purpose**:  
Returns the covariance matrix of a set of observations — the C analogue of NumPy's `numpy.cov(M)` with the defaults `rowvar=True` and `bias=False`. Each **row** of `matrix` is a variable and each **column** an observation; for an `m × N` input it returns the `m × m` covariance matrix, normalised by `N − 1`.

**Parameters**:  
- `matrix`: Observation matrix (rows = variables, cols = observations). Needs at least 2 columns.

**Return Value**:  
A new `m × m` covariance matrix, or `NULL` if `matrix` is `NULL`, has fewer than 2 observations, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Measuring how variables vary together — a building block of PCA, Gaussian models, and Kalman filters. Uses `ddof = 1` (divide by `N−1`) like NumPy's default.

---

### `Matrix* matrix_corrcoef(const Matrix* matrix)`

**Purpose**:  
Returns the Pearson correlation-coefficient matrix — the C analogue of NumPy's `numpy.corrcoef(M)` (default `rowvar=True`). Each row is a variable; entry `[i][k]` is `cov[i][k] / sqrt(cov[i][i]·cov[k][k])`, i.e. the covariance normalised by the standard deviations. The diagonal is `1` and the result is clamped to `[-1, 1]`.

**Parameters**:  
- `matrix`: Observation matrix (rows = variables, cols = observations). Needs at least 2 columns.

**Return Value**:  
A new `m × m` correlation matrix, or `NULL` if `matrix` is `NULL`, has fewer than 2 observations, or allocation fails. Free it with `matrix_deallocate`.

**Usage Case**:  
Quantifying linear relationships between variables (feature selection, exploratory analysis). A zero-variance variable yields `nan` correlations, matching NumPy.

---

### `int matrix_rank(const Matrix* matrix)`

**Purpose**:  
Computes the rank (number of linearly independent rows or columns) like `numpy.linalg.matrix_rank`: the number of singular values above `σmax · max(rows, cols) · DBL_EPSILON`. Works for any shape.

**Parameters**:  
- `matrix`: Input matrix.

**Return Value**:  
Rank as a non-negative integer (at most `min(rows, cols)`), or `-1` on `NULL` input or failure.

**Usage Case**:  
Detect linear dependence, verify full-rank conditions before inversion or least-squares solving.

---

### `bool matrix_is_equal(const Matrix* matrix1, const Matrix* matrix2)`

**Purpose**:  
Returns `true` if both matrices have the same dimensions and identical elements (exact floating-point comparison).

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare.

**Return Value**:  
`true` if equal; `false` on dimension mismatch, element difference, or `NULL` input.

**Usage Case**:  
Post-operation correctness assertions, unit test verification.

---

### `bool matrix_allclose(const Matrix* matrix1, const Matrix* matrix2, double rtol, double atol)`

**Purpose**:  
Returns `true` if two matrices are element-wise equal **within a tolerance** — the C analogue of NumPy's `numpy.allclose(A, B, rtol, atol)`. Each pair must satisfy `|a − b| <= atol + rtol·|b|`. Unlike `matrix_is_equal` (exact), this absorbs floating-point rounding. NumPy's defaults are `rtol = 1e-5`, `atol = 1e-8`.

**Parameters**:  
- `matrix1`, `matrix2`: Matrices to compare (same dimensions).
- `rtol`: Relative tolerance.
- `atol`: Absolute tolerance.

**Return Value**:  
`true` if element-wise close; `false` if either is `NULL`, the dimensions differ, or any pair is out of tolerance.

**Usage Case**:  
Comparing computed results against expected values in tests, where exact equality is too strict. Matching NumPy's default (`equal_nan=False`), any `NaN` makes the result `false`; equal infinities of the same sign compare as close.

---

### `bool matrix_is_square(const Matrix* matrix)`

**Purpose**:  
Returns `true` when the matrix has the same number of rows and columns.

**Parameters**:  
- `matrix`: Matrix to check.

**Return Value**:  
`true` if square; `false` otherwise or on `NULL` input.

**Usage Case**:  
Guard before operations that require square matrices (determinant, inversion, eigenvalues).

---

### `bool matrix_is_row(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix has exactly one row.

**Parameters**:  
- `matrix`: Matrix to check.

**Return Value**:  
`true` for a 1×n matrix; `false` otherwise.

**Usage Case**:  
Validate that a matrix represents a row vector before dot-product or concatenation.

---

### `bool matrix_is_columnar(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix has exactly one column.

**Parameters**:  
- `matrix`: Matrix to check.

**Return Value**:  
`true` for an n×1 matrix; `false` otherwise.

**Usage Case**:  
Validate that a matrix represents a column vector before outer-product or norm computations.

---

### `bool matrix_is_identity(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix is an identity matrix (square, ones on main diagonal, zeros elsewhere).

**Parameters**:  
- `matrix`: Matrix to check.

**Return Value**:  
`true` if identity; `false` otherwise.

**Usage Case**:  
Verify that `A × A^{-1} = I` after inversion, check for neutral element in multiplication.

---

### `bool matrix_is_idempotent(const Matrix* matrix)`

**Purpose**:  
Returns `true` if `A² = A` (idempotent / projection matrix condition).

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if idempotent; `false` otherwise or if not square.

**Usage Case**:  
Verify projection matrices in regression analysis or QR-based projectors.

---

### `bool matrix_is_symmetric(const Matrix* matrix)`

**Purpose**:  
Returns `true` if `A[i][j] == A[j][i]` for all `i, j` (exact equality). The matrix must be square.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if symmetric; `false` otherwise.

**Usage Case**:  
Pre-condition check before Cholesky decomposition, covariance matrix validation.

---

### `bool matrix_is_skew_symmetric(const Matrix* matrix)`

**Purpose**:  
Returns `true` if `A[i][j] == -A[j][i]` for all `i, j` (and diagonal elements are zero). Matrix must be square.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if skew-symmetric; `false` otherwise.

**Usage Case**:  
Validate cross-product matrices, antisymmetric operator checks.

---

### `bool matrix_is_diagonal(const Matrix* matrix)`

**Purpose**:  
Returns `true` if all off-diagonal elements are zero.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if diagonal; `false` otherwise.

**Usage Case**:  
Fast-path detection for diagonal matrices where multiplication is O(n) instead of O(n³).

---

### `bool matrix_is_upper_triangular(const Matrix* matrix)`

**Purpose**:  
Returns `true` if all elements below the main diagonal are zero.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if upper triangular; `false` otherwise.

**Usage Case**:  
Validate the U factor after LU/QR decomposition; enable efficient back-substitution.

---

### `bool matrix_is_lower_triangular(const Matrix* matrix)`

**Purpose**:  
Returns `true` if all elements above the main diagonal are zero.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if lower triangular; `false` otherwise.

**Usage Case**:  
Validate the L factor after LU decomposition; enable efficient forward-substitution.

---

### `bool matrix_is_orthogonal(const Matrix* matrix)`

**Purpose**:  
Returns `true` if `A × A^T = I` (columns are orthonormal). Uses exact element equality — floating-point computed matrices may not pass.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if orthogonal; `false` otherwise.

**Usage Case**:  
Validate rotation matrices, the Q factor in QR decomposition.

---

### `bool matrix_is_hankel(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix is a Hankel matrix (every element on each anti-diagonal is equal, i.e. `a[i][j] == a[i-1][j+1]` everywhere).

**Parameters**:  
- `matrix`: Matrix to check (any shape).

**Return Value**:  
`true` if Hankel; `false` otherwise.

**Usage Case**:  
Verify that a matrix produced by `matrix_hankel` was stored correctly, or check a data matrix for Hankel structure.

---

### `bool matrix_is_toeplitz(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix is a Toeplitz matrix (every element on each diagonal is equal).

**Parameters**:  
- `matrix`: Matrix to check (need not be square).

**Return Value**:  
`true` if Toeplitz; `false` otherwise.

**Usage Case**:  
Detect convolution structure in a matrix, validate output of `matrix_toeplitz`.

---

### `bool matrix_is_positive_definite(const Matrix* matrix)`

**Purpose**:  
Returns `true` if the matrix is symmetric and positive definite. Uses Cholesky decomposition as the positive-definiteness test.

**Parameters**:  
- `matrix`: Square matrix to check.

**Return Value**:  
`true` if positive definite; `false` otherwise (or if not symmetric / not square).

**Usage Case**:  
Pre-condition check before Cholesky factorisation, covariance matrix validation, convexity checks.

---

### `bool matrix_is_sparse(const Matrix* matrix)`

**Purpose**:  
Returns `true` if fewer than 30% of the elements are non-zero, i.e. more than 70% are exactly zero (a simple density-based sparsity heuristic).

**Parameters**:  
- `matrix`: Matrix to check.

**Return Value**:  
`true` if the non-zero ratio is below 0.3; `false` otherwise (or for `NULL` / empty input).

**Usage Case**:  
Choose between dense and sparse algorithms at runtime; warn when dense storage is wasteful.

---

### `Matrix* matrix_argmin_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the per-axis index of the minimum element — the C analogue of NumPy's `numpy.argmin(M, axis=...)`. With `axis = 0` it returns, for each column, the **row** index of that column's smallest value (a `1 × cols` vector); with `axis = 1` it returns, for each row, the **column** index of that row's smallest value (a `rows × 1` vector). On ties the first occurrence wins. It is the minimum-seeking twin of `matrix_argmax_axis`.

**Parameters**:  
- `matrix`: The matrix to search.  
- `axis`: `0` for column-wise (row) indices, `1` for row-wise (column) indices.

**Return Value**:  
A newly allocated vector of indices (stored as `double`), or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Locating the cheapest option per scenario, the closest cluster centre per point, or the best-scoring class per row of a score matrix.

---

### `Matrix* matrix_ptp_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the per-axis peak-to-peak range (`max − min`) — the C analogue of NumPy's `numpy.ptp(M, axis=...)`. With `axis = 0` it gives the spread of each column (`1 × cols`); with `axis = 1` the spread of each row (`rows × 1`).

**Parameters**:  
- `matrix`: The matrix to reduce.  
- `axis`: `0` for column ranges, `1` for row ranges.

**Return Value**:  
A newly allocated vector of per-axis ranges, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Quick dispersion checks per feature or per sample — feature-scaling diagnostics, signal dynamic range, outlier-band width.

---

### `Matrix* matrix_median_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Returns the per-axis median — the C analogue of NumPy's `numpy.median(M, axis=...)`. With `axis = 0` it gives the median of each column (`1 × cols`); with `axis = 1` the median of each row (`rows × 1`). For an even count it averages the two middle values. The input is not modified.

**Parameters**:  
- `matrix`: The matrix to reduce.  
- `axis`: `0` for column medians, `1` for row medians.

**Return Value**:  
A newly allocated vector of per-axis medians, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Robust per-feature or per-row central tendency that resists outliers — the workhorse summary for skewed data.

---

### `Matrix* matrix_count_nonzero_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Counts the non-zero elements along an axis — the C analogue of NumPy's `numpy.count_nonzero(M, axis=...)`. With `axis = 0` it counts down each column (`1 × cols`); with `axis = 1` across each row (`rows × 1`). As in NumPy, `-0.0` counts as zero while `nan`/`inf` count as non-zero.

**Parameters**:  
- `matrix`: The matrix to inspect.  
- `axis`: `0` for column counts, `1` for row counts.

**Return Value**:  
A newly allocated vector of counts (as `double`), or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Measuring per-feature sparsity, counting active entries per row, or finding how many non-missing observations each column carries.

---

### `Matrix* matrix_nonzero(const Matrix* matrix)`

**Purpose**:  
Returns the `(row, column)` indices of every non-zero element — the C analogue of NumPy's `numpy.nonzero(M)`. The matrix is scanned in row-major (C) order and the result is a `k × 2` matrix whose rows are the index pairs of the `k` non-zero elements.

**Parameters**:  
- `matrix`: The matrix to inspect.

**Return Value**:  
A newly allocated `k × 2` matrix of index pairs, or `NULL` if `matrix` is `NULL`, contains no non-zero elements, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Extracting the coordinates of active cells — converting a dense mask to a coordinate list, finding where a condition holds, or building a sparse representation.

---

### `Matrix* matrix_flip(const Matrix* matrix, int axis)`

**Purpose**:  
Reverses the order of elements along one axis — the C analogue of NumPy's `numpy.flip(M, axis=...)`. With `axis = 0` it reverses the row order (like `matrix_flipud`); with `axis = 1` it reverses the column order (like `matrix_fliplr`). The single runtime-selectable axis generalises the fixed `flipud`/`fliplr` pair. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `axis`: `0` to reverse rows, `1` to reverse columns.

**Return Value**:  
A newly allocated flipped matrix of the same shape, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Mirroring an image, reversing a time series, or flipping a convolution kernel — with the axis chosen at runtime.

---

### `Matrix* matrix_take(const Matrix* matrix, const Matrix* indices)`

**Purpose**:  
Gathers elements by a list of flattened indices — the C analogue of NumPy's `numpy.take(M, indices)`. The source matrix is flattened in row-major order and the elements at the given indices are gathered, producing a result with the **same shape as `indices`**. Negative indices count from the end (like NumPy/Python). Neither input is modified.

**Parameters**:  
- `matrix`: The source matrix to gather from.  
- `indices`: The flat indices to gather (any shape; integer-valued doubles, negatives allowed).

**Return Value**:  
A newly allocated matrix shaped like `indices`, or `NULL` if either input is `NULL`/empty, any index is out of range, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Fancy-indexing a lookup table, reordering or sampling elements, or applying a permutation produced by `matrix_argsort`.

---

### `Matrix* matrix_delete(const Matrix* matrix, size_t index, int axis)`

**Purpose**:  
Removes a single row or column — the C analogue of NumPy's `numpy.delete(M, index, axis=...)`. With `axis = 0` it removes row `index` (result `(rows−1) × cols`); with `axis = 1` it removes column `index` (result `rows × (cols−1)`). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `index`: The 0-based row or column index to remove.  
- `axis`: `0` to remove a row, `1` to remove a column.

**Return Value**:  
A newly allocated matrix with the row/column removed, or `NULL` if `matrix` is `NULL`, `axis` is invalid, `index` is out of range, removing would leave an empty matrix, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Dropping an unwanted feature column, removing a bad sample row, or excluding a pivot during an algorithm step.

---

### `Matrix* matrix_insert(const Matrix* matrix, size_t index, const Matrix* values, int axis)`

**Purpose**:  
Inserts a new row or column at a given position — the C analogue of NumPy's `numpy.insert(M, index, values, axis=...)`. With `axis = 0` it inserts `values` (a length-`cols` vector) as a new row *before* row `index` (result `(rows+1) × cols`); with `axis = 1` it inserts `values` (a length-`rows` vector) as a new column *before* column `index` (result `rows × (cols+1)`). `index` may equal the axis length to append at the end. Neither input is modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `index`: The 0-based position before which to insert (`0..axis-length`).  
- `values`: The vector to insert; its length must match the other dimension.  
- `axis`: `0` to insert a row, `1` to insert a column.

**Return Value**:  
A newly allocated matrix with the row/column inserted, or `NULL` if either input is `NULL`, `axis` is invalid, `index` is out of range, `values` has the wrong length, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Splicing a header row into data, adding a bias column to a design matrix, or injecting a computed feature at a specific position.

---

### `Matrix* matrix_append(const Matrix* matrix, const Matrix* values, int axis)`

**Purpose**:  
Appends rows or columns to the end of a matrix — the C analogue of NumPy's `numpy.append(M, values, axis=...)`. With `axis = 0` it appends `values` (which must have `cols` columns) below `M`; with `axis = 1` it appends `values` (which must have `rows` rows) to the right of `M`. It is the convenience equivalent of `matrix_vstack` / `matrix_hstack`. Neither input is modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `values`: The matrix to append (matching dimensions on the other axis).  
- `axis`: `0` to append rows, `1` to append columns.

**Return Value**:  
A newly allocated combined matrix, or `NULL` if either input is `NULL`, `axis` is invalid, the dimensions are incompatible, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Growing a dataset with new observations, concatenating a block of extra features, or accumulating results row by row.

---

### `Matrix* matrix_column_stack(const Matrix* a, const Matrix* b)`

**Purpose**:  
Stacks two 1-D vectors as the columns of a 2-D matrix — the C analogue of NumPy's `numpy.column_stack((a, b))` for 1-D inputs. Given two length-`n` vectors, it returns an `n × 2` matrix whose first column is `a` and second column is `b`. Neither input is modified.

**Parameters**:  
- `a`: The first vector (any shape; flattened).  
- `b`: The second vector, the same length as `a`.

**Return Value**:  
A newly allocated `n × 2` matrix, or `NULL` if either input is `NULL`, the lengths differ, a vector is empty, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Pairing x/y coordinates into a point list, joining a feature and its label into a two-column table, or assembling `(key, value)` columns.

---

### `Matrix* matrix_digitize(const Matrix* x, const Matrix* bins)`

**Purpose**:  
Returns the index of the bin to which each value belongs — the C analogue of NumPy's `numpy.digitize(x, bins)` with the default `right=False` and monotonically increasing `bins`. Each result element is the number of bin edges `≤ x` — i.e. the index `i` such that `bins[i-1] ≤ x < bins[i]`, with `0` for values below all edges and `len(bins)` for values at or above the last edge. The result has the same shape as `x`. Neither input is modified.

**Parameters**:  
- `x`: The values to bin (any shape).  
- `bins`: The sorted (ascending) bin edges (any shape; flattened).

**Return Value**:  
A newly allocated matrix shaped like `x` holding the bin indices (as `double`), or `NULL` if either input is `NULL`/empty or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Bucketing continuous values into categories — quantile/grade boundaries, colour-mapping a heat field, or discretising a feature before counting.

---

### `Matrix* matrix_bincount(const Matrix* x)`

**Purpose**:  
Counts occurrences of each non-negative integer — the C analogue of NumPy's `numpy.bincount(x)`. It treats the (flattened) elements of `x` as non-negative integers and returns a `1 × (max+1)` row vector whose `i`-th entry is the number of times `i` appears (so absent values in the range produce explicit zeros). The input is not modified.

**Parameters**:  
- `x`: The non-negative integer-valued matrix.

**Return Value**:  
A newly allocated `1 × (max+1)` row vector of counts, or `NULL` if `x` is `NULL`, empty, contains a negative value, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Tallying label frequencies, building a histogram of small integer codes, or counting votes per category.

---

### `bool matrix_histogram(const Matrix* x, size_t bins, Matrix** hist, Matrix** edges)`

**Purpose**:  
Computes the histogram of a set of values — the C analogue of NumPy's `numpy.histogram(x, bins)`. It divides the range `[min(x), max(x)]` into `bins` equal-width buckets, counts how many values fall in each, and reports both the counts and the bin edges. The rightmost edge is inclusive, so values equal to `max(x)` land in the last bin (matching NumPy). The input is not modified.

**Parameters**:  
- `x`: The values to histogram (any shape; flattened).  
- `bins`: The number of equal-width bins (must be greater than 0).  
- `hist`: Out-parameter receiving a `1 × bins` row vector of counts.  
- `edges`: Out-parameter receiving a `1 × (bins+1)` row vector of bin edges.

**Return Value**:  
`true` on success; `false` if `x` is `NULL`/empty, `bins` is 0, an out-pointer is `NULL`, or allocation fails. On success the caller owns `*hist` and `*edges` and must release each with `matrix_deallocate`.

**Usage Case**:  
Summarising a distribution for plotting or analysis — pixel-intensity histograms, error-magnitude distributions, or empirical density estimates.

---

### `Matrix* matrix_sinc(const Matrix* matrix)`

**Purpose**:  
Computes the normalized sinc of every element — the C analogue of NumPy's `numpy.sinc(x)`: `sin(π·x) / (π·x)`, with the removable singularity filled exactly as `sinc(0) = 1`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix (same shape) of sinc values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The impulse response of an ideal low-pass filter and the heart of Whittaker–Shannon interpolation — signal reconstruction, windowing, and resampling.

---

### `Matrix* matrix_around(const Matrix* matrix, int decimals)`

**Purpose**:  
Rounds every element to a given number of decimal places — the C analogue of NumPy's `numpy.around(x, decimals)`. It scales by `10**decimals`, rounds to the nearest integer using round-half-to-even (banker's rounding), then unscales, exactly matching NumPy (so a value that lands on `.5` after scaling rounds to the nearest even). This differs from `matrix_round`, which rounds halves away from zero at zero decimals. Negative `decimals` round to the left of the decimal point (tens, hundreds). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `decimals`: The number of decimal places to round to (may be negative).

**Return Value**:  
A newly allocated matrix (same shape) of rounded values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Presenting tidy figures in reports, quantising values to a grid, or comparing results at a fixed precision.

---

### `Matrix* matrix_exprel(const Matrix* matrix)`

**Purpose**:  
Computes the relative exponential of every element — the C analogue of SciPy's `scipy.special.exprel(x)`: `(exp(x) − 1) / x`, with the removable singularity filled exactly as `exprel(0) = 1`. It is built on `expm1` so the result stays accurate for `x` near zero (where the naive formula loses precision to catastrophic cancellation). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix (same shape) of exprel values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Stable evaluation of finance/decay formulas like `(e^x − 1)/x`, continuous-compounding factors, and series where the `x → 0` limit must remain well-behaved.

---

### `Matrix* matrix_gcd(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise greatest common divisor — the C analogue of NumPy's `numpy.gcd(a, b)`. Each element is treated as an integer and the result is the non-negative GCD of the two magnitudes (`gcd(0, 0) == 0`). Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first integer-valued matrix.  
- `b`: The second integer-valued matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of GCDs, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Reducing fractions to lowest terms, finding common factors across integer feature columns, or number-theory utilities.

---

### `Matrix* matrix_lcm(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise least common multiple — the C analogue of NumPy's `numpy.lcm(a, b)`. Each element is treated as an integer and the result is the non-negative LCM of the two magnitudes, with `lcm(0, x) == 0`. It is computed as `|a / gcd(a, b) · b|` to avoid intermediate overflow. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first integer-valued matrix.  
- `b`: The second integer-valued matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of LCMs, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Finding a common period or denominator — scheduling cycles, aligning sample rates, or combining fractions.

---

### `Matrix* matrix_logaddexp(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise log of summed exponentials — the C analogue of NumPy's `numpy.logaddexp(a, b)`: `log(exp(a) + exp(b))` evaluated in the numerically stable form `max + log1p(exp(min − max))`, which avoids overflow when the inputs are large. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of results, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Accumulating probabilities in log space — the staple of log-likelihood sums, softmax denominators, and the forward algorithm in HMMs, where direct exponentiation would overflow.

---

### `Matrix* matrix_fmax(const Matrix* a, const Matrix* b)`

**Purpose**:  
Element-wise NaN-ignoring maximum — the C analogue of NumPy's `numpy.fmax(a, b)`. It returns the larger of each pair, but when exactly one operand is NaN it returns the other (non-NaN) operand; the result is NaN only where both are NaN. This is unlike `matrix_maximum`, which propagates NaN. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of element-wise maxima, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Combining two measurement series while tolerating missing values — taking the better of two sensors where gaps are encoded as NaN.

---

### `Matrix* matrix_fmin(const Matrix* a, const Matrix* b)`

**Purpose**:  
Element-wise NaN-ignoring minimum — the C analogue of NumPy's `numpy.fmin(a, b)`. It returns the smaller of each pair, but when exactly one operand is NaN it returns the other (non-NaN) operand; the result is NaN only where both are NaN. This is unlike `matrix_minimum`, which propagates NaN. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of element-wise minima, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Combining two series while tolerating missing values — taking the lower of two bounds where gaps are encoded as NaN.

---

### `bool matrix_modf(const Matrix* matrix, Matrix** frac_part, Matrix** int_part)`

**Purpose**:  
Splits every element into its fractional and integer parts — the C analogue of NumPy's `numpy.modf(x)`. Each element `x` is decomposed so that `frac + int == x`, with both parts carrying the sign of `x` (e.g. `-2.5 → frac -0.5, int -2.0`). The two results have the same shape as the input, which is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `frac_part`: Out-parameter receiving the fractional parts.  
- `int_part`: Out-parameter receiving the (truncated-toward-zero) integer parts, as real values.

**Return Value**:  
`true` on success; `false` if `matrix` is `NULL`, an out-pointer is `NULL`, or allocation fails. On success the caller owns `*frac_part` and `*int_part` and must release each with `matrix_deallocate`.

**Usage Case**:  
Separating whole units from remainders — splitting a timestamp into seconds and sub-seconds, or pixels into integer and sub-pixel offsets.

---

### `bool matrix_divmod(const Matrix* a, const Matrix* b, Matrix** quotient, Matrix** remainder)`

**Purpose**:  
Computes the simultaneous element-wise quotient and remainder — the C analogue of NumPy's `numpy.divmod(a, b)`. For each pair it returns the floor-division quotient `floor(a / b)` and the remainder `a − quotient · b` (which carries the sign of the divisor, like `numpy.mod`). Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The dividend matrix.  
- `b`: The divisor matrix, the same shape as `a`.  
- `quotient`: Out-parameter receiving the floor-division quotients.  
- `remainder`: Out-parameter receiving the remainders.

**Return Value**:  
`true` on success; `false` if either input is `NULL`, the shapes differ, an out-pointer is `NULL`, or allocation fails. On success the caller owns `*quotient` and `*remainder` and must release each with `matrix_deallocate`.

**Usage Case**:  
Getting both halves of a division in one pass — converting a flat index to `(row, col)`, splitting totals into whole groups plus leftovers, or clock arithmetic.

---

### `Matrix* matrix_floor_divide(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise floor of the division — the C analogue of NumPy's `numpy.floor_divide(a, b)`: `floor(a / b)`. The result follows NumPy's sign and division-by-zero semantics exactly (e.g. `floor_divide(-7, 3) == -3`, and dividing a finite value by zero yields `+inf`, `-inf`, or `nan` just as `floor(a / b)` does). Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The dividend matrix.  
- `b`: The divisor matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of floor-divided values, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Integer-style bucketing of continuous data — mapping values to grid cells, computing page numbers from offsets, or any "how many whole `b`s fit in `a`" question.

---

### `Matrix* matrix_fix(const Matrix* matrix)`

**Purpose**:  
Rounds every element toward zero to the nearest integer — the C analogue of NumPy's `numpy.fix(x)` (floor for positive values, ceil for negative ones). This is behaviourally identical to `matrix_trunc`; it exists to mirror the NumPy name. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix (same shape) of truncated values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Discarding the fractional part symmetrically about zero — converting measurements to whole units without the directional bias of `floor`/`ceil`.

---

### `Matrix* matrix_rint(const Matrix* matrix)`

**Purpose**:  
Rounds every element to the nearest integer using round-half-to-even (banker's rounding) — the C analogue of NumPy's `numpy.rint(x)`, so e.g. `0.5 → 0`, `1.5 → 2`, `2.5 → 2`. This differs from `matrix_round`, which rounds halves away from zero. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix (same shape) of rounded values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Statistically unbiased rounding — quantising large batches of values where the systematic drift of always-round-half-up would accumulate.

---

### `Matrix* matrix_nan_to_num(const Matrix* matrix)`

**Purpose**:  
Replaces NaN and infinities with finite numbers — the C analogue of NumPy's `numpy.nan_to_num(x)` with default arguments: `NaN` becomes `0.0`, `+inf` becomes the largest finite double (`DBL_MAX`), and `-inf` becomes `-DBL_MAX`. Finite values pass through unchanged. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated finite matrix (same shape), or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Sanitising data before an algorithm that cannot tolerate non-finite values — clearing NaNs out of a feature matrix or capping overflowed intermediates.

---

### `Matrix* matrix_float_power(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise power in floating point — the C analogue of NumPy's `numpy.float_power(a, b)`: `a ** b` with the usual `pow` edge cases (e.g. a negative base with a non-integer exponent yields `nan`). Because this library is already double-valued, it is equivalent to `matrix_pow`; it exists to mirror the NumPy name. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The base matrix.  
- `b`: The exponent matrix, the same shape as `a`.

**Return Value**:  
A newly allocated matrix (same shape) of powers, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Element-wise exponentiation where integer overflow must never occur — applying per-element exponents in floating point with predictable IEEE edge behaviour.

---

### `Matrix* matrix_expit(const Matrix* matrix)`

**Purpose**:  
Computes the element-wise logistic sigmoid — the C analogue of SciPy's `scipy.special.expit(x)`: `1 / (1 + exp(-x))`, mapping the real line onto the open interval `(0, 1)`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix (same shape) of sigmoid values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The activation function at the heart of logistic regression and neural networks — squashing scores into probabilities.

---

### `Matrix* matrix_logit(const Matrix* matrix)`

**Purpose**:  
Computes the element-wise logit, the inverse sigmoid — the C analogue of SciPy's `scipy.special.logit(p)`: `log(p / (1 - p))`. As in SciPy, `p == 0` yields `-inf`, `p == 1` yields `+inf`, and `p` outside `[0, 1]` yields `nan`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix of probabilities.

**Return Value**:  
A newly allocated matrix (same shape) of logit values, or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Mapping probabilities back to the unbounded log-odds scale — the link function for logistic models and a building block for calibration.

---

### `Matrix* matrix_xlogy(const Matrix* x, const Matrix* y)`

**Purpose**:  
Computes the element-wise `x · log(y)` with the `x == 0` convention — the C analogue of SciPy's `scipy.special.xlogy(x, y)`. It is defined to be exactly `0` wherever `x == 0` (even if `y` is `0` or negative), which avoids the `0 · (-inf) = nan` that the naive product would produce. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `x`: The multiplier matrix.  
- `y`: The matrix whose logarithm is taken, the same shape as `x`.

**Return Value**:  
A newly allocated matrix (same shape) of results, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The safe term in entropy, cross-entropy, and Kullback–Leibler computations, where `0 · log(0)` must contribute `0` rather than `NaN`.

---

### `Matrix* matrix_heaviside(const Matrix* x, const Matrix* h0)`

**Purpose**:  
Computes the element-wise Heaviside step function — the C analogue of NumPy's `numpy.heaviside(x, h0)`: `0` where `x < 0`, `1` where `x > 0`, and the corresponding element of `h0` where `x == 0` (`NaN` inputs propagate to `NaN`). The two inputs must have the same shape and neither is modified.

**Parameters**:  
- `x`: The step argument matrix.  
- `h0`: The values to use where `x == 0`, the same shape as `x`.

**Return Value**:  
A newly allocated matrix (same shape) of step values, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Building switching/indicator signals — gating a response on a threshold crossing, with explicit control of the value exactly at the boundary.

---

### `Matrix* matrix_angle(const Matrix* real, const Matrix* imag)`

**Purpose**:  
Computes the element-wise phase angle from real and imaginary parts — the C analogue of NumPy's `numpy.angle(z)`. Given the real and imaginary parts of a complex value as two separate matrices, it returns `atan2(imag, real)`, the argument (phase) in radians in the range `(-π, π]`. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `real`: The real-part matrix.  
- `imag`: The imaginary-part matrix, the same shape as `real`.

**Return Value**:  
A newly allocated matrix (same shape) of phase angles, or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Recovering the phase of complex data held as separate real/imag matrices — spectra from `matrix_fft`-style output, or signal phase analysis.

---

### `Matrix* matrix_not_equal(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise inequality mask — the C analogue of NumPy's `numpy.not_equal(a, b)`: `1.0` wherever the corresponding elements differ and `0.0` where they are equal. Following IEEE semantics, any comparison involving `NaN` is unequal, so positions where either operand is `NaN` (including `NaN != NaN`) yield `1.0`. This is the elementwise complement of `matrix_equal_elementwise` and is distinct from `matrix_is_equal`, which returns a single bool. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated 0/1 mask matrix (same shape), or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Locating where two matrices disagree — diffing predictions against labels, or building a change mask between two snapshots.

---

### `Matrix* matrix_equal_elementwise(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise equality mask — the C analogue of NumPy's `numpy.equal(a, b)`: `1.0` wherever the corresponding elements are equal and `0.0` where they differ. Following IEEE semantics, any comparison involving `NaN` is unequal, so positions where either operand is `NaN` (including `NaN == NaN`) yield `0.0`. This is the element-wise complement of `matrix_not_equal` and is distinct from `matrix_is_equal`, which returns a single bool. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated 0/1 mask matrix (same shape), or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Building an agreement mask between two matrices — counting matching cells, or gating an update to only the positions that are already equal.

---

### `Matrix* matrix_logical_xor(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the element-wise logical XOR mask — the C analogue of NumPy's `numpy.logical_xor(a, b)`. Each element is treated as a boolean (non-zero is true, so `NaN` is true) and the result is `1.0` where exactly one operand is true and `0.0` otherwise. This completes the boolean set alongside `matrix_logical_and`, `matrix_logical_or`, and `matrix_logical_not`. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix, the same shape as `a`.

**Return Value**:  
A newly allocated 0/1 mask matrix (same shape), or `NULL` if either input is `NULL`, the shapes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Detecting where exactly one of two conditions holds — symmetric-difference masks, parity checks, or toggling logic.

---

### `Matrix* matrix_isposinf(const Matrix* matrix)`

**Purpose**:  
Tests element-wise for positive infinity — the C analogue of NumPy's `numpy.isposinf(x)`: `1.0` where an element is `+inf` and `0.0` everywhere else (finite values, `-inf`, and `NaN` all map to `0.0`). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated 0/1 mask matrix (same shape), or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Pinpointing positive overflow — flagging cells that blew up to `+inf` before cleaning them with `matrix_nan_to_num`.

---

### `Matrix* matrix_isneginf(const Matrix* matrix)`

**Purpose**:  
Tests element-wise for negative infinity — the C analogue of NumPy's `numpy.isneginf(x)`: `1.0` where an element is `-inf` and `0.0` everywhere else (finite values, `+inf`, and `NaN` all map to `0.0`). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated 0/1 mask matrix (same shape), or `NULL` if `matrix` is `NULL` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Pinpointing negative overflow — flagging cells that fell to `-inf`, e.g. `log(0)` results.

---

### `bool matrix_array_equal(const Matrix* a, const Matrix* b)`

**Purpose**:  
Tests whether two matrices are exactly equal — the C analogue of NumPy's `numpy.array_equal(a, b)` (default `equal_nan=False`): returns `true` only if both matrices have the same shape and every corresponding pair of elements is exactly equal. Because `NaN` is never equal to itself, any matrix containing `NaN` is unequal even to itself. This is behaviourally identical to `matrix_is_equal`; it exists to mirror the NumPy name.

**Parameters**:  
- `a`: The first matrix.  
- `b`: The second matrix.

**Return Value**:  
`true` if the matrices have identical shape and elements; `false` otherwise (including when either is `NULL`).

**Usage Case**:  
A single yes/no equality test for whole matrices — verifying that a computation reproduced an expected result.

---

### `double matrix_nansum(const Matrix* matrix)`

**Purpose**:  
Sums all elements, treating NaN as zero — the C analogue of NumPy's `numpy.nansum(x)`. It adds every element but skips `NaN`s (counting them as `0`); an empty or all-`NaN` matrix sums to `0.0`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The NaN-ignoring sum, or `0.0` if `matrix` is `NULL`/empty.

**Usage Case**:  
Totalling a dataset that contains missing values without first imputing them.

---

### `double matrix_nanmean(const Matrix* matrix)`

**Purpose**:  
Computes the mean over the non-NaN elements — the C analogue of NumPy's `numpy.nanmean(x)`: the sum of the non-`NaN` elements divided by their count. If every element is `NaN` (or the matrix is empty), the result is `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The NaN-ignoring mean, or `NaN` if there are no non-NaN elements.

**Usage Case**:  
Averaging measurements with gaps — the workhorse summary statistic for real-world data with missing entries.

---

### `double matrix_nanmin(const Matrix* matrix)`

**Purpose**:  
Finds the minimum over the non-NaN elements — the C analogue of NumPy's `numpy.nanmin(x)`: the smallest element, ignoring `NaN`s. If every element is `NaN` (or the matrix is empty), the result is `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The NaN-ignoring minimum, or `NaN` if there are no non-NaN elements.

**Usage Case**:  
Finding the floor of a noisy series that has missing readings.

---

### `double matrix_nanmax(const Matrix* matrix)`

**Purpose**:  
Finds the maximum over the non-NaN elements — the C analogue of NumPy's `numpy.nanmax(x)`: the largest element, ignoring `NaN`s. If every element is `NaN` (or the matrix is empty), the result is `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The NaN-ignoring maximum, or `NaN` if there are no non-NaN elements.

**Usage Case**:  
Finding the peak of a series that has missing readings.

---

### `double matrix_nanprod(const Matrix* matrix)`

**Purpose**:  
Multiplies all elements, treating NaN as one — the C analogue of NumPy's `numpy.nanprod(x)`. It multiplies every element but skips `NaN`s (counting them as `1`); an empty or all-`NaN` matrix yields `1.0`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The NaN-ignoring product, or `1.0` if `matrix` is `NULL`/empty.

**Usage Case**:  
Computing a product over data with missing factors — e.g. compounding growth rates where some periods are unrecorded.

---

### `Matrix* matrix_arange(double start, double stop, double step)`

**Purpose**:  
Builds a row vector of evenly spaced values over a half-open interval — the C analogue of NumPy's `numpy.arange(start, stop, step)`: a `1 × N` row vector starting at `start` and incrementing by `step`, stopping *before* `stop` (the endpoint is excluded). The length is `ceil((stop − start) / step)`, and element `i` is `start + i · step`.

**Parameters**:  
- `start`: The first value.  
- `stop`: The exclusive upper (or lower, for negative `step`) bound.  
- `step`: The spacing between values; must be non-zero.

**Return Value**:  
A newly allocated `1 × N` row vector, or `NULL` if `step` is `0` or the range is empty (no values fit). The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Generating index ranges and sample grids — the go-to way to fabricate an axis of evenly spaced values.

---

### `Matrix* matrix_linspace(double start, double stop, size_t num)`

**Purpose**:  
Builds a row vector of evenly spaced samples over a closed interval — the C analogue of NumPy's `numpy.linspace(start, stop, num)` with the default `endpoint=True`: a `1 × num` row vector of `num` samples spaced evenly over `[start, stop]`. The spacing is `(stop − start) / (num − 1)`; the first element is exactly `start` and the last is pinned exactly to `stop`. For `num == 1` the single element is `start`.

**Parameters**:  
- `start`: The first value (inclusive).  
- `stop`: The last value (inclusive).  
- `num`: The number of samples to generate; must be greater than 0.

**Return Value**:  
A newly allocated `1 × num` row vector, or `NULL` if `num` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Sampling a function over a fixed range — plotting axes, interpolation grids, and parameter sweeps where the endpoint must be hit exactly.

---

### `Matrix* matrix_ediff1d(const Matrix* matrix)`

**Purpose**:  
Computes the differences between consecutive flattened elements — the C analogue of NumPy's `numpy.ediff1d(x)`. It flattens the matrix in row-major order and returns a `1 × (N−1)` row vector whose `i`-th entry is `x[i+1] − x[i]`. This is the flat-vector counterpart of `matrix_diff` (which works along an axis). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix (must hold at least 2 elements).

**Return Value**:  
A newly allocated `1 × (N−1)` row vector of consecutive differences, or `NULL` if `matrix` is `NULL`, holds fewer than 2 elements, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Measuring step-to-step change in a flattened series — spacing between sorted values, inter-arrival gaps, or first differences of a signal.

---

### `Matrix* matrix_flatnonzero(const Matrix* matrix)`

**Purpose**:  
Returns the flattened indices of the non-zero elements — the C analogue of NumPy's `numpy.flatnonzero(x)`. It scans the matrix in row-major (C) order and returns a `1 × K` row vector of the flat indices of the `K` non-zero elements. This is the flat-index counterpart of `matrix_nonzero` (which returns `(row, col)` pairs). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated `1 × K` row vector of flat indices, or `NULL` if `matrix` is `NULL`, has no non-zero elements, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Listing the positions of active entries in a flattened mask — feeding indices to `matrix_take` or `matrix_unravel_index`.

---

### `Matrix* matrix_extract(const Matrix* condition, const Matrix* arr)`

**Purpose**:  
Extracts the elements selected by a non-zero condition mask — the C analogue of NumPy's `numpy.extract(condition, arr)`. It returns a `1 × K` row vector of the elements of `arr` (in row-major order) at the positions where the corresponding element of `condition` is non-zero. Both inputs must have the same shape and neither is modified.

**Parameters**:  
- `condition`: The selection mask (non-zero selects).  
- `arr`: The values to pull from, the same shape as `condition`.

**Return Value**:  
A newly allocated `1 × K` row vector of selected values, or `NULL` if either input is `NULL`, the shapes differ, no element is selected, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Pulling out the values that satisfy a predicate — gathering the in-range samples, or the entries flagged by a boolean condition.

---

### `Matrix* matrix_trim_zeros(const Matrix* matrix)`

**Purpose**:  
Trims leading and trailing zeros from a vector — the C analogue of NumPy's `numpy.trim_zeros(filt)` with the default `trim='fb'`. It flattens the matrix and removes the runs of exactly-zero elements at the front and back, keeping the inner portion (including any interior zeros). The input is not modified.

**Parameters**:  
- `matrix`: The source vector.

**Return Value**:  
A newly allocated `1 × K` row vector with leading/trailing zeros removed, or `NULL` if `matrix` is `NULL`/empty, every element is zero, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Cropping padding from a signal — stripping silent samples at the ends of a buffer, or zero-padding from a coefficient vector.

---

### `size_t matrix_nanargmax(const Matrix* matrix)`

**Purpose**:  
Returns the flat index of the maximum element, ignoring NaN — the C analogue of NumPy's `numpy.nanargmax(x)`: the row-major flat index of the largest non-`NaN` element (the first occurrence on ties). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The flat index of the maximum non-NaN element, or `SIZE_MAX` if `matrix` is `NULL`/empty or every element is `NaN`.

**Usage Case**:  
Finding the location of the peak in data that contains missing readings.

---

### `Matrix* matrix_nancumsum(const Matrix* matrix)`

**Purpose**:  
Computes the cumulative sum, treating NaN as zero — the C analogue of NumPy's `numpy.nancumsum(x)`. It flattens the matrix in row-major order and returns a `1 × N` row vector of the running totals, where each `NaN` contributes `0` (so the running total holds flat across `NaN`s rather than becoming `NaN`). The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated `1 × N` row vector of cumulative sums, or `NULL` if `matrix` is `NULL`/empty or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Running totals over a series with gaps — cumulative revenue or counts where some periods are unrecorded.

---

### `Matrix* matrix_nansum_axis(const Matrix* matrix, int axis)`

**Purpose**:  
Sums along an axis, treating NaN as zero — the C analogue of NumPy's `numpy.nansum(x, axis=...)`: `axis == 0` sums down each column (`1 × cols`); `axis == 1` sums across each row (`rows × 1`). `NaN`s are skipped, so an all-`NaN` slice sums to `0`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `axis`: `0` for column sums, `1` for row sums.

**Return Value**:  
A newly allocated vector of per-axis NaN-ignoring sums, or `NULL` if `matrix` is `NULL`, `axis` is neither 0 nor 1, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Per-feature or per-row totals over a dataset with missing entries, without first imputing them.

---

### `double matrix_gmean(const Matrix* matrix)`

**Purpose**:  
Computes the geometric mean of all elements — the C analogue of SciPy's `scipy.stats.gmean(x)`: `exp(mean(log(x)))`, the `n`-th root of the product of the `n` elements. Elements should be positive; a zero drives the result to `0`, and a negative element yields `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The geometric mean, or `NaN` if `matrix` is `NULL`/empty.

**Usage Case**:  
Averaging ratios and growth factors — the correct "central" rate for multiplicative data such as returns or scaling factors.

---

### `double matrix_sem(const Matrix* matrix)`

**Purpose**:  
Computes the standard error of the mean — the C analogue of SciPy's `scipy.stats.sem(x)` with the default `ddof=1`: the sample standard deviation divided by the square root of the count, `sqrt(sum((x − mean)²) / (n − 1)) / sqrt(n)`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The standard error of the mean, or `NaN` if `matrix` is `NULL` or holds fewer than 2 elements.

**Usage Case**:  
Quantifying the uncertainty of an estimated mean — error bars on an average, or confidence-interval half-widths.

---

### `size_t matrix_nunique(const Matrix* matrix)`

**Purpose**:  
Counts the number of distinct (non-NaN) values — the C analogue of pandas' `Series.nunique()` with the default `dropna=True`: the number of distinct values among the elements, excluding `NaN`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
The count of distinct non-NaN values (`0` if `matrix` is `NULL`/empty or all elements are `NaN`).

**Usage Case**:  
Gauging the cardinality of a column — how many categories a label takes, or whether a feature is constant.

---

### `Matrix* matrix_logspace(double start, double stop, size_t num, double base)`

**Purpose**:  
Builds a row vector spaced evenly on a logarithmic scale — the C analogue of NumPy's `numpy.logspace(start, stop, num, base)`: `base ** linspace(start, stop, num)`, i.e. `num` samples whose exponents are evenly spaced over the closed interval `[start, stop]`. The result is a `1 × num` row vector.

**Parameters**:  
- `start`: The exponent of the first value (`base ** start`).  
- `stop`: The exponent of the last value (`base ** stop`).  
- `num`: The number of samples to generate; must be greater than 0.  
- `base`: The base of the log scale (e.g. `10.0`).

**Return Value**:  
A newly allocated `1 × num` row vector, or `NULL` if `num` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Generating frequency or hyper-parameter grids that span several orders of magnitude — log-spaced sweep points.

---

### `Matrix* matrix_geomspace(double start, double stop, size_t num)`

**Purpose**:  
Builds a row vector spaced evenly on a geometric progression — the C analogue of NumPy's `numpy.geomspace(start, stop, num)`: `num` samples spaced evenly on a log scale from `start` to `stop`, both endpoints inclusive. Equivalent to `exp(linspace(log(start), log(stop), num))`, with the endpoints pinned exactly to `start` and `stop`. Both `start` and `stop` must be positive.

**Parameters**:  
- `start`: The first value (inclusive); must be greater than 0.  
- `stop`: The last value (inclusive); must be greater than 0.  
- `num`: The number of samples to generate; must be greater than 0.

**Return Value**:  
A newly allocated `1 × num` row vector, or `NULL` if `num` is `0`, `start` or `stop` is not positive, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Log-spaced grids defined by their endpoints rather than their exponents — e.g. samples from 1 to 1000 that double-ish at each step.

---

### `Matrix* matrix_resize(const Matrix* matrix, size_t rows, size_t cols)`

**Purpose**:  
Resizes a matrix, cycling its elements to fill the new shape — the C analogue of NumPy's `numpy.resize(a, new_shape)`. It returns a new `rows × cols` matrix whose elements are taken from the row-major flattening of `a`, repeating from the beginning as needed to fill (and truncating if the new shape is smaller). Unlike `matrix_reshape`, the total size need not match. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix (non-empty).  
- `rows`: The number of rows in the result; must be greater than 0.  
- `cols`: The number of columns in the result; must be greater than 0.

**Return Value**:  
A newly allocated `rows × cols` matrix filled by cycling the source elements, or `NULL` if `matrix` is `NULL`/empty, a dimension is `0`, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Tiling a pattern to a target size, or forcing data into a fixed shape by repeat-filling rather than erroring on a size mismatch.

---

### `Matrix* matrix_unravel_index(const Matrix* indices, size_t rows, size_t cols)`

**Purpose**:  
Converts flat indices into `(row, column)` coordinate pairs — the C analogue of NumPy's `numpy.unravel_index(indices, shape)` for a 2-D `(rows, cols)` shape. Each (flattened) input index `k` is mapped to the pair `(k / cols, k % cols)`, and the result is a `K × 2` matrix whose rows are the `(row, col)` pairs. This inverts `matrix_flatnonzero`-style flat indexing. Neither input is modified.

**Parameters**:  
- `indices`: The flat indices to convert (any shape; integer-valued, in `0 .. rows·cols−1`).  
- `rows`: The number of rows in the target shape.  
- `cols`: The number of columns in the target shape; must be greater than 0.

**Return Value**:  
A newly allocated `K × 2` matrix of `(row, col)` pairs, or `NULL` if `indices` is `NULL`/empty, `cols` is `0`, any index is negative or `>= rows·cols`, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Turning flat positions (e.g. from `matrix_argmax` or `matrix_flatnonzero`) back into grid coordinates for indexing or plotting.

---

### `Matrix* matrix_ravel(const Matrix* matrix)`

**Purpose**:  
Flattens a matrix into a single row vector — the C analogue of NumPy's `numpy.ravel(x)`: returns a `1 × (rows·cols)` row vector holding a row-major (C-order) copy of the elements. Unlike `matrix_to_array` (which returns a raw `double*`), this returns an owning `Matrix`. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated `1 × (rows·cols)` row vector, or `NULL` if `matrix` is `NULL`/empty or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Collapsing a 2-D matrix into a flat vector for serialisation, hashing, or feeding to a 1-D routine.

---

### `Matrix* matrix_dot(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the inner product of two vectors — the C analogue of NumPy's `numpy.dot(a, b)` for one-dimensional inputs: the scalar `sum(aᵢ·bᵢ)` of two equal-length vectors, returned as a `1 × 1` matrix. Both operands must be vectors (a single row or column); for 2-D matrix multiplication use `matrix_multiply` instead. Neither input is modified.

**Parameters**:  
- `a`: The first vector (`1 × N` or `N × 1`).  
- `b`: The second vector, the same length as `a`.

**Return Value**:  
A newly allocated `1 × 1` matrix holding the inner product, or `NULL` if either input is `NULL`, is not a vector, the lengths differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The dot product of two feature vectors — similarity scores, projections, and weighted sums.

---

### `Matrix* matrix_vdot(const Matrix* a, const Matrix* b)`

**Purpose**:  
Computes the inner product of two flattened matrices — the C analogue of NumPy's `numpy.vdot(a, b)`: flattens both operands in row-major order (regardless of their shapes) and returns the scalar `sum(aᵢ·bᵢ)` as a `1 × 1` matrix. The two matrices only need the same total number of elements. (For real-valued data this matches `numpy.vdot`, which conjugates its first argument only for complex input.) Neither input is modified.

**Parameters**:  
- `a`: The first matrix (any shape).  
- `b`: The second matrix, with the same total size as `a`.

**Return Value**:  
A newly allocated `1 × 1` matrix holding the flattened inner product, or `NULL` if either input is `NULL`/empty, the total sizes differ, or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
The Frobenius inner product of two matrices (`sum` of element-wise products) — used in gradient sums and the numerator of matrix cosine similarity.

---

### `double matrix_trace_offset(const Matrix* matrix, int k)`

**Purpose**:  
Sums the elements along a diagonal at a given offset — the C analogue of NumPy's `numpy.trace(x, offset=k)`: sums `x[i][i+k]` along the diagonal offset by `k` from the main diagonal (`k > 0` selects a super-diagonal, `k < 0` a sub-diagonal). This generalizes `matrix_trace` (the `k == 0` case) and works for rectangular matrices. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.  
- `k`: The diagonal offset (`0` is the main diagonal).

**Return Value**:  
The sum along the chosen diagonal (`0.0` if `matrix` is `NULL` or the offset selects no valid elements).

**Usage Case**:  
Summing off-diagonal bands — e.g. the energy just above or below the main diagonal in a correlation or transition matrix.

---

### `Matrix* matrix_atleast_2d(const Matrix* matrix)`

**Purpose**:  
Ensures a vector is represented as an explicit row matrix — the C analogue of NumPy's `numpy.atleast_2d(x)`: a row or column vector is returned as a `1 × N` row matrix (treating it as a 1-D array, as NumPy does); a genuine 2-D matrix (more than one row and column) is returned unchanged. In every case a new owning copy is produced. The input is not modified.

**Parameters**:  
- `matrix`: The source matrix.

**Return Value**:  
A newly allocated matrix — `1 × N` for a vector input, or a same-shape copy for a 2-D input — or `NULL` if `matrix` is `NULL`/empty or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Normalising inputs to a 2-D shape before a routine that assumes rows and columns — guarding against a bare vector being treated as 1-D.

---

### `Matrix* matrix_fftfreq(size_t n, double d)`

**Purpose**:  
Returns the DFT sample frequencies — the C analogue of NumPy's `numpy.fft.fftfreq(n, d)`: a `1 × n` row vector of the frequency bins for a length-`n` DFT with sample spacing `d`, ordered `[0, 1, …, (n−1)/2, −(n/2), …, −1] / (d·n)` (non-negative frequencies first, then the negative ones).

**Parameters**:  
- `n`: The window length (number of samples); must be greater than 0.  
- `d`: The sample spacing (e.g. the time step), typically `1.0`.

**Return Value**:  
A newly allocated `1 × n` row vector of sample frequencies, or `NULL` if `n` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Building the frequency axis that labels the output bins of a full (complex) FFT.

---

### `Matrix* matrix_rfftfreq(size_t n, double d)`

**Purpose**:  
Returns the sample frequencies for a real FFT — the C analogue of NumPy's `numpy.fft.rfftfreq(n, d)`: a `1 × (n/2 + 1)` row vector of the non-negative frequency bins `[0, 1, …, n/2] / (d·n)` produced by a real-input FFT of length `n` with sample spacing `d`.

**Parameters**:  
- `n`: The window length (number of samples); must be greater than 0.  
- `d`: The sample spacing (e.g. the time step), typically `1.0`.

**Return Value**:  
A newly allocated `1 × (n/2 + 1)` row vector of sample frequencies, or `NULL` if `n` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Building the frequency axis for the half-spectrum returned by a real-valued FFT.

---

### `Matrix* matrix_hanning(size_t M)`

**Purpose**:  
Builds a Hann (Hanning) window — the C analogue of NumPy's `numpy.hanning(M)`: a `1 × M` row vector of the window `0.5 − 0.5·cos(2π·n / (M−1))` for `n = 0 … M−1`. As in NumPy, a length-1 window is the single value `1.0`.

**Parameters**:  
- `M`: The number of points in the window; must be greater than 0.

**Return Value**:  
A newly allocated `1 × M` row vector holding the window, or `NULL` if `M` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Tapering a signal before an FFT to suppress spectral leakage.

---

### `Matrix* matrix_hamming(size_t M)`

**Purpose**:  
Builds a Hamming window — the C analogue of NumPy's `numpy.hamming(M)`: a `1 × M` row vector of the window `0.54 − 0.46·cos(2π·n / (M−1))` for `n = 0 … M−1`. As in NumPy, a length-1 window is the single value `1.0`.

**Parameters**:  
- `M`: The number of points in the window; must be greater than 0.

**Return Value**:  
A newly allocated `1 × M` row vector holding the window, or `NULL` if `M` is `0` or allocation fails. The caller owns the result and must release it with `matrix_deallocate`.

**Usage Case**:  
Tapering a signal before an FFT — the Hamming window trades a little main-lobe width for lower side-lobes versus the Hann window.

---

## Example 1 : how to create matrix object and set and get value from it also print 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 4);

    if (!matrix) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }
    
    matrix_set(matrix, 1, 1, 15.32); // row 1 and colon 1
    matrix_print(matrix);

    fmt_printf("Value of matrix in row %zu and col %zu is %lf\n", 1, 1, matrix_get(matrix, 1, 1));

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
|  0.00000  0.00000  0.00000  0.00000 |
|  0.00000 15.32000  0.00000  0.00000 |
|  0.00000  0.00000  0.00000  0.00000 |
Value of matrix in row 1 and col 1 is 15.320000
```

---

## Example 2 : how to add two Matrix with `matrix_add`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include "time/std_time.h"

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, random_randint(1, 10));
    matrix_set(mat, 0, 1, random_randint(1, 10));
    matrix_set(mat, 1, 0, random_randint(1, 10));
    matrix_set(mat, 1, 1, random_randint(1, 10));
}

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    Matrix* matrix1 = matrix_create(2, 2);
    Matrix* matrix2 = matrix_create(2, 2);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }

    fillMatrix(matrix1);
    fillMatrix(matrix2);

    matrix_print(matrix1);
    fmt_printf("\n");
    matrix_print(matrix2);

    Matrix* sum = matrix_add(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(sum);

    matrix_deallocate(sum);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```
**Result:**
```
| 10.00000  6.00000 |
|  2.00000  4.00000 |

| 4.00000 8.00000 |
| 1.00000 9.00000 |

| 14.00000 14.00000 |
|  3.00000 13.00000 |
```

---

## Example 3 : subtract two matrix with `matrix_subtract`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include "time/std_time.h"

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, random_randint(1, 10));
    matrix_set(mat, 0, 1, random_randint(1, 10));
    matrix_set(mat, 1, 0, random_randint(1, 10));
    matrix_set(mat, 1, 1, random_randint(1, 10));
}

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    Matrix* matrix1 = matrix_create(2, 2);
    Matrix* matrix2 = matrix_create(2, 2);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }

    fillMatrix(matrix1);
    fillMatrix(matrix2);

    Matrix* subtraction = matrix_subtract(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(subtraction);

    matrix_deallocate(subtraction);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```
**Result:**
```
| -2.00000  5.00000 |
| -7.00000  3.00000 |
```

---

## Example 4 : multiply two Matrix with `matrix_multiply`

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include "random/random.h"
#include "time/std_time.h"

void fillMatrix(Matrix *mat) {
    matrix_set(mat, 0, 0, random_randint(1, 10));
    matrix_set(mat, 0, 1, random_randint(1, 10));
    matrix_set(mat, 1, 0, random_randint(1, 10));
    matrix_set(mat, 1, 1, random_randint(1, 10));
}


int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    Matrix* matrix1 = matrix_create(2, 2);
    Matrix* matrix2 = matrix_create(2, 2);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Can not create matrix object");
        exit(-1);
    }

    fillMatrix(matrix1);
    fillMatrix(matrix2);

    matrix_print(matrix1);
    fmt_printf("\n");
    matrix_print(matrix2);

    Matrix* product = matrix_multiply(matrix1, matrix2);

    fmt_printf("\n");
    matrix_print(product);

    matrix_deallocate(product);
    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);
    
    return 0;
}
```
**Result:**
```
| 2.00000 7.00000 |
| 9.00000 6.00000 |

| 1.00000 8.00000 |
| 4.00000 5.00000 |

|  30.00000  51.00000 |
|  33.00000 102.00000 |
```

---

## Example 5 : mutiply elements of Matrix with a scalar 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // create 2 X 3 Matrices
    Matrix* matrix = matrix_create(2, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 1, 2, 6.0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    double scalar = 2.0;
    if (matrix_scalar_multiply(matrix, scalar)) {
        fmt_printf("\nMatrix after scalar multiplication by %lf:\n", scalar);
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Scalar multiplication failed\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |

Matrix after scalar multiplication by 2.000000:
|  2.00000  4.00000  6.00000 |
|  8.00000 10.00000 12.00000 |
```

---

## Example 6 : check Matrix is square or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* mat = matrix_create(2, 3);

    if (!mat) {
        fmt_fprintf(stderr, "Can not create matrix");
        exit(-1);
    }

    if (matrix_is_square(mat)) {
        fmt_printf("Matrix is square");
    }
    else {
        fmt_printf("Matrix is not square");
    }

    matrix_deallocate(mat);
    return 0;
}
```
**Result:**
```
Matrix is not square
```

---

## Example 7 : create identity matrix and check is equal with other one or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create two identity matrix of size 3x3
    Matrix* matrix1 = matrix_create_identity(3);
    Matrix* matrix2 = matrix_create_identity(3);

    if (!matrix1 || !matrix2) {
        fmt_fprintf(stderr, "Failed to create identity matrices\n");
        exit(-1); 
    }

    if (matrix_is_equal(matrix1, matrix2)) {
        fmt_printf("Matrix1 and Matrix2 are equal.\n");
    } 
    else {
        fmt_printf("Matrix1 and Matrix2 are not equal.\n");
    }

    matrix_set(matrix1, 2, 2, 2.0); 

    if (matrix_is_equal(matrix1, matrix2)) {
        fmt_printf("After modification, Matrix1 and Matrix2 are still equal.\n");
    } 
    else {
        fmt_printf("After modification, Matrix1 and Matrix2 are not equal.\n");
    }

    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);

    return 0;
}
```
**Result:**
```
Matrix1 and Matrix2 are equal.
After modification, Matrix1 and Matrix2 are not equal.
```

---

## Example 8 : check Matrix is identity or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* identityMatrix = matrix_create_identity(3);
    if (!identityMatrix) {
        fmt_fprintf(stderr, "Failed to create identity matrix\n");
        exit(-1);
    }

    if (matrix_is_identity(identityMatrix)) {
        fmt_printf("The matrix is an identity matrix.\n");
    } 
    else {
        fmt_printf("The matrix is not an identity matrix.\n");
    }

    Matrix* nonIdentityMatrix = matrix_create(3, 3);
    if (!nonIdentityMatrix) {
        fmt_fprintf(stderr, "Failed to create non-identity matrix\n");
        exit(-1);
    }

    matrix_set(nonIdentityMatrix, 0, 0, 1.0);
    matrix_set(nonIdentityMatrix, 1, 1, 1.0);
    matrix_set(nonIdentityMatrix, 2, 2, 0.5);


    if (matrix_is_identity(nonIdentityMatrix)) {
        fmt_printf("The matrix is an identity matrix.\n");
    } 
    else {
        fmt_printf("The matrix is not an identity matrix.\n");
    }

    matrix_deallocate(identityMatrix);
    matrix_deallocate(nonIdentityMatrix);

    return 0;
}
```
**Result:**
```
The matrix is an identity matrix.
The matrix is not an identity matrix.
```

---

## Example 9 : check Matrix is idempotent or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Idempotent matrix
    Matrix* matrix1 = matrix_create(3, 3);
    double data1[] = {-1, 1, 1, -1, 1, 1, -1, 1, 1};
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            matrix_set(matrix1, i, j, data1[i * 3 + j]);
        }
    }

    // Non-idempotent matrix
    Matrix* matrix2 = matrix_create(2, 2);
    double data2[] = {1, 2, 3, 4};
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            matrix_set(matrix2, i, j, data2[i * 2 + j]);
        }
    }

    if (matrix_is_idempotent(matrix1)) {
        fmt_printf("Matrix 1 is idempotent.\n");
    } 
    else {
        fmt_printf("Matrix 1 is not idempotent.\n");
    }

    if (matrix_is_idempotent(matrix2)) {
        fmt_printf("Matrix 2 is idempotent.\n");
    } 
    else {
        fmt_printf("Matrix 2 is not idempotent.\n");
    }

    matrix_deallocate(matrix1);
    matrix_deallocate(matrix2);

    return 0;
}
```
**Result:**
```
Matrix 1 is idempotent.
Matrix 2 is not idempotent.
```

---

## Example 10 : check row and columnar Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* rowMatrix = matrix_create(1, 3); 
    Matrix* columnMatrix = matrix_create(3, 1); 
    Matrix* regularMatrix = matrix_create(2, 2);

    if (matrix_is_row(rowMatrix)) {
        fmt_printf("RowMatrix is a row matrix.\n");
    } 
    else {
        fmt_printf("RowMatrix is not a row matrix.\n");
    }

    if (matrix_is_columnar(columnMatrix)) {
        fmt_printf("ColumnMatrix is a columnar matrix.\n");
    } 
    else {
        fmt_printf("ColumnMatrix is not a columnar matrix.\n");
    }

    if (!matrix_is_row(regularMatrix) && !matrix_is_columnar(regularMatrix)) {
        fmt_printf("RegularMatrix is neither a row matrix nor a columnar matrix.\n");
    }

    matrix_deallocate(rowMatrix);
    matrix_deallocate(columnMatrix);
    matrix_deallocate(regularMatrix);

    return 0;
}
```
**Result:**
```
RowMatrix is a row matrix.
ColumnMatrix is a columnar matrix.
RegularMatrix is neither a row matrix nor a columnar matrix.
```

---

## Example 11 : get diagonal Matrix as row or columns 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    Matrix* diagonalMatrix = matrix_get_main_diagonal_as_column(matrix);
    Matrix* diagonalRowMatrix = matrix_get_main_diagonal_as_row(matrix);

    if (diagonalMatrix) {
        fmt_printf("Main diagonal as a column matrix:\n");
        matrix_print(diagonalMatrix); 
    } 
    else {
        fmt_printf("Failed to get the main diagonal as a column matrix.\n");
    }

    if (diagonalRowMatrix) {
        fmt_printf("Main diagonal as a row matrix:\n");
        matrix_print(diagonalRowMatrix);
    } 
    else {
        fmt_printf("Failed to get the main diagonal as a row matrix.\n");
    }

    matrix_deallocate(matrix);
    matrix_deallocate(diagonalMatrix);
    matrix_deallocate(diagonalRowMatrix);

    return 0;
}
```
**Result:**
```
Main diagonal as a column matrix:
| 0.00000 |
| 0.00000 |
| 0.00000 |
Main diagonal as a row matrix:
| 0.00000 0.00000 0.00000 |
```

---

## Example 12 : get minor diagonal as row and columns 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_printf("Failed to create matrix.\n");
        return -1;
    }

    double values[9] = {
        1.0, 2.0, 3.0, // First row
        4.0, 5.0, 6.0, // Second row
        7.0, 8.0, 9.0  // Third row
    };

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            matrix_set(matrix, i, j, values[i * 3 + j]);
        }
    }

    Matrix* minorDiagonalRow = matrix_get_minor_diagonal_as_row(matrix);
    if (minorDiagonalRow) {
        fmt_printf("Minor diagonal as a row matrix:\n");
        matrix_print(minorDiagonalRow); 
    } 
    else {
        fmt_printf("Failed to get the minor diagonal as a row matrix.\n");
    }

    Matrix* minorDiagonalColumn = matrix_get_minor_diagonal_as_column(matrix);
    if (minorDiagonalColumn) {
        fmt_printf("Minor diagonal as a column matrix:\n");
        matrix_print(minorDiagonalColumn); 
    } 
    else {
        fmt_printf("Failed to get the minor diagonal as a column matrix.\n");
    }

    matrix_deallocate(matrix);
    matrix_deallocate(minorDiagonalRow);
    matrix_deallocate(minorDiagonalColumn);

    return 0;
}
```
**Result:**
```
Minor diagonal as a row matrix:
| 3.00000 5.00000 7.00000 |
Minor diagonal as a column matrix:
| 3.00000 |
| 5.00000 |
| 7.00000 |
```

---

## Example 13 : create transpose Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 1, 2, 6.0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    Matrix* transposedMatrix = matrix_transpose(matrix);
    if (!transposedMatrix) {
        fmt_fprintf(stderr, "Failed to transpose the matrix\n");
        matrix_deallocate(matrix);
        exit(-1);
    }

    fmt_printf("\nTransposed matrix:\n");
    matrix_print(transposedMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(transposedMatrix);

    return 0;
}
```
**Result:**
```
Original matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |

Transposed matrix:
| 1.00000 4.00000 |
| 2.00000 5.00000 |
| 3.00000 6.00000 |
```

---

## Example 14 : check Matrix is symmetric or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrixA = matrix_create(4, 4);
    double valuesA[16] = {9, 13, 3, 6, 13, 11, 7, 6, 3, 7, 4, 7, 6, 6, 7, 10};

    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            matrix_set(matrixA, i, j, valuesA[i * 4 + j]);
        }
    }

    if (matrix_is_symmetric(matrixA)) {
        fmt_printf("Matrix A is symmetric.\n");
    } 
    else {
        fmt_printf("Matrix A is not symmetric.\n");
    }

    matrix_deallocate(matrixA);
    return 0;
}
```
**Result:**
```
Matrix A is symmetric.
```

---

## Example 15 : Matrix is upper or lower triangular.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* upperTriangular = matrix_create(3, 3);
    matrix_set(upperTriangular, 0, 0, 1);
    matrix_set(upperTriangular, 0, 1, 2);
    matrix_set(upperTriangular, 0, 2, 3);
    

    Matrix* lowerTriangular = matrix_create(3, 3);
    matrix_set(lowerTriangular, 0, 0, 1);
    matrix_set(lowerTriangular, 1, 0, 2);
    matrix_set(lowerTriangular, 1, 1, 3);
    matrix_set(lowerTriangular, 2, 0, 4);
    matrix_set(lowerTriangular, 2, 1, 5);
    matrix_set(lowerTriangular, 2, 2, 6);
    

    if (matrix_is_upper_triangular(upperTriangular)) {
        fmt_printf("The first matrix is upper triangular.\n");
    } 
    else {
        fmt_printf("The first matrix is not upper triangular.\n");
    }

    if (matrix_is_lower_triangular(lowerTriangular)) {
        fmt_printf("The second matrix is lower triangular.\n");
    } 
    else {
        fmt_printf("The second matrix is not lower triangular.\n");
    }

    matrix_deallocate(upperTriangular);
    matrix_deallocate(lowerTriangular);

    return 0;
}
```
**Result:**
```
The first matrix is upper triangular.
The second matrix is lower triangular.
```

---

## Example 16 : is skew symmetric or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create a 2x2 skew-symmetric matrix
    Matrix* skewSymmetric = matrix_create(2, 2);
    
    matrix_set(skewSymmetric, 0, 0, 0); 
    matrix_set(skewSymmetric, 0, 1, 2); 
    matrix_set(skewSymmetric, 1, 0, -2); 
    matrix_set(skewSymmetric, 1, 1, 0); 

    if (matrix_is_skew_symmetric(skewSymmetric)) {
        fmt_printf("The matrix is skew-symmetric.\n");
    } 
    else {
        fmt_printf("The matrix is not skew-symmetric.\n");
    }

    matrix_deallocate(skewSymmetric);
    return 0;
}
```
**Result:**
```
The matrix is skew-symmetric.
```

---

## Example 17: Determinant of a 2X2 Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);

    matrix_set(matrix, 0, 0, 4); 
    matrix_set(matrix, 0, 1, 7); 
    matrix_set(matrix, 1, 0, 2); 
    matrix_set(matrix, 1, 1, 6); 

    double det = matrix_determinant(matrix);
    fmt_printf("Determinant of the 2x2 matrix is: %lf\n", det);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Determinant of the 2x2 matrix is: 10.000000
```

## Example 18: Determinant of a 3X3 Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);

    matrix_set(matrix, 0, 0, 6); 
    matrix_set(matrix, 0, 1, 1); 
    matrix_set(matrix, 0, 2, 1); 
    matrix_set(matrix, 1, 0, 4); 
    matrix_set(matrix, 1, 1, -2);
    matrix_set(matrix, 1, 2, 5); 
    matrix_set(matrix, 2, 0, 2); 
    matrix_set(matrix, 2, 1, 8); 
    matrix_set(matrix, 2, 2, 7); 

    double det = matrix_determinant(matrix);
    fmt_printf("Determinant of the 3x3 matrix is: %lf\n", det);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Determinant of the 3x3 matrix is: -306.000000
```

---

## Example 19: Determinant Calculation with Identity Matrix

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Create a 4x4 identity matrix
    Matrix* identityMatrix = matrix_create_identity(4);

    double det = matrix_determinant(identityMatrix);
    fmt_printf("Determinant of the 4x4 identity matrix is: %lf\n", det);

    matrix_deallocate(identityMatrix);
    return 0;
}
```
**Result:**
```
Determinant of the 4x4 identity matrix is: 1.000000
```

---

## Example 20 : sum of diagonal elements of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Cannot create matrix object\n");
        exit(-1);
    }

    matrix_set(matrix, 0, 0, 1.0); 
    matrix_set(matrix, 0, 1, 2.0);
    matrix_set(matrix, 0, 2, 3.0);
    matrix_set(matrix, 1, 0, 4.0);
    matrix_set(matrix, 1, 1, 5.0); 
    matrix_set(matrix, 1, 2, 6.0);
    matrix_set(matrix, 2, 0, 7.0);
    matrix_set(matrix, 2, 1, 8.0);
    matrix_set(matrix, 2, 2, 9.0); 

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    double trace = matrix_trace(matrix);
    fmt_printf("Trace of the matrix is: %lf\n", trace);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
Trace of the matrix is: 15.000000
```

---

## Example 21 : get inverse of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 4 7 |
    // | 2 6 |
    matrix_set(matrix, 0, 0, 4);
    matrix_set(matrix, 0, 1, 7);
    matrix_set(matrix, 1, 0, 2);
    matrix_set(matrix, 1, 1, 6);


    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    Matrix* inverseMatrix = matrix_inverse(matrix);
    if (!inverseMatrix) {
        fmt_fprintf(stderr, "Failed to calculate the inverse of the matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nInverse Matrix:\n");
    matrix_print(inverseMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(inverseMatrix);

    return 0;
}
```
**Result:**
```
Original Matrix:
| 4.00000 7.00000 |
| 2.00000 6.00000 |

Inverse Matrix:
|  0.60000 -0.70000 |
| -0.20000  0.40000 |
```

---

## Example 22: How to raise a square matrix to an integer power also how to copy 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 1 2 |
    // | 3 4 |
    matrix_set(matrix, 0, 0, 1);
    matrix_set(matrix, 0, 1, 2);
    matrix_set(matrix, 1, 0, 3);
    matrix_set(matrix, 1, 1, 4);

    Matrix* matrixCopy = matrix_copy(matrix);
    if (!matrixCopy) {
        fmt_fprintf(stderr, "Failed to copy matrix.\n");
        matrix_deallocate(matrix);
        
        return -1;
    }

    int power = 2;
    Matrix* matrixPowered = matrix_power(matrix, power);
    if (!matrixPowered) {
        fmt_fprintf(stderr, "Failed to raise matrix to power %d.\n", power);
        matrix_deallocate(matrix);
        matrix_deallocate(matrixCopy);

        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    fmt_printf("\nCopied Matrix (should be identical to original):\n");
    matrix_print(matrixCopy);

    fmt_printf("\nMatrix Raised to Power %d:\n", power);
    matrix_print(matrixPowered);

    matrix_deallocate(matrix);
    matrix_deallocate(matrixCopy);
    matrix_deallocate(matrixPowered);

    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Copied Matrix (should be identical to original):
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Matrix Raised to Power 2:
|  7.00000 10.00000 |
| 15.00000 22.00000 |
```

---

## Example 23 : get rank of matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // | 1 2 3 |
    // | 4 5 6 |
    // | 7 8 9 |
    matrix_set(matrix, 0, 0, 1);
    matrix_set(matrix, 0, 1, 2);
    matrix_set(matrix, 0, 2, 3);
    matrix_set(matrix, 1, 0, 4);
    matrix_set(matrix, 1, 1, 5);
    matrix_set(matrix, 1, 2, 6);
    matrix_set(matrix, 2, 0, 7);
    matrix_set(matrix, 2, 1, 8);
    matrix_set(matrix, 2, 2, 9);

    int rank = matrix_rank(matrix);

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    fmt_printf("\nRank of the matrix is: %d\n", rank);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Rank of the matrix is: 2
```

---

## Example 24 : check Matrix is diagonal or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    matrix_set(matrix, 0, 0, 1.0);
    matrix_set(matrix, 1, 1, 5.0);
    matrix_set(matrix, 2, 2, 9.0);

    fmt_printf("Initial matrix:\n");
    matrix_print(matrix);

    // Check if the matrix is diagonal
    if (matrix_is_diagonal(matrix)) {
        fmt_printf("The matrix is diagonal.\n");
    } 
    else {
        fmt_printf("The matrix is not diagonal.\n");
    }

    // Modify the matrix to make it non-diagonal
    matrix_set(matrix, 0, 1, 2.0);

    fmt_printf("\nModified matrix:\n");
    matrix_print(matrix);

    if (matrix_is_diagonal(matrix)) {
        fmt_printf("The matrix is diagonal.\n");
    } 
    else {
        fmt_printf("The matrix is not diagonal.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Initial matrix:
| 1.00000 0.00000 0.00000 |
| 0.00000 5.00000 0.00000 |
| 0.00000 0.00000 9.00000 |
The matrix is diagonal.

Modified matrix:
| 1.00000 2.00000 0.00000 |
| 0.00000 5.00000 0.00000 |
| 0.00000 0.00000 9.00000 |
The matrix is not diagonal.
```

---

## Example 25 : Matrix is orthogonal or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Orthogonal matrix example: Q = [0 -1; 1 0], Q * Q^T = I
    Matrix* matrix = matrix_create(2, 2);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    matrix_set(matrix, 0, 0, 0);
    matrix_set(matrix, 0, 1, -1);
    matrix_set(matrix, 1, 0, 1);
    matrix_set(matrix, 1, 1, 0);

    fmt_printf("Original matrix:\n");
    matrix_print(matrix);

    if (matrix_is_orthogonal(matrix)) {
        fmt_printf("The matrix is orthogonal.\n");
    } 
    else {
        fmt_printf("The matrix is not orthogonal.\n");
    }

    // Modify the matrix to make it non-orthogonal
    matrix_set(matrix, 0, 0, 1); 

    fmt_printf("\nModified matrix:\n");
    matrix_print(matrix);

    if (matrix_is_orthogonal(matrix)) {
        fmt_printf("The modified matrix is orthogonal.\n");
    } 
    else {
        fmt_printf("The modified matrix is not orthogonal.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original matrix:
|  0.00000 -1.00000 |
|  1.00000  0.00000 |
The matrix is orthogonal.

Modified matrix:
|  1.00000 -1.00000 |
|  1.00000  0.00000 |
The modified matrix is not orthogonal.
```

---

## Example 26 : product matrix Kronecker 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    // Define matrix A (2x2)
    Matrix* A = matrix_create(2, 2);
    matrix_set(A, 0, 0, 1); 
    matrix_set(A, 0, 1, 2); 
    matrix_set(A, 1, 0, 3); 
    matrix_set(A, 1, 1, 4); 

    // Define matrix B (2x2)
    Matrix* B = matrix_create(2, 2);
    matrix_set(B, 0, 0, 0); 
    matrix_set(B, 0, 1, 5); 
    matrix_set(B, 1, 0, 6); 
    matrix_set(B, 1, 1, 7); 

    Matrix* C = matrix_kronecker_product(A, B);
    if (!C) {
        fmt_fprintf(stderr, "Failed to compute the Kronecker product.\n");
        matrix_deallocate(A);
        matrix_deallocate(B);
        
        return -1;
    }

    fmt_printf("Matrix A:\n");
    matrix_print(A);

    fmt_printf("\nMatrix B:\n");
    matrix_print(B);

    fmt_printf("\nKronecker product of A and B:\n");
    matrix_print(C);

    matrix_deallocate(A);
    matrix_deallocate(B);
    matrix_deallocate(C);

    return 0;
}
```
**Result:**
```
Matrix A:
| 1.00000 2.00000 |
| 3.00000 4.00000 |

Matrix B:
| 0.00000 5.00000 |
| 6.00000 7.00000 |

Kronecker product of A and B:
|  0.00000  5.00000  0.00000 10.00000 |
|  6.00000  7.00000 12.00000 14.00000 |
|  0.00000 15.00000  0.00000 20.00000 |
| 18.00000 21.00000 24.00000 28.00000 |
```

---

## Example 27 : create hankel Matrix from first row and last col 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 2, 3, 4};
    Matrix* firstRow = matrix_create(1, 4);
    for (size_t i = 0; i < 4; i++) {
        matrix_set(firstRow, 0, i, firstRowData[i]);
    }

    double lastColData[] = {4, 5, 6, 7};
    Matrix* lastCol = matrix_create(4, 1);
    for (size_t i = 0; i < 4; i++) {
        matrix_set(lastCol, i, 0, lastColData[i]);
    }

    // Generate the Hankel matrix
    Matrix* hankelMatrix = matrix_hankel(firstRow, lastCol);
    if (hankelMatrix) {
        fmt_printf("Hankel Matrix:\n");
        matrix_print(hankelMatrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to generate Hankel matrix.\n");
    }

    matrix_deallocate(firstRow);
    matrix_deallocate(lastCol);
    matrix_deallocate(hankelMatrix);

    return EXIT_SUCCESS;
}
```
**Result:**
```
Hankel Matrix:
| 1.00000 2.00000 3.00000 4.00000 |
| 2.00000 3.00000 4.00000 5.00000 |
| 3.00000 4.00000 5.00000 6.00000 |
| 4.00000 5.00000 6.00000 7.00000 |
```

---

## Example 28 : check matrix is hankle or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    size_t n = 4; 
    Matrix* matrix = matrix_create(n, n);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return EXIT_FAILURE;
    }

    double values[] = {1, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            if (i + j < n) {
                matrix_set(matrix, i, j, values[i + j]);
            } 
            else {
                matrix_set(matrix, i, j, values[n - 1 + (i + j - n + 1)]);
            }
        }
    }

    fmt_printf("Matrix:\n");
    matrix_print(matrix);

    if (matrix_is_hankel(matrix)) {
        fmt_printf("The matrix is Hankel.\n");
    } 
    else {
        fmt_printf("The matrix is not Hankel.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix:
| 1.00000 2.00000 3.00000 4.00000 |
| 2.00000 3.00000 4.00000 3.00000 |
| 3.00000 4.00000 3.00000 4.00000 |
| 4.00000 3.00000 4.00000 5.00000 |
The matrix is Hankel.
```

---

## Example 29 : create matrix from array then create toeplitz matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 4, 5, 6};
    double firstColData[] = {1, 2, 3};

    Matrix* firstRow = matrix_from_array(firstRowData, 1, sizeof(firstRowData) / sizeof(firstRowData[0]));
    Matrix* firstCol = matrix_from_array(firstColData, sizeof(firstColData) / sizeof(firstColData[0]), 1);

    fmt_printf("First row\n");
    matrix_print(firstRow);

    fmt_printf("First Col\n");
    matrix_print(firstCol);
    
    Matrix* toeplitzMatrix = matrix_toeplitz(firstRow, firstCol);

    fmt_printf("Toeplitz Matrix:\n");
    matrix_print(toeplitzMatrix);

    matrix_deallocate(firstRow);
    matrix_deallocate(firstCol);
    matrix_deallocate(toeplitzMatrix);

    return 0;
}
```
**Result:**
```
First row
| 1.00000 4.00000 5.00000 6.00000 |
First Col
| 1.00000 |
| 2.00000 |
| 3.00000 |
Toeplitz Matrix:
| 1.00000 4.00000 5.00000 6.00000 |
| 2.00000 1.00000 4.00000 5.00000 |
| 3.00000 2.00000 1.00000 4.00000 |
```

---

## Example 30 : create circulant matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double firstRowData[] = {1, 2, 3};
    Matrix* firstRow = matrix_from_array(firstRowData, 1, sizeof(firstRowData) / sizeof(firstRowData[0]));

    Matrix* circulantMatrix = matrix_circulant(firstRow);

    fmt_printf("Circulan Matrix:\n");
    matrix_print(circulantMatrix);

    matrix_deallocate(firstRow);
    matrix_deallocate(circulantMatrix);

    return 0;
}
```
**Result:**
```
Circulan Matrix:
| 1.00000 2.00000 3.00000 |
| 3.00000 1.00000 2.00000 |
| 2.00000 3.00000 1.00000 |
```

---

## Example 31 : create hilbert Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* hilbert = matrix_hilbert(3);

    fmt_printf("Hilbert Matrix : \n");
    matrix_print(hilbert);


    matrix_deallocate(hilbert);
    return 0;
}
```
**Result:**
```
Hilbert Matrix : 
| 1.00000 0.50000 0.33333 |
| 0.50000 0.33333 0.25000 |
| 0.33333 0.25000 0.20000 |
```

---

## Example 32 : create helmert matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* helmert = matrix_helmert(5, true);

    fmt_printf("Helmert Matrix : \n");
    matrix_print(helmert);

    matrix_deallocate(helmert);
    return 0;
}
```
**Result:**
```
Helmert Matrix :
|  0.44721  0.44721  0.44721  0.44721  0.44721 |
|  0.70711 -0.70711  0.00000  0.00000  0.00000 |
|  0.40825  0.40825 -0.81650  0.00000  0.00000 |
|  0.28868  0.28868  0.28868 -0.86603  0.00000 |
|  0.22361  0.22361  0.22361  0.22361 -0.89443 |
```

---

## Example 33 : Cofactor of Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double arr[] = {1, 9, 3, 2, 5, 4, 3, 7, 8};
    Matrix* mat = matrix_from_array(arr,3, 3);
    Matrix* cofactor = matrix_cofactor(mat);

    fmt_printf("Cofactor of Matrix : \n");
    matrix_print(cofactor);

    matrix_deallocate(mat);
    matrix_deallocate(cofactor);
    return 0;
}
```
**Result:**
```
Cofactor of Matrix : 
|  12.00000  -4.00000  -1.00000 |
| -51.00000  -1.00000  20.00000 |
|  21.00000   2.00000 -13.00000 |
```

---

## Example 34 : calculate cholesky decomposition of a Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    size_t n = 3; 

    Matrix* matrix = matrix_from_array(data, n, n);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    Matrix* chol = matrix_cholesky_decomposition(matrix);
    if (!chol) {
        fmt_fprintf(stderr, "Cholesky decomposition failed.\n");
        matrix_deallocate(matrix); 
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    fmt_printf("Cholesky Decomposition (Upper Triangular Matrix):\n");
    matrix_print(chol);

    matrix_deallocate(matrix);
    matrix_deallocate(chol);

    return 0;
}
```
**Result:**
```
Original Matrix:
|   4.00000  12.00000 -16.00000 |
|  12.00000  37.00000 -43.00000 |
| -16.00000 -43.00000  98.00000 |
Cholesky Decomposition (Upper Triangular Matrix):
|  2.00000  6.00000 -8.00000 |
|  0.00000  1.00000  5.00000 |
|  0.00000  0.00000  3.00000 |
```

---

## Example 35 : LU decomposition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double A_data[] = {4, 3, 2, 3, 2, 1, 2, 1, 3};
    size_t n = 3; 
    Matrix* A = matrix_from_array(A_data, n, n);

    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    Matrix *L, *U;
    bool success = matrix_lu_decomposition(A, &L, &U);
    if (!success) {
        fmt_fprintf(stderr, "LU decomposition failed.\n");
        matrix_deallocate(A);
        return -1;
    }

    fmt_printf("Original Matrix A:\n");
    matrix_print(A);

    fmt_printf("Lower Triangular Matrix L:\n");
    matrix_print(L);

    fmt_printf("Upper Triangular Matrix U:\n");
    matrix_print(U);

    matrix_deallocate(A);
    matrix_deallocate(L);
    matrix_deallocate(U);

    return 0;
}
```
**Result:**
```
Original Matrix A:
| 4.00000 3.00000 2.00000 |
| 3.00000 2.00000 1.00000 |
| 2.00000 1.00000 3.00000 |
Lower Triangular Matrix L:
| 1.00000 0.00000 0.00000 |
| 0.75000 1.00000 0.00000 |
| 0.50000 2.00000 1.00000 |
Upper Triangular Matrix U:
|  4.00000  3.00000  2.00000 |
|  0.00000 -0.25000 -0.50000 |
|  0.00000  0.00000  3.00000 |
```

---

## Example 36 : QR decomposition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double A_data[] = {4, 12, -16, 12, 37, -43, -16, -43, 98};
    size_t n = 3;
    Matrix* A = matrix_from_array(A_data, n, n);
    
    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    Matrix *Q, *R;
    bool success = matrix_qr_decomposition(A, &Q, &R);
    if (!success) {
        fmt_fprintf(stderr, "QR decomposition failed.\n");
        matrix_deallocate(A); 
        return -1;
    }

    fmt_printf("Original Matrix A:\n");
    matrix_print(A);

    fmt_printf("Orthogonal Matrix Q:\n");
    matrix_print(Q);

    fmt_printf("Upper Triangular Matrix R:\n");
    matrix_print(R);

    matrix_deallocate(A);
    matrix_deallocate(Q);
    matrix_deallocate(R);

    return 0;
}
```
**Result:**
```
Original Matrix A:
|   4.00000  12.00000 -16.00000 |
|  12.00000  37.00000 -43.00000 |
| -16.00000 -43.00000  98.00000 |
Orthogonal Matrix Q:
|  0.19612  0.16948  0.96582 |
|  0.58835  0.76762 -0.25416 |
| -0.78446  0.61809  0.05083 |
Upper Triangular Matrix R:
|   20.39608   57.85426 -105.31436 |
|    0.00000    3.85806   24.85307 |
|    0.00000    0.00000    0.45750 |
```

---

## Example 37 : Pascal matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* pascal = matrix_pascal(5);

    fmt_printf("Pascal Matrix : \n");
    matrix_print(pascal);

    matrix_deallocate(pascal);
    return 0;
}
```
**Result:**
```
Pascal Matrix :
|  1.00000  1.00000  1.00000  1.00000  1.00000 |
|  1.00000  2.00000  3.00000  4.00000  5.00000 |
|  1.00000  3.00000  6.00000 10.00000 15.00000 |
|  1.00000  4.00000 10.00000 20.00000 35.00000 |
|  1.00000  5.00000 15.00000 35.00000 70.00000 |
```

---

## Example 38 : different kind of Norm 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {1, -2, 3, -4, 5, -6, 7, -8, 9};
    Matrix* matrix = matrix_from_array(data, 3, 3);
    
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // Compute norms
    double frobeniusNorm = matrix_frobenius_norm(matrix);
    double l1Norm = matrix_l1_norm(matrix);
    double infinityNorm = matrix_infinity_norm(matrix);


    fmt_printf("Frobenius Norm: %lf\n", frobeniusNorm);
    fmt_printf("L1 Norm: %lf\n", l1Norm);
    fmt_printf("Infinity Norm: %lf\n", infinityNorm);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Frobenius Norm: 16.881943
L1 Norm: 18.000000
Infinity Norm: 24.000000
```

---

## Example 39 : Matrix is positive definite or not 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* posDefMatrix = matrix_create(2, 2);
    matrix_set(posDefMatrix, 0, 0, 2);  
    matrix_set(posDefMatrix, 0, 1, -1);
    matrix_set(posDefMatrix, 1, 0, -1);
    matrix_set(posDefMatrix, 1, 1, 2);

    // Create a non-positive definite matrix (symmetric but not positive definite)
    Matrix* nonPosDefMatrix = matrix_create(2, 2);
    matrix_set(nonPosDefMatrix, 0, 0, 0);  
    matrix_set(nonPosDefMatrix, 0, 1, 1);
    matrix_set(nonPosDefMatrix, 1, 0, 1);
    matrix_set(nonPosDefMatrix, 1, 1, 0);

    if (matrix_is_positive_definite(posDefMatrix)) {
        fmt_printf("The matrix is positive definite.\n");
    } 
    else {
        fmt_printf("The matrix is not positive definite.\n");
    }

    if (matrix_is_positive_definite(nonPosDefMatrix)) {
        fmt_printf("The matrix is positive definite.\n");
    } 
    else {
        fmt_printf("The matrix is not positive definite.\n");
    }

    matrix_deallocate(posDefMatrix);
    matrix_deallocate(nonPosDefMatrix);

    return 0;
}
```
**Result:**
```
The matrix is positive definite.
The matrix is not positive definite.
```

---

## Example 40 : generate projection of a Matrix 

```c 
#include "matrix/matrix.h"
#include "fmt/fmt.h"


int main() {
    double matrix[] = {
        1, 2,
        3, 4,
        5, 6
    };
    size_t rows = 3;
    size_t cols = 2;
    Matrix* A = matrix_from_array(matrix, rows, cols);

    if (!A) {
        fmt_printf("Failed to create matrix A.\n");
        return -1;
    }

    fmt_printf("Matrix A:\n");
    matrix_print(A);

    // Compute the projection matrix P for the column space of A
    Matrix* projection = matrix_projection(A);
    if (!projection) {
        fmt_printf("Failed to compute the projection matrix.\n");
        matrix_deallocate(A); 
        return -1;
    }

    fmt_printf("\nProjection Matrix P for the column space of A:\n");
    matrix_print(projection);

    matrix_deallocate(A);
    matrix_deallocate(projection);

    return 0;
}
``` 
**Result:**
```
Matrix A:
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |

Projection Matrix P for the column space of A:
|  0.83333  0.33333 -0.16667 |
|  0.33333  0.33333  0.33333 |
| -0.16667  0.33333  0.83333 |
```

---

## Example 41 : create vandermond matrix from give matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {4, 3, 1};
    Matrix* matrix = matrix_from_array(data, 1, 3);
    Matrix* vander = matrix_vandermonde(matrix, 3);

    if (!matrix || !vander) {
        fmt_fprintf(stderr, "vander creation or generate matrix from array failed\n");
        return -1;
    }

    fmt_printf("Vandermond Matrix : \n");
    matrix_print(vander);

    matrix_deallocate(matrix);
    matrix_deallocate(vander);

    return 0;   
}
```
**Result:**
```
Vandermond Matrix : 
|  1.00000  4.00000 16.00000 |
|  1.00000  3.00000  9.00000 |
|  1.00000  1.00000  1.00000 |
```

---

## Example 42 : create companion matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double data[] = {1, 2, 3};
    Matrix* matrix = matrix_from_array(data, 1, 3);
    Matrix* companion = matrix_companion(matrix, 3);

    fmt_printf("Companion Matrix : \n");
    matrix_print(companion);

    matrix_deallocate(companion);
    matrix_deallocate(matrix);
    return 0;   
}
```
**Result:**
```
Companion Matrix : 
| -2.00000 -3.00000 |
|  1.00000  0.00000 |
```

Its eigenvalues, `-1 ± 1.41421i`, are the roots of `x² + 2x + 3` (the same matrix as `scipy.linalg.companion([1, 2, 3])`).

---

## Example 43 : How to apply a function to matrix_map also fill and matrix with prefer value 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

double sine_of_element(double x) {
    return sin(x);
}

int main() {
    Matrix* matrix = matrix_create(3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    // Fill the matrix with the value PI / 4
    if (!matrix_fill(matrix, 3.141596 / 4)) {
        fmt_fprintf(stderr, "Failed to fill matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("Matrix after filling with PI / 4:\n");
    matrix_print(matrix);

    // Apply the sine function to each element of the matrix
    Matrix* resultMatrix = matrix_map(matrix, sine_of_element);
    if (!resultMatrix) {
        fmt_fprintf(stderr, "Failed to apply function to matrix.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying the sine function:\n");
    matrix_print(resultMatrix);

    matrix_deallocate(matrix);
    matrix_deallocate(resultMatrix);

    return 0;
}
```
**Result:**
```
Matrix after filling with PI / 4:
| 0.78540 0.78540 0.78540 |
| 0.78540 0.78540 0.78540 |
| 0.78540 0.78540 0.78540 |

Matrix after applying the sine function:
| 0.70711 0.70711 0.70711 |
| 0.70711 0.70711 0.70711 |
| 0.70711 0.70711 0.70711 |
```

---

## Example 44 : how to use matrix_row_addition and matrix_col_addition 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"


int main() {
    double values[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    Matrix* matrix = matrix_from_array(values, 3, 3);
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    // Add the first row to the second row
    if (matrix_row_addition(matrix, 1, 0, 1.0)) {
        fmt_printf("\nMatrix after adding first row to the second row:\n");
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to add rows.\n");
    }

    // Add the first column to the third column, scaling the first column by 2
    if (matrix_col_addition(matrix, 2, 0, 2.0)) {
        fmt_printf("\nMatrix after adding first column (scaled by 2) to the third column:\n");
        matrix_print(matrix);
    } 
    else {
        fmt_fprintf(stderr, "Failed to add columns.\n");
    }

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after adding first row to the second row:
| 1.00000 2.00000 3.00000 |
| 5.00000 7.00000 9.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after adding first column (scaled by 2) to the third column:
|  1.00000  2.00000  5.00000 |
|  5.00000  7.00000 19.00000 |
|  7.00000  8.00000 23.00000 |
```

---

## Example 45 : Apply function for specefic row and col 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

double square(double x) {
    return x * x;
}

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 

    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix:\n");
    matrix_print(matrix);

    // Apply 'square' function to the second row (index 1)
    if (!matrix_apply_to_row(matrix, 1, square)) {
        fmt_fprintf(stderr, "Failed to apply function to row.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying 'square' function to the second row:\n");
    matrix_print(matrix);

    // Apply 'square' function to the first column (index 0)
    if (!matrix_apply_to_col(matrix, 0, square)) {
        fmt_fprintf(stderr, "Failed to apply function to column.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("\nMatrix after applying 'square' function to the first column:\n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix:
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

Matrix after applying 'square' function to the second row:
|  1.00000  2.00000  3.00000 |
| 16.00000 25.00000 36.00000 |
|  7.00000  8.00000  9.00000 |

Matrix after applying 'square' function to the first column:
|   1.00000   2.00000   3.00000 |
| 256.00000  25.00000  36.00000 |
|  49.00000   8.00000   9.00000 |
```

---

## Example 46 : finding minimum and maximum element of a Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {
        -2.5, 3.1, 5.0,
        7.2, -8.6, 1.0,
        4.5, -9.1, 2.3
    };
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Given Matrix:\n");
    matrix_print(matrix);

    double minElement = matrix_min_element(matrix);
    fmt_printf("\nMinimum element in the matrix: %lf\n", minElement);

    double maxElement = matrix_max_element(matrix);
    fmt_printf("Maximum element in the matrix: %lf\n", maxElement);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Given Matrix:
| -2.50000  3.10000  5.00000 |
|  7.20000 -8.60000  1.00000 |
|  4.50000 -9.10000  2.30000 |

Minimum element in the matrix: -9.100000
Maximum element in the matrix: 7.200000
```

---

## Example 47 : create leslie matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double f[] = {0.1, 2.0, 1.0, 0.1}; // Fecundity coefficients
    double s[] = {0.2, 0.8, 0.7};      // Survival coefficients
    size_t f_size = sizeof(f) / sizeof(f[0]);
    size_t s_size = sizeof(s) / sizeof(s[0]);

    Matrix* fl = matrix_from_array(f, 1, 4);
    Matrix* sl = matrix_from_array(s, 1, 3);
    
    if (!fl || !sl) {
        fmt_fprintf(stderr, "Error: can not create Matrix from array fl or sl or both of them");
        return -1;
    }

    Matrix* leslieMatrix = matrix_leslie(fl, f_size, sl, s_size);
    if (leslieMatrix) {
        fmt_printf("Leslie Matrix:\n");
        matrix_print(leslieMatrix);
        matrix_deallocate(leslieMatrix);

    }

    matrix_deallocate(fl);
    matrix_deallocate(sl);
    return 0;
}
```
**Result:**
```
Leslie Matrix:
| 0.10000 2.00000 1.00000 0.10000 |
| 0.20000 0.00000 0.00000 0.00000 |
| 0.00000 0.80000 0.00000 0.00000 |
| 0.00000 0.00000 0.70000 0.00000 |
```

---

## Example 48 : create inverse hilbert Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* invHilbert = matrix_inverse_hilbert(4);

    if (!invHilbert) {
        fmt_fprintf(stderr, "Error: can not create inverse hilbert matrix.\n");
        return -1;
    }
    
    fmt_printf("Inverse Hilbert Matrix : \n");
    matrix_print(invHilbert);

    matrix_deallocate(invHilbert);
    return 0;
}
```
**Result:**
```
Inverse Hilbert Matrix : 
|    16.00000  -120.00000   240.00000  -140.00000 |
|  -120.00000  1200.00000 -2700.00000  1680.00000 |
|   240.00000 -2700.00000  6480.00000 -4200.00000 |
|  -140.00000  1680.00000 -4200.00000  2800.00000 |
```

---

## Example 49 : get row or col of Matrix as Matrix object 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix : \n");
    matrix_print(matrix);

    Matrix* row = matrix_get_row(matrix, 1);
    if (!row) {
        fmt_fprintf(stderr, "Row Matrix is null and invalid.\n");
        matrix_deallocate(matrix);
        return -1;
    }

    fmt_printf("row of matrix : \n");
    matrix_print(row);

    Matrix* col = matrix_get_col(matrix, 2);
    if (!col) {
        fmt_fprintf(stderr, "Col Matrix is null and invalid.\n");
        matrix_deallocate(row);
        matrix_deallocate(matrix);
        return -1;
    };

    fmt_printf("col of matrix : \n");
    matrix_print(col);

    matrix_deallocate(col);
    matrix_deallocate(row);
    matrix_deallocate(matrix);

    return 0;
}
```
**Result:**
```
Original Matrix : 
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
row of matrix :
| 4.00000 5.00000 6.00000 |
col of matrix :
| 3.00000 |
| 6.00000 |
| 9.00000 |
```

---

## Example 50 : Matrix to double Array 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix* matrix = matrix_from_array(values, 3, 3); 
    if (!matrix) {
        fmt_fprintf(stderr, "Failed to create matrix.\n");
        return -1;
    }

    fmt_printf("Original Matrix : \n");
    matrix_print(matrix);

    double* data = matrix_to_array(matrix);
    fmt_printf("\n");

    for (size_t i = 0; i < (matrix->rows * matrix->cols); i++) {
        fmt_printf("%lf ", data[i]);
    }

    free(data);
    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Original Matrix : 
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |

1.000000 2.000000 3.000000 4.000000 5.000000 6.000000 7.000000 8.000000 9.000000
```

---

## Example 51 : get size of Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    double dataA[] = {1, 0, 1, 0}; // 2x2 matrix
    Matrix* A = matrix_from_array(dataA, 2, 2);
    if (!A) {
        fmt_fprintf(stderr, "Failed to create matrix A.\n");
        return -1;
    }

    fmt_printf("Matrix A Size is %zu\n", matrix_size(A));
    matrix_print(A);

    matrix_deallocate(A);
    return 0;
}
```
**Result:**
```
Matrix A Size is 4
| 1.00000 0.00000 |
| 1.00000 0.00000 |
```

---

## Example 52 : create random Matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_random(3, 3, 1, 10);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix random is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix random is : 
| 7.00000 3.00000 7.00000 |
| 1.00000 1.00000 7.00000 |
| 9.00000 5.00000 5.00000 |
```

---

## Example 53 : create walsh matrix 

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main() {
    Matrix* matrix = matrix_walsh(8);

    if (!matrix) {
        fmt_printf("Error in creation matrix object");
        return -1;        
    }

    fmt_printf("Matrix walsh is : \n");
    matrix_print(matrix);

    matrix_deallocate(matrix);
    return 0;
}
```
**Result:**
```
Matrix walsh is : 
|  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000  1.00000 |
|  1.00000 -1.00000  1.00000 -1.00000  1.00000 -1.00000  1.00000 -1.00000 |
|  1.00000  1.00000 -1.00000 -1.00000  1.00000  1.00000 -1.00000 -1.00000 |
|  1.00000 -1.00000 -1.00000  1.00000  1.00000 -1.00000 -1.00000  1.00000 |
|  1.00000  1.00000  1.00000  1.00000 -1.00000 -1.00000 -1.00000 -1.00000 |
|  1.00000 -1.00000  1.00000 -1.00000 -1.00000  1.00000 -1.00000  1.00000 |
|  1.00000  1.00000 -1.00000 -1.00000 -1.00000 -1.00000  1.00000  1.00000 |
|  1.00000 -1.00000 -1.00000  1.00000 -1.00000  1.00000  1.00000 -1.00000 |
```

---

## Example 54 : Solving a linear system with `matrix_solve` (and `matrix_hadamard_product`)

`matrix_solve` solves `A x = b` directly via Gaussian elimination with partial pivoting — the everyday use of a matrix library. Here we solve a 3×3 system with the known solution `(2, 3, -1)`, verify it by reconstructing `A·x`, and show the element-wise (Hadamard) product.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /*   linear system  A x = b:
         2x +  y -  z =   8
        -3x -  y + 2z = -11
        -2x +  y + 2z =  -3
       (known solution: x = 2, y = 3, z = -1)                         */
    double a[] = {
         2,  1, -1,
        -3, -1,  2,
        -2,  1,  2
    };
    double bv[] = { 8, -11, -3 };

    Matrix* A = matrix_from_array(a, 3, 3);
    Matrix* b = matrix_from_array(bv, 3, 1);

    Matrix* x = matrix_solve(A, b);
    if (x) {
        fmt_printf("solution: x=%.0f, y=%.0f, z=%.0f\n", matrix_get(x, 0, 0), matrix_get(x, 1, 0), matrix_get(x, 2, 0));

        /* Verify by reconstructing A*x, which should equal b. */
        Matrix* check = matrix_multiply(A, x);
        fmt_printf("A*x     = [%.0f, %.0f, %.0f]  (== b)\n", matrix_get(check, 0, 0), matrix_get(check, 1, 0), matrix_get(check, 2, 0));

        matrix_deallocate(check);
        matrix_deallocate(x);
    }

    /* Element-wise (Hadamard) product, distinct from matrix_multiply. */
    double p[] = { 1, 2, 3, 4 };
    double q[] = { 5, 6, 7, 8 };
    Matrix* P = matrix_from_array(p, 2, 2);
    Matrix* Q = matrix_from_array(q, 2, 2);
    Matrix* H = matrix_hadamard_product(P, Q);

    fmt_printf("hadamard: [%.0f, %.0f, %.0f, %.0f]\n", matrix_get(H, 0, 0), matrix_get(H, 0, 1), matrix_get(H, 1, 0), matrix_get(H, 1, 1));

    matrix_deallocate(H);
    matrix_deallocate(P);
    matrix_deallocate(Q);
    matrix_deallocate(A);
    matrix_deallocate(b);
    
    return 0;
}
```

**Result**

```
solution: x=2, y=3, z=-1
A*x     = [8, -11, -3]  (== b)
hadamard: [5, 12, 21, 32]
```

---

## Example 55 : all-ones matrices with `matrix_ones` (the C analogue of `numpy.ones`)

`matrix_ones(rows, cols)` returns a matrix in which every element is `1.0`, just like NumPy's `numpy.ones(shape)`. Because this library always stores a 2-D matrix of `double`, NumPy's `dtype` / `order` parameters do not apply. A 1-D NumPy shape such as `np.ones(5)` maps to a row vector `matrix_ones(1, 5)`, while `np.ones((2, 1))` maps to a column vector `matrix_ones(2, 1)`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    Matrix* a = matrix_ones(1, 5);
    fmt_printf("ones(1, 5):\n");
    matrix_print(a);

    Matrix* b = matrix_ones(2, 1);
    fmt_printf("ones(2, 1):\n");
    matrix_print(b);

    Matrix* c = matrix_ones(2, 2);
    fmt_printf("ones(2, 2):\n");
    matrix_print(c);

    matrix_deallocate(a);
    matrix_deallocate(b);
    matrix_deallocate(c);

    return 0;
}
```

**Result**

```
ones(1, 5):
| 1.00000 1.00000 1.00000 1.00000 1.00000 |
ones(2, 1):
| 1.00000 |
| 1.00000 |
ones(2, 2):
| 1.00000 1.00000 |
| 1.00000 1.00000 |
```

---

## Example 56 : all-zeros matrices with `matrix_zeros` (the C analogue of `numpy.zeros`)

`matrix_zeros(rows, cols)` returns a matrix in which every element is `0.0`, just like NumPy's `numpy.zeros(shape)`. As with `matrix_ones`, NumPy's `dtype` / `order` parameters do not apply — the type is always `double` and storage is row-major. A 1-D NumPy shape such as `np.zeros(5)` maps to a row vector `matrix_zeros(1, 5)`, while `np.zeros((2, 1))` maps to a column vector `matrix_zeros(2, 1)`. A zeros matrix is the additive identity, so `Z + M == M`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.zeros(5)      -> a 1x5 row vector of zeros */
    Matrix* a = matrix_zeros(1, 5);
    fmt_printf("zeros(1, 5):\n");
    matrix_print(a);

    /* np.zeros((2, 1)) -> a 2x1 column vector */
    Matrix* b = matrix_zeros(2, 1);
    fmt_printf("zeros(2, 1):\n");
    matrix_print(b);

    /* np.zeros((2, 2)) */
    Matrix* z = matrix_zeros(2, 2);
    fmt_printf("zeros(2, 2):\n");
    matrix_print(z);

    /* zeros is the additive identity: Z + M == M */
    double d[] = { 1, 2, 3, 4 };
    Matrix* m   = matrix_from_array(d, 2, 2);
    Matrix* sum = matrix_add(z, m);
    fmt_printf("zeros + M == M: %s\n", matrix_is_equal(sum, m) ? "true" : "false");

    matrix_deallocate(a);
    matrix_deallocate(b);
    matrix_deallocate(z);
    matrix_deallocate(m);
    matrix_deallocate(sum);

    return 0;
}
```

**Result**

```
zeros(1, 5):
| 0.00000 0.00000 0.00000 0.00000 0.00000 |
zeros(2, 1):
| 0.00000 |
| 0.00000 |
zeros(2, 2):
| 0.00000 0.00000 |
| 0.00000 0.00000 |
zeros + M == M: true
```

---

## Example 57 : element-wise math — `matrix_abs`, `matrix_negate`, `matrix_scalar_add`, `matrix_divide`, `matrix_sqrt`

These five functions are the C analogues of NumPy's `numpy.abs`, `numpy.negative`, `numpy.add(M, c)`, `numpy.divide`, and `numpy.sqrt`. Each returns a **new** matrix and leaves its input unchanged. This example applies them to one matrix `M` and prints every result — note that `M / abs(M)` reproduces the element-wise sign, and `sqrt(abs(M))` the element-wise root.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double data[] = {  -4.0,  9.0,
                      -16.0, 25.0 };
    Matrix* M = matrix_from_array(data, 2, 2);
    fmt_printf("M =\n");
    matrix_print(M);

    /* numpy.abs(M) */
    Matrix* a = matrix_abs(M);
    fmt_printf("abs(M) =\n");
    matrix_print(a);

    /* numpy.negative(M) */
    Matrix* n = matrix_negate(M);
    fmt_printf("negate(M) =\n");
    matrix_print(n);

    /* numpy.add(M, 10) */
    Matrix* s = matrix_scalar_add(M, 10.0);
    fmt_printf("M + 10 =\n");
    matrix_print(s);

    /* numpy.sqrt(abs(M)) */
    Matrix* r = matrix_sqrt(a);
    fmt_printf("sqrt(abs(M)) =\n");
    matrix_print(r);

    /* numpy.divide(M, abs(M)) -> element-wise sign (+/-1) */
    Matrix* d = matrix_divide(M, a);
    fmt_printf("M / abs(M) =\n");
    matrix_print(d);

    matrix_deallocate(M);
    matrix_deallocate(a);
    matrix_deallocate(n);
    matrix_deallocate(s);
    matrix_deallocate(r);
    matrix_deallocate(d);

    return 0;
}
```

**Result**

```
M =
|  -4.00000   9.00000 |
| -16.00000  25.00000 |
abs(M) =
|  4.00000  9.00000 |
| 16.00000 25.00000 |
negate(M) =
|   4.00000  -9.00000 |
|  16.00000 -25.00000 |
M + 10 =
|  6.00000 19.00000 |
| -6.00000 35.00000 |
sqrt(abs(M)) =
| 2.00000 3.00000 |
| 4.00000 5.00000 |
M / abs(M) =
| -1.00000  1.00000 |
| -1.00000  1.00000 |
```

---

## Example 58 : `matrix_sign`, `matrix_reciprocal`, `matrix_clip`

The C analogues of NumPy's `numpy.sign`, `numpy.reciprocal`, and `numpy.clip`. Each returns a **new** matrix and leaves its input unchanged: `sign` maps every element to `-1`/`0`/`+1`, `reciprocal` to `1/x`, and `clip` clamps every element into `[min, max]`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* numpy.sign : -1 / 0 / +1 per element */
    double sd[] = { -3.0,  0.0, 2.0,
                     5.0, -8.0, 0.5 };
    Matrix* S  = matrix_from_array(sd, 2, 3);
    Matrix* sg = matrix_sign(S);
    fmt_printf("sign(S) =\n");
    matrix_print(sg);

    /* numpy.reciprocal : 1/x per element */
    double rd[] = { 1.0, 2.0, 4.0, 0.5 };
    Matrix* R  = matrix_from_array(rd, 2, 2);
    Matrix* rc = matrix_reciprocal(R);
    fmt_printf("reciprocal(R) =\n");
    matrix_print(rc);

    /* numpy.clip : clamp every element to [0, 6] */
    double cd[] = { 1.0, 5.0, 8.0, -2.0 };
    Matrix* C  = matrix_from_array(cd, 2, 2);
    Matrix* cl = matrix_clip(C, 0.0, 6.0);
    fmt_printf("clip(C, 0, 6) =\n");
    matrix_print(cl);

    matrix_deallocate(S); matrix_deallocate(sg);
    matrix_deallocate(R); matrix_deallocate(rc);
    matrix_deallocate(C); matrix_deallocate(cl);

    return 0;
}
```

**Result**

```
sign(S) =
| -1.00000  0.00000  1.00000 |
|  1.00000 -1.00000  1.00000 |
reciprocal(R) =
| 1.00000 0.50000 |
| 0.25000 2.00000 |
clip(C, 0, 6) =
| 1.00000 5.00000 |
| 6.00000 0.00000 |
```

---

## Example 59 : the rounding family — `matrix_round`, `matrix_floor`, `matrix_ceil`

The C analogues of NumPy's `numpy.round`, `numpy.floor`, and `numpy.ceil`. Note that `matrix_round` uses **banker's rounding** (round half to even), exactly like `np.round`: in the row below `2.5` rounds to `2` (nearest even) and `3.5` rounds to `4` — not `3` and `4` as round-half-up would give.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double data[] = { 2.3, 2.5, 2.7, 3.5 };
    Matrix* M = matrix_from_array(data, 1, 4);
    fmt_printf("M =\n");
    matrix_print(M);

    /* numpy.round : ties round to even -> 2.5 becomes 2, 3.5 becomes 4 */
    Matrix* rd = matrix_round(M);
    fmt_printf("round(M) =\n");
    matrix_print(rd);

    /* numpy.floor */
    Matrix* fl = matrix_floor(M);
    fmt_printf("floor(M) =\n");
    matrix_print(fl);

    /* numpy.ceil */
    Matrix* ce = matrix_ceil(M);
    fmt_printf("ceil(M) =\n");
    matrix_print(ce);

    matrix_deallocate(M);
    matrix_deallocate(rd);
    matrix_deallocate(fl);
    matrix_deallocate(ce);

    return 0;
}
```

**Result**

```
M =
| 2.30000 2.50000 2.70000 3.50000 |
round(M) =
| 2.00000 2.00000 3.00000 4.00000 |
floor(M) =
| 2.00000 2.00000 2.00000 3.00000 |
ceil(M) =
| 3.00000 3.00000 3.00000 4.00000 |
```

---

## Example 60 : whole-matrix reductions — `matrix_sum`, `matrix_prod`, `matrix_mean`, `matrix_ptp`, `matrix_argmax`, `matrix_argmin`

These are the C analogues of NumPy's `numpy.sum`, `numpy.prod`, `numpy.mean`, `numpy.ptp`, `numpy.argmax`, and `numpy.argmin` applied to the whole matrix. The `arg*` functions return a flattened, row-major (C-order) index, exactly like NumPy: here the maximum `6.0` sits at flat index `5` and the minimum `1.0` at index `0`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M =\n");
    matrix_print(M);

    fmt_printf("sum    = %.1f\n", matrix_sum(M));
    fmt_printf("prod   = %.1f\n", matrix_prod(M));
    fmt_printf("mean   = %.1f\n", matrix_mean(M));
    fmt_printf("ptp    = %.1f\n", matrix_ptp(M));
    fmt_printf("argmax = %d\n", (int)matrix_argmax(M));
    fmt_printf("argmin = %d\n", (int)matrix_argmin(M));

    matrix_deallocate(M);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
sum    = 21.0
prod   = 720.0
mean   = 3.5
ptp    = 5.0
argmax = 5
argmin = 0
```

---

## Example 61 : per-axis reductions — `matrix_sum_axis`, `matrix_mean_axis`, `matrix_prod_axis`

The C analogues of NumPy's `numpy.sum(M, axis=...)`, `numpy.mean(M, axis=...)`, and `numpy.prod(M, axis=...)`. As in NumPy, `axis=0` reduces **down the columns** (yielding a `1 × cols` row vector) and `axis=1` reduces **across the rows** (yielding a `rows × 1` column vector).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.sum(M, axis=0): column sums (1x3) */
    Matrix* s0 = matrix_sum_axis(M, 0);
    fmt_printf("sum axis=0 (column sums) =\n");
    matrix_print(s0);

    /* np.sum(M, axis=1): row sums (2x1) */
    Matrix* s1 = matrix_sum_axis(M, 1);
    fmt_printf("sum axis=1 (row sums) =\n");
    matrix_print(s1);

    /* np.mean(M, axis=0): column means (1x3) */
    Matrix* m0 = matrix_mean_axis(M, 0);
    fmt_printf("mean axis=0 (column means) =\n");
    matrix_print(m0);

    /* np.prod(M, axis=1): row products (2x1) */
    Matrix* p1 = matrix_prod_axis(M, 1);
    fmt_printf("prod axis=1 (row products) =\n");
    matrix_print(p1);

    matrix_deallocate(M);
    matrix_deallocate(s0);
    matrix_deallocate(s1);
    matrix_deallocate(m0);
    matrix_deallocate(p1);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
sum axis=0 (column sums) =
| 5.00000 7.00000 9.00000 |
sum axis=1 (row sums) =
|  6.00000 |
| 15.00000 |
mean axis=0 (column means) =
| 2.50000 3.50000 4.50000 |
prod axis=1 (row products) =
|   6.00000 |
| 120.00000 |
```

---

## Example 62 : `matrix_eye` (rectangular / offset identity) and `matrix_count_nonzero`

`matrix_eye(rows, cols, k)` is the C analogue of NumPy's `numpy.eye(N, M, k)` — a rectangular identity whose `1`s sit on the `k`-th diagonal. `matrix_count_nonzero` (the analogue of `numpy.count_nonzero`) then confirms each one holds exactly three ones.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.eye(3) : square identity */
    Matrix* a = matrix_eye(3, 3, 0);
    fmt_printf("eye(3, 3, 0) =\n");
    matrix_print(a);

    /* np.eye(3, 4) : rectangular, ones on the main diagonal */
    Matrix* b = matrix_eye(3, 4, 0);
    fmt_printf("eye(3, 4, 0) =\n");
    matrix_print(b);

    /* np.eye(4, 4, k=1) : ones on the first super-diagonal */
    Matrix* c = matrix_eye(4, 4, 1);
    fmt_printf("eye(4, 4, 1) =\n");
    matrix_print(c);

    /* count_nonzero : how many ones does each contain? */
    fmt_printf("count_nonzero: eye(3,3,0)=%d, eye(3,4,0)=%d, eye(4,4,1)=%d\n",
               (int)matrix_count_nonzero(a),
               (int)matrix_count_nonzero(b),
               (int)matrix_count_nonzero(c));

    matrix_deallocate(a);
    matrix_deallocate(b);
    matrix_deallocate(c);

    return 0;
}
```

**Result**

```
eye(3, 3, 0) =
| 1.00000 0.00000 0.00000 |
| 0.00000 1.00000 0.00000 |
| 0.00000 0.00000 1.00000 |
eye(3, 4, 0) =
| 1.00000 0.00000 0.00000 0.00000 |
| 0.00000 1.00000 0.00000 0.00000 |
| 0.00000 0.00000 1.00000 0.00000 |
eye(4, 4, 1) =
| 0.00000 1.00000 0.00000 0.00000 |
| 0.00000 0.00000 1.00000 0.00000 |
| 0.00000 0.00000 0.00000 1.00000 |
| 0.00000 0.00000 0.00000 0.00000 |
count_nonzero: eye(3,3,0)=3, eye(3,4,0)=3, eye(4,4,1)=3
```

---

## Example 63 : `matrix_cumsum` / `matrix_cumsum_axis` and `matrix_maximum` / `matrix_minimum`

The C analogues of NumPy's `numpy.cumsum` (flattened and per-axis) and `numpy.maximum` / `numpy.minimum` (element-wise over two matrices). Note `matrix_cumsum` flattens in row-major order to a `1 × 6` vector, while `matrix_cumsum_axis(M, 1)` keeps the shape and accumulates across each row.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.cumsum(M) : flattened running total (1x6) */
    Matrix* cs = matrix_cumsum(M);
    fmt_printf("cumsum (flattened) =\n");
    matrix_print(cs);

    /* np.cumsum(M, axis=1) : running total across each row */
    Matrix* c1 = matrix_cumsum_axis(M, 1);
    fmt_printf("cumsum axis=1 (across rows) =\n");
    matrix_print(c1);

    /* np.maximum / np.minimum : element-wise over two matrices */
    double ad[] = { 1.0, 5.0, 3.0,
                    8.0, 2.0, 6.0 };
    double bd[] = { 4.0, 2.0, 9.0,
                    1.0, 7.0, 6.0 };
                    
    Matrix* A = matrix_from_array(ad, 2, 3);
    Matrix* B = matrix_from_array(bd, 2, 3);
    Matrix* mx = matrix_maximum(A, B);
    Matrix* mn = matrix_minimum(A, B);

    fmt_printf("maximum(A, B) =\n");
    matrix_print(mx);

    fmt_printf("minimum(A, B) =\n");
    matrix_print(mn);

    matrix_deallocate(M);
    matrix_deallocate(cs);
    matrix_deallocate(c1);
    matrix_deallocate(A);
    matrix_deallocate(B);
    matrix_deallocate(mx);
    matrix_deallocate(mn);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
cumsum (flattened) =
|  1.00000  3.00000  6.00000 10.00000 15.00000 21.00000 |
cumsum axis=1 (across rows) =
|  1.00000  3.00000  6.00000 |
|  4.00000  9.00000 15.00000 |
maximum(A, B) =
| 4.00000 5.00000 9.00000 |
| 8.00000 7.00000 6.00000 |
minimum(A, B) =
| 1.00000 2.00000 3.00000 |
| 1.00000 2.00000 6.00000 |
```

---

## Example 64 : reshaping & flipping — `matrix_reshape`, `matrix_flipud`, `matrix_fliplr`, `matrix_rot90`

The C analogues of NumPy's `numpy.reshape`, `numpy.flipud`, `numpy.fliplr`, and `numpy.rot90`. Each returns a **new** matrix and leaves its input unchanged. `reshape` preserves the row-major element order; `rot90(M, 1)` rotates 90° counterclockwise, turning the `2 × 3` matrix into a `3 × 2` one.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M (2x3) =\n");
    matrix_print(M);

    /* np.reshape(M, (3, 2)) : same elements, row-major order */
    Matrix* r = matrix_reshape(M, 3, 2);
    fmt_printf("reshape(3, 2) =\n");
    matrix_print(r);

    /* np.flipud(M) : reverse row order */
    Matrix* fu = matrix_flipud(M);
    fmt_printf("flipud(M) =\n");
    matrix_print(fu);

    /* np.fliplr(M) : reverse column order */
    Matrix* fl = matrix_fliplr(M);
    fmt_printf("fliplr(M) =\n");
    matrix_print(fl);

    /* np.rot90(M) : 90 degrees counterclockwise (2x3 -> 3x2) */
    Matrix* q = matrix_rot90(M, 1);
    fmt_printf("rot90(M, 1) =\n");
    matrix_print(q);

    matrix_deallocate(M);
    matrix_deallocate(r);
    matrix_deallocate(fu);
    matrix_deallocate(fl);
    matrix_deallocate(q);

    return 0;
}
```

**Result**

```
M (2x3) =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
reshape(3, 2) =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
flipud(M) =
| 4.00000 5.00000 6.00000 |
| 1.00000 2.00000 3.00000 |
fliplr(M) =
| 3.00000 2.00000 1.00000 |
| 6.00000 5.00000 4.00000 |
rot90(M, 1) =
| 3.00000 6.00000 |
| 2.00000 5.00000 |
| 1.00000 4.00000 |
```

---

## Example 65 : `matrix_roll` / `matrix_roll_axis` and `matrix_tril` / `matrix_triu`

The C analogues of NumPy's `numpy.roll` (flattened and per-axis) and `numpy.tril` / `numpy.triu`. `roll(T, 1)` rotates the flattened sequence by one (so the last element `9` wraps to the front), `roll(T, 1, axis=0)` shifts whole rows down, and `tril`/`triu` keep the lower/upper triangle.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double td[] = { 1.0, 2.0, 3.0,
                    4.0, 5.0, 6.0,
                    7.0, 8.0, 9.0 };
    Matrix* T = matrix_from_array(td, 3, 3);
    fmt_printf("T =\n");
    matrix_print(T);

    /* np.roll(T, 1) : flattened cyclic shift by 1 */
    Matrix* r = matrix_roll(T, 1);
    fmt_printf("roll(T, 1) =\n");
    matrix_print(r);

    /* np.roll(T, 1, axis=0) : shift rows down by 1 */
    Matrix* ra = matrix_roll_axis(T, 1, 0);
    fmt_printf("roll(T, 1, axis=0) =\n");
    matrix_print(ra);

    /* np.tril(T) : lower-triangular part */
    Matrix* lo = matrix_tril(T, 0);
    fmt_printf("tril(T) =\n");
    matrix_print(lo);

    /* np.triu(T) : upper-triangular part */
    Matrix* up = matrix_triu(T, 0);
    fmt_printf("triu(T) =\n");
    matrix_print(up);

    matrix_deallocate(T);
    matrix_deallocate(r);
    matrix_deallocate(ra);
    matrix_deallocate(lo);
    matrix_deallocate(up);

    return 0;
}
```

**Result**

```
T =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
roll(T, 1) =
| 9.00000 1.00000 2.00000 |
| 3.00000 4.00000 5.00000 |
| 6.00000 7.00000 8.00000 |
roll(T, 1, axis=0) =
| 7.00000 8.00000 9.00000 |
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
tril(T) =
| 1.00000 0.00000 0.00000 |
| 4.00000 5.00000 0.00000 |
| 7.00000 8.00000 9.00000 |
triu(T) =
| 1.00000 2.00000 3.00000 |
| 0.00000 5.00000 6.00000 |
| 0.00000 0.00000 9.00000 |
```

---

## Example 66 : combining matrices — `matrix_hstack` and `matrix_vstack`

The C analogues of NumPy's `numpy.hstack` and `numpy.vstack`. `hstack` joins matrices side by side (same row count); `vstack` stacks them one above the other (same column count).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double ad[] = { 1.0, 2.0, 3.0, 4.0 };
    Matrix* A = matrix_from_array(ad, 2, 2);

    fmt_printf("A =\n");
    matrix_print(A);

    double bd[] = { 5.0, 6.0 };
    Matrix* B = matrix_from_array(bd, 2, 1);

    fmt_printf("B =\n");
    matrix_print(B);

    /* np.hstack((A, B)) : B to the right of A */
    Matrix* h = matrix_hstack(A, B);
    fmt_printf("hstack(A, B) =\n");
    matrix_print(h);

    double cd[] = { 7.0, 8.0 };
    Matrix* C = matrix_from_array(cd, 1, 2);
    fmt_printf("C =\n");
    matrix_print(C);

    /* np.vstack((A, C)) : C below A */
    Matrix* v = matrix_vstack(A, C);
    fmt_printf("vstack(A, C) =\n");
    matrix_print(v);

    matrix_deallocate(A);
    matrix_deallocate(B);
    matrix_deallocate(C);
    matrix_deallocate(h);
    matrix_deallocate(v);

    return 0;
}
```

**Result**

```
A =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
B =
| 5.00000 |
| 6.00000 |
hstack(A, B) =
| 1.00000 2.00000 5.00000 |
| 3.00000 4.00000 6.00000 |
C =
| 7.00000 8.00000 |
vstack(A, C) =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 7.00000 8.00000 |
```

---

## Example 67 : replication & padding — `matrix_tile`, `matrix_repeat`, `matrix_repeat_axis`, `matrix_pad`

The C analogues of NumPy's `numpy.tile`, `numpy.repeat` (flattened and per-axis), and `numpy.pad`. `tile` repeats the whole matrix in a grid, `repeat` duplicates each element, and `pad` frames the matrix with a constant border.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0,
                   3.0, 4.0 };
    Matrix* M = matrix_from_array(d, 2, 2);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.tile(M, (2, 2)) : repeat the whole matrix in a 2x2 grid */
    Matrix* t = matrix_tile(M, 2, 2);
    fmt_printf("tile(M, 2, 2) =\n");
    matrix_print(t);

    /* np.repeat(M, 2) : flatten, repeat each element twice */
    Matrix* rp = matrix_repeat(M, 2);
    fmt_printf("repeat(M, 2) =\n");
    matrix_print(rp);

    /* np.repeat(M, 2, axis=0) : repeat each row */
    Matrix* rr = matrix_repeat_axis(M, 2, 0);
    fmt_printf("repeat(M, 2, axis=0) =\n");
    matrix_print(rr);

    /* np.pad(M, 1, constant_values=0) : zero border */
    Matrix* p = matrix_pad(M, 1, 0.0);
    fmt_printf("pad(M, 1, 0) =\n");
    matrix_print(p);

    matrix_deallocate(M);
    matrix_deallocate(t);
    matrix_deallocate(rp);
    matrix_deallocate(rr);
    matrix_deallocate(p);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
tile(M, 2, 2) =
| 1.00000 2.00000 1.00000 2.00000 |
| 3.00000 4.00000 3.00000 4.00000 |
| 1.00000 2.00000 1.00000 2.00000 |
| 3.00000 4.00000 3.00000 4.00000 |
repeat(M, 2) =
| 1.00000 1.00000 2.00000 2.00000 3.00000 3.00000 4.00000 4.00000 |
repeat(M, 2, axis=0) =
| 1.00000 2.00000 |
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 3.00000 4.00000 |
pad(M, 1, 0) =
| 0.00000 0.00000 0.00000 0.00000 |
| 0.00000 1.00000 2.00000 0.00000 |
| 0.00000 3.00000 4.00000 0.00000 |
| 0.00000 0.00000 0.00000 0.00000 |
```

---

## Example 68 : statistics — `matrix_var`, `matrix_std`, `matrix_median`

The C analogues of NumPy's `numpy.var`, `numpy.std`, and `numpy.median`. `var`/`std` use the population definition (`ddof = 0`), exactly like NumPy's defaults — these results also match the `statistics` library's `statistics_pvariance` / `statistics_pstdev` / `statistics_median`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 3.0, 1.0, 2.0,
                   6.0, 5.0, 4.0 };
    Matrix* S = matrix_from_array(d, 2, 3);
    fmt_printf("S =\n");
    matrix_print(S);

    fmt_printf("var    = %.6f\n", matrix_var(S));
    fmt_printf("std    = %.6f\n", matrix_std(S));
    fmt_printf("median = %.1f\n", matrix_median(S));

    matrix_deallocate(S);

    return 0;
}
```

**Result**

```
S =
| 3.00000 1.00000 2.00000 |
| 6.00000 5.00000 4.00000 |
var    = 2.916667
std    = 1.707825
median = 3.5
```

---

## Example 69 : sorting — `matrix_sort` and `matrix_argsort`

The C analogues of NumPy's `numpy.sort` and `numpy.argsort` along an axis. `sort(S, 1)` sorts each row ascending; `argsort(S, 1)` gives the per-row indices that would sort it (so row `[3, 1, 2]` → `[1, 2, 0]`: the smallest value `1` is at index `1`, then `2` at index `2`, then `3` at index `0`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 3.0, 1.0, 2.0,
                   6.0, 5.0, 4.0 };
    Matrix* S = matrix_from_array(d, 2, 3);
    fmt_printf("S =\n");
    matrix_print(S);

    /* np.sort(S, axis=1) : sort each row ascending */
    Matrix* s = matrix_sort(S, 1);
    fmt_printf("sort(S, axis=1) =\n");
    matrix_print(s);

    /* np.argsort(S, axis=1) : indices that sort each row */
    Matrix* a = matrix_argsort(S, 1);
    fmt_printf("argsort(S, axis=1) =\n");
    matrix_print(a);

    matrix_deallocate(S);
    matrix_deallocate(s);
    matrix_deallocate(a);

    return 0;
}
```

**Result**

```
S =
| 3.00000 1.00000 2.00000 |
| 6.00000 5.00000 4.00000 |
sort(S, axis=1) =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
argsort(S, axis=1) =
| 1.00000 2.00000 0.00000 |
| 2.00000 1.00000 0.00000 |
```

---

## Example 70 : `matrix_diff` and `matrix_allclose`

The C analogues of NumPy's `numpy.diff` (discrete difference between adjacent elements) and `numpy.allclose` (approximate equality within a tolerance — unlike the exact `matrix_is_equal`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 4.0, 7.0,
                   1.0, 3.0, 6.0, 10.0 };
    Matrix* M = matrix_from_array(d, 2, 4);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.diff(M, axis=1) : difference between adjacent columns */
    Matrix* df = matrix_diff(M, 1);
    fmt_printf("diff(M, axis=1) =\n");
    matrix_print(df);

    /* np.allclose : approximate equality within a tolerance */
    double a[] = { 1.0, 2.0, 3.0 };
    double b[] = { 1.0000001, 2.0, 3.0 };
    Matrix* A = matrix_from_array(a, 1, 3);
    Matrix* B = matrix_from_array(b, 1, 3);
    bool close = matrix_allclose(A, B, 1e-5, 1e-8);
    fmt_printf("allclose(A, B, 1e-5, 1e-8) = %s\n", close ? "true" : "false");

    matrix_deallocate(M);
    matrix_deallocate(df);
    matrix_deallocate(A);
    matrix_deallocate(B);

    return 0;
}
```

**Result**

```
M =
|  1.00000  2.00000  4.00000  7.00000 |
|  1.00000  3.00000  6.00000 10.00000 |
diff(M, axis=1) =
| 1.00000 2.00000 3.00000 |
| 2.00000 3.00000 4.00000 |
allclose(A, B, 1e-5, 1e-8) = true
```

---

## Example 71 : vector products — `matrix_outer`, `matrix_cross`, `matrix_convolve`

The C analogues of NumPy's `numpy.outer`, `numpy.cross` (3-D), and `numpy.convolve` (full mode). `outer` builds a rank-1 matrix, `cross` returns the perpendicular vector, and `convolve` slides one sequence over another.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.outer([1,2,3], [4,5]) */
    double a[] = { 1.0, 2.0, 3.0 };
    double b[] = { 4.0, 5.0 };
    Matrix* A = matrix_from_array(a, 1, 3);
    Matrix* B = matrix_from_array(b, 1, 2);
    Matrix* o = matrix_outer(A, B);
    fmt_printf("outer([1,2,3], [4,5]) =\n");
    matrix_print(o);

    /* np.cross([1,2,3], [4,5,6]) */
    double u[] = { 1.0, 2.0, 3.0 };
    double v[] = { 4.0, 5.0, 6.0 };
    Matrix* U = matrix_from_array(u, 1, 3);
    Matrix* V = matrix_from_array(v, 1, 3);
    Matrix* c = matrix_cross(U, V);
    fmt_printf("cross([1,2,3], [4,5,6]) =\n");
    matrix_print(c);

    /* np.convolve([1,2,3], [0,1,0.5]) */
    double p[] = { 1.0, 2.0, 3.0 };
    double q[] = { 0.0, 1.0, 0.5 };
    Matrix* P = matrix_from_array(p, 1, 3);
    Matrix* Q = matrix_from_array(q, 1, 3);
    Matrix* cv = matrix_convolve(P, Q);
    fmt_printf("convolve([1,2,3], [0,1,0.5]) =\n");
    matrix_print(cv);

    matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(o);
    matrix_deallocate(U); matrix_deallocate(V); matrix_deallocate(c);
    matrix_deallocate(P); matrix_deallocate(Q); matrix_deallocate(cv);

    return 0;
}
```

**Result**

```
outer([1,2,3], [4,5]) =
|  4.00000  5.00000 |
|  8.00000 10.00000 |
| 12.00000 15.00000 |
cross([1,2,3], [4,5,6]) =
| -3.00000  6.00000 -3.00000 |
convolve([1,2,3], [0,1,0.5]) =
| 0.00000 1.00000 2.50000 4.00000 1.50000 |
```

---

## Example 72 : `matrix_cov` and `matrix_corrcoef`

The C analogues of NumPy's `numpy.cov` and `numpy.corrcoef`. As in NumPy (`rowvar=True`), each **row** is a variable and each **column** an observation, so a `2 × 3` matrix yields a `2 × 2` covariance/correlation matrix.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* two variables (rows), three observations (cols) */
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 7.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M (2 variables, 3 observations) =\n");
    matrix_print(M);

    /* np.cov(M) : covariance matrix */
    Matrix* cov = matrix_cov(M);
    fmt_printf("cov(M) =\n");
    matrix_print(cov);

    /* np.corrcoef(M) : Pearson correlation matrix */
    Matrix* cc = matrix_corrcoef(M);
    fmt_printf("corrcoef(M) =\n");
    matrix_print(cc);

    matrix_deallocate(M);
    matrix_deallocate(cov);
    matrix_deallocate(cc);

    return 0;
}
```

**Result**

```
M (2 variables, 3 observations) =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 7.00000 |
cov(M) =
| 1.00000 1.50000 |
| 1.50000 2.33333 |
corrcoef(M) =
| 1.00000 0.98198 |
| 0.98198 1.00000 |
```

---

## Example 73 : `matrix_eig` and `matrix_svd`

The C analogues of NumPy's `numpy.linalg.eig` (for a symmetric matrix → real eigenvalues, via Jacobi) and `numpy.linalg.svd` (thin SVD, via one-sided Jacobi). Eigenvalues come back ascending and singular values descending, matching NumPy. (Eigen/singular *vectors* are unique only up to sign, so this example prints the values, which are unique.)

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.linalg.eig of a symmetric matrix (real eigenvalues) */
    double sd[] = { 2.0, 1.0,
                    1.0, 2.0 };
    Matrix* S = matrix_from_array(sd, 2, 2);
    fmt_printf("S =\n");
    matrix_print(S);

    Matrix* vals = NULL;
    Matrix* vecs = NULL;
    if (matrix_eig(S, &vals, &vecs)) {
        fmt_printf("eigenvalues (ascending) =\n");
        matrix_print(vals);
    }

    /* np.linalg.svd of a 3x2 matrix (thin SVD) */
    double ad[] = { 1.0, 2.0,
                    3.0, 4.0,
                    5.0, 6.0 };
    Matrix* A = matrix_from_array(ad, 3, 2);
    fmt_printf("A =\n");
    matrix_print(A);

    Matrix* U = NULL;
    Matrix* Sg = NULL;
    Matrix* V = NULL;
    if (matrix_svd(A, &U, &Sg, &V)) {
        fmt_printf("singular values =\n");
        matrix_print(Sg);
    }

    matrix_deallocate(S); matrix_deallocate(vals); matrix_deallocate(vecs);
    matrix_deallocate(A); matrix_deallocate(U); matrix_deallocate(Sg); matrix_deallocate(V);

    return 0;
}
```

**Result**

```
S =
| 2.00000 1.00000 |
| 1.00000 2.00000 |
eigenvalues (ascending) =
| 1.00000 |
| 3.00000 |
A =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
singular values =
| 9.52552 |
| 0.51430 |
```

---

## Example 74 : `matrix_pinv`, `matrix_cond`, `matrix_lstsq`

The C analogues of NumPy's `numpy.linalg.pinv` (Moore–Penrose pseudo-inverse), `numpy.linalg.cond` (2-norm condition number), and `numpy.linalg.lstsq` (least-squares solution). `pinv` turns a `3 × 2` matrix into its `2 × 3` pseudo-inverse; `lstsq` fits `A·x = b` in the least-squares sense.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double ad[] = { 1.0, 2.0,
                    3.0, 4.0,
                    5.0, 6.0 };
    Matrix* A = matrix_from_array(ad, 3, 2);
    fmt_printf("A =\n");
    matrix_print(A);

    /* np.linalg.pinv(A) : Moore-Penrose pseudo-inverse (2x3) */
    Matrix* P = matrix_pinv(A);
    fmt_printf("pinv(A) =\n");
    matrix_print(P);

    /* np.linalg.cond(A) : 2-norm condition number */
    fmt_printf("cond(A) = %.5f\n", matrix_cond(A));

    /* np.linalg.lstsq : least-squares solution of A x = b */
    double al[] = { 1.0, 1.0,
                    1.0, 2.0,
                    1.0, 3.0 };
    double bl[] = { 1.0, 2.0, 2.0 };
    Matrix* AL = matrix_from_array(al, 3, 2);
    Matrix* BL = matrix_from_array(bl, 3, 1);
    Matrix* x = matrix_lstsq(AL, BL);
    fmt_printf("lstsq solution =\n");
    matrix_print(x);

    matrix_deallocate(A); matrix_deallocate(P);
    matrix_deallocate(AL); matrix_deallocate(BL); matrix_deallocate(x);

    return 0;
}
```

**Result**

```
A =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
pinv(A) =
| -1.33333 -0.33333  0.66667 |
|  1.08333  0.33333 -0.41667 |
cond(A) = 18.52131
lstsq solution =
| 0.66667 |
| 0.50000 |
```

---

## Example 75 : powers, roots & logs — `matrix_square`, `matrix_cbrt`, `matrix_exp`, `matrix_log`, `matrix_log10`

The C analogues of NumPy's `numpy.square`, `numpy.cbrt`, `numpy.exp`, `numpy.log`, and `numpy.log10`. Note `cbrt` accepts negatives (where `sqrt` would give `nan`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.square([1,2,3,4]) */
    double sd[] = { 1.0, 2.0, 3.0, 4.0 };
    Matrix* S = matrix_from_array(sd, 1, 4);
    Matrix* sq = matrix_square(S);
    fmt_printf("square([1,2,3,4]) =\n");
    matrix_print(sq);

    /* np.cbrt([8,27,-8,-27]) : negatives valid, unlike sqrt */
    double cd[] = { 8.0, 27.0, -8.0, -27.0 };
    Matrix* C = matrix_from_array(cd, 1, 4);
    Matrix* cb = matrix_cbrt(C);
    fmt_printf("cbrt([8,27,-8,-27]) =\n");
    matrix_print(cb);

    /* np.exp([0,1,2]) */
    double ed[] = { 0.0, 1.0, 2.0 };
    Matrix* E = matrix_from_array(ed, 1, 3);
    Matrix* ex = matrix_exp(E);
    fmt_printf("exp([0,1,2]) =\n");
    matrix_print(ex);

    /* np.log([1,10,100]) */
    double ld[] = { 1.0, 10.0, 100.0 };
    Matrix* L = matrix_from_array(ld, 1, 3);
    Matrix* lg = matrix_log(L);
    fmt_printf("log([1,10,100]) =\n");
    matrix_print(lg);

    /* np.log10([1,10,100,1000]) */
    double l10[] = { 1.0, 10.0, 100.0, 1000.0 };
    Matrix* L10 = matrix_from_array(l10, 1, 4);
    Matrix* lg10 = matrix_log10(L10);
    fmt_printf("log10([1,10,100,1000]) =\n");
    matrix_print(lg10);

    matrix_deallocate(S); matrix_deallocate(sq);
    matrix_deallocate(C); matrix_deallocate(cb);
    matrix_deallocate(E); matrix_deallocate(ex);
    matrix_deallocate(L); matrix_deallocate(lg);
    matrix_deallocate(L10); matrix_deallocate(lg10);

    return 0;
}
```

**Result**

```
square([1,2,3,4]) =
|  1.00000  4.00000  9.00000 16.00000 |
cbrt([8,27,-8,-27]) =
|  2.00000  3.00000 -2.00000 -3.00000 |
exp([0,1,2]) =
| 1.00000 2.71828 7.38906 |
log([1,10,100]) =
| 0.00000 2.30259 4.60517 |
log10([1,10,100,1000]) =
| 0.00000 1.00000 2.00000 3.00000 |
```

---

## Example 76 : trig, power & modulo — `matrix_sin`, `matrix_cos`, `matrix_tan`, `matrix_pow`, `matrix_mod`, `matrix_trunc`

The C analogues of NumPy's `numpy.sin`/`cos`/`tan`, `numpy.power` (element-wise `a^b`), `numpy.mod`, and `numpy.trunc`. Note `matrix_mod` follows the **divisor's** sign like NumPy (`mod(-3, 5) = 2`), not C's `fmod`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

#define PI 3.14159265358979323846

int main(void) {
    /* np.sin / np.cos at 0, pi/2, pi */
    double td[] = { 0.0, PI/2.0, PI };
    Matrix* T = matrix_from_array(td, 1, 3);
    Matrix* sn = matrix_sin(T);
    Matrix* cs = matrix_cos(T);
    fmt_printf("sin([0, pi/2, pi]) =\n");
    matrix_print(sn);
    fmt_printf("cos([0, pi/2, pi]) =\n");
    matrix_print(cs);

    /* np.tan at 0, pi/4 */
    double tnd[] = { 0.0, PI/4.0 };
    Matrix* TN = matrix_from_array(tnd, 1, 2);
    Matrix* tn = matrix_tan(TN);
    fmt_printf("tan([0, pi/4]) =\n");
    matrix_print(tn);

    /* np.power(A, B) : element-wise a^b */
    double pa[] = { 2.0, 3.0, 4.0 };
    double pb[] = { 3.0, 2.0, 0.5 };
    Matrix* PA = matrix_from_array(pa, 1, 3);
    Matrix* PB = matrix_from_array(pb, 1, 3);
    Matrix* pw = matrix_pow(PA, PB);
    fmt_printf("power([2,3,4], [3,2,0.5]) =\n");
    matrix_print(pw);

    /* np.mod : sign follows the divisor (unlike C fmod) */
    double ma[] = { -3.0, 7.0, -7.0, 3.0 };
    double mb[] = {  5.0, 5.0, -5.0, -5.0 };
    Matrix* MA = matrix_from_array(ma, 2, 2);
    Matrix* MB = matrix_from_array(mb, 2, 2);
    Matrix* mo = matrix_mod(MA, MB);
    fmt_printf("mod([-3,7,-7,3], [5,5,-5,-5]) =\n");
    matrix_print(mo);

    /* np.trunc : round toward zero */
    double trd[] = { 2.7, -2.7, 2.3, -2.3 };
    Matrix* TR = matrix_from_array(trd, 2, 2);
    Matrix* tr = matrix_trunc(TR);
    fmt_printf("trunc([2.7,-2.7,2.3,-2.3]) =\n");
    matrix_print(tr);

    matrix_deallocate(T); matrix_deallocate(sn); matrix_deallocate(cs);
    matrix_deallocate(TN); matrix_deallocate(tn);
    matrix_deallocate(PA); matrix_deallocate(PB); matrix_deallocate(pw);
    matrix_deallocate(MA); matrix_deallocate(MB); matrix_deallocate(mo);
    matrix_deallocate(TR); matrix_deallocate(tr);

    return 0;
}
```

**Result**

```
sin([0, pi/2, pi]) =
| 0.00000 1.00000 0.00000 |
cos([0, pi/2, pi]) =
|  1.00000  0.00000 -1.00000 |
tan([0, pi/4]) =
| 0.00000 1.00000 |
power([2,3,4], [3,2,0.5]) =
| 8.00000 9.00000 2.00000 |
mod([-3,7,-7,3], [5,5,-5,-5]) =
|  2.00000  2.00000 |
| -2.00000 -2.00000 |
trunc([2.7,-2.7,2.3,-2.3]) =
|  2.00000 -2.00000 |
|  2.00000 -2.00000 |
```

---

## Example 77 : masks & selection — `matrix_isnan`, `matrix_isinf`, `matrix_isfinite`, `matrix_where`

The C analogues of NumPy's `numpy.isnan`/`isinf`/`isfinite` (which return 1/0 masks) and `numpy.where` (element-wise select). A common idiom is `matrix_where(matrix_isnan(M), zeros, M)` to replace every `NaN` with `0`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    /* a matrix with NaN and infinities */
    double d[] = { 1.0, NAN, INFINITY,
                  -INFINITY, 0.0, 2.0 };
    Matrix* M = matrix_from_array(d, 2, 3);

    /* np.isnan / np.isinf / np.isfinite -> 1/0 masks */
    Matrix* nanm = matrix_isnan(M);
    Matrix* infm = matrix_isinf(M);
    Matrix* finm = matrix_isfinite(M);
    fmt_printf("isnan(M) =\n");    matrix_print(nanm);
    fmt_printf("isinf(M) =\n");    matrix_print(infm);
    fmt_printf("isfinite(M) =\n"); matrix_print(finm);

    /* np.where(cond, a, b) : pick a where cond is true, else b */
    double cd[] = { 1, 0, 0, 1 };
    double ad[] = { 10, 20, 30, 40 };
    double bd[] = { 1, 2, 3, 4 };
    Matrix* C = matrix_from_array(cd, 2, 2);
    Matrix* A = matrix_from_array(ad, 2, 2);
    Matrix* B = matrix_from_array(bd, 2, 2);
    Matrix* w = matrix_where(C, A, B);
    fmt_printf("where(C, A, B) =\n");
    matrix_print(w);

    matrix_deallocate(M); matrix_deallocate(nanm); matrix_deallocate(infm); matrix_deallocate(finm);
    matrix_deallocate(C); matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(w);

    return 0;
}
```

**Result**

```
isnan(M) =
| 0.00000 1.00000 0.00000 |
| 0.00000 0.00000 0.00000 |
isinf(M) =
| 0.00000 0.00000 1.00000 |
| 1.00000 0.00000 0.00000 |
isfinite(M) =
| 1.00000 0.00000 0.00000 |
| 0.00000 1.00000 1.00000 |
where(C, A, B) =
| 10.00000  2.00000 |
|  3.00000 40.00000 |
```

---

## Example 78 : comparisons, logical ops & per-axis reductions

The C analogues of NumPy's `numpy.greater`/`less` and `numpy.logical_and`/`logical_or` (all returning 1/0 masks), plus `numpy.min`/`max`/`argmax` along an axis. Here `axis=1` reduces across each row.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double gd[] = { 1, 5, 3 };
    double hd[] = { 2, 5, 1 };
    Matrix* G = matrix_from_array(gd, 1, 3);
    Matrix* H = matrix_from_array(hd, 1, 3);

    /* np.greater / np.less -> 1/0 masks */
    Matrix* gt = matrix_greater(G, H);
    Matrix* lt = matrix_less(G, H);
    fmt_printf("greater([1,5,3], [2,5,1]) =\n"); matrix_print(gt);
    fmt_printf("less([1,5,3], [2,5,1]) =\n");    matrix_print(lt);

    /* np.logical_and / np.logical_or on 1/0 masks */
    double xd[] = { 1, 0, 2 };
    double yd[] = { 1, 1, 0 };
    Matrix* X = matrix_from_array(xd, 1, 3);
    Matrix* Y = matrix_from_array(yd, 1, 3);
    Matrix* la = matrix_logical_and(X, Y);
    Matrix* lo = matrix_logical_or(X, Y);
    fmt_printf("logical_and([1,0,2], [1,1,0]) =\n"); matrix_print(la);
    fmt_printf("logical_or([1,0,2], [1,1,0]) =\n");  matrix_print(lo);

    /* np.min / np.max / np.argmax along an axis */
    double sd[] = { 3, 1, 2,
                    6, 5, 4 };
    Matrix* S = matrix_from_array(sd, 2, 3);
    fmt_printf("S =\n"); matrix_print(S);
    Matrix* mn = matrix_min_axis(S, 1);
    Matrix* mx = matrix_max_axis(S, 1);
    Matrix* am = matrix_argmax_axis(S, 1);
    fmt_printf("min axis=1 (row minima) =\n");    matrix_print(mn);
    fmt_printf("max axis=1 (row maxima) =\n");    matrix_print(mx);
    fmt_printf("argmax axis=1 (row argmax) =\n"); matrix_print(am);

    matrix_deallocate(G); matrix_deallocate(H); matrix_deallocate(gt); matrix_deallocate(lt);
    matrix_deallocate(X); matrix_deallocate(Y); matrix_deallocate(la); matrix_deallocate(lo);
    matrix_deallocate(S); matrix_deallocate(mn); matrix_deallocate(mx); matrix_deallocate(am);

    return 0;
}
```

**Result**

```
greater([1,5,3], [2,5,1]) =
| 0.00000 0.00000 1.00000 |
less([1,5,3], [2,5,1]) =
| 1.00000 0.00000 0.00000 |
logical_and([1,0,2], [1,1,0]) =
| 1.00000 0.00000 0.00000 |
logical_or([1,0,2], [1,1,0]) =
| 1.00000 1.00000 1.00000 |
S =
| 3.00000 1.00000 2.00000 |
| 6.00000 5.00000 4.00000 |
min axis=1 (row minima) =
| 1.00000 |
| 4.00000 |
max axis=1 (row maxima) =
| 3.00000 |
| 6.00000 |
argmax axis=1 (row argmax) =
| 0.00000 |
| 0.00000 |
```

---

## Example 79 : `matrix_any` / `matrix_all`, `matrix_cumprod`, `matrix_cummax` / `matrix_cummin`

The C analogues of NumPy's `numpy.any`/`numpy.all` (boolean reductions), `numpy.cumprod` (cumulative product), and `numpy.maximum.accumulate`/`numpy.minimum.accumulate` (running max/min). The cumulative functions here flatten in row-major order.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 3.0, 1.0, 2.0,
                   6.0, 5.0, 4.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.any / np.all : true if any / all elements are non-zero */
    fmt_printf("any(M) = %s, all(M) = %s\n",
               matrix_any(M) ? "true" : "false",
               matrix_all(M) ? "true" : "false");

    /* np.cumprod(M) : flattened cumulative product */
    Matrix* cp = matrix_cumprod(M);
    fmt_printf("cumprod (flattened) =\n");
    matrix_print(cp);

    /* np.maximum.accumulate / np.minimum.accumulate (flattened) */
    Matrix* cmx = matrix_cummax(M);
    Matrix* cmn = matrix_cummin(M);
    fmt_printf("cummax (flattened) =\n"); matrix_print(cmx);
    fmt_printf("cummin (flattened) =\n"); matrix_print(cmn);

    matrix_deallocate(M); matrix_deallocate(cp); matrix_deallocate(cmx); matrix_deallocate(cmn);

    return 0;
}
```

**Result**

```
M =
| 3.00000 1.00000 2.00000 |
| 6.00000 5.00000 4.00000 |
any(M) = true, all(M) = true
cumprod (flattened) =
|   3.00000   3.00000   6.00000  36.00000 180.00000 720.00000 |
cummax (flattened) =
| 3.00000 3.00000 3.00000 6.00000 6.00000 6.00000 |
cummin (flattened) =
| 3.00000 1.00000 1.00000 1.00000 1.00000 1.00000 |
```

---

## Example 80 : `matrix_var_axis` / `matrix_std_axis`, `matrix_average`, `matrix_diag`

The C analogues of NumPy's `numpy.var`/`numpy.std` along an axis, `numpy.average` (weighted mean), and the constructor form of `numpy.diag` (build a diagonal matrix from a vector).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double d[] = { 1.0, 2.0, 3.0,
                   4.0, 5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 2, 3);
    fmt_printf("M =\n");
    matrix_print(M);

    /* np.var / np.std along axis=0 (per column) */
    Matrix* v = matrix_var_axis(M, 0);
    Matrix* s = matrix_std_axis(M, 0);
    fmt_printf("var axis=0 =\n"); matrix_print(v);
    fmt_printf("std axis=0 =\n"); matrix_print(s);

    /* np.average with weights, then unweighted */
    double wd[] = { 1.0, 2.0, 3.0,
                    4.0, 5.0, 6.0 };
    Matrix* W = matrix_from_array(wd, 2, 3);
    fmt_printf("average(M, weights=W) = %.5f\n", matrix_average(M, W));
    fmt_printf("average(M) [unweighted] = %.5f\n", matrix_average(M, NULL));

    /* np.diag([1,2,3]) : build a diagonal matrix from a vector */
    double dv[] = { 1.0, 2.0, 3.0 };
    Matrix* V = matrix_from_array(dv, 1, 3);
    Matrix* D = matrix_diag(V);
    fmt_printf("diag([1,2,3]) =\n");
    matrix_print(D);

    matrix_deallocate(M); matrix_deallocate(v); matrix_deallocate(s);
    matrix_deallocate(W); matrix_deallocate(V); matrix_deallocate(D);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
var axis=0 =
| 2.25000 2.25000 2.25000 |
std axis=0 =
| 1.50000 1.50000 1.50000 |
average(M, weights=W) = 4.33333
average(M) [unweighted] = 3.50000
diag([1,2,3]) =
| 1.00000 0.00000 0.00000 |
| 0.00000 2.00000 0.00000 |
| 0.00000 0.00000 3.00000 |
```

---

## Example 81 : `matrix_diagonal`, `matrix_fill_diagonal`, `matrix_meshgrid`

The C analogues of NumPy's `numpy.diagonal(M, k)` (extract the k-th diagonal), `numpy.fill_diagonal` (set the main diagonal in place), and `numpy.meshgrid` (coordinate grids from two vectors).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double td[] = { 1.0, 2.0, 3.0,
                    4.0, 5.0, 6.0,
                    7.0, 8.0, 9.0 };
    Matrix* T = matrix_from_array(td, 3, 3);
    fmt_printf("T =\n");
    matrix_print(T);

    /* np.diagonal(T, k) : extract the k-th diagonal */
    Matrix* d0 = matrix_diagonal(T, 0);
    Matrix* d1 = matrix_diagonal(T, 1);
    fmt_printf("diagonal(T, 0) =\n"); matrix_print(d0);
    fmt_printf("diagonal(T, 1) =\n"); matrix_print(d1);

    /* np.fill_diagonal(T, 0) : set the main diagonal in place */
    matrix_fill_diagonal(T, 0.0);
    fmt_printf("after fill_diagonal(T, 0) =\n");
    matrix_print(T);

    /* np.meshgrid([1,2,3], [4,5]) */
    double xd[] = { 1.0, 2.0, 3.0 };
    double yd[] = { 4.0, 5.0 };
    Matrix* xv = matrix_from_array(xd, 1, 3);
    Matrix* yv = matrix_from_array(yd, 1, 2);
    Matrix* X = NULL; Matrix* Y = NULL;
    matrix_meshgrid(xv, yv, &X, &Y);
    fmt_printf("meshgrid X =\n"); matrix_print(X);
    fmt_printf("meshgrid Y =\n"); matrix_print(Y);

    matrix_deallocate(T); matrix_deallocate(d0); matrix_deallocate(d1);
    matrix_deallocate(xv); matrix_deallocate(yv); matrix_deallocate(X); matrix_deallocate(Y);

    return 0;
}
```

**Result**

```
T =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
diagonal(T, 0) =
| 1.00000 5.00000 9.00000 |
diagonal(T, 1) =
| 2.00000 6.00000 |
after fill_diagonal(T, 0) =
| 0.00000 2.00000 3.00000 |
| 4.00000 0.00000 6.00000 |
| 7.00000 8.00000 0.00000 |
meshgrid X =
| 1.00000 2.00000 3.00000 |
| 1.00000 2.00000 3.00000 |
meshgrid Y =
| 4.00000 4.00000 4.00000 |
| 5.00000 5.00000 5.00000 |
```

---

## Example 82 : `matrix_unique`, `matrix_searchsorted`, `matrix_split`

The C analogues of NumPy's `numpy.unique` (sorted distinct values), `numpy.searchsorted` (binary-search insertion indices), and `numpy.split` (equal blocks). Note `matrix_split` returns an **array** of matrices that the caller must free (each element, then the array).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* np.unique : sorted unique values */
    double ud[] = { 3.0, 1.0, 2.0,
                    1.0, 3.0, 2.0 };
    Matrix* U = matrix_from_array(ud, 2, 3);
    Matrix* uq = matrix_unique(U);
    fmt_printf("unique =\n"); matrix_print(uq);

    /* np.searchsorted([1,3,5,7], [2,5,8]) */
    double ad[] = { 1.0, 3.0, 5.0, 7.0 };
    double vd[] = { 2.0, 5.0, 8.0 };
    Matrix* A = matrix_from_array(ad, 1, 4);
    Matrix* V = matrix_from_array(vd, 1, 3);
    Matrix* ss = matrix_searchsorted(A, V);
    fmt_printf("searchsorted =\n"); matrix_print(ss);

    /* np.split(M, 3, axis=1) : three column blocks */
    double sd[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
                    7.0, 8.0, 9.0, 10.0, 11.0, 12.0 };
    Matrix* M = matrix_from_array(sd, 2, 6);
    size_t cnt = 0;
    Matrix** parts = matrix_split(M, 3, 1, &cnt);
    for (size_t i = 0; i < cnt; ++i) {
        fmt_printf("split block %d =\n", (int)i);
        matrix_print(parts[i]);
    }

    matrix_deallocate(U); matrix_deallocate(uq);
    matrix_deallocate(A); matrix_deallocate(V); matrix_deallocate(ss);
    matrix_deallocate(M);
    for (size_t i = 0; i < cnt; ++i) matrix_deallocate(parts[i]);
    free(parts);

    return 0;
}
```

**Result**

```
unique =
| 1.00000 2.00000 3.00000 |
searchsorted =
| 1.00000 2.00000 4.00000 |
split block 0 =
| 1.00000 2.00000 |
| 7.00000 8.00000 |
split block 1 =
|  3.00000  4.00000 |
|  9.00000 10.00000 |
split block 2 =
|  5.00000  6.00000 |
| 11.00000 12.00000 |
```

---

## Example 83 : data analysis — `matrix_percentile`, `matrix_standardize`, `matrix_normalize`, `matrix_rolling_mean`

The C analogues of NumPy's `numpy.percentile`, SciPy's `zscore` (per-column standardisation), min-max scaling, and pandas' `rolling().mean()`. Note `matrix_rolling_mean` fills the warm-up region (the first `window − 1` rows here) with `NaN`, exactly like pandas.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.percentile : linear interpolation */
    double pd[] = { 1.0, 2.0, 3.0, 4.0 };
    Matrix* P = matrix_from_array(pd, 1, 4);

    fmt_printf("percentile(50) = %.5f\n", 
    matrix_percentile(P, 50.0));
    
    fmt_printf("percentile(25) = %.5f\n", 
    matrix_percentile(P, 25.0));

    /* per-column z-score and min-max scaling */
    double zd[] = { 1.0, 2.0,
                    3.0, 4.0,
                    5.0, 6.0 };
    Matrix* Z = matrix_from_array(zd, 3, 2);
    fmt_printf("Z =\n"); matrix_print(Z);
    Matrix* st = matrix_standardize(Z);
    Matrix* nm = matrix_normalize(Z);

    fmt_printf("standardize(Z) =\n"); 
    matrix_print(st);

    fmt_printf("normalize(Z) =\n"); 
    matrix_print(nm);

    /* pandas-style rolling mean, window=2, down each column */
    double rd[] = { 1.0, 10.0,
                    2.0, 20.0,
                    3.0, 30.0,
                    4.0, 40.0 };
    Matrix* R = matrix_from_array(rd, 4, 2);
    Matrix* rm = matrix_rolling_mean(R, 2, 0);

    fmt_printf("rolling_mean(window=2, axis=0) =\n");
    matrix_print(rm);

    matrix_deallocate(P); matrix_deallocate(Z); matrix_deallocate(st); 
    matrix_deallocate(R); matrix_deallocate(rm); matrix_deallocate(nm);

    return 0;
}
```

**Result**

```
percentile(50) = 2.50000
percentile(25) = 1.75000
Z =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
standardize(Z) =
| -1.22474 -1.22474 |
|  0.00000  0.00000 |
|  1.22474  1.22474 |
normalize(Z) =
| 0.00000 0.00000 |
| 0.50000 0.50000 |
| 1.00000 1.00000 |
rolling_mean(window=2, axis=0) =
|      nan      nan |
|  1.50000 15.00000 |
|  2.50000 25.00000 |
|  3.50000 35.00000 |
```

---

## Example 84 : `matrix_shift`, `matrix_pct_change`, `matrix_fillna`

The C analogues of pandas' `DataFrame.shift` (non-cyclic shift with a fill value), `pct_change` (fractional change), and `fillna` (replace NaN). A common idiom chains them: `pct_change` introduces a `NaN`, then `fillna` cleans it up.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    double d[] = { 1.0, 2.0,
                   3.0, 4.0,
                   5.0, 6.0 };
    Matrix* M = matrix_from_array(d, 3, 2);
    fmt_printf("M =\n");
    matrix_print(M);

    /* df.shift(1) : move rows down by 1, fill the top with NaN */
    Matrix* sh = matrix_shift(M, 1, 0, NAN);
    fmt_printf("shift(1, axis=0, fill=NaN) =\n");
    matrix_print(sh);

    /* df.shift(1, fill_value=0) : same, but fill with 0 */
    Matrix* sh0 = matrix_shift(M, 1, 0, 0.0);
    fmt_printf("shift(1, axis=0, fill=0) =\n");
    matrix_print(sh0);

    /* df.pct_change() : fractional change down each column */
    double pd[] = { 1.0, 2.0, 4.0, 4.0 };
    Matrix* P = matrix_from_array(pd, 4, 1);
    Matrix* pc = matrix_pct_change(P, 0);

    fmt_printf("pct_change(axis=0) =\n");
    matrix_print(pc);

    /* df.fillna(0) : replace the NaN warm-up with 0 */
    Matrix* filled = matrix_fillna(pc, 0.0);
    fmt_printf("fillna(pct_change, 0) =\n");
    matrix_print(filled);

    matrix_deallocate(M); 
    matrix_deallocate(sh); 
    matrix_deallocate(sh0);
    matrix_deallocate(P); 
    matrix_deallocate(pc); 
    matrix_deallocate(filled);

    return 0;
}
```

**Result**

```
M =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
shift(1, axis=0, fill=NaN) =
|     nan     nan |
| 1.00000 2.00000 |
| 3.00000 4.00000 |
shift(1, axis=0, fill=0) =
| 0.00000 0.00000 |
| 1.00000 2.00000 |
| 3.00000 4.00000 |
pct_change(axis=0) =
|     nan |
| 1.00000 |
| 1.00000 |
| 0.00000 |
fillna(pct_change, 0) =
| 0.00000 |
| 1.00000 |
| 1.00000 |
| 0.00000 |
```

---

## Example 85 : `matrix_describe` (per-column summary statistics)

The C analogue of pandas' `DataFrame.describe()`. For a matrix of 5 observations × 2 variables it returns an `8 × 2` summary — the rows being count, mean, std, min, 25%, 50%, 75%, and max. (The std is the sample std, `ddof = 1`, like pandas.)

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* 5 observations, 2 variables (columns) */
    double d[] = {  1.0, 10.0,
                    2.0, 20.0,
                    3.0, 30.0,
                    4.0, 40.0,
                    5.0, 50.0 };
    Matrix* M = matrix_from_array(d, 5, 2);
    fmt_printf("M =\n");
    matrix_print(M);

    /* df.describe() : 8 rows = count, mean, std, min, 25%, 50%, 75%, max */
    Matrix* desc = matrix_describe(M);
    fmt_printf("describe(M)  [rows: count, mean, std, min, 25pct, 50pct, 75pct, max] =\n");
    matrix_print(desc);

    matrix_deallocate(M);
    matrix_deallocate(desc);

    return 0;
}
```

**Result**

```
M =
|  1.00000 10.00000 |
|  2.00000 20.00000 |
|  3.00000 30.00000 |
|  4.00000 40.00000 |
|  5.00000 50.00000 |
describe(M)  [rows: count, mean, std, min, 25pct, 50pct, 75pct, max] =
|  5.00000  5.00000 |
|  3.00000 30.00000 |
|  1.58114 15.81139 |
|  1.00000 10.00000 |
|  2.00000 20.00000 |
|  3.00000 30.00000 |
|  4.00000 40.00000 |
|  5.00000 50.00000 |
```

---

## Example 86 : `matrix_nuclear_norm`, `matrix_spectral_norm`, `matrix_polyval`, `matrix_slogdet`

The C analogues of NumPy's `numpy.linalg.norm(M, 'nuc')` (sum of singular values), `numpy.linalg.norm(M, 2)` (largest singular value — the true 2-norm), `numpy.polyval`, and `numpy.linalg.slogdet` (overflow-safe sign and `log|det|`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double ad[] = { 1.0, 2.0,
                    3.0, 4.0,
                    5.0, 6.0 };
    Matrix* A = matrix_from_array(ad, 3, 2);
    fmt_printf("A =\n");
    matrix_print(A);

    /* np.linalg.norm(A, 'nuc') and norm(A, 2) */
    fmt_printf("nuclear_norm(A)  = %.5f\n", 
    matrix_nuclear_norm(A));

    fmt_printf("spectral_norm(A) = %.5f\n", 
    matrix_spectral_norm(A));

    /* np.polyval([3,0,1], [5,0,2]) : evaluate 3x^2 + 1 */
    double pc[] = { 3.0, 0.0, 1.0 };
    double px[] = { 5.0, 0.0, 2.0 };
    Matrix* P = matrix_from_array(pc, 1, 3);
    Matrix* X = matrix_from_array(px, 1, 3);
    Matrix* pv = matrix_polyval(P, X);

    fmt_printf("polyval([3,0,1], [5,0,2]) =\n");
    matrix_print(pv);

    /* np.linalg.slogdet([[1,2],[3,4]]) */
    double md[] = { 1.0, 2.0, 3.0, 4.0 };
    Matrix* M = matrix_from_array(md, 2, 2);
    double sign, logabsdet;

    matrix_slogdet(M, &sign, &logabsdet);
    fmt_printf("slogdet([[1,2],[3,4]]): sign = %.1f, logabsdet = %.5f\n", sign, logabsdet);

    matrix_deallocate(A); 
    matrix_deallocate(P); 
    matrix_deallocate(X); 
    matrix_deallocate(pv); 
    matrix_deallocate(M);

    return 0;
}
```

**Result**

```
A =
| 1.00000 2.00000 |
| 3.00000 4.00000 |
| 5.00000 6.00000 |
nuclear_norm(A)  = 10.03982
spectral_norm(A) = 9.52552
polyval([3,0,1], [5,0,2]) =
| 76.00000  1.00000 13.00000 |
slogdet([[1,2],[3,4]]): sign = -1.0, logabsdet = 0.69315
```

---

## Example 87 : `matrix_roots` and `matrix_expm`

The C analogues of NumPy's `numpy.roots` (polynomial roots — returned as an `n × 2` matrix of real/imag parts, since they may be complex) and SciPy's `scipy.linalg.expm` (matrix exponential). Here `expm([[0,-1],[1,0]])` is the rotation matrix by 1 radian.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.roots([1,-6,11,-6]) : roots of (x-1)(x-2)(x-3) */
    double rc[] = { 1.0, -6.0, 11.0, -6.0 };
    Matrix* C = matrix_from_array(rc, 1, 4);
    Matrix* r = matrix_roots(C);

    fmt_printf("roots([1,-6,11,-6])  [columns: real, imag] =\n");
    matrix_print(r);

    /* np.roots([1,0,1]) : roots of x^2 + 1 = +-i */
    double rc2[] = { 1.0, 0.0, 1.0 };
    Matrix* C2 = matrix_from_array(rc2, 1, 3);
    Matrix* r2 = matrix_roots(C2);

    fmt_printf("roots([1,0,1])  [columns: real, imag] =\n");
    matrix_print(r2);

    /* scipy.linalg.expm : matrix exponential of a rotation generator */
    double ed[] = { 0.0, -1.0,
                    1.0,  0.0 };
    Matrix* E = matrix_from_array(ed, 2, 2);
    Matrix* ex = matrix_expm(E);

    fmt_printf("expm([[0,-1],[1,0]]) =\n");
    matrix_print(ex);

    matrix_deallocate(C); matrix_deallocate(r); matrix_deallocate(C2); matrix_deallocate(r2);
    matrix_deallocate(E); matrix_deallocate(ex);

    return 0;
}
```

**Result**

```
roots([1,-6,11,-6])  [columns: real, imag] =
| 2.00000 0.00000 |
| 3.00000 0.00000 |
| 1.00000 0.00000 |
roots([1,0,1])  [columns: real, imag] =
|  0.00000  1.00000 |
|  0.00000 -1.00000 |
expm([[0,-1],[1,0]]) =
|  0.54030 -0.84147 |
|  0.84147  0.54030 |
```

---

## Example 88 : more element-wise math — logs, exps & hyperbolic functions

The C analogues of NumPy's `numpy.log2`/`exp2`, `numpy.expm1`/`log1p` (accurate near zero), and `numpy.sinh`/`cosh`/`tanh`. `tanh` is the classic neural-network activation, squashing values into `(-1, 1)`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* np.log2 / np.exp2 (inverses on powers of two) */
    double pw[] = { 1.0, 2.0, 4.0, 8.0 };
    Matrix* PW = matrix_from_array(pw, 1, 4);
    Matrix* l2 = matrix_log2(PW);

    fmt_printf("log2([1,2,4,8]) =\n"); 
    matrix_print(l2);

    double ex[] = { 0.0, 1.0, 2.0, 3.0 };
    Matrix* EX = matrix_from_array(ex, 1, 4);
    Matrix* x2 = matrix_exp2(EX);

    fmt_printf("exp2([0,1,2,3]) =\n");
    matrix_print(x2);

    /* np.expm1 / np.log1p (accurate near zero) */
    double sm[] = { 0.0, 1.0 };
    Matrix* SM = matrix_from_array(sm, 1, 2);
    Matrix* e1 = matrix_expm1(SM);
    Matrix* lp = matrix_log1p(SM);

    fmt_printf("expm1([0,1]) =\n"); 
    matrix_print(e1);

    fmt_printf("log1p([0,1]) =\n"); 
    matrix_print(lp);

    /* np.sinh / np.cosh / np.tanh */
    double td[] = { -1.0, 0.0, 1.0 };
    Matrix* T = matrix_from_array(td, 1, 3);
    Matrix* sh = matrix_sinh(T);
    Matrix* ch = matrix_cosh(T);
    Matrix* th = matrix_tanh(T);

    fmt_printf("sinh([-1,0,1]) =\n"); 
    matrix_print(sh);

    fmt_printf("cosh([-1,0,1]) =\n"); 
    matrix_print(ch);

    fmt_printf("tanh([-1,0,1]) =\n"); 
    matrix_print(th);

    matrix_deallocate(PW); matrix_deallocate(l2); matrix_deallocate(EX); matrix_deallocate(x2);
    matrix_deallocate(SM); matrix_deallocate(e1); matrix_deallocate(lp);
    matrix_deallocate(T); matrix_deallocate(sh); matrix_deallocate(ch); matrix_deallocate(th);

    return 0;
}
```

**Result**

```
log2([1,2,4,8]) =
| 0.00000 1.00000 2.00000 3.00000 |
exp2([0,1,2,3]) =
| 1.00000 2.00000 4.00000 8.00000 |
expm1([0,1]) =
| 0.00000 1.71828 |
log1p([0,1]) =
| 0.00000 0.69315 |
sinh([-1,0,1]) =
| -1.17520  0.00000  1.17520 |
cosh([-1,0,1]) =
| 1.54308 1.00000 1.54308 |
tanh([-1,0,1]) =
| -0.76159  0.00000  0.76159 |
```

---

## Example 89 : inverse trig, angle conversion, sign, and binary math

The C analogues of NumPy's `numpy.arcsin`/`arccos`/`arctan`, `numpy.deg2rad`/`rad2deg`, `numpy.signbit` (note `-0.0 → 1`), and the binary `numpy.arctan2`, `numpy.hypot`, `numpy.copysign`, and `numpy.fmod` (which keeps the dividend's sign, unlike `matrix_mod`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* inverse trig */
    double sd[] = { 0.0, 0.5, 1.0 };
    Matrix* S = matrix_from_array(sd, 1, 3);
    Matrix* as = matrix_arcsin(S);
    Matrix* ac = matrix_arccos(S);

    fmt_printf("arcsin([0,0.5,1]) =\n"); 
    matrix_print(as);

    fmt_printf("arccos([0,0.5,1]) =\n"); 
    matrix_print(ac);

    double atd[] = { 0.0, 1.0 };
    Matrix* AT = matrix_from_array(atd, 1, 2);
    Matrix* at = matrix_arctan(AT);

    fmt_printf("arctan([0,1]) =\n"); 
    matrix_print(at);

    /* angle conversion */
    double dd[] = { 0.0, 90.0, 180.0 };
    Matrix* D = matrix_from_array(dd, 1, 3);
    Matrix* r = matrix_deg2rad(D);

    fmt_printf("deg2rad([0,90,180]) =\n"); 
    matrix_print(r);

    Matrix* back = matrix_rad2deg(r);
    fmt_printf("rad2deg(deg2rad(...)) =\n"); 
    matrix_print(back);

    /* signbit (note -0.0 -> 1) */
    double sbd[] = { -3.0, 0.0, 5.0, -0.0 };
    Matrix* SB = matrix_from_array(sbd, 1, 4);
    Matrix* sgn = matrix_signbit(SB);

    fmt_printf("signbit([-3,0,5,-0]) =\n"); 
    matrix_print(sgn);

    /* binary: arctan2, hypot, copysign, fmod */
    double yd[] = { 1.0, 1.0 }; 
    double xd[] = { 1.0, -1.0 };
    Matrix* Y = matrix_from_array(yd, 1, 2); 
    Matrix* X = matrix_from_array(xd, 1, 2);
    Matrix* a2 = matrix_arctan2(Y, X);

    fmt_printf("arctan2([1,1], [1,-1]) =\n"); 
    matrix_print(a2);

    double px[] = { 3.0, 5.0 }; 
    double py[] = { 4.0, 12.0 };
    Matrix* PX = matrix_from_array(px, 1, 2); 
    Matrix* PY = matrix_from_array(py, 1, 2);
    Matrix* hp = matrix_hypot(PX, PY);

    fmt_printf("hypot([3,5], [4,12]) =\n"); 
    matrix_print(hp);

    double cm[] = { 3.0, -3.0 }; 
    double csg[] = { -1.0, 1.0 };
    Matrix* CM = matrix_from_array(cm, 1, 2); 
    Matrix* CSG = matrix_from_array(csg, 1, 2);
    Matrix* cp = matrix_copysign(CM, CSG);

    fmt_printf("copysign([3,-3], [-1,1]) =\n"); 
    matrix_print(cp);

    double fx[] = { -3.0, 7.0 }; 
    double fy[] = { 5.0, 5.0 };
    Matrix* FX = matrix_from_array(fx, 1, 2); 
    Matrix* FY = matrix_from_array(fy, 1, 2);
    Matrix* fm = matrix_fmod(FX, FY);

    fmt_printf("fmod([-3,7], [5,5])  (sign of dividend) =\n"); 
    matrix_print(fm);

    matrix_deallocate(S); 
    matrix_deallocate(as); 
    matrix_deallocate(ac);
    matrix_deallocate(AT);
    matrix_deallocate(at);
    matrix_deallocate(D); 
    matrix_deallocate(r); 
    matrix_deallocate(back);
    matrix_deallocate(SB); 
    matrix_deallocate(sgn);
    matrix_deallocate(Y); matrix_deallocate(X); matrix_deallocate(a2);
    matrix_deallocate(PX); matrix_deallocate(PY); matrix_deallocate(hp);
    matrix_deallocate(CM); matrix_deallocate(CSG); matrix_deallocate(cp);
    matrix_deallocate(FX); matrix_deallocate(FY); matrix_deallocate(fm);

    return 0;
}
```

**Result**

```
arcsin([0,0.5,1]) =
| 0.00000 0.52360 1.57080 |
arccos([0,0.5,1]) =
| 1.57080 1.04720 0.00000 |
arctan([0,1]) =
| 0.00000 0.78540 |
deg2rad([0,90,180]) =
| 0.00000 1.57080 3.14159 |
rad2deg(deg2rad(...)) =
|   0.00000  90.00000 180.00000 |
signbit([-3,0,5,-0]) =
| 1.00000 0.00000 0.00000 1.00000 |
arctan2([1,1], [1,-1]) =
| 0.78540 2.35619 |
hypot([3,5], [4,12]) =
|  5.00000 13.00000 |
copysign([3,-3], [-1,1]) =
| -3.00000  3.00000 |
fmod([-3,7], [5,5])  (sign of dividend) =
| -3.00000  2.00000 |
```

---

## Example 90 : per-axis reductions, non-zero coordinates, and flip

The C analogues of NumPy's `numpy.argmin(axis=)`, `numpy.ptp(axis=)`, `numpy.median(axis=)`, `numpy.count_nonzero(axis=)`, `numpy.nonzero` (which returns `(row, col)` coordinate pairs), and `numpy.flip` (a runtime-selectable axis that generalises `flipud`/`fliplr`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* per-axis argmin / ptp / median / count_nonzero */
    double md[] = { 3, 1, 2,
                    9, 7, 8,
                    6, 4, 5 };
    Matrix* M = matrix_from_array(md, 3, 3);
    Matrix* amn0 = matrix_argmin_axis(M, 0);
    Matrix* amn1 = matrix_argmin_axis(M, 1);

    fmt_printf("argmin axis=0 (row of each col min) =\n");
    matrix_print(amn0);

    fmt_printf("argmin axis=1 (col of each row min) =\n");
    matrix_print(amn1);

    Matrix* p0 = matrix_ptp_axis(M, 0);
    fmt_printf("ptp axis=0 (column ranges) =\n");
    matrix_print(p0);

    /* median per row of a 4x3 */
    double ad[] = { 1,  5,  3,
                    8,  2,  9,
                    4,  7,  6,
                    0, 11, 10 };
    Matrix* A = matrix_from_array(ad, 4, 3);
    Matrix* med = matrix_median_axis(A, 1);

    fmt_printf("median axis=1 (row medians) =\n");
    matrix_print(med);

    /* count_nonzero + nonzero coordinates */
    double zd[] = { 0, 2, 0,
                    3, 0, 5,
                    0, 0, 8 };
    Matrix* Z = matrix_from_array(zd, 3, 3);
    Matrix* cnz = matrix_count_nonzero_axis(Z, 0);

    fmt_printf("count_nonzero axis=0 =\n");
    matrix_print(cnz);

    Matrix* nz = matrix_nonzero(Z);
    fmt_printf("nonzero (row,col) pairs =\n");
    matrix_print(nz);

    /* flip both ways */
    Matrix* f0 = matrix_flip(M, 0);
    Matrix* f1 = matrix_flip(M, 1);

    fmt_printf("flip axis=0 (rows reversed) =\n");
    matrix_print(f0);

    fmt_printf("flip axis=1 (cols reversed) =\n");
    matrix_print(f1);

    matrix_deallocate(M); matrix_deallocate(amn0); matrix_deallocate(amn1);
    matrix_deallocate(p0);
    matrix_deallocate(A); matrix_deallocate(med);
    matrix_deallocate(Z); matrix_deallocate(cnz); matrix_deallocate(nz);
    matrix_deallocate(f0); matrix_deallocate(f1);
    return 0;
}
```

**Result**

```
argmin axis=0 (row of each col min) =
| 0.00000 0.00000 0.00000 |
argmin axis=1 (col of each row min) =
| 1.00000 |
| 1.00000 |
| 1.00000 |
ptp axis=0 (column ranges) =
| 6.00000 6.00000 6.00000 |
median axis=1 (row medians) =
|  3.00000 |
|  8.00000 |
|  6.00000 |
| 10.00000 |
count_nonzero axis=0 =
| 1.00000 1.00000 2.00000 |
nonzero (row,col) pairs =
| 0.00000 1.00000 |
| 1.00000 0.00000 |
| 1.00000 2.00000 |
| 2.00000 2.00000 |
flip axis=0 (rows reversed) =
| 6.00000 4.00000 5.00000 |
| 9.00000 7.00000 8.00000 |
| 3.00000 1.00000 2.00000 |
flip axis=1 (cols reversed) =
| 2.00000 1.00000 3.00000 |
| 8.00000 7.00000 9.00000 |
| 5.00000 4.00000 6.00000 |
```

---

## Example 91 : indexing, structural edits, and binning

The C analogues of NumPy's `numpy.take` (flat gather with negative wrap-around), `numpy.delete` / `numpy.insert` / `numpy.append` (row/column edits), `numpy.column_stack`, `numpy.digitize`, `numpy.bincount`, and `numpy.histogram` (returning both counts and bin edges).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    double md[] = { 1, 2, 3,
                    4, 5, 6 };
    Matrix* M = matrix_from_array(md, 2, 3);

    /* take: gather by flat indices (negative wraps) */
    double id[] = { 0, 5, 3, -1 };
    Matrix* I = matrix_from_array(id, 2, 2);
    Matrix* tk = matrix_take(M, I);

    fmt_printf("take(M, [[0,5],[3,-1]]) =\n");
    matrix_print(tk);

    /* delete a column, insert a row */
    Matrix* dc = matrix_delete(M, 1, 1);
    fmt_printf("delete col 1 =\n");
    matrix_print(dc);

    double rv[] = { 7, 8, 9 };
    Matrix* RV = matrix_from_array(rv, 1, 3);
    Matrix* ins = matrix_insert(M, 1, RV, 0);

    fmt_printf("insert row [7,8,9] at index 1 =\n");
    matrix_print(ins);

    /* append a row */
    Matrix* ap = matrix_append(M, RV, 0);
    fmt_printf("append row [7,8,9] =\n");
    matrix_print(ap);

    /* column_stack two vectors */
    double av[] = { 1, 2, 3 };
    double bv[] = { 4, 5, 6 };
    Matrix* AV = matrix_from_array(av, 1, 3);
    Matrix* BV = matrix_from_array(bv, 1, 3);
    Matrix* cs = matrix_column_stack(AV, BV);

    fmt_printf("column_stack([1,2,3],[4,5,6]) =\n");
    matrix_print(cs);

    /* digitize values into bins [0,1,2.5,4,10] */
    double xv[] = { 0.2, 6.4, 3.0, 1.6, 8.0 };
    double bn[] = { 0.0, 1.0, 2.5, 4.0, 10.0 };
    Matrix* X = matrix_from_array(xv, 1, 5);
    Matrix* BN = matrix_from_array(bn, 1, 5);
    Matrix* dg = matrix_digitize(X, BN);

    fmt_printf("digitize =\n");
    matrix_print(dg);

    /* bincount of small integers */
    double cv[] = { 1, 2, 2, 3, 3, 3, 0, 5 };
    Matrix* CV = matrix_from_array(cv, 1, 8);
    Matrix* bc = matrix_bincount(CV);

    fmt_printf("bincount =\n");
    matrix_print(bc);

    /* histogram into 4 bins */
    double hv[] = { 1, 2, 1, 3, 5, 8, 5, 2, 3, 9 };
    Matrix* H = matrix_from_array(hv, 1, 10);
    Matrix* hist = NULL;
    Matrix* edges = NULL;

    matrix_histogram(H, 4, &hist, &edges);
    fmt_printf("histogram counts =\n");
    matrix_print(hist);

    fmt_printf("histogram edges =\n");
    matrix_print(edges);

    matrix_deallocate(M); matrix_deallocate(I); matrix_deallocate(tk);
    matrix_deallocate(dc);
    matrix_deallocate(RV); matrix_deallocate(ins); matrix_deallocate(ap);
    matrix_deallocate(AV); matrix_deallocate(BV); matrix_deallocate(cs);
    matrix_deallocate(X); matrix_deallocate(BN); matrix_deallocate(dg);
    matrix_deallocate(CV); matrix_deallocate(bc);
    matrix_deallocate(H); matrix_deallocate(hist); matrix_deallocate(edges);
    return 0;
}
```

**Result**

```
take(M, [[0,5],[3,-1]]) =
| 1.00000 6.00000 |
| 4.00000 6.00000 |
delete col 1 =
| 1.00000 3.00000 |
| 4.00000 6.00000 |
insert row [7,8,9] at index 1 =
| 1.00000 2.00000 3.00000 |
| 7.00000 8.00000 9.00000 |
| 4.00000 5.00000 6.00000 |
append row [7,8,9] =
| 1.00000 2.00000 3.00000 |
| 4.00000 5.00000 6.00000 |
| 7.00000 8.00000 9.00000 |
column_stack([1,2,3],[4,5,6]) =
| 1.00000 4.00000 |
| 2.00000 5.00000 |
| 3.00000 6.00000 |
digitize =
| 1.00000 4.00000 3.00000 2.00000 4.00000 |
bincount =
| 1.00000 1.00000 2.00000 3.00000 0.00000 1.00000 |
histogram counts =
| 4.00000 2.00000 2.00000 2.00000 |
histogram edges =
| 1.00000 3.00000 5.00000 7.00000 9.00000 |
```

---

## Example 92 : element-wise math — sinc, banker's rounding, exprel & logaddexp

The C analogues of NumPy's `numpy.sinc` (normalized cardinal sine), `numpy.around` (round-half-to-even — note `2.675 → 2.68`), SciPy's `scipy.special.exprel` (`(eˣ−1)/x`, stable near zero), and `numpy.logaddexp` (stable `log(eᵃ+eᵇ)`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* sinc: normalized sin(pi x)/(pi x), 1 at x=0 */
    double sd[] = { 0.0, 0.25, 0.5, 0.75, 1.5 };
    Matrix* S = matrix_from_array(sd, 1, 5);
    Matrix* sc = matrix_sinc(S);
    fmt_printf("sinc([0,0.25,0.5,0.75,1.5]) =\n");
    matrix_print(sc);

    /* around: round half-to-even to 2 decimals (note 2.675 -> 2.68) */
    double ad[] = { 1.23456, 3.14159, 2.675, -1.55, 2.5 };
    Matrix* A = matrix_from_array(ad, 1, 5);
    Matrix* ar = matrix_around(A, 2);
    fmt_printf("around([1.23456,3.14159,2.675,-1.55,2.5], 2) =\n");
    matrix_print(ar);

    /* exprel: (exp(x)-1)/x, 1 at x=0 */
    double ed[] = { -1.0, 0.0, 1.0, 2.0 };
    Matrix* E = matrix_from_array(ed, 1, 4);
    Matrix* ex = matrix_exprel(E);
    fmt_printf("exprel([-1,0,1,2]) =\n");
    matrix_print(ex);

    /* logaddexp: stable log(exp(a)+exp(b)) */
    double la[] = { 0.0, 1.0, -1.0 };
    double lb[] = { 0.0, 2.0, -2.0 };
    Matrix* LA = matrix_from_array(la, 1, 3);
    Matrix* LB = matrix_from_array(lb, 1, 3);
    Matrix* lg = matrix_logaddexp(LA, LB);
    fmt_printf("logaddexp([0,1,-1], [0,2,-2]) =\n");
    matrix_print(lg);

    matrix_deallocate(S); matrix_deallocate(sc);
    matrix_deallocate(A); matrix_deallocate(ar);
    matrix_deallocate(E); matrix_deallocate(ex);
    matrix_deallocate(LA); matrix_deallocate(LB); matrix_deallocate(lg);
    return 0;
}
```

**Result**

```
sinc([0,0.25,0.5,0.75,1.5]) =
|  1.00000  0.90032  0.63662  0.30011 -0.21221 |
around([1.23456,3.14159,2.675,-1.55,2.5], 2) =
|  1.23000  3.14000  2.68000 -1.55000  2.50000 |
exprel([-1,0,1,2]) =
| 0.63212 1.00000 1.71828 3.19453 |
logaddexp([0,1,-1], [0,2,-2]) =
|  0.69315  2.31326 -0.68674 |
```

---

## Example 93 : integer GCD/LCM, NaN-aware min/max, and modf/divmod

The C analogues of NumPy's `numpy.gcd`/`numpy.lcm` (element-wise integer divisors), `numpy.fmax`/`numpy.fmin` (which ignore NaN — the result is NaN only where *both* operands are NaN), and `numpy.modf`/`numpy.divmod` (which return two matrices via out-parameters).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    /* gcd & lcm of two integer-valued vectors */
    double da[] = { 12, 100, 7, 0 };
    double db[] = { 18, 60, 13, 5 };
    Matrix* A = matrix_from_array(da, 1, 4);
    Matrix* B = matrix_from_array(db, 1, 4);
    Matrix* g = matrix_gcd(A, B);
    Matrix* l = matrix_lcm(A, B);
    fmt_printf("gcd([12,100,7,0], [18,60,13,5]) =\n");
    matrix_print(g);
    fmt_printf("lcm([12,100,7,0], [18,60,13,5]) =\n");
    matrix_print(l);

    /* fmax / fmin ignore NaN (NaN only where both are NaN) */
    double nan = (double)NAN;
    double xa[] = { 1.0, nan, 3.0, nan };
    double xb[] = { 2.0, 5.0, nan, nan };
    Matrix* XA = matrix_from_array(xa, 1, 4);
    Matrix* XB = matrix_from_array(xb, 1, 4);
    Matrix* mx = matrix_fmax(XA, XB);
    Matrix* mn = matrix_fmin(XA, XB);
    fmt_printf("fmax([1,nan,3,nan], [2,5,nan,nan]) =\n");
    matrix_print(mx);
    fmt_printf("fmin([1,nan,3,nan], [2,5,nan,nan]) =\n");
    matrix_print(mn);

    /* modf: split into fractional and integer parts (both keep sign) */
    double md[] = { 2.5, -2.5, 3.25 };
    Matrix* M = matrix_from_array(md, 1, 3);
    Matrix* fp = NULL;
    Matrix* ip = NULL;
    matrix_modf(M, &fp, &ip);
    fmt_printf("modf([2.5,-2.5,3.25]) frac =\n");
    matrix_print(fp);
    fmt_printf("modf([2.5,-2.5,3.25]) int =\n");
    matrix_print(ip);

    /* divmod: floor-division quotient + remainder (sign of divisor) */
    double na[] = { 7.0, -7.0, 7.0, -7.0 };
    double nb[] = { 3.0, 3.0, -3.0, -3.0 };
    Matrix* NA = matrix_from_array(na, 1, 4);
    Matrix* NB = matrix_from_array(nb, 1, 4);
    Matrix* q = NULL;
    Matrix* r = NULL;
    matrix_divmod(NA, NB, &q, &r);
    fmt_printf("divmod([7,-7,7,-7], [3,3,-3,-3]) quotient =\n");
    matrix_print(q);
    fmt_printf("divmod([7,-7,7,-7], [3,3,-3,-3]) remainder =\n");
    matrix_print(r);

    matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(g); matrix_deallocate(l);
    matrix_deallocate(XA); matrix_deallocate(XB); matrix_deallocate(mx); matrix_deallocate(mn);
    matrix_deallocate(M); matrix_deallocate(fp); matrix_deallocate(ip);
    matrix_deallocate(NA); matrix_deallocate(NB); matrix_deallocate(q); matrix_deallocate(r);
    return 0;
}
```

**Result**

```
gcd([12,100,7,0], [18,60,13,5]) =
|  6.00000 20.00000  1.00000  5.00000 |
lcm([12,100,7,0], [18,60,13,5]) =
|  36.00000 300.00000  91.00000   0.00000 |
fmax([1,nan,3,nan], [2,5,nan,nan]) =
| 2.00000 5.00000 3.00000     nan |
fmin([1,nan,3,nan], [2,5,nan,nan]) =
| 1.00000 5.00000 3.00000     nan |
modf([2.5,-2.5,3.25]) frac =
|  0.50000 -0.50000  0.25000 |
modf([2.5,-2.5,3.25]) int =
|  2.00000 -2.00000  3.00000 |
divmod([7,-7,7,-7], [3,3,-3,-3]) quotient =
|  2.00000 -3.00000 -3.00000  2.00000 |
divmod([7,-7,7,-7], [3,3,-3,-3]) remainder =
|  1.00000  2.00000 -2.00000 -1.00000 |
```

---

## Example 94 : floor-division, truncation, banker's rounding, NaN cleanup & inequality

The C analogues of NumPy's `numpy.floor_divide` (with full sign/zero semantics), `numpy.fix` (round toward zero — like `matrix_trunc`), `numpy.rint` (round-half-to-even), `numpy.nan_to_num` (replace NaN with 0), and `numpy.not_equal` (an element-wise 0/1 mask).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    /* floor_divide: floor(a/b), NumPy sign semantics */
    double da[] = { 7, -7, 7, -7 };
    double db[] = { 3, 3, -3, -3 };
    Matrix* A = matrix_from_array(da, 1, 4);
    Matrix* B = matrix_from_array(db, 1, 4);
    Matrix* fd = matrix_floor_divide(A, B);
    fmt_printf("floor_divide([7,-7,7,-7], [3,3,-3,-3]) =\n");
    matrix_print(fd);

    /* fix: round toward zero */
    double fx[] = { 2.7, -2.7, 3.5, -3.5 };
    Matrix* FX = matrix_from_array(fx, 1, 4);
    Matrix* fi = matrix_fix(FX);
    fmt_printf("fix([2.7,-2.7,3.5,-3.5]) =\n");
    matrix_print(fi);

    /* rint: round half-to-even */
    double rt[] = { 0.5, 1.5, 2.5, -2.5, 2.7 };
    Matrix* RT = matrix_from_array(rt, 1, 5);
    Matrix* ri = matrix_rint(RT);
    fmt_printf("rint([0.5,1.5,2.5,-2.5,2.7]) =\n");
    matrix_print(ri);

    /* nan_to_num: NaN -> 0 */
    double nn[] = { 1.0, (double)NAN, 3.0, (double)NAN };
    Matrix* NN = matrix_from_array(nn, 1, 4);
    Matrix* nc = matrix_nan_to_num(NN);
    fmt_printf("nan_to_num([1,nan,3,nan]) =\n");
    matrix_print(nc);

    /* not_equal: 0/1 mask */
    double xa[] = { 1, 2, 3, 4 };
    double xb[] = { 1, 5, 3, 8 };
    Matrix* XA = matrix_from_array(xa, 1, 4);
    Matrix* XB = matrix_from_array(xb, 1, 4);
    Matrix* ne = matrix_not_equal(XA, XB);
    fmt_printf("not_equal([1,2,3,4], [1,5,3,8]) =\n");
    matrix_print(ne);

    matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(fd);
    matrix_deallocate(FX); matrix_deallocate(fi);
    matrix_deallocate(RT); matrix_deallocate(ri);
    matrix_deallocate(NN); matrix_deallocate(nc);
    matrix_deallocate(XA); matrix_deallocate(XB); matrix_deallocate(ne);
    return 0;
}
```

**Result**

```
floor_divide([7,-7,7,-7], [3,3,-3,-3]) =
|  2.00000 -3.00000 -3.00000  2.00000 |
fix([2.7,-2.7,3.5,-3.5]) =
|  2.00000 -2.00000  3.00000 -3.00000 |
rint([0.5,1.5,2.5,-2.5,2.7]) =
|  0.00000  2.00000  2.00000 -2.00000  3.00000 |
nan_to_num([1,nan,3,nan]) =
| 1.00000 0.00000 3.00000 0.00000 |
not_equal([1,2,3,4], [1,5,3,8]) =
| 0.00000 1.00000 0.00000 1.00000 |
```

---

## Example 95 : float power, sigmoid/logit, xlogy, Heaviside step & complex phase

The C analogues of NumPy's `numpy.float_power` (`a**b` in floating point), SciPy's `scipy.special.expit`/`logit` (sigmoid and its inverse) and `scipy.special.xlogy` (`x·log(y)` with the `0·log(0)=0` convention), plus NumPy's `numpy.heaviside` (step with a chosen value at zero) and `numpy.angle` (phase of `real + i·imag`).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* float_power: a**b in floating point */
    double ba[] = { 2, 3, 9, 16 };
    double bb[] = { 3, 2, 0.5, 0.5 };
    Matrix* BA = matrix_from_array(ba, 1, 4);
    Matrix* BB = matrix_from_array(bb, 1, 4);
    Matrix* fp = matrix_float_power(BA, BB);
    fmt_printf("float_power([2,3,9,16], [3,2,0.5,0.5]) =\n");
    matrix_print(fp);

    /* expit: logistic sigmoid */
    double ed[] = { -2, -1, 0, 1, 2 };
    Matrix* E = matrix_from_array(ed, 1, 5);
    Matrix* ex = matrix_expit(E);
    fmt_printf("expit([-2,-1,0,1,2]) =\n");
    matrix_print(ex);

    /* logit: inverse sigmoid */
    double ld[] = { 0.1, 0.5, 0.9 };
    Matrix* L = matrix_from_array(ld, 1, 3);
    Matrix* lg = matrix_logit(L);
    fmt_printf("logit([0.1,0.5,0.9]) =\n");
    matrix_print(lg);

    /* xlogy: x*log(y), 0 when x==0 */
    double xx[] = { 0, 2, 3 };
    double xy[] = { 0, 1, 2 };
    Matrix* XX = matrix_from_array(xx, 1, 3);
    Matrix* XY = matrix_from_array(xy, 1, 3);
    Matrix* xl = matrix_xlogy(XX, XY);
    fmt_printf("xlogy([0,2,3], [0,1,2]) =\n");
    matrix_print(xl);

    /* heaviside: step with h0 at x==0 */
    double hx[] = { -2, 0, 3, 0 };
    double h0[] = { 0.5, 0.5, 0.5, 1.0 };
    Matrix* HX = matrix_from_array(hx, 1, 4);
    Matrix* H0 = matrix_from_array(h0, 1, 4);
    Matrix* hv = matrix_heaviside(HX, H0);
    fmt_printf("heaviside([-2,0,3,0], [0.5,0.5,0.5,1.0]) =\n");
    matrix_print(hv);

    /* angle: atan2(imag, real) */
    double re[] = { 1, 0, -1, 1 };
    double im[] = { 0, 1, 0, 1 };
    Matrix* RE = matrix_from_array(re, 1, 4);
    Matrix* IM = matrix_from_array(im, 1, 4);
    Matrix* an = matrix_angle(RE, IM);
    fmt_printf("angle(real=[1,0,-1,1], imag=[0,1,0,1]) =\n");
    matrix_print(an);

    matrix_deallocate(BA); matrix_deallocate(BB); matrix_deallocate(fp);
    matrix_deallocate(E); matrix_deallocate(ex);
    matrix_deallocate(L); matrix_deallocate(lg);
    matrix_deallocate(XX); matrix_deallocate(XY); matrix_deallocate(xl);
    matrix_deallocate(HX); matrix_deallocate(H0); matrix_deallocate(hv);
    matrix_deallocate(RE); matrix_deallocate(IM); matrix_deallocate(an);
    return 0;
}
```

**Result**

```
float_power([2,3,9,16], [3,2,0.5,0.5]) =
| 8.00000 9.00000 3.00000 4.00000 |
expit([-2,-1,0,1,2]) =
| 0.11920 0.26894 0.50000 0.73106 0.88080 |
logit([0.1,0.5,0.9]) =
| -2.19722  0.00000  2.19722 |
xlogy([0,2,3], [0,1,2]) =
| 0.00000 0.00000 2.07944 |
heaviside([-2,0,3,0], [0.5,0.5,0.5,1.0]) =
| 0.00000 0.50000 1.00000 1.00000 |
angle(real=[1,0,-1,1], imag=[0,1,0,1]) =
| 0.00000 1.57080 3.14159 0.78540 |
```

---

## Example 96 : equality masks, logical XOR, infinity tests & whole-matrix equality

The C analogues of NumPy's `numpy.equal` (element-wise `a == b` mask, where `NaN == NaN` is false), `numpy.logical_xor`, `numpy.isposinf`/`numpy.isneginf`, and `numpy.array_equal` (a single bool for the whole matrix).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    double inf = (double)INFINITY;
    double nan = (double)NAN;

    /* equal: element-wise a==b (nan==nan -> 0) */
    double da[] = { 1, 2, 3, nan };
    double db[] = { 1, 5, 3, nan };
    Matrix* A = matrix_from_array(da, 1, 4);
    Matrix* B = matrix_from_array(db, 1, 4);
    Matrix* eq = matrix_equal_elementwise(A, B);
    fmt_printf("equal([1,2,3,nan], [1,5,3,nan]) =\n");
    matrix_print(eq);

    /* logical_xor */
    double xa[] = { 0, 1, 0, 1 };
    double xb[] = { 0, 0, 1, 1 };
    Matrix* XA = matrix_from_array(xa, 1, 4);
    Matrix* XB = matrix_from_array(xb, 1, 4);
    Matrix* xr = matrix_logical_xor(XA, XB);
    fmt_printf("logical_xor([0,1,0,1], [0,0,1,1]) =\n");
    matrix_print(xr);

    /* isposinf / isneginf */
    double id[] = { 1, inf, -inf, nan };
    Matrix* I = matrix_from_array(id, 1, 4);
    Matrix* pp = matrix_isposinf(I);
    Matrix* nn = matrix_isneginf(I);
    fmt_printf("isposinf([1,inf,-inf,nan]) =\n");
    matrix_print(pp);
    fmt_printf("isneginf([1,inf,-inf,nan]) =\n");
    matrix_print(nn);

    /* array_equal: single bool */
    double c1[] = { 1, 2, 3 };
    double c2[] = { 1, 2, 3 };
    double c3[] = { 1, 2, 4 };
    Matrix* C1 = matrix_from_array(c1, 1, 3);
    Matrix* C2 = matrix_from_array(c2, 1, 3);
    Matrix* C3 = matrix_from_array(c3, 1, 3);
    if (matrix_array_equal(C1, C2)) {
        fmt_printf("array_equal([1,2,3], [1,2,3]) = true\n");
    } else {
        fmt_printf("array_equal([1,2,3], [1,2,3]) = false\n");
    }
    if (matrix_array_equal(C1, C3)) {
        fmt_printf("array_equal([1,2,3], [1,2,4]) = true\n");
    } else {
        fmt_printf("array_equal([1,2,3], [1,2,4]) = false\n");
    }

    matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(eq);
    matrix_deallocate(XA); matrix_deallocate(XB); matrix_deallocate(xr);
    matrix_deallocate(I); matrix_deallocate(pp); matrix_deallocate(nn);
    matrix_deallocate(C1); matrix_deallocate(C2); matrix_deallocate(C3);
    return 0;
}
```

**Result**

```
equal([1,2,3,nan], [1,5,3,nan]) =
| 1.00000 0.00000 1.00000 0.00000 |
logical_xor([0,1,0,1], [0,0,1,1]) =
| 0.00000 1.00000 1.00000 0.00000 |
isposinf([1,inf,-inf,nan]) =
| 0.00000 1.00000 0.00000 0.00000 |
isneginf([1,inf,-inf,nan]) =
| 0.00000 0.00000 1.00000 0.00000 |
array_equal([1,2,3], [1,2,3]) = true
array_equal([1,2,3], [1,2,4]) = false
```

---

## Example 97 : NaN-aware reductions and range builders

The C analogues of NumPy's `numpy.nansum`/`nanmean`/`nanmin`/`nanmax`/`nanprod` (reductions that ignore `NaN`) and the range factories `numpy.arange` (half-open) and `numpy.linspace` (closed, endpoint pinned).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    double nan = (double)NAN;

    /* NaN-aware reductions */
    double vd[] = { 1, nan, 3, nan };
    Matrix* V = matrix_from_array(vd, 1, 4);
    fmt_printf("nansum([1,nan,3,nan])  = %.5lf\n", matrix_nansum(V));
    fmt_printf("nanmean([1,nan,3,nan]) = %.5lf\n", matrix_nanmean(V));

    double wd[] = { 4, nan, 2, nan };
    Matrix* W = matrix_from_array(wd, 1, 4);
    fmt_printf("nanmin([4,nan,2,nan])  = %.5lf\n", matrix_nanmin(W));
    fmt_printf("nanmax([4,nan,2,nan])  = %.5lf\n", matrix_nanmax(W));

    double pd[] = { 2, nan, 3, nan };
    Matrix* P = matrix_from_array(pd, 1, 4);
    fmt_printf("nanprod([2,nan,3,nan]) = %.5lf\n", matrix_nanprod(P));

    /* arange & linspace */
    Matrix* ar = matrix_arange(0, 2, 0.5);
    fmt_printf("arange(0, 2, 0.5) =\n");
    matrix_print(ar);

    Matrix* ls = matrix_linspace(0, 1, 5);
    fmt_printf("linspace(0, 1, 5) =\n");
    matrix_print(ls);

    matrix_deallocate(V); matrix_deallocate(W); matrix_deallocate(P);
    matrix_deallocate(ar); matrix_deallocate(ls);
    return 0;
}
```

**Result**

```
nansum([1,nan,3,nan])  = 4.00000
nanmean([1,nan,3,nan]) = 2.00000
nanmin([4,nan,2,nan])  = 2.00000
nanmax([4,nan,2,nan])  = 4.00000
nanprod([2,nan,3,nan]) = 6.00000
arange(0, 2, 0.5) =
| 0.00000 0.50000 1.00000 1.50000 |
linspace(0, 1, 5) =
| 0.00000 0.25000 0.50000 0.75000 1.00000 |
```

---

## Example 98 : flat differences, indexing, selection, trimming, NaN cumsum, resize & unravel

The C analogues of NumPy's `numpy.ediff1d`, `numpy.flatnonzero`, `numpy.extract`, `numpy.trim_zeros`, `numpy.nancumsum`, `numpy.resize` (cycle-fill to a new shape), and `numpy.unravel_index` (flat indices → `(row, col)` pairs).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    /* ediff1d: consecutive differences */
    double dd[] = { 1, 2, 4, 7, 0 };
    Matrix* D = matrix_from_array(dd, 1, 5);
    Matrix* ed = matrix_ediff1d(D);
    fmt_printf("ediff1d([1,2,4,7,0]) =\n");
    matrix_print(ed);

    /* flatnonzero: flat indices of nonzeros */
    double fz[] = { 0, 3, 0, 5, 0, 8 };
    Matrix* FZ = matrix_from_array(fz, 2, 3);
    Matrix* fn = matrix_flatnonzero(FZ);
    fmt_printf("flatnonzero([[0,3,0],[5,0,8]]) =\n");
    matrix_print(fn);

    /* extract: values where mask nonzero */
    double cm[] = { 1, 0, 1, 0, 1, 0 };
    double vm[] = { 10, 20, 30, 40, 50, 60 };
    Matrix* CM = matrix_from_array(cm, 2, 3);
    Matrix* VM = matrix_from_array(vm, 2, 3);
    Matrix* ex = matrix_extract(CM, VM);
    fmt_printf("extract(mask, [10..60]) =\n");
    matrix_print(ex);

    /* trim_zeros: drop leading/trailing zeros */
    double tz[] = { 0, 0, 1, 2, 0, 3, 0, 0 };
    Matrix* TZ = matrix_from_array(tz, 1, 8);
    Matrix* tr = matrix_trim_zeros(TZ);
    fmt_printf("trim_zeros([0,0,1,2,0,3,0,0]) =\n");
    matrix_print(tr);

    /* nancumsum: running sum, NaN as 0 */
    double nc[] = { 1, (double)NAN, 3, 4 };
    Matrix* NC = matrix_from_array(nc, 2, 2);
    Matrix* cs = matrix_nancumsum(NC);
    fmt_printf("nancumsum([1,nan,3,4]) =\n");
    matrix_print(cs);

    /* resize: cycle-fill to new shape */
    double rz[] = { 1, 2, 3, 4 };
    Matrix* RZ = matrix_from_array(rz, 1, 4);
    Matrix* rs = matrix_resize(RZ, 2, 3);
    fmt_printf("resize([1,2,3,4], 2, 3) =\n");
    matrix_print(rs);

    /* unravel_index: flat -> (row,col) for shape (3,4) */
    double ui[] = { 0, 5, 7, 11 };
    Matrix* UI = matrix_from_array(ui, 1, 4);
    Matrix* ur = matrix_unravel_index(UI, 3, 4);
    fmt_printf("unravel_index([0,5,7,11], (3,4)) =\n");
    matrix_print(ur);

    matrix_deallocate(D); matrix_deallocate(ed);
    matrix_deallocate(FZ); matrix_deallocate(fn);
    matrix_deallocate(CM); matrix_deallocate(VM); matrix_deallocate(ex);
    matrix_deallocate(TZ); matrix_deallocate(tr);
    matrix_deallocate(NC); matrix_deallocate(cs);
    matrix_deallocate(RZ); matrix_deallocate(rs);
    matrix_deallocate(UI); matrix_deallocate(ur);
    return 0;
}
```

**Result**

```
ediff1d([1,2,4,7,0]) =
|  1.00000  2.00000  3.00000 -7.00000 |
flatnonzero([[0,3,0],[5,0,8]]) =
| 1.00000 3.00000 5.00000 |
extract(mask, [10..60]) =
| 10.00000 30.00000 50.00000 |
trim_zeros([0,0,1,2,0,3,0,0]) =
| 1.00000 2.00000 0.00000 3.00000 |
nancumsum([1,nan,3,4]) =
| 1.00000 1.00000 4.00000 8.00000 |
resize([1,2,3,4], 2, 3) =
| 1.00000 2.00000 3.00000 |
| 4.00000 1.00000 2.00000 |
unravel_index([0,5,7,11], (3,4)) =
| 0.00000 0.00000 |
| 1.00000 1.00000 |
| 1.00000 3.00000 |
| 2.00000 3.00000 |
```

---

## Example 99 : NaN-aware axis sum & argmax, geometric mean, SEM, distinct count, log-spaced ranges

The C analogues of NumPy's `numpy.nansum(axis=)` and `numpy.nanargmax`, SciPy's `scipy.stats.gmean` and `scipy.stats.sem`, pandas' `Series.nunique`, and NumPy's `numpy.logspace`/`numpy.geomspace`.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"
#include <math.h>

int main(void) {
    /* nansum_axis: per-axis sum ignoring NaN */
    double md[] = { 1, (double)NAN, 3, (double)NAN, 5, 6 };
    Matrix* M = matrix_from_array(md, 2, 3);
    Matrix* a0 = matrix_nansum_axis(M, 0);
    Matrix* a1 = matrix_nansum_axis(M, 1);
    fmt_printf("nansum axis=0 =\n");
    matrix_print(a0);
    fmt_printf("nansum axis=1 =\n");
    matrix_print(a1);

    /* nanargmax: flat index of max ignoring NaN */
    double na[] = { 1, (double)NAN, 5, 5, 2 };
    Matrix* NA = matrix_from_array(na, 1, 5);
    fmt_printf("nanargmax([1,nan,5,5,2]) = %lu\n", (unsigned long)matrix_nanargmax(NA));

    /* gmean, sem, nunique */
    double gd[] = { 1, 2, 3, 4 };
    Matrix* G = matrix_from_array(gd, 1, 4);
    fmt_printf("gmean([1,2,3,4]) = %.5lf\n", matrix_gmean(G));

    double sd[] = { 1, 2, 3, 4, 5 };
    Matrix* S = matrix_from_array(sd, 1, 5);
    fmt_printf("sem([1,2,3,4,5]) = %.5lf\n", matrix_sem(S));

    double ud[] = { 1, 2, 2, 3, (double)NAN, (double)NAN };
    Matrix* U = matrix_from_array(ud, 1, 6);
    fmt_printf("nunique([1,2,2,3,nan,nan]) = %lu\n", (unsigned long)matrix_nunique(U));

    /* logspace & geomspace */
    Matrix* lg = matrix_logspace(0, 2, 3, 10.0);
    fmt_printf("logspace(0, 2, 3, base=10) =\n");
    matrix_print(lg);

    Matrix* gm = matrix_geomspace(1, 1000, 4);
    fmt_printf("geomspace(1, 1000, 4) =\n");
    matrix_print(gm);

    matrix_deallocate(M); matrix_deallocate(a0); matrix_deallocate(a1);
    matrix_deallocate(NA); matrix_deallocate(G); matrix_deallocate(S); matrix_deallocate(U);
    matrix_deallocate(lg); matrix_deallocate(gm);
    return 0;
}
```

**Result**

```
nansum axis=0 =
| 1.00000 5.00000 9.00000 |
nansum axis=1 =
|  4.00000 |
| 11.00000 |
nanargmax([1,nan,5,5,2]) = 2
gmean([1,2,3,4]) = 2.21336
sem([1,2,3,4,5]) = 0.70711
nunique([1,2,2,3,nan,nan]) = 3
logspace(0, 2, 3, base=10) =
|   1.00000  10.00000 100.00000 |
geomspace(1, 1000, 4) =
|    1.00000   10.00000  100.00000 1000.00000 |
```

---

## Example 100 : ravel, vector dot/vdot, offset trace & atleast_2d

The C analogues of NumPy's `numpy.ravel` (flatten), `numpy.dot` (1-D inner product), `numpy.vdot` (flattened inner product), `numpy.trace(offset=k)` (diagonal-band sum), and `numpy.atleast_2d` (promote a vector to a `1×N` row).

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* ravel: flatten 2x3 -> 1x6 */
    double rd[] = { 1, 2, 3, 4, 5, 6 };
    Matrix* R = matrix_from_array(rd, 2, 3);
    Matrix* rv = matrix_ravel(R);
    fmt_printf("ravel([[1,2,3],[4,5,6]]) =\n");
    matrix_print(rv);

    /* dot: inner product of two vectors */
    double da[] = { 1, 2, 3 };
    double db[] = { 4, 5, 6 };
    Matrix* A = matrix_from_array(da, 1, 3);
    Matrix* B = matrix_from_array(db, 1, 3);
    Matrix* dt = matrix_dot(A, B);
    fmt_printf("dot([1,2,3], [4,5,6]) =\n");
    matrix_print(dt);

    /* vdot: flattened inner product of two matrices */
    double va[] = { 1, 2, 3, 4 };
    double vb[] = { 5, 6, 7, 8 };
    Matrix* VA = matrix_from_array(va, 2, 2);
    Matrix* VB = matrix_from_array(vb, 2, 2);
    Matrix* vd = matrix_vdot(VA, VB);
    fmt_printf("vdot([[1,2],[3,4]], [[5,6],[7,8]]) =\n");
    matrix_print(vd);

    /* trace_offset: diagonal sums at offsets */
    double td[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    Matrix* T = matrix_from_array(td, 3, 3);
    fmt_printf("trace offset  0 = %.5lf\n", matrix_trace_offset(T, 0));
    fmt_printf("trace offset +1 = %.5lf\n", matrix_trace_offset(T, 1));
    fmt_printf("trace offset -1 = %.5lf\n", matrix_trace_offset(T, -1));

    /* atleast_2d: promote a column vector to a 1xN row */
    double cd[] = { 1, 2, 3 };
    Matrix* C = matrix_from_array(cd, 3, 1);
    Matrix* a2 = matrix_atleast_2d(C);
    fmt_printf("atleast_2d(column [1,2,3]) =\n");
    matrix_print(a2);

    matrix_deallocate(R); matrix_deallocate(rv);
    matrix_deallocate(A); matrix_deallocate(B); matrix_deallocate(dt);
    matrix_deallocate(VA); matrix_deallocate(VB); matrix_deallocate(vd);
    matrix_deallocate(T);
    matrix_deallocate(C); matrix_deallocate(a2);
    return 0;
}
```

**Result**

```
ravel([[1,2,3],[4,5,6]]) =
| 1.00000 2.00000 3.00000 4.00000 5.00000 6.00000 |
dot([1,2,3], [4,5,6]) =
| 32.00000 |
vdot([[1,2],[3,4]], [[5,6],[7,8]]) =
| 70.00000 |
trace offset  0 = 15.00000
trace offset +1 = 8.00000
trace offset -1 = 12.00000
atleast_2d(column [1,2,3]) =
| 1.00000 2.00000 3.00000 |
```

---

## Example 101 : FFT frequency axes and window functions

The C analogues of NumPy's `numpy.fft.fftfreq`/`numpy.fft.rfftfreq` (the frequency bins for a full and a real FFT) and the `numpy.hanning`/`numpy.hamming` window functions.

```c
#include "matrix/matrix.h"
#include "fmt/fmt.h"

int main(void) {
    /* fftfreq: full DFT frequency bins */
    Matrix* ff = matrix_fftfreq(8, 1.0);
    fmt_printf("fftfreq(8, d=1.0) =\n");
    matrix_print(ff);

    /* rfftfreq: real-FFT frequency bins */
    Matrix* rf = matrix_rfftfreq(8, 1.0);
    fmt_printf("rfftfreq(8, d=1.0) =\n");
    matrix_print(rf);

    /* hanning window */
    Matrix* hn = matrix_hanning(5);
    fmt_printf("hanning(5) =\n");
    matrix_print(hn);

    /* hamming window */
    Matrix* hm = matrix_hamming(5);
    fmt_printf("hamming(5) =\n");
    matrix_print(hm);

    matrix_deallocate(ff); matrix_deallocate(rf);
    matrix_deallocate(hn); matrix_deallocate(hm);
    return 0;
}
```

**Result**

```
fftfreq(8, d=1.0) =
|  0.00000  0.12500  0.25000  0.37500 -0.50000 -0.37500 -0.25000 -0.12500 |
rfftfreq(8, d=1.0) =
| 0.00000 0.12500 0.25000 0.37500 0.50000 |
hanning(5) =
| 0.00000 0.50000 1.00000 0.50000 0.00000 |
hamming(5) =
| 0.08000 0.54000 1.00000 0.54000 0.08000 |
```

---

## License

This project is open-source and available under [ISC License].