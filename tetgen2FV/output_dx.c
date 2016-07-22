#include <stdio.h> 
#include <string.h> 
#include <sys/types.h>
#include <math.h>
#include "strfunc.h"
#include "tetgen2FV.h"
#include "voronoi.h"
#include "output_dx.h"

#ifdef DEBUG
#define no_printf printf
#endif


int output_dx_full(struct tet_mesh *tet_mesh, char *outfile_name)
{

  FILE *outfile;
  struct voronoi_facet *vfp;
  struct node **node_array;
  struct node *np;
  struct node_list *node_head;
  struct edge_list *elp;
  struct tet_list *tlp,*tet_head;
  struct polygon_list *plp;
  struct polygon *pop;
  struct vector3 *vp;
  u_int node_count;
  u_int tet_count;
  u_int edge_count;
  u_int boundaryface_count;
  u_int voronoi_node_count;
  u_int voronoi_edge_count;
  u_int voronoi_boundary_edge_count;
  u_int voronoi_triangle_count;
  int data_offset;
  float nx,ny,nz;
  float fi;
  u_int n1,n2,n3;
  u_int i,j,k;
  char obj_name[64];

  fprintf(stderr,"tetgen2FV: output DX mesh:\n");

  node_count=tet_mesh->nnodes;
  node_head=tet_mesh->node_head;
  node_array=tet_mesh->nodes;
  tet_count=tet_mesh->ntets;
  voronoi_node_count=tet_mesh->ntets+tet_mesh->nboundaryfaces+tet_mesh->nboundaryedges+tet_mesh->nboundarynodes;
  boundaryface_count=tet_mesh->nboundaryfaces;
  voronoi_boundary_edge_count=tet_mesh->voronoi_boundary_nedges_tot;
  tet_head=tet_mesh->tet_head;
 
  edge_count=tet_mesh->nedges;
  /* multiply the edge and triangle counts by 2 because 
     we'll duplicate the shared voronoi facets as we output them */
  voronoi_edge_count=(2*tet_mesh->voronoi_facet_nedges_tot)+tet_mesh->voronoi_cap_nedges_tot;
  voronoi_triangle_count=(2*tet_mesh->voronoi_facet_ntriangles_tot)+tet_mesh->voronoi_cap_ntriangles_tot;

  if ((outfile=fopen(outfile_name,"w"))==NULL)
  {
    fprintf(stderr,"Cannot open DX outfile %s",outfile_name);
    return(1);
  }

  data_offset=0;
  
  sprintf(obj_name,"tet mesh nodes");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,node_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=node_count*3*4;

  sprintf(obj_name,"tet mesh tets");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 4 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"tetrahedra\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*4*4;

  sprintf(obj_name,"tet indices");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"tet material");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"multi-material node flag");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,node_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=node_count*1*4;

  sprintf(obj_name,"tet mesh circumcenters, voronoi mesh nodes");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*3*4;

  sprintf(obj_name,"circumcenter indices");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"tet/circumcenter delaunay status");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"tet mesh connections by tet mesh node");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 4 items %d lsb ieee data %d\n",obj_name,4*tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"tetrahedra\"\n");
  fprintf(outfile,"#\n");
  data_offset+=4*tet_count*4*4;

  sprintf(obj_name,"tet mesh tet to tet mesh node mapping");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,4*tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=4*tet_count*1*4;

  sprintf(obj_name,"boundary face circumcenters");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,boundaryface_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=boundaryface_count*3*4;

  sprintf(obj_name,"voronoi mesh nodes");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,voronoi_node_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_node_count*3*4;

  sprintf(obj_name,"voronoi mesh facets");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,voronoi_triangle_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"triangles\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_triangle_count*3*4;

  sprintf(obj_name,"voronoi mesh triangle to tet mesh node mapping");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,voronoi_triangle_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_triangle_count*1*4;

  sprintf(obj_name,"voronoi mesh triangle facet indices");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,voronoi_triangle_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_triangle_count*1*4;

  sprintf(obj_name,"voronoi mesh edges");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 2 items %d lsb ieee data %d\n",obj_name,voronoi_edge_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"lines\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_edge_count*2*4;

  sprintf(obj_name,"voronoi mesh edge to tet mesh node mapping");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,voronoi_edge_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_edge_count*1*4;

  sprintf(obj_name,"voronoi mesh edge facet indices");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,voronoi_edge_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_edge_count*1*4;

  sprintf(obj_name,"voronoi mesh boundary edges");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 2 items %d lsb ieee data %d\n",obj_name,voronoi_boundary_edge_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"lines\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_boundary_edge_count*2*4;

  sprintf(obj_name,"voronoi mesh boundary edge to tet mesh node mapping");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,voronoi_boundary_edge_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=voronoi_boundary_edge_count*1*4;

  sprintf(obj_name,"tet mesh tets field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "tet mesh nodes");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "tet mesh tets");
  fprintf(outfile,"component \"tet index\" value \"%s\"\n",
    "tet indices");
  fprintf(outfile,"component \"tet material\" value \"%s\"\n",
    "tet material");
  fprintf(outfile,"component \"multi-material node flag\" value \"%s\"\n",
    "multi-material node flag");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"tet mesh circumcenters, voronoi mesh nodes field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "tet mesh circumcenters, voronoi mesh nodes");
  fprintf(outfile,"component \"tet index\" value \"%s\"\n",
    "circumcenter indices");
  fprintf(outfile,"component \"delaunay status\" value \"%s\"\n",
    "tet/circumcenter delaunay status");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"tet mesh tets by voronoi volume element field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "tet mesh nodes");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "tet mesh connections by tet mesh node");
  fprintf(outfile,"component \"node index\" value \"%s\"\n",
    "tet mesh tet to tet mesh node mapping");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"boundary face circumcenters field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "boundary face circumcenters");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"voronoi mesh facets field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "voronoi mesh nodes");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "voronoi mesh facets");
  fprintf(outfile,"component \"node index\" value \"%s\"\n",
    "voronoi mesh triangle to tet mesh node mapping");
  fprintf(outfile,"component \"facet index\" value \"%s\"\n",
    "voronoi mesh triangle facet indices");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"voronoi mesh edges field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "voronoi mesh nodes");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "voronoi mesh edges");
  fprintf(outfile,"component \"node index\" value \"%s\"\n",
    "voronoi mesh edge to tet mesh node mapping");
  fprintf(outfile,"component \"facet index\" value \"%s\"\n",
    "voronoi mesh edge facet indices");
  fprintf(outfile,"#\n");

  sprintf(obj_name,"voronoi mesh boundary edges field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "boundary face circumcenters");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "voronoi mesh boundary edges");
  fprintf(outfile,"component \"node index\" value \"%s\"\n",
    "voronoi mesh boundary edge to tet mesh node mapping");
  fprintf(outfile,"#\n");

  fprintf(outfile,"object \"default\" class group\n");
  fprintf(outfile,"member \"delaunay_tets\" value \"%s\"\n",
    "tet mesh tets field");
  fprintf(outfile,"member \"tet_circumcenters\" value \"%s\"\n",
    "tet mesh circumcenters, voronoi mesh nodes field");
  fprintf(outfile,"member \"delaunay_tets_by_voronoi_volume\" value \"%s\"\n",
    "tet mesh tets by voronoi volume element field");
  fprintf(outfile,"member \"boundary_circumcenters\" value \"%s\"\n",
    "boundary face circumcenters field");
  fprintf(outfile,"member \"voronoi_facets\" value \"%s\"\n",
    "voronoi mesh facets field");
  fprintf(outfile,"member \"voronoi_edges\" value \"%s\"\n",
    "voronoi mesh edges field");
  fprintf(outfile,"member \"voronoi_boundary_edges\" value \"%s\"\n",
    "voronoi mesh boundary edges field");
  fprintf(outfile,"#\n");
  fprintf(outfile,"end\n");


  /* output tet mesh nodes */
  for (i=0; i<node_count; i++)
  {
    np=node_array[i];
    nx=np->x;
    ny=np->y;
    nz=np->z;
    fwrite(&nx,sizeof(float),1,outfile);
    fwrite(&ny,sizeof(float),1,outfile);
    fwrite(&nz,sizeof(float),1,outfile);
  }


  /* output tet mesh tet connections */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fwrite(&tlp->tet->node_index[0],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[1],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[2],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[3],sizeof(u_int),1,outfile);
    tlp=tlp->next;
  }


  /* output tet indices */
  fi=0;
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fwrite(&fi,sizeof(float),1,outfile);
    fi++;
    tlp=tlp->next;
  }


  /* output tet material */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fi=tlp->tet->matnum;
    fwrite(&fi,sizeof(float),1,outfile);
    tlp=tlp->next;
  }


  /* output multi-material node flags */
  for (i=0; i<node_count; i++)
  {
    fi=node_array[i]->material_count;
    fwrite(&fi,sizeof(float),1,outfile);
  }


  /* output tet mesh tet circumcenters, voronoi mesh nodes */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    nx=tlp->tet->cent.x;
    ny=tlp->tet->cent.y;
    nz=tlp->tet->cent.z;
    fwrite(&nx,sizeof(float),1,outfile);
    fwrite(&ny,sizeof(float),1,outfile);
    fwrite(&nz,sizeof(float),1,outfile);
    tlp=tlp->next;
  }


  /* output tet circumcenter indices */
  fi=0;
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fwrite(&fi,sizeof(float),1,outfile);
    fi++;
    tlp=tlp->next;
  }


  /* output tet/circumcenter delaunay status */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fi=tlp->tet->delaunay;
    fwrite(&fi,sizeof(float),1,outfile);
    tlp=tlp->next;
  }


  /* output tet mesh connections by tet mesh node */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    for (i=0; i<4; i++)
    {
      fwrite(&tlp->tet->node_index[0],sizeof(u_int),1,outfile);
      fwrite(&tlp->tet->node_index[1],sizeof(u_int),1,outfile);
      fwrite(&tlp->tet->node_index[2],sizeof(u_int),1,outfile);
      fwrite(&tlp->tet->node_index[3],sizeof(u_int),1,outfile);
    }
    tlp=tlp->next;
  }


  /* output tet mesh tet to tet mesh node mapping */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    for (i=0; i<4; i++)
    {
      fi=tlp->tet->node_index[i];
      fwrite(&fi,sizeof(float),1,outfile);
    }
    tlp=tlp->next;
  }


  /* output boundary face circumcenters */
  for (plp=tet_mesh->boundary_head; plp!=NULL; plp=plp->next)
  {
    pop=plp->polygon;
    nx=pop->cent.x;
    ny=pop->cent.y;
    nz=pop->cent.z;
    fwrite(&nx,sizeof(float),1,outfile);
    fwrite(&ny,sizeof(float),1,outfile);
    fwrite(&nz,sizeof(float),1,outfile);
  }
  

  /* output voronoi mesh nodes */
  for (i=0; i<voronoi_node_count; i++)
  {
    vp=tet_mesh->voronoi_nodes[i];
    nx=vp->x;
    ny=vp->y;
    nz=vp->z;
    fwrite(&nx,sizeof(float),1,outfile);
    fwrite(&ny,sizeof(float),1,outfile);
    fwrite(&nz,sizeof(float),1,outfile);
  }


  /* output voronoi mesh facets as triangles */
  for (i=0;i<node_count;i++)
  {
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes-2; j++)
        {
          n1=vfp->node_index[0];
          n2=vfp->node_index[j+1];
          n3=vfp->node_index[j+2];
          fwrite(&n1,sizeof(u_int),1,outfile);
          fwrite(&n2,sizeof(u_int),1,outfile);
          fwrite(&n3,sizeof(u_int),1,outfile);
        }
      }
    }
  }


  /* output voronoi mesh triangle to tet mesh node mapping */
  for (i=0;i<node_count;i++)
  {
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes-2; j++)
        {
          fi=i;
          fwrite(&fi,sizeof(float),1,outfile);
        }
      }
    }
  }


  /* output voronoi mesh triangle facet indices */
  for (i=0;i<node_count;i++)
  {
    k=0;
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes-2; j++)
        {
          fi=k;
          fwrite(&fi,sizeof(float),1,outfile);
        }
      }
      k++;
    }
  }


  /* output voronoi mesh edges as line segments */
  for (i=0;i<node_count;i++)
  {
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes; j++)
        {
          if (j<vfp->nnodes-1)
          {
            n1=vfp->node_index[j];
            n2=vfp->node_index[j+1];
          }
          else
          {
            n1=vfp->node_index[j];
            n2=vfp->node_index[0];
          }
          fwrite(&n1,sizeof(u_int),1,outfile);
          fwrite(&n2,sizeof(u_int),1,outfile);
        }
      }
    }
  }


  /* output voronoi mesh edge to tet mesh node mapping */
  for (i=0;i<node_count;i++)
  {
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes; j++)
        {
          fi=i;
          fwrite(&fi,sizeof(float),1,outfile);
        }
      }
    }
  }


  /* output voronoi mesh edge facet indices */
  for (i=0;i<node_count;i++)
  {
    k=0;
    for (elp=node_array[i]->edge_head; elp!=NULL; elp=elp->next)
    {
      for (vfp=elp->edge->facet; vfp!=NULL; vfp=vfp->next)
      {
        for (j=0; j<vfp->nnodes; j++)
        {
          fi=k;
          fwrite(&fi,sizeof(float),1,outfile);
        }
      }
      k++;
    }
  }


  /* output voronoi mesh boundary edges */
  for (i=0; i<node_count; i++)
  {
    np=node_array[i];
    if (np->node_type==BOUNDARY || np->node_type==INTERFACE)
    {
      for (plp=np->boundary_head; plp!=NULL; plp=plp->next)
      {
        if (plp->next!=NULL)
        {
          n1=plp->polygon->polygon_index;
          n2=plp->next->polygon->polygon_index;
        }
        else {
          n1=plp->polygon->polygon_index;
          n2=np->boundary_head->polygon->polygon_index;
        }
        fwrite(&n1,sizeof(u_int),1,outfile);
        fwrite(&n2,sizeof(u_int),1,outfile);
      }
    } 
  }


  /* output voronoi mesh boundary edge to tet mesh node mapping */
  for (i=0; i<node_count; i++)
  {
    np=node_array[i];
    if (np->node_type==BOUNDARY || np->node_type==INTERFACE)
    {
      for (j=0; j<np->nboundaryfaces; j++)
      {
        fi=i;
        fwrite(&fi,sizeof(float),1,outfile);
      }
    } 
  }


  fclose(outfile);

  fprintf(stderr,"tetgen2FV: finished output DX mesh.\n");

  return(0);

}



int output_dx_tet_mesh(struct tet_mesh *tet_mesh, char *outfile_name)
{

  FILE *outfile;
  struct voronoi_facet *vfp;
  struct node **node_array;
  struct node *np;
  struct node_list *node_head;
  struct edge_list *elp;
  struct tet_list *tlp,*tet_head;
  struct polygon_list *plp;
  struct polygon *pop;
  struct vector3 *vp;
  u_int node_count;
  u_int tet_count;
  u_int edge_count;
  u_int boundaryface_count;
  u_int voronoi_node_count;
  u_int voronoi_edge_count;
  u_int voronoi_boundary_edge_count;
  u_int voronoi_triangle_count;
  int data_offset;
  float nx,ny,nz;
  float fi;
  u_int n1,n2,n3;
  u_int i,j,k;
  char obj_name[64];

  fprintf(stderr,"tetgen2FV: output DX mesh:\n");

  node_count=tet_mesh->nnodes;
  node_head=tet_mesh->node_head;
  node_array=tet_mesh->nodes;
  tet_count=tet_mesh->ntets;
  voronoi_node_count=tet_mesh->ntets+tet_mesh->nboundaryfaces+tet_mesh->nboundaryedges+tet_mesh->nboundarynodes;
  boundaryface_count=tet_mesh->nboundaryfaces;
  voronoi_boundary_edge_count=tet_mesh->voronoi_boundary_nedges_tot;
  tet_head=tet_mesh->tet_head;
 
  edge_count=tet_mesh->nedges;
  /* multiply the edge and triangle counts by 2 because 
     we'll duplicate the shared voronoi facets as we output them */
  voronoi_edge_count=(2*tet_mesh->voronoi_facet_nedges_tot)+tet_mesh->voronoi_cap_nedges_tot;
  voronoi_triangle_count=(2*tet_mesh->voronoi_facet_ntriangles_tot)+tet_mesh->voronoi_cap_ntriangles_tot;

  if ((outfile=fopen(outfile_name,"w"))==NULL)
  {
    fprintf(stderr,"Cannot open DX outfile %s",outfile_name);
    return(1);
  }

  data_offset=0;
  
  sprintf(obj_name,"tet mesh nodes");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 1 shape 3 items %d lsb ieee data %d\n",obj_name,node_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"positions\"\n");
  fprintf(outfile,"#\n");
  data_offset+=node_count*3*4;

  sprintf(obj_name,"tet mesh tets");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type int rank 1 shape 4 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"attribute \"ref\" string \"positions\"\n");
  fprintf(outfile,"attribute \"element type\" string \"tetrahedra\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*4*4;

  sprintf(obj_name,"tet indices");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"tet material");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class array type float rank 0 items %d lsb ieee data %d\n",obj_name,tet_count,data_offset);
  fprintf(outfile,"attribute \"dep\" string \"connections\"\n");
  fprintf(outfile,"#\n");
  data_offset+=tet_count*1*4;

  sprintf(obj_name,"tet mesh tets field");
  fprintf(outfile,"# %s\n",obj_name);
  fprintf(outfile,"object \"%s\" class field\n",obj_name);
  fprintf(outfile,"component \"positions\" value \"%s\"\n",
    "tet mesh nodes");
  fprintf(outfile,"component \"connections\" value \"%s\"\n",
    "tet mesh tets");
  fprintf(outfile,"component \"tet index\" value \"%s\"\n",
    "tet indices");
  fprintf(outfile,"component \"tet material\" value \"%s\"\n",
    "tet material");
  fprintf(outfile,"#\n");

  fprintf(outfile,"object \"default\" class group\n");
  fprintf(outfile,"member \"delaunay_tets\" value \"%s\"\n",
    "tet mesh tets field");
  fprintf(outfile,"#\n");
  fprintf(outfile,"end\n");


  /* output tet mesh nodes */
  for (i=0; i<node_count; i++)
  {
    np=node_array[i];
    nx=np->x;
    ny=np->y;
    nz=np->z;
    fwrite(&nx,sizeof(float),1,outfile);
    fwrite(&ny,sizeof(float),1,outfile);
    fwrite(&nz,sizeof(float),1,outfile);
  }


  /* output tet mesh tet connections */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fwrite(&tlp->tet->node_index[0],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[1],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[2],sizeof(u_int),1,outfile);
    fwrite(&tlp->tet->node_index[3],sizeof(u_int),1,outfile);
    tlp=tlp->next;
  }


  /* output tet indices */
  fi=0;
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fwrite(&fi,sizeof(float),1,outfile);
    fi++;
    tlp=tlp->next;
  }


  /* output tet material */
  tlp=tet_head;
  while (tlp!=NULL)
  {
    fi=tlp->tet->matnum;
    fwrite(&fi,sizeof(float),1,outfile);
    tlp=tlp->next;
  }



  fclose(outfile);

  fprintf(stderr,"tetgen2FV: finished output DX mesh.\n");

  return(0);

}

