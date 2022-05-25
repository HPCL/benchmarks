/*
 * N Body problem
 *
 * Available in High Performance Parallelism Pearls ch 9/10
 *
 * questions ask Brian Gravelle gravelle@lanl.gov
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#define double double

#define TRUE  1
#define FALSE 0

#ifndef GAP
#define GAP 0
#endif

struct Inputs 
{
  size_t order; // size of array
  size_t time_steps;
  int threads;
};

struct Parameters 
{
  double softing;
  double dt;
};

struct Particles 
{
  double *m;
  double *x;
  double *y;
  double *z;
  double *vx;
  double *vy;
  double *vz;
};


void get_input(int argc, char **argv, struct Inputs* input);
void n_body_problem(size_t order, size_t time_steps,
                    struct Parameters* parameters,
                    struct Particles* particles);
void create_particles(size_t order, struct Particles* particles);
void destroy_particles(size_t order, struct Particles* particles);


// main function
int main(int argc, char **argv) {

  struct Inputs input;
  struct Particles particles;
  struct Parameters parameters;
  parameters.softing = 0.1;
  parameters.dt = 0.1;

  get_input(argc, argv, &input);

  size_t correct;
  double run_time;
  double mflops;
	
  // size_t i,j,k,r;
  
  double start, end;

  omp_set_num_threads(input.threads);
  int threads = omp_get_max_threads();
  printf("\nAvailable threads =  %d \n", threads);


#ifdef USE_CALI
  cali_init();
  cali_id_t thread_attr = cali_create_attribute("thread_id", CALI_TYPE_INT, CALI_ATTR_ASVALUE | CALI_ATTR_SKIP_EVENTS);

#ifdef USE_CALI_REG
  #pragma omp parallel
  {
  cali_set_int(thread_attr, omp_get_thread_num());
  }
#endif
#ifdef USE_CALI_UNCORE
  cali_set_int(thread_attr, omp_get_thread_num());
#endif

#endif


  // initialize the matrices
  printf("init...\n");
  create_particles(input.order, &particles);

  printf("Running N Body .....");fflush(stdout);
  start = omp_get_wtime();


  n_body_problem(input.order, input.time_steps, &parameters, &particles);
  

  end = omp_get_wtime();
  printf("%f\n\n", end-start);

  destroy_particles(input.order, &particles);

  return 0;

}



void n_body_problem(size_t order, size_t time_steps,
                    struct Parameters* parameters,
                    struct Particles* particles) {

  // double dx;
  // double dy;
  // double dz;
  const double dt = parameters->dt;
  const double s  = parameters->softing;

  for (int i = 0; i < time_steps; i++) {


#ifdef USE_CALI_UNCORE
CALI_MARK_BEGIN("N_Body");
#endif

    // compute
    #pragma omp parallel
    {

#ifdef USE_CALI_REG
CALI_MARK_BEGIN("N_Body");
#endif
    
    #pragma omp for
    for (int j = 0; j < order; j++) {
      double Fx = 0.0;
      double Fy = 0.0;
      double Fz = 0.0;
      #pragma loop loop_fission_target
      for (int k = 0; k < order; k++) {
        double dx,dy,dz,dr_sq,dr_recip,dr_cube,m_r;
        dx = particles->x[k] - particles->x[j];
        dy = particles->y[k] - particles->y[j];
        dz = particles->z[k] - particles->z[j];
        dr_sq = dx*dx + dy*dy + dz*dz + s;
      #pragma statement fission_point
        dr_recip = 1.0 / sqrt(dr_sq);
      #pragma statement fission_point
        dr_cube = dr_recip*dr_recip*dr_recip;
        m_r = particles->m[k] * dr_cube;

        Fx += m_r * dx;
        Fy += m_r * dy;
        Fz += m_r * dz;
      }
      particles->vx[j] += Fx * dt;
      particles->vy[j] += Fy * dt;
      particles->vz[j] += Fz * dt;
    }

    //update
    #pragma omp for
    for (int j = 0; j < order; j++) {
      particles->x[j] += particles->vx[j] * dt;
      particles->y[j] += particles->vy[j] * dt;
      particles->z[j] += particles->vz[j] * dt;
    }

#ifdef USE_CALI_REG
CALI_MARK_END("N_Body");
#endif

    } //parallel

#ifdef USE_CALI_UNCORE
CALI_MARK_END("N_Body");
#endif
  } // time step


}


void create_particles(size_t order, struct Particles* particles) {
  particles->m  = (double*)malloc(order*sizeof(double)+GAP);
  particles->x  = (double*)malloc(order*sizeof(double)+GAP);
  particles->y  = (double*)malloc(order*sizeof(double)+GAP);
  particles->z  = (double*)malloc(order*sizeof(double)+GAP);
  particles->vx = (double*)malloc(order*sizeof(double)+GAP);
  particles->vy = (double*)malloc(order*sizeof(double)+GAP);
  particles->vz = (double*)malloc(order*sizeof(double)+GAP);
}

void destroy_particles(size_t order, struct Particles* particles) {
  free(particles->x);
  free(particles->y);
  free(particles->z);
  free(particles->vx);
  free(particles->vy);
  free(particles->vz);
}


void get_input(int argc, char **argv, struct Inputs* input) {

  int i = 1;

  input->order      = 65536; // 2^16
  input->time_steps = 4;
  input->threads    = 16;

  for(i = 1; i < argc; i++) {

    if ( !(strcmp("-h", argv[i])) || !(strcmp("--help", argv[i])) ) {
      printf("\n");
      printf("n body problem help usage:\n");
      printf("  -h --help ...................... print this message\n");
      printf("  -n -o -p --order [] ............ set the number of particles (65536)\n");
      printf("  -s --time_steps [] ............. set the number of time steps (4)\n");
      printf("  -t --num_threads [] ............ set the number of threads (16)\n");
      printf("\n");
      exit(0);
    }

    if ( !(strcmp("-n", argv[i])) || !(strcmp("-p", argv[i])) || !(strcmp("-o", argv[i])) || !(strcmp("--order", argv[i])) ) {
      if (i++ < argc){
        input->order = atoi(argv[i]);
      } else {
        printf("Please include a flop count with that option o/order\n");
        exit(1);
      }
    }

    if ( !(strcmp("-s", argv[i])) || !(strcmp("--time_steps", argv[i])) ) {
      if (i++ < argc){
        input->time_steps = atoi(argv[i]);
      } else {
        printf("Please include a count of time steps count with option t/time_steps\n");
        exit(1);
      }
    }

    if ( !(strcmp("-t", argv[i])) || !(strcmp("--num_threads", argv[i])) ) {
      if (i++ < argc){
        input->threads = atoi(argv[i]);
      } else {
        printf("Please include a number of threads with option n/num_threads\n");
        exit(1);
      }
    }

  }

}

