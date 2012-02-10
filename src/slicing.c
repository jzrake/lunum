

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>

typedef unsigned char Bool;
typedef double complex Complex;


enum ArrayType {
  ARRAY_TYPE_BOOL,
  ARRAY_TYPE_CHAR,
  ARRAY_TYPE_SHORT,
  ARRAY_TYPE_INT,
  ARRAY_TYPE_LONG,
  ARRAY_TYPE_FLOAT,
  ARRAY_TYPE_DOUBLE,
  ARRAY_TYPE_COMPLEX,
};

struct Buffer {
  int size, owner;
  void *data;
  enum ArrayType dtype;
} ;

int buffer_sizeof(enum ArrayType T)
{
  switch (T) {
  case ARRAY_TYPE_BOOL    : return sizeof(Bool);
  case ARRAY_TYPE_CHAR    : return sizeof(char);
  case ARRAY_TYPE_SHORT   : return sizeof(short);
  case ARRAY_TYPE_INT     : return sizeof(int);
  case ARRAY_TYPE_LONG    : return sizeof(long);
  case ARRAY_TYPE_FLOAT   : return sizeof(float);
  case ARRAY_TYPE_DOUBLE  : return sizeof(double);
  case ARRAY_TYPE_COMPLEX : return sizeof(Complex);
  }
}

struct Buffer buffer_new(int size, enum ArrayType T, void *data)
{
  struct Buffer buf;

  if (data == NULL) {
    buf.data = malloc(size*buffer_sizeof(T));
    buf.owner = 1;
  }
  else {
    buf.data = data;
    buf.owner = 0;
  }

  buf.size = size;
  buf.dtype = T;

  return buf;
}

void buffer_free(struct Buffer *buf)
{
  if (buf->owner) free(buf->data);
  buf->data = NULL;
  buf->dtype = -1;
  buf->size = 0;
  buf->owner = 0;
}

void buffer_extract_slice(struct Buffer *B0, const struct Buffer *B1,
			  int *start, int *size, int *stride, int Nd)
// -----------------------------------------------------------------------------
// Extract the slice from B1, and insert it contiguously into B0
// -----------------------------------------------------------------------------
// @start  : starting indices into B1
// @size   : number of entries to extract along each axis
// @stride : distance between entries of B1 along each axis
// @Nd     : the number of axes in each buffer
// -----------------------------------------------------------------------------
{

  char *b0 = (char*) B0->data;
  char *b1 = (char*) B1->data;

  int *N = size;
  int *S = stride;
  int *J = (int*) malloc(Nd*sizeof(int));
  for (int d=0; d<Nd; ++d) J[d] = 0;

  int sizeof_T = buffer_sizeof(B0->dtype);
  int m = 0; // indexes into B0, advanced uniformly

  while (J[0] < N[0]) {

    int M = 0;
    for (int d=0; d<Nd; ++d) M += (J[d] + start[d]) * S[d];

    // ----- use the index x -----
    printf("%d %d %d %d: %d\n", J[0], J[1], J[2], J[3], M);
    memcpy(b0 + (m++)*sizeof_T, b1 + M*sizeof_T, sizeof_T);
    // -----                 -----

    ++J[Nd-1];
    for (int d=Nd-1; d!=0; --d) {
      if (J[d] == N[d]) {
	J[d] = 0;
	++J[d-1];
      }
    }
  }

  free(J);
}

void buffer_insert_slice(struct Buffer *b0, const struct Buffer *b1,
			 int *start, int *size, int *stride, int Nd)
// -----------------------------------------------------------------------------
// Extract contiguously from b1, and insert as a slice into b0
// -----------------------------------------------------------------------------
{

}


int main()
{
  double *mydata = (double*) malloc(144*sizeof(double));

  struct Buffer A = buffer_new(144, ARRAY_TYPE_DOUBLE, NULL);
  struct Buffer B = buffer_new(144, ARRAY_TYPE_DOUBLE, mydata);
  
  int start [4] = {0,0,0,0};
  int size  [4] = {4,3,4,3};
  int stride[4] = {36,12,3,1};

  buffer_extract_slice(&A, &B, start, size, stride, 4);

  buffer_free(&A);
  buffer_free(&B);

  free(mydata);

  return 0;
}
