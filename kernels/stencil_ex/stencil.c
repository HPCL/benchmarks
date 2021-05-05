/*
 * Simple Stencil example
 * Main program example
 *
 * Brian J Gravelle
 * gravelle@cs.uoregon.edu
 *
 */

#include "mesh.h"

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#ifdef USE_LIKWID
#include <likwid-marker.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#ifndef X_SIZE
#define X_SIZE 10000
#endif

#ifndef Y_SIZE
#define Y_SIZE 20000
#endif

#ifndef TIME
#define TIME 0.0
#endif

#ifndef STEP
#define STEP 1.0
#endif

#ifndef TIME_STOP
#define TIME_STOP 5.0
#endif

#ifndef FILE_NAME
#define FILE_NAME "openmp_results.txt"
#endif

#if USE_AOS

// create and fill the mesh with starting values
int init_mesh(struct Mesh ***mesh, int x_size, int y_size) {

  struct Mesh **_mesh;

  int err = FALSE;
  int i, j;

  double H = 100;
  double V = 1000;

  // allocate memory and verify that it worked
  _mesh = (struct Mesh**) malloc(x_size * sizeof(struct Mesh*));
  if(_mesh == NULL) err = TRUE;
  for (i = 0; i < x_size; ++i) {
    _mesh[i] = (struct Mesh*) malloc(y_size * sizeof(struct Mesh));
    if(_mesh[i] == NULL) err = TRUE;
  }

  // define starting values
  for (i = 0; i < x_size; i++) {
    V = 1000;
    for (j = 0; j < y_size; j++) {
      ACCESS_MESH(_mesh, i, j, avg) = H;
      ACCESS_MESH(_mesh, i, j, sum) = V;
      ACCESS_MESH(_mesh, i, j, pde) = i*j;
      ACCESS_MESH(_mesh, i, j, dep) = H+V;
      // _mesh[i][j].avg = H;
      // _mesh[i][j].sum = V;
      // _mesh[i][j].pde = i*j;
      // _mesh[i][j].dep = H+V;
      V += 1000;
    }
    H += 100;
  }

  *mesh = _mesh;

  return err;
}


// liberate the memory
void free_mesh(MESH mesh, int x_size, int y_size) {

  int i;

  for (i = 0; i < x_size; ++i) {
    free(mesh[i]);
  }
  free(mesh);

}


#else // USE_SOA

// create and fill the mesh with starting values
int init_mesh(struct Mesh *mesh, int x_size, int y_size) {

  int err = FALSE;
  int i, j;

  double H = 100;
  double V = 1000;

  // allocate memory and verify that it worked
  mesh->avg = (double**) malloc(x_size * sizeof(double*));
  mesh->sum = (double**) malloc(x_size * sizeof(double*));
  mesh->pde = (double**) malloc(x_size * sizeof(double*));
  mesh->dep = (double**) malloc(x_size * sizeof(double*));
  if(mesh->avg == NULL) err = TRUE;
  if(mesh->sum == NULL) err = TRUE;
  if(mesh->pde == NULL) err = TRUE;
  if(mesh->dep == NULL) err = TRUE;
  for (i = 0; i < x_size; ++i) {
    mesh->avg[i] = (double*) malloc(y_size * sizeof(double));
    mesh->sum[i] = (double*) malloc(y_size * sizeof(double));
    mesh->pde[i] = (double*) malloc(y_size * sizeof(double));
    mesh->dep[i] = (double*) malloc(y_size * sizeof(double));
    if(mesh->avg[i] == NULL) err = TRUE;
    if(mesh->sum[i] == NULL) err = TRUE;
    if(mesh->pde[i] == NULL) err = TRUE;
    if(mesh->dep[i] == NULL) err = TRUE;
  }

  // define starting values
  for (i = 0; i < x_size; i++) {
    V = 1000;
    for (j = 0; j < y_size; j++) {
      mesh->sum[i][j] = H;
      mesh->avg[i][j] = V;
      mesh->pde[i][j] = i*j;
      mesh->dep[i][j] = H+V;
      V += 1000;
    }
    H += 100;
  }

  return err;
}


// liberate the memory
void free_mesh(struct Mesh mesh, int x_size, int y_size){

  int i;

  for (i = 0; i < x_size; ++i) {
    free(mesh.avg[i]);
    free(mesh.sum[i]);
    free(mesh.pde[i]);
    free(mesh.dep[i]);
  }
  free(mesh.avg);
  free(mesh.sum);
  free(mesh.pde);
  free(mesh.dep);

}


#endif

double pythag(double x1, double y1, double x2, double y2) {
  return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

// perform one iteration of the timestep
void do_timestep(MESH mesh, MESH temp_mesh, int x_size, int y_size, double time, double dt) {

  int thr_id;
  double dt2 = dt*dt;
  double C = 0.25, dx2 = 1.0;

  int _x, _y, n, i, j, t;

#ifdef USE_CALI_UNCORE
CALI_MARK_BEGIN("computation");
#endif
  // looping over all rows of the matrix
  // main source of paralleism 
  #pragma omp parallel private(_y, n, t, thr_id, dx2)
  {

    // establish temporary mesh for this thread
    thr_id = omp_get_thread_num();
    int neighbors[NUM_NEIGHBORS][2];

#ifdef USE_CALI_REG
CALI_MARK_BEGIN("computation");
#endif

#ifdef USE_LIKWID
LIKWID_MARKER_START("computation");
#endif

  #pragma omp for 
  for (_x = 0; _x < x_size; _x++) {

    // fill next temp row with starting values
    for (_y = 0; _y < y_size; _y++) {

      ACCESS_MESH(temp_mesh, _x, _y, avg) = 0;
      ACCESS_MESH(temp_mesh, _x, _y, sum) = 0;
      ACCESS_MESH(temp_mesh, _x, _y, pde) = -2*dt2 * ACCESS_MESH(mesh, _x, _y, pde) * C;
      ACCESS_MESH(temp_mesh, _x, _y, dep) = -2*dt2 * ACCESS_MESH(mesh, _x, _y, dep) * C;

    }

    // actually do some computation
    #pragma omp simd
    for (_y = 0; _y < y_size; _y++) {

      get_neighbors(x_size, y_size, _x, _y, neighbors);
      #pragma unroll
      for(n = 0; n < NUM_NEIGHBORS; n++) {
        ACCESS_MESH(temp_mesh, _x, _y, avg) += ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], avg);
      }
      ACCESS_MESH(temp_mesh, _x, _y, avg) /= NUM_NEIGHBORS;
      #pragma unroll
      for(n = 0; n < NUM_NEIGHBORS; n++) {
        ACCESS_MESH(temp_mesh, _x, _y, sum) += ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], sum)/NUM_NEIGHBORS;
      }
      #pragma unroll
      for(n = 0; n < NUM_NEIGHBORS; n++){
        dx2 = pythag(_x, _y, neighbors[n][X], neighbors[n][Y]); // dx^2
        ACCESS_MESH(temp_mesh, _x, _y, pde) += (-2*dt2 * ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], pde)) / ((dx2 + 1.0) * C);
      }
      #pragma unroll
      for(n = 0; n < NUM_NEIGHBORS; n++){
        dx2 = pythag(_x, _y, neighbors[n][X], neighbors[n][Y]); // dx^2
        ACCESS_MESH(temp_mesh, _x, _y, dep) += (ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], avg)*dt2 * \
                                  ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], dep)) / \
                                  ((dx2 + ACCESS_MESH(mesh, neighbors[n][X], neighbors[n][Y], sum)) * C);
       }
    } // _y loop

  } // _x loop
#ifdef USE_CALI_REG
CALI_MARK_END("computation");
#endif
#ifdef USE_LIKWID
LIKWID_MARKER_STOP("computation");
#endif

  } // parallel region

#ifdef USE_CALI_UNCORE
CALI_MARK_END("computation");
#endif
} // do time step

// print the mesh
void print_mesh(MESH mesh, int x_size, int y_size) {

  int i, j;

  for (i = 0; i < x_size; i++) {
    printf("x = %d\n", i);
    for (j = 0; j < y_size; j++) {
      printf("%10.2e ", ACCESS_MESH(mesh, i, j, avg));
    }
    printf("\n");

    for (j = 0; j < y_size; j++) {
      printf("%10.2e ", ACCESS_MESH(mesh, i, j, sum));
    }
    printf("\n");

    for (j = 0; j < y_size; j++) {
      printf("%10.2e ", ACCESS_MESH(mesh, i, j, pde));
    }
    printf("\n");

    for (j = 0; j < y_size; j++) {
      printf("%10.2e ", ACCESS_MESH(mesh, i, j, dep));
    }
    printf("\n\n");
  }

}

// print the mesh to file
void output_mesh(FILE* file, MESH mesh, int x_size, int y_size) {

  int i, j;

  for (i = 0; i < x_size; i++) {
    fprintf(file, "x = %d\n", i);
    for (j = 0; j < y_size; j++) {
      fprintf(file, "%10.2e ", ACCESS_MESH(mesh, i, j, avg));
    }
    fprintf(file, "\n");

    for (j = 0; j < y_size; j++) {
      fprintf(file, "%10.2e ", ACCESS_MESH(mesh, i, j, sum));
    }
    fprintf(file, "\n");

    for (j = 0; j < y_size; j++) {
      fprintf(file, "%10.2e ", ACCESS_MESH(mesh, i, j, pde));
    }
    fprintf(file, "\n");

    for (j = 0; j < y_size; j++) {
      fprintf(file, "%10.2e ", ACCESS_MESH(mesh, i, j, dep));
    }
    fprintf(file, "\n\n");
  }
}

// runs a small test mesh over one timestep for manual verification
int test_small_mesh() {

  int err = FALSE;

  MESH mesh_1;
  MESH mesh_2;
  int x_size = 5;
  int y_size = 10;
  double time = 0.0;

  printf("init_mesh...\n");
  err = err | init_mesh(&mesh_1, x_size, y_size);
  err = err | init_mesh(&mesh_2, TEMP_ROWS, y_size);
  
#if USE_AOS 
  if(mesh_1 == NULL)     return 1;
  if(mesh_2 == NULL)     return 1;
#else
  if(mesh_1.avg == NULL) return 1;
  if(mesh_2.avg == NULL) return 1;
  if(mesh_1.sum == NULL) return 1;
  if(mesh_2.sum == NULL) return 1;
  if(mesh_1.pde == NULL) return 1;
  if(mesh_2.pde == NULL) return 1;
  if(mesh_1.dep == NULL) return 1;
  if(mesh_2.dep == NULL) return 1;
#endif

  printf("print_mesh...\n");
  print_mesh(mesh_1, x_size, y_size);
  printf("do_timestep...\n");
  do_timestep(mesh_1, mesh_2, x_size, y_size, time, 1.0);
  printf("print_mesh...\n");
  print_mesh(mesh_1, x_size, y_size);
  printf("free_mesh...\n");
  free_mesh(mesh_1, x_size, y_size);
  free_mesh(mesh_2, TEMP_ROWS, y_size);

  return err;
}

// main driver of performance experiments
int run_custom_mesh(int x_size, int y_size, double time, double step, double time_stop) {

  int err = FALSE;

  MESH main_mesh;
  MESH temp_mesh;

  double wall_tot_start, wall_tot_end;
  double wall_init_start, wall_init_end;
  double wall_step_start, wall_step_end;
  double wall_free_start, wall_free_end;

  int num_thr;
  //omp_set_num_threads(4);
  #pragma omp parallel
  {
    if(omp_get_thread_num()==0)
      num_thr = omp_get_num_threads();
  }

  printf("\n\nRunning new Stencil with \n\
    x_size     = %d \n\
    y_size     = %d \n\
    pattern    = %d \n\
    num_thr    = %d \n\
    start time = %f \n\
    time step  = %f \n\
    end time   = %f \n\n",
    x_size, y_size, STENCIL_TYPE, num_thr, time, step, time_stop);

  wall_tot_start = omp_get_wtime();
  wall_init_start = omp_get_wtime();
  printf("init_mesh......."); fflush(stdout);
  err = err | init_mesh(&main_mesh, x_size, y_size);
  err = err | init_mesh(&temp_mesh, x_size, y_size);
  
#if USE_AOS 
  if(main_mesh == NULL)     return 1;
  if(temp_mesh == NULL)     return 1;
#else
  if(main_mesh.avg == NULL) return 1;
  if(temp_mesh.avg == NULL) return 1;
  if(main_mesh.sum == NULL) return 1;
  if(temp_mesh.sum == NULL) return 1;
  if(main_mesh.pde == NULL) return 1;
  if(temp_mesh.pde == NULL) return 1;
  if(main_mesh.dep == NULL) return 1;
  if(temp_mesh.dep == NULL) return 1;
#endif


  wall_init_end = omp_get_wtime();
  printf("%fs\n", (wall_init_end - wall_init_start));

#ifdef DO_IO
  printf("output to file....."); fflush(stdout);
  double io_start = omp_get_wtime();
  FILE* file = fopen(FILE_NAME, "w+");
  fprintf(file, "\n\nRunning new Stencil with \n\
    x_size     = %d \n\
    y_size     = %d \n\
    pattern    = %d \n\
    start time = %f \n\
    time step  = %f \n\
    end time   = %f \n\n",
    x_size, y_size, STENCIL_TYPE, time, step, time_stop);
  output_mesh(file, main_mesh, x_size, y_size);
  printf("%fs\n", (omp_get_wtime() - io_start));
#endif

  while(time < time_stop) {

    printf("timestep %.2f...", time); fflush(stdout);
    wall_step_start = omp_get_wtime();
    do_timestep(main_mesh, temp_mesh, x_size, y_size, time, step);
    time += step;
    wall_step_end = omp_get_wtime();
    printf("%fs\n", (wall_step_end - wall_step_start));

    printf("timestep %.2f...", time); fflush(stdout);
    wall_step_start = omp_get_wtime();
    do_timestep(temp_mesh, main_mesh, x_size, y_size, time, step);
    time += step;
    wall_step_end = omp_get_wtime();
    printf("%fs\n", (wall_step_end - wall_step_start));

  }

#ifdef DO_IO
  io_start = omp_get_wtime();
  printf("output to file....."); fflush(stdout);
  fprintf(file, "\n\n");
  output_mesh(file, main_mesh, x_size, y_size);
  fprintf(file, "\n\n");
  fclose(file);
  printf("%fs\n", (omp_get_wtime() - io_start));
#endif

  printf("free_mesh.......\n"); fflush(stdout);
  wall_free_start = omp_get_wtime();
  free_mesh(main_mesh, x_size, y_size);
  free_mesh(temp_mesh, x_size, y_size);
  wall_free_end = omp_get_wtime();
  printf("%fs\n", (wall_free_end - wall_free_start));

  wall_tot_end = omp_get_wtime();
  printf("\n total time: %fs\n", (wall_tot_end - wall_tot_start));

  return err;
}


// main function
int main(int argc, char **argv) {

#ifdef USE_CALI
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

#ifdef USE_LIKWID
LIKWID_MARKER_INIT;
#endif

  int err = FALSE;

  // err = err | test_small_mesh();
  // printf("\n\n");
  err = err | run_custom_mesh(X_SIZE, Y_SIZE, TIME, STEP, TIME_STOP);

#ifdef USE_LIKWID
LIKWID_MARKER_CLOSE;
#endif

  return err;
}




