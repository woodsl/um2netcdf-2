/**============================================================================
                 U M 2 N e t C D F  V e r s i o n 2 . 0
                 --------------------------------------

    Main author: Mark Cheeseman
                 National Institute of Water & Atmospheric Research (Ltd)
                 Wellington, New Zealand
                 February 2014

    UM2NetCDF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    UM2NetCDF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    A copy of the GNU General Public License can be found in the main UM2NetCDF
    directory.  Alternatively, please see <http://www.gnu.org/licenses/>.
 **============================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <sys/types.h>
#include <netinet/in.h>

#define   expon   0x7F000000
#define   sign  0x80000000
#define   tiss  0x00FFFFFF
#define   etis  0x007FFFFF
#define   nrm   0x00F00000

/***
 *** STATUS_CHECK 
 ***
 *** The status code from the last function is checked.  If a failure is 
 *** indicated, (i.e. status=0) the program is stopped immediately.
 ***
 ***   Mark Cheeseman, NIWA
 ***   December 11, 2013
 ***/

void status_check( int status, char *message ) {

     if ( status==0 ) { 
        printf( "\n %s \n\n", message ); 
        exit(1);
     }

}


/***
 *** IBM2IEEE 
 ***
 *** Subroutine that converts IBM 32-bit float data into IEEE 32-bit float
 *** data.
 ***
 *** INPUT:    n -> # of values to be converted
 ***         ibm -> character pointer to IBM float32 data
 ***        ieee -> 
 ***
 ***   Mark Cheeseman, NIWA
 ***   January 17, 2014
 ***/

double ibm2ieee( uint32_t ibm ) {

       float    ieee;
       double   val;
       int32_t  ibe, it;
       uint32_t ibs, ibt;
       int      k;

       union { uint32_t i; float r; } u, res;

       ibs = ibm & sign;
       ibe = ibm & expon ;
       ibt = ibm & tiss;
       if (ibt == 0) {
          ibe = 0 ;
       } else {
          if ( (ibe != 0) && (ibt & nrm) == 0 ) {
             u.i = ibm ;
             u.r = u.r + 0e0 ;
             ibe = u.i & expon ;
             ibt = u.i & tiss ;
          }
          /* mantissa */
          it = ibt << 8;
          for (k = 0; (k < 5) && (it >= 0); k++ ) {
              it = it << 1;
          }
          if ( k < 4 ) {
             ibt = (it >> 8) & etis;
             ibe = (ibe >> 22) - 256 + 127 - k - 1;
             if (ibe < 0) {
                ibe = ibt = 0;
             }
             if (ibe >= 255) {
                ibe = 255; ibt = 0;
             }
             ibe = ibe << 23;
          }
       }

       res.i = ibs | ibe | ibt;
       ieee = res.r;
       if (ibe == 255<<23) {
          printf( "ERROR: numerical overflow in ibm2ieee\n" );
          exit(1);
       }

       val = (double ) ieee;
       return val;
}


double ibm2ieee_do_nothing( uint32_t ibm ) {

       float    ieee;
       double   val;
       int32_t  ibe, it;
       uint32_t ibs, ibt;
       int      k;

       union { uint32_t i; float r; } u, res;

       ibs = ibm & sign;
       ibe = ibm & expon ;
       ibt = ibm & tiss;
       if (ibt == 0) {
          ibe = 0 ;
       } else {
          if ( (ibe != 0) && (ibt & nrm) == 0 ) {
             u.i = ibm ;
             u.r = u.r + 0e0 ;
             ibe = u.i & expon ;
             ibt = u.i & tiss ;
          }
          /* mantissa */
          it = ibt << 8;
          for (k = 0; (k < 5) && (it >= 0); k++ ) {
              it = it << 1;
          }
          if ( k < 4 ) {
             ibt = (it >> 8) & etis;
             ibe = (ibe >> 22) - 256 + 127 - k - 1;
             if (ibe < 0) {
                ibe = ibt = 0;
             }
             if (ibe >= 255) {
                ibe = 255; ibt = 0;
             }
             ibe = ibe << 23;
          }
       }

       res.i = ibs | ibe | ibt;
       ieee = res.r;
       if (ibe == 255<<23) {
          printf( "ERROR: numerical overflow in ibm2ieee\n" );
          exit(1);
       }

       val = (double ) ieee;
       return val;

}

/***
 *** USAGE
 ***
 *** Displays the command-line flags available for um2netcdf.
 ***
 ***   Mark Cheeseman, NIWA
 ***   January 17, 2014
 ***/
  
void usage() {
     printf( "\nUsage:  um2netcdf.x [ OPTIONS ] <input-file> <stash-file> \n\n" );
     printf( "  where\n\n" );
     printf( "    input-file  --> file whose contents are to be parsed and transferred to a netCDF\n" );
     printf( "                    file. It can be an UM fields or dump file.\n\n" );
     printf( "    stash-file  --> XML file that contains the metadata definitions of various UM\n" );
     printf( "                    variables. These definitions include units, title, valid values\n" );
     printf( "                    etc...\n\n" );
     printf( "  The following options can be specified.  They MUST appear before the input filename!\n\n" );
     printf( "    -h used to display this help message\n" );
     printf( "    -i interpolates all fields onto the thermodynamic grid (eg. P-points on an Arakawa-C grid)\n" );
     printf( "    -r fields written in reduced precision (eg. INT/FLOAT instead of LONG/DOUBLE)\n" );
     printf( "    -n output NetCDF file will not contain any NetCDF-4 features (chunking and/or compression)\n" );
     printf( "    -o <filename> \n");
     printf( "       used to specify a filename to the output NetCDF file\n" );
     printf( "    -s used to specify a set of stash codes of UM variables that can be selectively extracted\n" );
     printf( "       from the input UM fields file into the NetCDF output file. Selected stash codes should\n" );
     printf( "       be in a space-delimited list.  Example:\n\n" );
     printf( "          um2netcdf.x -i -r -o test.nc -s 3209 3210 input.um stash.xml\n\n" );
     printf( "    -c used to specify the name of the run configuration XML file\n" );
     printf( "    -b used to specify a set of stash codes of UM variables to be ignored if located in the\n" );
     printf( "       input UM fields file into the NetCDF output file. Specified stash codes should\n" );
     printf( "       be in a space-delimited list.  Example:\n\n" );
     printf( "            um2netcdf.x -i -r -o test.nc -b 3209 3210 input.um stash.xml -c config.xml\n\n" );
     printf( "   It does not matter which order you put the option flags.\n\n" );
}


/***
 *** IEEE_USAGE_MESSAGE
 ***
 *** Displays a message instructing user how to use the IEEE utility when
 *** trying to convert/display an UM input file with WGDOS-packed fields.
 ***
 ***   Mark Cheeseman, NIWA
 ***   May 6, 2014
 ***/

void ieee_usage_message() {
     printf( "\nWARNING: you are attempting to convert an UM Fields or Dump field with\n" );
     printf( "           fields that are WGDOS-packed.  You will need to unpack the input\n" );
     printf( "           file using the following procedure:\n\n" );
     printf( "STEP 1: log into FitzRoy or Barometer\n\n" );
     printf( "STEP 2: set the UMDIR environment variable as so:\n" );
     printf( "        export UMDIR=/oper/admin/um_fcm/um\n\n" );
     printf( "STEP 3: Run the IEEE utility that comes with the UM release as so:\n" );
     printf( "        /oper/admin/um_fcm/um/vn8.4/ibm/utils/ieee -64e <input_filename> <output_filename>\n\n" );
}
 
