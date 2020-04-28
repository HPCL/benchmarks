program main
!=================================================================
!
!  ZHEEVR Example. (based on the Intel MKL example)
!  ==============
!
!  Program computes eigenvalues specified by a selected range of values
!  and corresponding eigenvectors of a complex Hermitian matrix A using the
!  Relatively Robust Representations.
!
!  Description.
!  ============
!
!  The routine computes selected eigenvalues and, optionally, eigenvectors of
!  an n-by-n complex Hermitian matrix A. The eigenvector v(j) of A satisfies
!
!  A*v(j) = lambda(j)*v(j)
!
!  where lambda(j) is its eigenvalue. The computed eigenvectors are
!  orthonormal.
!  Eigenvalues and eigenvectors can be selected by specifying either a range
!  of values or a range of indices for the desired eigenvalues.
!
!  =============================================================================
!
use omp_lib
IMPLICIT NONE
!     .. Parameters ..
INTEGER N
PARAMETER (N = 5000)
LOGICAL OUTPUT 
PARAMETER (OUTPUT = .FALSE.)
! Version indicates which algorithm to use 1=ZHEEV, 2=ZHEEVR
INTEGER VERSION
PARAMETER (VERSION = 1)
INTEGER LDA, LDZ
PARAMETER (LDA = N, LDZ = N)
INTEGER LWMAX
PARAMETER (LWMAX = 1000000)
!
!     .. Local Scalars ..
INTEGER info, lwork, lrwork, liwork, il, iu, m, ompthreads
DOUBLE PRECISION abstol, vl, vu
DOUBLE PRECISION :: tstart, tend
!
!     .. Local Arrays ..
INTEGER isuppz(N), iwork(LWMAX)
DOUBLE PRECISION w(N), rwork(LWMAX)
DOUBLE PRECISION ar(LDA*2, N*2)
DOUBLE COMPLEX a(LDA, N), z(LDZ, N), work(LWMAX)
EQUIVALENCE(ar,a)

!
!     .. External Subroutines ..
EXTERNAL ZHEEV
EXTERNAL ZHEEVR
!
!     .. Auxillary Subroutines ..
INTERFACE
    SUBROUTINE PRINT_MATRIX (Desc, M, N, A, Lda, Maxnum)
        CHARACTER*(*), INTENT(IN) :: Desc
        INTEGER, INTENT (IN) :: M
        INTEGER, INTENT (IN) :: N
        DOUBLE COMPLEX, INTENT(IN) :: A(Lda, *)
        INTEGER, INTENT(IN) :: Lda
        INTEGER, INTENT(IN), OPTIONAL :: Maxnum
    END SUBROUTINE PRINT_MATRIX
    SUBROUTINE PRINT_RMATRIX (Desc, M, N, R, Lda, Maxnum)
        CHARACTER*(*), INTENT(IN) :: Desc
        INTEGER, INTENT (IN) :: M
        INTEGER, INTENT (IN) :: N
        DOUBLE PRECISION, INTENT(IN) :: R(Lda, *)
        INTEGER, INTENT(IN) :: Lda
        INTEGER, INTENT(IN), OPTIONAL :: Maxnum
    END SUBROUTINE PRINT_RMATRIX
END INTERFACE
!
!     .. Intrinsic Functions ..
INTRINSIC INT, MIN

!
!     .. Executable Statements ..
!
!     Populate matrix with random values
!CALL RANDOM_INIT(.true., .true.)
CALL RANDOM_SEED()
CALL RANDOM_NUMBER(ar)

IF (OUTPUT .EQV. .TRUE.) THEN
  WRITE (*, *) 'ZHEEVR Example Program Results'
ENDIF

!     Negative ABSTOL means using the default value
abstol = -1.0
!     Set VL, VU to compute eigenvalues in half-open (VL,VU] interval,
!     and change the second parameter in the call to ZHEEVR to 'Values'
vl = -5.0
vu = 5.0
!
!     Query the optimal workspace.
!
lwork = -1
lrwork = -1
liwork = -1

ompthreads = omp_get_max_threads()
tstart = omp_get_wtime()

IF (VERSION == 1) THEN
  !     Compute all eigenvalues and eigenvectors
  CALL ZHEEV( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, info)
  lwork = MIN( LWMAX, INT( work( 1 ) ) )
  !     Solve eigenproblem.
  CALL ZHEEV( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, info)
ELSE
  !     Compute all eigenvalues and eigenvectors (indicated by the second
  !     parameter (RANGE='All')
  CALL ZHEEVR('Vectors', 'All', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
          & m, w, z, LDZ, isuppz, work, lwork, rwork, lrwork, iwork, liwork, &
          & info)
  lwork = MIN(LWMAX, INT(work(1)))
  lrwork = MIN(LWMAX, INT(rwork(1)))
  liwork = MIN(LWMAX, iwork(1))
  !
  !     Solve eigenproblem.
  !
  CALL ZHEEVR('Vectors', 'Values', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
          & m, w, z, LDZ, isuppz, work, lwork, rwork, lrwork, iwork, liwork, &
          & info)
ENDIF
!
!     Check for convergence.
!
IF (info>0) THEN
    WRITE (*, *) 'The algorithm failed to compute eigenvalues.'
    STOP
ENDIF
tend = omp_get_wtime()

!
!     Print the number of eigenvalues found.
!
WRITE (*, '(/A,I2,A,F6.2)', advance="no") 'threads=', ompthreads, ',time=', tend-tstart
WRITE (*, '(A,I5)', advance="no") ',N=', N
WRITE (*, '(A,I5)') ',number_eigenvalues_found=', m
!
!     Print eigenvalues.
!
 
IF (OUTPUT .EQV. .TRUE.) THEN
      CALL PRINT_RMATRIX('Selected eigenvalues', 1, m, w, 1, Maxnum = 12)
!
!     Print eigenvectors.
!
      CALL PRINT_MATRIX('Selected eigenvectors (stored columnwise)', N, m, &
        & z, LDZ, Maxnum = 12)
ENDIF
END
!
!     End of ZHEEVR Example.
!
!  =============================================================================
!
!     Auxiliary routine: printing a matrix.
!
SUBROUTINE PRINT_MATRIX(Desc, M, N, A, Lda, Maxnum)
    IMPLICIT NONE
    CHARACTER*(*), INTENT(IN) :: Desc
    INTEGER, INTENT (IN) :: M
    INTEGER, INTENT (IN) :: N
    DOUBLE COMPLEX, INTENT(IN) :: A(Lda, *)
    INTEGER, INTENT(IN) :: Lda
    INTEGER, INTENT(IN), OPTIONAL :: Maxnum
    !
    INTEGER i, j, Nprint
    !
    IF (PRESENT(Maxnum)) THEN
        Nprint = Maxnum
    ELSE
        Nprint = M
    ENDIF
    WRITE (*, *)
    WRITE (*, *) Desc
    DO i = 1, MIN(Nprint, M)
        WRITE (*, 99001) (A(i, j), j = 1, N)
        !
        99001    FORMAT (11(:, 1X, '(', F6.2, ',', F6.2, ')'))
    ENDDO
    IF (Nprint < M) THEN
        WRITE (*, *) "..."
    ENDIF
END

!
!     Auxiliary routine: printing a real matrix.
!
SUBROUTINE PRINT_RMATRIX(Desc, M, N, R, Lda, Maxnum)
    IMPLICIT NONE
    CHARACTER*(*), INTENT(IN) :: Desc
    INTEGER, INTENT (IN) :: M
    INTEGER, INTENT (IN) :: N
    DOUBLE PRECISION, INTENT(IN) :: R(Lda, *)
    INTEGER, INTENT(IN) :: Lda
    INTEGER, INTENT(IN), OPTIONAL :: Maxnum
    !
    INTEGER i, j, Nprint
    !
    IF (PRESENT(Maxnum)) THEN
        Nprint = Maxnum
    ELSE
        Nprint = M
    ENDIF
    WRITE (*, *)
    WRITE (*, *) Desc
    DO i = 1, MIN(Nprint, M)
        WRITE (*, 99001) (R(i, j), j = 1, N)
        !
        99001    FORMAT (11(:, 1X, F6.2))
    ENDDO
    IF (Nprint < M) THEN
        WRITE (*, *) "..."
    ENDIF
END

