#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "parse.h"
#include "imod2reconstruct.h"

extern FILE *yyin;
char *infile;
char *recon_ser_prefix;
char *curr_file;
int line_num;

/* Begin main */
int main(argc,argv)
  int argc; 
  char *argv[];  
{
  char *argstr;

        if (argc<3) {
      	  fprintf(stderr,"\nUsage: %s [-h] imod_file_name reconstruct_series_prefix_name  \n\n",argv[0]);
          fprintf(stderr,"  Read an ascii format IMOD file and convert to RECONSTRUCT series format.\n");
          fprintf(stderr,"  Output is written to a sequence of files containing the Reconstruct Series\n");
          fprintf(stderr,"  and Traces where the files are named using the prefix.\n\n");
          fprintf(stderr,"  Example:\n");
          fprintf(stderr,"    %s my_imod_model.amod my_series_dir/my_series\n\n",argv[0]);
          fprintf(stderr,"    will generate files in my_series_dir named:\n");
          fprintf(stderr,"    my_series.ser\n");
          fprintf(stderr,"    my_series.1\n");
          fprintf(stderr,"    my_series.2\n");
          fprintf(stderr,"    my_series.3\n");
          fprintf(stderr,"    ...\n\n");
          fflush(stdout);
          exit(1);
        }
        if (argc>=3) {
          argstr=argv[1];
          if (strcmp(argstr,"-h")==0) {
      	    fprintf(stderr,"\nUsage: %s [-h] imod_file_name reconstruct_series_prefix_name  \n\n",argv[0]);
            fprintf(stderr,"  Read ascii IMOD input file and convert to RECONSTRUCT series format.\n");
            fprintf(stderr,"  Output is written to Reconstruct Series named using prefix.\n\n");
            fflush(stdout);
            exit(1);
          }
        }
        if (argc==3) {
	  infile=argv[1];
          if ((yyin=fopen(infile,"r"))==NULL) {
            fprintf(stderr,"imod2reconstruct: error opening file: %s\n",infile);
            fflush(stdout);
            exit(1);
          }
          curr_file=infile;
	  recon_ser_prefix=argv[2];
        }

	if (yyparse()) {
	  fprintf(stderr,"imod2reconstruct: error parsing file: %s\n",infile);
	  exit(1);
	} 
	fclose(yyin);

	exit(0);
}
