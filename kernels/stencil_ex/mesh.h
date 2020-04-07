/*
 * Simple Stencil example
 *
 *
 * Brian J Gravelle
 * gravelle@cs.uoregon.edu
 *
 */

#ifndef __MESH_H__
#define __MESH_H__


#define TRUE  1
#define FALSE 0

#define X 0
#define Y 1


#if STENCIL_TYPE == 0
#define NUM_NEIGHBORS 9
#define TEMP_ROWS     2

#elif STENCIL_TYPE == 1
#define NUM_NEIGHBORS 5
#define TEMP_ROWS     2

#elif STENCIL_TYPE == 2
#define NUM_NEIGHBORS 9
#define TEMP_ROWS     3


#endif

#if USE_AOS
#define MESH struct Mesh**
#define ACCESS_MESH(mesh, x, y, var) mesh[x][y].var

// point for each mesh location
// contains three "physical values"
//   - desciptions explain behavior on each time step
//   - 3 doubles are 192 bits which don't fit nicely into 
//     cache lines or vector registers
//   - note that 4 is 256
struct Mesh
{

  double avg;   // update to the average to itself and the neighbors
  double sum;   // summation of a fraction of itself and neighbors
  double pde;   // roughly based on partial differential equation stencils
  double dep;   // dependant on other values
  
};

// create and fill the mesh with starting values
int init_mesh(struct Mesh ***mesh, int x_size, int y_size);
void free_mesh(struct Mesh ** mesh, int x_size, int y_size);


#else // USE_SOA
#define MESH struct Mesh
#define ACCESS_MESH(mesh, x, y, var) mesh.var[x][y]

// point for each mesh location
// contains three "physical values"
//   - desciptions explain behavior on each time step
//   - 3 doubles are 192 bits which don't fit nicely into 
//     cache lines or vector registers
//   - note that 4 is 256
struct Mesh
{

  double** avg;   // update to the average to itself and the neighbors
  double** sum;   // summation of a fraction of itself and neighbors
  double** pde;   // roughly based on partial differential equation stencils
  double** dep;   // dependant on other values
  
};

// create and fill the mesh with starting values
int init_mesh(struct Mesh *mesh, int x_size, int y_size);
void free_mesh(struct Mesh mesh, int x_size, int y_size);

#endif


// for point (x,y) in mesh get the list of neighbors 
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]);

// perform one iteration of the timestep
void do_timestep(MESH mesh, MESH new_mesh, int x_size, int y_size, double time, double dt);

// print a grid of the mesh
void print_mesh(MESH mesh, int x_size, int y_size);


#endif


