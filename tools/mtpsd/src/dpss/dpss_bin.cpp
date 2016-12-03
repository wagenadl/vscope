/*

  Copyright (c) 2010 C. Antonio Sanchez

  This file is part of DPSS.

  DPSS is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  DPSS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DPSS.  If not, see <http://www.gnu.org/licenses/>.

*/

/* Adapted by Daniel Wagenaar from dpss_cmd.cpp for use by vscope.
   (C) 2011 Daniel Wagenaar.
*/

#include "dpss.h"

#include <cmath>
#include <cstring>
#include <cstdio>
#include <sstream>

//usage:  dpss N nW [[lower_seq] upper_seq] [interp_method] [N_interp_base] [trace]
int main(int argc, char* argv[]);

template <class T> bool str_to(char *arg, T& dest);         //converts char array arguments to class T
void print_dpss(dpss *mydpss, FILE *ofd);                               //prints the results of dpss to stdout
void print_dpss(const double *h, uint_t n, int nseq, FILE *ofd);  //prints the results of dpss to stdout
dpss_workspace cmd_parse_args(char *inargs[], int nargin);  //parses command line arguments, creating workspace

////////////////////////////////////////////////////////////////////////////
//  CODE
////////////////////////////////////////////////////////////////////////////

static const char helpstr[] = { "\
Usage: mkdpss [ofn] n nW K\n\
\n\
`mkdpss' calculates and prints a set of discrete prolate spheroidal sequences\n\
(aka Slepian Sequences).  These sequences are typically used in \n\
multitaper spectral analysis.\n\
\n\
Example:\n\
dpss 100 3.33333333 2 _tapers/12-3.33.taper calculates and stores the first\n\
                two DPSSs of length 12 and time-bandwidth product nW=3.33.\n\
\n\
See the `mtpsd' Documentation for further details.\n\n"
};

//case insensitive comparison
int mystrnicmp (const std::string &s1, const std::string &s2, const int n)
{   
  for (int ii=0; ii<n; ii++){
    if (toupper(s1[ii])>toupper(s2[ii]) )
      return 1;
    else if ( toupper(s1[ii])<toupper(s2[ii]) )
      return -1;
  }
  return 0;
}

//converts string to type T using streams
template <class T>
bool str_to(char *arg, T& dest){
  std::istringstream sstr(arg);
  return !(sstr >> dest).fail();
}

//creates a string consisting of a repeated character (for display purposes)
char *rep_string(char *out, char rep, int n){
    
  if (n<0) n=0;       
  for (int ii=0; ii<n; ii++)
    out[ii]=rep;
  out[n]='\0';
  return out;
    
}

//prints the discrete prolate spheroidal sequences
void print_dpss(dpss *mydpss, FILE *ofd) {
  print_dpss(mydpss->ph(), mydpss->size(0), mydpss->size(1), ofd);
    
}

void print_dpss(const double *h, uint_t n, int nseq, FILE *ofd) {
  for (int jj=0; jj<nseq; jj++) {
    for (unsigned int ii=0; ii<n; ii++) {
      double x = h[jj*n+ii];
      fwrite(&x, sizeof(double), 1, ofd);
    }
  }
}

//parses the input arguments to create a workspace
//NOTE: little checking on valid parameters is done here
//      since the workspace will be fixed when a dpss class
//      is created.
dpss_workspace cmd_parse_args(char *inargs[], int nargin){
    
  dpss_workspace args;
    
  if (nargin!=3) throw ERR("Usage: mkdpss [ofn] n nW K");

  uint_t n, tmp;
  double nW;
    
  if (!str_to<uint_t>(inargs[0],n))
    throw ERR("Invalid dpss length");
  args.n=n;
  if ( n > MAX_N ){
    args.interp_method=SPLINE;
    args.interp_base=MAX_N;
  } else {                 
    args.interp_method=NONE;
    args.interp_base=n;
  }

  if (!str_to<double>(inargs[1],nW))
    throw ERR("Invalid nW");
  args.nW=nW;

  args.seql=0;

  if (!str_to<uint_t>(inargs[2],tmp))
    throw ERR("Invalid K");
  args.sequ=max<uint_t>(tmp,1)-1;            //adjust so that numbers start at zero
    
  return args;
    
}

//main routine
int main(int argc, char* argv[]){
    
  dpss_workspace dpss_args;
    
  if (argc>1) {
    if ((mystrnicmp(argv[1],"--help", 6)==0) || (mystrnicmp(argv[1],"-h",2)==0)) {
      printf("%s",helpstr);
      return 0;
    }
  }

  FILE *ofd = stdout;
  if (argc>=5) {
    ofd = fopen(argv[1], "wb");
    argc--;
    argv++;
  }
  if (!ofd) {
    perror("dpss_bin");
    return 1;
  }
  
  //parse arguments
  try {
    dpss_args=cmd_parse_args(argv+1, argc-1);
  } catch (ERR err) {
    fprintf(stderr, "dpss_bin: %s\n", err.getmsg());
    return 1;
  }

 
  dpss myseq(dpss_args);
  try {
    myseq.compute();
  } catch (ERR err) {
    fprintf(stderr, "dpss_bin: %s\n", err.getmsg());
    return 1;             
  }

  print_dpss(&myseq, ofd);

  if (ofd!=stdout)
    fclose(ofd);

  return 0;
}
