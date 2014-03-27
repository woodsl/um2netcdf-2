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
 *** USAGE
 ***
 *** Displays the command-line flags available for um2netcdf.
 ***
 ***   Mark Cheeseman, NIWA
 ***   January 17, 2014
 ***/
  
void usage() {
     printf( "\nUsage:  um2netcdf.x [-i] [-r] [-s] <input-UM-fieldfile> <stash-xml-file> \n\n" );
     printf( "  where\n" );
     printf( "      -i interpolates all fields onto the thermodynamic grid (eg. P-points on an Arakawa-C grid)\n" );
     printf( "      -r fields written in reduced precision (eg. INT/FLOAT instead of LONG/DOUBLE)\n" );
     printf( "      -s used to specify a set of stash codes of UM variables that can be selectively extracted\n" );
     printf( "         from the input UM fields file into the NetCDF output file. This should be the last flag\n" );
     printf( "         specified by the user.  Example:\n\n " );
     printf( "            um2netcdf.x -i -r -s 3209 3210 input.um stash.xml\n\n" );
}


/***
 *** IBM_IEEE_FLOAT_CONVERSION 
 ***
 *** Subroutine that converts an array of floats from IBM float32 to IEEE float32 format.
 *** For reference, the 2 formats differ in bit structure in the following way:
 ***
 ***                IBM Float              IEEE Float 
 ***  Sign           bit 0                   bit 0
 ***  Exponent       bit 1-7   [7 total]     bit 1-8 [8 total]
 ***  Mantissa       bit 8-31  [24 total]    bit 9-31  [23 total]
 ***
 *** INPUT:  val -> ptr to array of input values
 ***           N -> number of input values to be converted
 *** 
 ***   Mark Cheeseman, NIWA
 ***   January 23, 2014
 ***/

float ibm_ieee_float_conversion( uint32_t ibm_val ) {

     uint32_t mantissa, exponent, sign_bit;     

     if ( ibm_val==0 ) { return 0.0; }

     sign_bit = ibm_val >> 31 & 0x01;
     exponent = ibm_val >> 24 & 0x7f;
     mantissa = (ibm_val & 0x00ffffff) / ((float ) pow(2,24) );
     return (1 -2*sign_bit)*mantissa*pow(16,exponent-64); 

}
