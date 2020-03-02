//C system files
#include <math.h>
#include <assert.h>
#include <string.h>

//C++ system files
#include <vector>
#include <iostream>
#include <cstdio>

/**
 *This program takes a vdb file and proccess its information in order to eliminate the 
 *rows that contain irrelevant information to generate FEM meshes
 */
int main( int argc , char** argv ){
	
	FILE *fin, *fout;
	fin = fopen( argv[ 1 ] , "r" );
	fout = fopen( argv[ 2 ] , "w" );

	char line[128];
	
	while( !feof( fin ) ){
		fgets( line , 128 , fin );
		if(  line[ 0 ] == 'A'  ){
			if(  line[ 1 ] == 'T'  ){
				if(  line[ 2 ] == 'O'  ){
					if(  line[ 3 ] == 'M'  ){
						fprintf( fout , "%s" , line );
					}
				}
			}			
		}
		

	}
	
	fclose( fin );
	fclose( fout );
  return 0;
}
