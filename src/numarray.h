

#ifndef __Array_HEADER__
#define __Array_HEADER__

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

enum ArrayOperation {
  ARRAY_OP_ADD,
  ARRAY_OP_SUB,
  ARRAY_OP_MUL,
  ARRAY_OP_DIV,
  ARRAY_OP_POW,
};

struct Array
{
  void *data;
  enum ArrayType dtype;
  int size, ndims, owns;
  int *shape, *lower;
} ;


char         *array_typename(enum ArrayType T);
struct Array  array_new_zeros(int N, enum ArrayType T);
struct Array  array_new_copy(const struct Array *B, enum ArrayType T);
void          array_del(struct Array *A);
void          array_assign_from_scalar(struct Array *A, const void *val);
void          array_assign_from_array(struct Array *A, const struct Array *B);
void          array_binary_op(const struct Array *A,
			      const struct Array *B,
			      struct Array *C, enum ArrayOperation op);
int           array_sizeof(enum ArrayType T);
int           array_resize(struct Array *A, const int *N, const int *L, int Nd);

#endif // __Array_HEADER__
