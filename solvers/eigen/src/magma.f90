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
!
! ifort -assume nounderscore -O3 -g -Icompil/magma-2.5.3/include -Lcompil/magma-2.5.3/lib -lmagma -o diag diag.f90 -mkl
  
  use omp_lib
  use magma
implicit none
!     .. Parameters ..
integer N
parameter (N = 5000)
logical OUTPUT 
parameter (OUTPUT = .false.)
! Version indicates which algorithm to use 1=ZHEEV, 2=ZHEEVD, 3=ZHEEVR, 4=ZHEEVX
integer VERSION
integer LDA, LDZ
parameter (LDA = N, LDZ = N)
integer LWMAX
parameter (LWMAX = 1000000)
!
!     .. Local Scalars ..
integer i, info, lwork, lrwork, liwork, il, iu, m, ompthreads
double precision abstol, vl, vu
double precision :: tstart, tend
character(len=2) :: arg

!
!     .. Local Arrays ..
integer isuppz(N), iwork(LWMAX), ifail(N)
double precision w(N), rwork(LWMAX)
double precision ar(LDA*2, N*2)
double complex a(LDA, N), z(LDZ, N), work(LWMAX)
equivalence(ar,a)

! MAGMA stuff
!magma_devptr_t :: dA, dZ
integer(kind=8) :: dA, dZ, queue
integer          :: dev
complex*16       :: wA(N), wZ( n, n )
integer          :: ldwa, nb, ldwz
integer          :: mout(1)

!
!     .. External Subroutines ..
external ZHEEV
external ZHEEVX
external ZHEEVD
external ZHEEVR
!
!     .. Auxillary Subroutines ..
interface
    subroutine PRINT_MATRIX (Desc, M, N, A, Lda, Maxnum)
        character*(*), intent(in) :: Desc
        integer, intent (in) :: M
        integer, intent (in) :: N
        double complex, intent(in) :: A(Lda, *)
        integer, intent(in) :: Lda
        integer, intent(in), optional :: Maxnum
    end subroutine PRINT_MATRIX
    subroutine PRINT_RMATRIX (Desc, M, N, R, Lda, Maxnum)
        character*(*), intent(in) :: Desc
        integer, intent (in) :: M
        integer, intent (in) :: N
        double precision, intent(in) :: R(Lda, *)
        integer, intent(in) :: Lda
        integer, intent(in), optional :: Maxnum
    end subroutine PRINT_RMATRIX
end interface
!
!     .. Intrinsic Functions ..
intrinsic INT, MIN

!
!     .. Executable Statements ..
!
!     Populate matrix with random values
call random_seed()
call random_number(ar)

VERSION=3  ! default 

call get_command_argument(1, arg)
if (len_trim(arg) == 0) then 
  VERSION=3
else 
  READ(arg,*)VERSION  
endif

if (OUTPUT .eqv. .true.) then
  write (*, *) 'ZHEEVR Example Program Results'
endif

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

! MAGMA
call magmaf_init()
dev = 0
call magmaf_queue_create( dev, queue )

info = magmaf_zmalloc( dA, m*n )
info = magmaf_zmalloc( dZ, m*n )
ldwa = N
ldwz = N
nb = magmaf_get_zhetrd_nb( N )

   ! difference setmatrix vs copymatrix? Same interface
call magmaf_zsetmatrix( m, n, a, LDA, dA, LDA, queue )
call magmaf_queue_sync( queue )


! Version indicates which algorithm to use 1=ZHEEV, 2=ZHEEVD, 3=ZHEEVR, 4=ZHEEVX
if (VERSION == 1) then
  !!     Compute all eigenvalues and eigenvectors using QR
  call ZHEEV( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, info)
  lwork = MIN( LWMAX, INT( work( 1 ) ) )
  !     Solve eigenproblem.
  call ZHEEV( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, info)
else if (VERSION == 2) then
! Program computes all eigenvalues and eigenvectors of a complex Hermitian matrix A using divide and conquer algorithm,
  call ZHEEVD( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, &
              lrwork, iwork, liwork, info)
      lwork = MIN( LWMAX, INT( work( 1 ) ) )
      lrwork = MIN( LWMAX, INT( rwork( 1 ) ) )
      liwork = MIN( LWMAX, iwork( 1 ) )
!  *     Solve eigenproblem.  *
  call ZHEEVD( 'Vectors', 'Lower', N, a, LDA, w, work, lwork, rwork, &
              lrwork, iwork, liwork, info)
else if (VERSION == 3) then
! Relatively Robust Representation (RRR)
  !     Compute all eigenvalues and eigenvectors (indicated by the second
  !     parameter (RANGE='All')
  call ZHEEVR('Vectors', 'All', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
          & m, w, z, LDZ, isuppz, work, lwork, rwork, lrwork, iwork, liwork, &
          & info)
  lwork = MIN(LWMAX, INT(work(1)))
  lrwork = MIN(LWMAX, INT(rwork(1)))
  liwork = MIN(LWMAX, iwork(1))
  !
  !     Solve eigenproblem.
  !
  call ZHEEVR('Vectors', 'Values', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
          & m, w, z, LDZ, isuppz, work, lwork, rwork, lrwork, iwork, liwork, &
          & info)
else if (VERSION == 4) then
!  QR iteration, expert
  call ZHEEVX( 'Vectors', 'Values', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
               m, w, z, LDZ, work, lwork, rwork, iwork, ifail, info) 
  lwork = MIN( LWMAX, INT( work( 1 ) ) )
  !     Solve eigenproblem.
  call ZHEEVX( 'Vectors', 'Values', 'Lower', N, a, LDA, vl, vu, il, iu, abstol, &
               m, w, z, LDZ, work, lwork, rwork, iwork, ifail, info) 


else if ( VERSION == 11 ) then

   write( 5,*) "Not implemented yet. Come back later!"

else if ( VERSION == 12 ) then

   call MAGMAF_ZHEEVD_GPU( 'Vectors', 'Lower', n, dA, LDA, w, &
        wA, ldwa, work, lwork, rwork, lrwork, iwork, liwork, info)

   lwork = max( N + N*NB, 2*N + N**2 )
   lrwork = 1 + 5*N + 2*N**2
   liwork = 3 + 5*N
   
   call MAGMAF_ZHEEVD_GPU( 'Vectors', 'Lower', n, dA, LDA, w, &
        wA, ldwa, work, lwork, rwork, lrwork, iwork, liwork, info)

else if ( VERSION == 13 ) then

   call MAGMAF_ZHEEVR_GPU( 'Vectors', 'All', 'Lower', n, dA, LDA, vl, vu, il, iu, abstol, &
           mout, w,  dZ, ldz, isuppz, wA, ldwa, wZ, ldwz, work, lwork, rwork, lrwork, iwork, liwork, &
           info)
   lwork = (n+1)*n
   lrwork = max( 1, 24*n )
   liwork = max( 1, 10*n )

   call MAGMAF_ZHEEVR_GPU( 'Vectors', 'All', 'Lower', n, dA, LDA, vl, vu, il, iu, abstol, &
        mout, w,  dZ, ldz, isuppz, wA, ldwa, wZ, ldwz, work, lwork, rwork, lrwork, iwork, liwork, &
        info)
   m = mout(1)
   
else if ( VERSION == 14 ) then

   write( 5,*) "Not implemented yet. Come back later!"

   !magmaf_zheevx_gpu( jobz, range, uplo, n, dA, ldda, vl, vu, il, iu, abstol,  &
!        mout, w, dZ, lddz, wA, ldwa, wZ, ldwz, work, lwork, rwork, iwork, ifail, info )

!   call MAGMAF_ZHEEVX_GPU( 'Vectors', 'Values', 'Lower', n, dA, LDA, vl, vu, il, iu, abstol, &
!       m, w, dZ, LDZ, wA, ldwa, wZ, ldwz, &
!       work, lwork, rwork, iwork, ifail, info ) 

   !!  lwork = MIN( LWMAX, INT( work( 1 ) ) )
!!  call MAGMAF_ZHEEVX_GPU( 'Vectors', 'Values', 'Lower', N, dA, LDA, vl, vu, il, iu, abstol, &
!!       m, w, dZ, LDZ, wA, ldwa, wZ, ldwz, &
!!       work, lwork, rwork, iwork, ifail, info) 

endif

call magmaf_zgetmatrix( m, n, dA, lda, a, lda, queue )
call magmaf_queue_sync( queue )

!
!     Check for convergence.
!
if (info>0) then
    write (*, *) 'The algorithm failed to compute eigenvalues.'
    STOP
endif
tend = omp_get_wtime()

!
!     Print the number of eigenvalues found.
!
write (*, '(A,I1,A,I2,A,F6.2)', advance="no") 'alg=', VERSION, ',threads=', ompthreads, ',time=', tend-tstart
write (*, '(A,I5)', advance="no") ',N=', N
write (*, '(A,I5)') ',number_eigenvalues_found=', m
!
!     Print eigenvalues.
!
 
if (OUTPUT .eqv. .true.) then
  if (VERSION == 1 .or. VERSION == 2) then
    m = N
    z = a
  endif
  call PRINT_RMATRIX('Selected eigenvalues', 1, m, w, 1, Maxnum = 12)
!
!     Print eigenvectors.
!
  call PRINT_MATRIX('Selected eigenvectors (stored columnwise)', N, m, z, LDZ, Maxnum = 12)
endif

call magmaf_queue_destroy( queue )
info = magmaf_free( dA )
info = magmaf_free( dZ )
call magmaf_finalize()

end program main
!
!     End of ZHEEVR Example.
!
!  =============================================================================
!
!     Auxiliary routine: printing a matrix.
!
subroutine PRINT_MATRIX(Desc, M, N, A, Lda, Maxnum)
    implicit none
    character*(*), intent(in) :: Desc
    integer, intent (in) :: M
    integer, intent (in) :: N
    double complex, intent(in) :: A(Lda, *)
    integer, intent(in) :: Lda
    integer, intent(in), optional :: Maxnum
    !
    integer i, j, Nprint
    !
    if (present(Maxnum)) then
        Nprint = Maxnum
    else
        Nprint = M
    endif
    write (*, *)
    write (*, *) Desc
    do i = 1, MIN(Nprint, M)
        write (*, 99001) (A(i, j), j = 1, N)
        !
        99001    FORMAT (11(:, 1X, '(', F6.2, ',', F6.2, ')'))
    enddo
    if (Nprint < M) then
        write (*, *) "..."
    endif
end

!
!     Auxiliary routine: printing a real matrix.
!
subroutine PRINT_RMATRIX(Desc, M, N, R, Lda, Maxnum)
    implicit none
    character*(*), intent(in) :: Desc
    integer, intent (in) :: M
    integer, intent (in) :: N
    double precision, intent(in) :: R(Lda, *)
    integer, intent(in) :: Lda
    integer, intent(in), optional :: Maxnum
    !
    integer i, j, Nprint
    !
    if (present(Maxnum)) then
        Nprint = Maxnum
    else
        Nprint = M
    endif
    write (*, *)
    write (*, *) Desc
    do i = 1, MIN(Nprint, M)
        write (*, 99001) (R(i, j), j = 1, N)
        !
        99001    FORMAT (11(:, 1X, F6.2))
    enddo
    if (Nprint < M) then
        write (*, *) "..."
    endif
end

