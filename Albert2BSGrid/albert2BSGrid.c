#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <alberta.h>

#include "trans2BSMacro.c"

#define MACRO

#if XDR 
#define READ_MESH read_mesh_xdr
#define READ_DOF_REAL_VEC read_dof_real_vec_xdr
#else
#define READ_MESH read_mesh
#define READ_DOF_REAL_VEC read_dof_real_vec
#endif

static const FE_SPACE *fe_space;         /* initialized by init_dof_admin() */

void init_dof_admin(MESH *mesh)
{
    FUNCNAME("init_dof_admin");
    int             degree = 1;                                 
    const BAS_FCTS  *lagrange;                                  
                                                                      
    lagrange = get_lagrange(degree);                            
    TEST_EXIT(lagrange)("no lagrange BAS_FCTS\n");              
    fe_space = get_fe_space(mesh, lagrange->name, nil, lagrange);
return;                                                     
}                                                             

BOUNDARY *init_bound(MESH *mesh,int ibdry)
{
  void *neu = NULL;
  return (BOUNDARY *) neu;
}

struct ellipt_leaf_data                                       
{                                                             
  REAL estimate;            /*  one real for the estimate                   */
};                                                            
                                                              
void init_leaf_data(LEAF_DATA_INFO *leaf_data_info)           
{                                                             
  leaf_data_info->leaf_data_size = sizeof(struct ellipt_leaf_data);
  leaf_data_info->coarsen_leaf_data = nil; /* no transformation             */
  leaf_data_info->refine_leaf_data = nil;  /* no transformation             */
  return;                                                     
}                                                             

const BOUNDARY *initBoundary(MESH *Spmesh, int bound)
{
  BOUNDARY *b = (BOUNDARY *) malloc(sizeof(BOUNDARY));
  assert(b != NULL);

  // bound is of type signed char which goes from -127 to 128 
  assert((bound > -128) && (bound < 129));
  
  b->param_bound = NULL;
  b->bound = bound;
  
  return b;
}


int main(int args, char *argv[]) 
{
  MESH *spidermesh;
  char filename[2024];
  char bsoutfile[2024];
  
  if( args < 3) 
  {
    fprintf(stderr,"usage Alberta2ALUGrid <alberta-in-file> <alu3dgrid-macro-grid> <optional: refinement level>\n");
    exit(EXIT_FAILURE);
  }

  sprintf(filename,"%s",argv[1]);
  sprintf(bsoutfile,"%s",argv[2]);
  
  printf("Filename: %s\n",filename);
  
  spidermesh = get_mesh("spider",init_dof_admin,init_leaf_data);
  read_macro(spidermesh,filename,initBoundary);
  
  if(args == 4) 
  { 
    int level = atoi(argv[3]);
    printf("refine %d levels! \n",level);
    global_refine(spidermesh,level);

    write_macro(spidermesh,"fakefile_not_use");
    write_macro(spidermesh,"fakefile_not_use");
    free_mesh(spidermesh);
    spidermesh = get_mesh("spider",init_dof_admin,init_leaf_data);
    read_macro(spidermesh,"fakefile_not_use",initBoundary);
    remove ("fakefile_not_use");
  }
  
  trans2BSMacroGrid(spidermesh,bsoutfile);

  return (EXIT_SUCCESS);
}

