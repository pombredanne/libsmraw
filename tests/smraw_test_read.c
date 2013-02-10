/*
 * Storage media (SM) RAW library read testing program
 *
 * Copyright (c) 2010-2013, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <file_stream.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "smraw_test_libcerror.h"
#include "smraw_test_libcstring.h"
#include "smraw_test_libsmraw.h"

#define SMRAW_TEST_READ_BUFFER_SIZE	4096

/* Tests libsmraw_handle_get_offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int smraw_test_get_offset(
     libsmraw_handle_t *handle,
     off64_t expected_offset,
     libcerror_error_t **error )
{
	static char *function = "smraw_test_get_offset";
	off64_t result_offset = 0;

	if( expected_offset != -1 )
	{
		if( libsmraw_handle_get_offset(
		     handle,
		     &result_offset,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve offset.",
			 function );

			return( -1 );
		}
		if( result_offset != expected_offset )
		{
			fprintf(
			 stderr,
			 "%s: unexpected result offset: %" PRIi64 "\n",
			 function,
			 result_offset );

			return( 0 );
		}
	}
	return( 1 );
}

/* Tests libsmraw_handle_seek_offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int smraw_test_seek_offset(
     libsmraw_handle_t *handle,
     off64_t input_offset,
     int input_whence,
     off64_t expected_offset )
{
	libsmraw_error_t *error = NULL;
	off64_t result_offset   = 0;
	int result              = 0;

	if( handle == NULL )
	{
		return( -1 );
	}
	result_offset = libsmraw_handle_seek_offset(
	                 handle,
	                 input_offset,
	                 input_whence,
	                 &error );

	if( result_offset == -1 )
	{
		libsmraw_error_backtrace_fprint(
		 error,
		 stderr );

		libsmraw_error_free(
		 &error );
	}
	if( result_offset == -1 )
	{
		libsmraw_error_backtrace_fprint(
		 error,
		 stderr );

		libsmraw_error_free(
		 &error );
	}
	if( result_offset != expected_offset )
	{
		fprintf(
		 stderr,
		 "Unexpected result offset: %" PRIi64 "\n",
		 result_offset );
	}
	else
	{
		result = 1;
	}
	return( result );
}

/* Tests libsmraw_handle_read_buffer
 * Returns 1 if successful, 0 if not or -1 on error
 */
int smraw_test_read_buffer(
     libsmraw_handle_t *handle,
     size64_t input_size,
     size64_t expected_size )
{
	uint8_t buffer[ SMRAW_TEST_READ_BUFFER_SIZE ];

	libsmraw_error_t *error = NULL;
	size64_t remaining_size = 0;
	size64_t result_size    = 0;
	size_t read_size        = 0;
	ssize_t read_count      = 0;
	int result              = 0;

	if( handle == NULL )
	{
		return( -1 );
	}
	remaining_size = input_size;

	while( remaining_size > 0 )
	{
		read_size = SMRAW_TEST_READ_BUFFER_SIZE;

		if( remaining_size < (size64_t) read_size )
		{
			read_size = (size_t) remaining_size;
		}
		read_count = libsmraw_handle_read_buffer(
			      handle,
			      buffer,
			      read_size,
			      &error );

		if( read_count < 0 )
		{
			libsmraw_error_backtrace_fprint(
			 error,
			 stderr );

			libsmraw_error_free(
			 &error );

			break;
		}
		remaining_size -= (size64_t) read_count;
		result_size    += (size64_t) read_count;

		if( read_count != (ssize_t) read_size )
		{
			break;
		}
	}
	if( expected_size == result_size )
	{
		result = 1;
	}
	else
	{
		fprintf(
		 stderr,
		 "Unexpected read count: %" PRIu64 "\n",
		 result_size );
	}
	return( result );
}

/* Tests reading data at a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int smraw_test_read(
     libsmraw_handle_t *handle,
     off64_t input_offset,
     int input_whence,
     size64_t input_size,
     off64_t expected_offset,
     size64_t expected_size )
{
	libcerror_error_t *error   = NULL;
	const char *whence_string = NULL;
	int result                = 0;

	if( handle == NULL )
	{
		return( -1 );
	}
	if( input_whence == SEEK_CUR )
	{
		whence_string = "SEEK_CUR";
	}
	else if( input_whence == SEEK_END )
	{
		whence_string = "SEEK_END";
	}
	else if( input_whence == SEEK_SET )
	{
		whence_string = "SEEK_SET";
	}
	else
	{
		whence_string = "UNKNOWN";
	}
	fprintf(
	 stdout,
	 "Testing reading range with offset: %" PRIi64 ", whence: %s and size: %" PRIu64 "\t",
	 input_offset,
	 whence_string,
	 input_size );

	result = smraw_test_seek_offset(
	          handle,
	          input_offset,
	          input_whence,
	          expected_offset );

	if( result == 1 )
	{
		if( input_offset >= 0 )
		{
			result = smraw_test_read_buffer(
				  handle,
				  input_size,
				  expected_size );
		}
	}
	if( result == 1 )
	{
		if( input_offset >= 0 )
		{
			result = smraw_test_get_offset(
			          handle,
			          input_offset + expected_size,
			          &error );
		}
	}
	if( result != 0 )
	{
		fprintf(
		 stdout,
		 "(PASS)" );
	}
	else
	{
		fprintf(
		 stdout,
		 "(FAIL)" );
	}
	fprintf(
	 stdout,
	 "\n" );

	if( result == -1 )
	{
		libcerror_error_backtrace_fprint(
		 error,
		 stdout );

		libcerror_error_free(
		 &error );
	}
	return( result );
}

/* Tests reading data on the segment file size boundary
 * Returns 1 if successful, 0 if not or -1 on error
 */
int smraw_test_read_segment_file(
     libsmraw_handle_t *handle )
{
	libsmraw_error_t *error    = NULL;
	size64_t segment_file_size = 0;
	int result                 = 0;

	if( handle == NULL )
	{
		return( -1 );
	}
	if( libsmraw_handle_get_maximum_segment_size(
	     handle,
	     &segment_file_size,
	     &error ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to retrieve segment file size.\n" );

		libsmraw_error_backtrace_fprint(
		 error,
		 stderr );

		libsmraw_error_free(
		 &error );

		return( -1 );
	}
	if( segment_file_size > (size64_t) INT64_MAX )
	{
		fprintf(
		 stderr,
		 "Segment file size exceeds maximum.\n" );

		libsmraw_error_backtrace_fprint(
		 error,
		 stderr );

		libsmraw_error_free(
		 &error );

		return( -1 );
	}
	segment_file_size -= 1;

	fprintf(
	 stdout,
	 "Testing reading range with offset: %" PRIu64 ", whence: SEEK_SET and size: %" PRIu64 "\t",
	 segment_file_size,
	 segment_file_size );

	result = smraw_test_seek_offset(
	          handle,
	          segment_file_size,
	          SEEK_SET,
	          segment_file_size );

	if( result == 1 )
	{
		result = smraw_test_read_buffer(
			  handle,
			  segment_file_size,
			  segment_file_size );
	}
	if( result == 1 )
	{
		result = smraw_test_read_buffer(
			  handle,
			  segment_file_size,
			  segment_file_size );
	}
	if( result != 0 )
	{
		fprintf(
		 stdout,
		 "(PASS)" );
	}
	else
	{
		fprintf(
		 stdout,
		 "(FAIL)" );
	}
	fprintf(
	 stdout,
	 "\n" );

	return( result );
}

/* The main program
 */
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
int wmain( int argc, wchar_t * const argv[] )
#else
int main( int argc, char * const argv[] )
#endif
{
	libsmraw_error_t *error   = NULL;
	libsmraw_handle_t *handle = NULL;
	size64_t media_size       = 0;

	if( argc < 2 )
	{
		fprintf(
		 stderr,
		 "Missing filename(s).\n" );

		return( EXIT_FAILURE );
	}
	/* Initialization
	 */
	if( libsmraw_handle_initialize(
	     &handle,
	     &error ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to create handle.\n" );

		goto on_error;
	}
#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libsmraw_handle_open_wide(
	     handle,
	     &( argv[ 1 ] ),
	     argc - 1,
	     LIBSMRAW_OPEN_READ,
	     &error ) != 1 )
#else
	if( libsmraw_handle_open(
	     handle,
	     &( argv[ 1 ] ),
	     argc - 1,
	     LIBSMRAW_OPEN_READ,
	     &error ) != 1 )
#endif
	{
		fprintf(
		 stderr,
		 "Unable to open file(s).\n" );

		goto on_error;
	}
	if( libsmraw_handle_get_media_size(
	     handle,
	     &media_size,
	     &error ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to retrieve media size.\n" );

		goto on_error;
	}
	if( media_size > (size64_t) INT64_MAX )
	{
		fprintf(
		 stderr,
		 "Media size exceeds maximum.\n" );

		goto on_error;
	}
	fprintf(
	 stdout,
	 "Media size: %" PRIu64 " bytes\n",
	 media_size );

	/* Case 0: test full read
	 */

	/* Test: offset: 0 size: <media_size>
	 * Expected result: offset: 0 size: <media_size>
	 */
	if( smraw_test_read(
	     handle,
	     0,
	     SEEK_SET,
	     media_size,
	     0,
	     media_size ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test read.\n" );

		goto on_error;
	}
	/* Test: offset: 0 size: <media_size>
	 * Expected result: offset: 0 size: <media_size>
	 */
	if( smraw_test_read(
	     handle,
	     0,
	     SEEK_SET,
	     media_size,
	     0,
	     media_size ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test read.\n" );

		goto on_error;
	}

	/* Case 1: test random read
	 */

	/* Test: offset: <media_size / 7> size: <media_size / 2>
	 * Expected result: offset: <media_size / 7> size: <media_size / 2>
	 */
	if( smraw_test_read(
	     handle,
	     (off64_t) ( media_size / 7 ),
	     SEEK_SET,
	     media_size / 2,
	     (off64_t) ( media_size / 7 ),
	     media_size / 2 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test read.\n" );

		goto on_error;
	}
	/* Test: offset: <media_size / 7> size: <media_size / 2>
	 * Expected result: offset: <media_size / 7> size: <media_size / 2>
	 */
	if( smraw_test_read(
	     handle,
	     (off64_t) ( media_size / 7 ),
	     SEEK_SET,
	     media_size / 2,
	     (off64_t) ( media_size / 7 ),
	     media_size / 2 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test read.\n" );

		goto on_error;
	}

	/* Case 3: test read beyond media size
	 */

	if( media_size < 1024 )
	{
		/* Test: offset: <media_size - 1024> size: 4096
		 * Expected result: offset: -1 size: <undetermined>
		 */
		if( smraw_test_read(
		     handle,
		     (off64_t) ( media_size - 1024 ),
		     SEEK_SET,
		     4096,
		     -1,
		     (size64_t) -1 ) != 1 )
		{
			fprintf(
			 stderr,
			 "Unable to test read.\n" );

			goto on_error;
		}
		/* Test: offset: <media_size - 1024> size: 4096
		 * Expected result: offset: -1 size: <undetermined>
		 */
		if( smraw_test_read(
		     handle,
		     (off64_t) ( media_size - 1024 ),
		     SEEK_SET,
		     4096,
		     -1,
		     (size64_t) -1 ) != 1 )
		{
			fprintf(
			 stderr,
			 "Unable to test read.\n" );

			goto on_error;
		}
	}
	else
	{
		/* Test: offset: <media_size - 1024> size: 4096
		 * Expected result: offset: <media_size - 1024> size: 1024
		 */
		if( smraw_test_read(
		     handle,
		     (off64_t) ( media_size - 1024 ),
		     SEEK_SET,
		     4096,
		     (off64_t) ( media_size - 1024 ),
		     1024 ) != 1 )
		{
			fprintf(
			 stderr,
			 "Unable to test read.\n" );

			goto on_error;
		}
		/* Test: offset: <media_size - 1024> size: 4096
		 * Expected result: offset: <media_size - 1024> size: 1024
		 */
		if( smraw_test_read(
		     handle,
		     (off64_t) ( media_size - 1024 ),
		     SEEK_SET,
		     4096,
		     (off64_t) ( media_size - 1024 ),
		     1024 ) != 1 )
		{
			fprintf(
			 stderr,
			 "Unable to test read.\n" );

			goto on_error;
		}
	}
	if( argc > 3 )
	{
		/* Case 4: test on segment file size boundary
		 */
		if( smraw_test_read_segment_file(
		     handle ) != 1 )
		{
			fprintf(
			 stderr,
			 "Unable to test read segment file.\n" );

			goto on_error;
		}
	}
	/* Clean up
	 */
	if( libsmraw_handle_close(
	     handle,
	     &error ) != 0 )
	{
		fprintf(
		 stderr,
		 "Unable to close file(s).\n" );

		goto on_error;
	}
	if( libsmraw_handle_free(
	     &handle,
	     &error ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to free handle.\n" );

		goto on_error;
	}
	return( EXIT_SUCCESS );

on_error:
	if( error != NULL )
	{
		libsmraw_error_backtrace_fprint(
		 error,
		 stderr );
		libsmraw_error_free(
		 &error );
	}
	if( handle != NULL )
	{
		libsmraw_handle_close(
		 handle,
		 NULL );
		libsmraw_handle_free(
		 &handle,
		 NULL );
	}
	return( EXIT_FAILURE );
}

