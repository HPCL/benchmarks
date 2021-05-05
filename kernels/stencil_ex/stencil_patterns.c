/*
 * Simple Stencil example
 * patterns for the stencil define STENCIL_TYPE to choose
 *
 * Brian J Gravelle
 * gravelle@cs.uoregon.edu
 *
 */


#include "mesh.h"

#ifdef USE_CALI
#include <caliper/cali.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#ifndef STENCIL_TYPE
#define STENCIL_TYPE 0
#endif

#if STENCIL_TYPE == 0
// NUM_NEIGHBORS = 9
// one deep box around point

// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n;

  n = 0;
  for (_x = -1; _x <= 1; _x++) {
    for (_y = -1; _y <= 1; _y++) {

      neighbors[n][X] = x + _x;
      neighbors[n][Y] = y + _y;

      if((neighbors[n][X] < 0) || (neighbors[n][X] >= x_size) )
        neighbors[n][X] = x;      
      if((neighbors[n][Y] < 0) || (neighbors[n][Y] >= y_size) )
        neighbors[n][Y] = y;

      n++;
    }
  }

  return n;
}


#elif STENCIL_TYPE == 1
// NUM_NEIGHBORS = 5
// one deep +

// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n=0;

  for (_x = -1; _x <= 1; _x++) {

    neighbors[n][X] = x + _x;
    neighbors[n][Y] = y;
    if((neighbors[n][X] < 0) || (neighbors[n][X] >= x_size) ){
      neighbors[n][X] = x;  
    }
    n++;

    if(n > NUM_NEIGHBORS){
      printf("ERROR getting neighbors :(\n");
    }
  }

  for (_y = -1; _y <= 1; _y++) {

    if(_y != 0) { // don't double up on the center point
      neighbors[n][X] = x;
      neighbors[n][Y] = y + _y;
      if((neighbors[n][Y] < 0) || (neighbors[n][Y] >= y_size) ) {
        neighbors[n][Y] = y;
      }
      n++;
    } 

    if(n > NUM_NEIGHBORS){
      printf("ERROR getting neighbors\n");
    }
  }


  return n;
}

#elif STENCIL_TYPE == 2
// NUM_NEIGHBORS = 9
// two deep +


// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n=0;

  for (_x = -2; _x <= 2; _x++) {

    neighbors[n][X] = x + _x;
    neighbors[n][Y] = y;
      if( neighbors[n][X] < 0 ) {
        neighbors[n][X] = 0;
      }
      if( neighbors[n][X] >= x_size ) {
        neighbors[n][X] = x_size-1;
      }
      if( neighbors[n][X] == 0 ) {
        neighbors[n][Y] = 0;
      }
    n++;

  }

  for (_y = -2; _y <= 2; _y++) {

    if(_y != 0) { // don't double up on the center point
      neighbors[n][X] = x;
      neighbors[n][Y] = y + _y;
      if( neighbors[n][Y] < 0 ) {
        neighbors[n][Y] = 0;
      }
      if( neighbors[n][Y] >= y_size ) {
        neighbors[n][Y] = y_size-1;
      }
      n++;  
    }

  }

  if(n != NUM_NEIGHBORS){
    printf("ERROR getting neighbors\n");
  }

  return n;
}



#elif STENCIL_TYPE == 3
// NUM_NEIGHBORS = 17
// four deep +

// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n=0;

  for (_x = -4; _x <= 4; _x++) {

    neighbors[n][X] = x + _x;
    neighbors[n][Y] = y;
      if( neighbors[n][X] < 0 ) {
        neighbors[n][X] = 0;
      }
      if( neighbors[n][X] >= x_size ) {
        neighbors[n][X] = x_size-1;
      }
    n++;

  }

  for (_y = -4; _y <= 4; _y++) {

    if(_y != 0) { // don't double up on the center point
      neighbors[n][X] = x;
      neighbors[n][Y] = y + _y;
      if( neighbors[n][Y] < 0 ) {
        neighbors[n][Y] = 0;
      }
      if( neighbors[n][Y] >= y_size ) {
        neighbors[n][Y] = y_size-1;
      }
      n++;
    }

  }

  if(n != NUM_NEIGHBORS){
    printf("ERROR getting neighbors\n");
  }

  return n;
}



#elif STENCIL_TYPE == 4
// NUM_NEIGHBORS = 81 
// four deep halo

// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n;

  n = 0;
  for (_x = -4; _x <= 4; _x++) {
    for (_y = -4; _y <= 4; _y++) {

      neighbors[n][X] = x + _x;
      neighbors[n][Y] = y + _y;

      if((neighbors[n][X] < 0) || (neighbors[n][X] >= x_size) )
        neighbors[n][X] = x;
      if((neighbors[n][Y] < 0) || (neighbors[n][Y] >= y_size) )
        neighbors[n][Y] = y;

      n++;
    }
  }

  if(n != NUM_NEIGHBORS){
    printf("ERROR getting neighbors\n");
  }

  return n;
}




#elif STENCIL_TYPE == 5
// NUM_NEIGHBORS = 25
// two deep halo

// for point (x,y) in mesh get the list of neighbors 
// returns the number of neighbors
int get_neighbors(int x_size, int y_size, int x, int y, int neighbors[NUM_NEIGHBORS][2]) {

  int _x, _y, n;

  n = 0;
  for (_x = -2; _x <= 2; _x++) {
    for (_y = -2; _y <= 2; _y++) {

      neighbors[n][X] = x + _x;
      neighbors[n][Y] = y + _y;

      if((neighbors[n][X] < 0) || (neighbors[n][X] >= x_size) )
        neighbors[n][X] = x;
      if((neighbors[n][Y] < 0) || (neighbors[n][Y] >= y_size) )
        neighbors[n][Y] = y;

      n++;
    }
  }

  if(n != NUM_NEIGHBORS){
    printf("ERROR getting neighbors\n");
  }

  return n;
}


#endif

