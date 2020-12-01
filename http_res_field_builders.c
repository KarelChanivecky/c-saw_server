/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT
 * c_saw_server
 * http_res_encoder.h
 */
#define _XOPEN_SOURCE



#include "http_res_field_builders.h"
#include "return_codes.h"
#include "http_req_parser.h"

#include <dc/stdlib.h>
#include <dc/unistd.h>
#include <dc/fcntl.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "math.h"
#include "unistd.h"
#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include "ctype.h"


#define SP " "
#define SEPARATOR ": "


// make status field
#define HTTP_VERSION "HTTP/1.0"
#define REASON_200 "STATUS_OK"
#define REASON_304 "Not modified since cached time"
#define REASON_403 "Path points outside of public directory"
#define REASON_404 "Page not found"
#define REASON_405 "Only "
#define REASON_505 "Internal server error"
#define REASON_501 "Not Implemented"

struct http_date {
    int month;
    int date;
    int year;
    int hours;
    int minutes;
    int seconds;
};
typedef struct http_date http_date;

char * make_status_field( int code ) {
    size_t field_length = strlen( HTTP_VERSION ) + 8; // 2 spaces, crlf, code, terminating null char
    char * reason;
    switch ( code ) {
        case 200:
            reason = REASON_200;
            field_length += strlen( reason );
            break;
        case 304:
            reason = REASON_304;
            field_length += strlen( reason );
            break;
        case 403:
            reason = REASON_403;
            field_length += strlen( reason );
            break;
        case 404:
            reason = REASON_404;
            field_length += strlen( reason );
            break;
        case 405:
            reason = REASON_405;
            field_length += strlen( reason );
            break;
        case 505:
            reason = REASON_505;
            field_length += strlen( reason );
            break;
        case 501:
            reason = REASON_501;
            field_length += strlen( reason );
            break;
    }
    char * status_line = dc_malloc( sizeof( char ) * field_length );
    char code_str[4];
    sprintf( code_str, "%d", code );

    strncat( status_line, HTTP_VERSION, strlen( HTTP_VERSION ) + 1 );
    strncat( status_line, SP, 2 );
    strncat( status_line, code_str, 4 );
    strncat( status_line, SP, 2 );
    strncat( status_line, reason, strlen( reason ) + 1 );
    return status_line;
}



// make last modified
#define LAST_MODIFIED_FIELD_NAME "Last-Modified"
#define MAX_TIME_STR_LEN 30

char * make_last_modified( char * path ) {
    struct stat file_stat;
    stat( path, &file_stat );
    struct tm last_modified_tm;
    gmtime_r( &file_stat.st_mtime, &last_modified_tm );
    char time_str[MAX_TIME_STR_LEN];
    size_t time_len = strftime( time_str, MAX_TIME_STR_LEN, HTTP_TIME_FORMAT, &last_modified_tm );
    size_t last_modified_len = strlen( LAST_MODIFIED_FIELD_NAME ) + time_len + 3; // : SP and null byte
    char * last_modified_str = ( char * ) dc_malloc( last_modified_len );
    strncat( last_modified_str, LAST_MODIFIED_FIELD_NAME, strlen( LAST_MODIFIED_FIELD_NAME ) + 1 );
    strncat( last_modified_str, SEPARATOR, 3 );
    strncat( last_modified_str, time_str, time_len + 1 );
    return last_modified_str;
}

#define CONTENT_LENGTH_FIELD_NAME "Content-Length"

char * make_content_length( char * path ) {
    struct stat file_stat;
    stat( path, &file_stat );
    int digits_in_size = ( int ) floor( log10( file_stat.st_size )) + 1;
    char file_size[digits_in_size + 1];
    sprintf( file_size, "%ld", file_stat.st_size );
    size_t content_length_str_len =
            strlen( CONTENT_LENGTH_FIELD_NAME ) + digits_in_size + 3; // separator, null byte
    char * content_length_str = ( char * ) dc_malloc( sizeof( char ) * content_length_str_len );
    strncat( content_length_str, CONTENT_LENGTH_FIELD_NAME, strlen( CONTENT_LENGTH_FIELD_NAME ) + 1 );
    strncat( content_length_str, SEPARATOR, 3 );
    strncat( content_length_str, file_size, digits_in_size + 1 );
    return content_length_str;
}

#define CONTENT_TYPE_FIELD_NAME "Content-Type"
#define FILE_COMMAND_CALL "file"
#define FILE_COMMAND_OPTIONS { "--mime-type", "-b", NULL, NULL }
#define MAX_CONTENT_TYPE_LENGTH 255

void execute_file( int out, char * path ) {
    char * options[] = FILE_COMMAND_OPTIONS;
    options[2] = path;
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        dup2( out, STDOUT_FILENO );
        execvp( FILE_COMMAND_CALL, options );
        fprintf( stderr, "Execute error at make content-type: %s", strerror(errno));
        exit( EXIT_EXEC );
    }
    if ( child_pid == -1 ) {
        fprintf( stderr, "Fork error at make content-type: %s", strerror(errno));
        exit( EXIT_FORK );
    }
    int status;
    int wait_status = waitpid( child_pid, &status, 0 );
    if ( wait_status == -1 ) {
        fprintf( stderr, "Wait error at make content-type: %s", strerror(errno));
        exit( EXIT_WAITPID );
    }
    if ( !WIFEXITED( status )) {
        fprintf( stderr, "Child error at make content-type: %d", status );
        exit( EXIT_CHILD );
    }
}

char * read_type( int src ) {
    char content_type_buf[MAX_CONTENT_TYPE_LENGTH];
    size_t bytes_read = dc_read( src, content_type_buf, MAX_CONTENT_TYPE_LENGTH );
    char * content_type_str = ( char * ) dc_malloc( sizeof( char ) * bytes_read + 1 );
    strncpy( content_type_str, content_type_buf, bytes_read );
    content_type_str[bytes_read] = 0;
    return content_type_str;

}

char * get_content_type( char * path ) {
    int fds[2];
    dc_pipe( fds );
    execute_file( fds[1], path );
    char * content_type = read_type( fds[0] );
    dc_close( fds[0] );
    dc_close( fds[1] );
    return content_type;
}


char * make_content_type( char * path ) {
    char * content_type = get_content_type( path );
    size_t content_type_field_len = strlen( content_type ) + strlen( CONTENT_TYPE_FIELD_NAME ) + 2; // separator
    char * content_type_field = ( char * ) dc_malloc( sizeof( char ) * content_type_field_len + 1 );
    strncat( content_type_field, CONTENT_TYPE_FIELD_NAME, strlen( CONTENT_TYPE_FIELD_NAME ) + 1 );
    strncat( content_type_field, SEPARATOR, 3 );
    strncat( content_type_field, content_type, strlen( content_type ) + 1 );
    free( content_type );
    return content_type_field;
}

#define DATE_BLOCK 35
#define SIMPLE_REQUEST "Simple-Request"


char * delete_leading_whitespaces( char * line ) {
    while ( isspace(( unsigned char ) *line ))
        line++;
    return line;
}

/**
 * This function is only responsible for checking if the path requested in http request goes beyond the scope of root directory.
 * @param path
 * @return
 */
bool path_in_bounds( char * path ) {

    size_t len = strlen( path );
    int forward_counter = *( path + len - 1 ) == URI_PATH_DELIMITER ? 1 : 0;
    int back_counter = 0;

    char buf[len + 1];
    strncpy( buf, path, len );
    buf[len] = 0;
    char ** args = tokenize_string( buf, "/\0", DEFAULT_INITIAL_TOKEN_ALLOC_COUNT );
    int i = 0;
    while ( args[i] ) {
        if ( !strcmp( args[i], ".." )) {
            back_counter++;
        } else {
            forward_counter++;
        }
        free( args[i] );
        i++;
    }
    free( args );
    return back_counter <= forward_counter;
}

bool is_simple_req( char * request_type ) {
    char * buffer = delete_leading_whitespaces( request_type );
    return strcmp( buffer, SIMPLE_REQUEST ) == 0;
}

char * prepare_entity_body( char * path ) {
    if ( !path ) {
        return NULL;
    }
    int src_fd = open( path, O_RDONLY );
    if (src_fd == -1 ) {
        if (errno == ENOENT
        || errno == EACCES
        || errno == EAGAIN
        || errno == ENOMEM
        || errno == EOVERFLOW) {
            return NULL;
        }
    }
    return read_fd( src_fd );
//    unsigned int counter = 0;
//    uint8_t temp = 0;
//
//    size_t c_read;
//    c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
//    while ( c_read > 0 ) {
//        counter++;
//        c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
//    }
//
//    char * body = malloc( sizeof( char ) * ( counter + 1 ));
//    close( src_fd );
//    src_fd = open( path, O_RDONLY );
//
//    temp = 0;
//    c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
//    while ( c_read > 0 ) {
//        strncat( body, ( char * ) &temp, 1 );
//        c_read = read( src_fd, &temp, BYTES_PER_CYCLE );
//    }
//
//    body = strcat( body, "\0" );
//    strncat( body, "\0", 1 );
//    res->body = body;
//    free( body );
}

#define TIME_FIELD_NAME "Date: "

char * get_date() {
    char buf[DATE_BLOCK];
    time_t now = time( 0 );
    struct tm tm = *gmtime( &now );
    size_t value_len = strftime( buf, sizeof buf, HTTP_TIME_FORMAT, &tm );
    size_t name_len = strlen( TIME_FIELD_NAME );
    char * date = dc_malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( date, TIME_FIELD_NAME, name_len );
    strncat( date, buf, value_len );
    return date;
}

#define SERVER_FIELD_NAME "Server: "
#define SERVER_NAME "C-Saw"

char * get_server() {
    size_t name_len = strlen( SERVER_FIELD_NAME );
    size_t value_len = strlen( SERVER_NAME );
    char * server = dc_malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( server, SERVER_FIELD_NAME, name_len );
    strncat( server, SERVER_NAME, value_len );
    return server;
}

#define ALLOWED_METHODS "GET/HEAD"
#define ALLOWED_METHODS_FIELD_NAME "Allow: "

char * get_allow() {
    size_t len_field_name = strlen( ALLOWED_METHODS_FIELD_NAME );
    size_t len_field_value = strlen( ALLOWED_METHODS );
    char * allow = dc_malloc( sizeof( char ) * ( len_field_name + len_field_value + 3 ));
    strncpy( allow, ALLOWED_METHODS_FIELD_NAME, len_field_name );
    strncat( allow, ALLOWED_METHODS, len_field_value );
    return allow;
}

#define EXPIRE_FIELD_NAME "Expires: "

char * get_expires( size_t minutes ) {
    if ( minutes == 0 ) {
        return NULL;
    }
    char buf[30];
    time_t now = time( 0 );

    struct tm expire = *gmtime( &now );
    expire.tm_min += minutes;

    mktime( &expire );

    size_t name_len = strlen( EXPIRE_FIELD_NAME );
    size_t value_len = strftime( buf, sizeof buf, HTTP_TIME_FORMAT, &expire );
    char * expiry_date = malloc( sizeof( char ) * ( value_len + name_len + 1 ));
    strncpy( expiry_date, EXPIRE_FIELD_NAME, name_len );
    strncat( expiry_date, buf, value_len );
    return expiry_date;
}

bool file_exists( char * path ) {
    struct stat st;
    bool status = stat( path, &st ) == -1 && errno == ENOENT;
    return status;
}


char ** dynamic_tokenize_time( char * req, char * delim ) {
    int index = 0;
    int block = DEFAULT_INITIAL_TOKEN_ALLOC_COUNT;
    char ** args = malloc( sizeof( char * ) * block );
    size_t line_len = strcspn( req, delim );
    char * token = malloc( sizeof( char ) * ( line_len + 1 ));
    memcpy( token, req, line_len );
    token[line_len] = '\0';

    while ( token != NULL) {
        args[index++] = token;
        if ( index >= block ) {
            block += DEFAULT_INITIAL_TOKEN_ALLOC_COUNT;
            args = realloc( args, block * sizeof( char * ));
            if ( !args ) {
                fprintf( stderr, "Memory allocation error\n" );
                exit( EXIT_FAILURE );
            }
        }

        req += line_len + 1;
        line_len = strcspn( req, delim );
        if ( line_len == 0 )
            break;
        else {
            token = malloc( sizeof( char ) * ( line_len + 1 ));
            memcpy( token, req, line_len );
            token[line_len] = '\0';
        }
    }

    free( token );

    return args;
}


int get_month( char * month ) {
    int i = 0;
    if ( !strcmp( month, "Jan" ))
        i = 1;
    else if ( !strcmp( month, "Feb" ))
        i = 2;
    else if ( !strcmp( month, "Mar" ))
        i = 3;
    else if ( !strcmp( month, "Apr" ))
        i = 4;
    else if ( !strcmp( month, "May" ))
        i = 5;
    else if ( !strcmp( month, "Jun" ))
        i = 6;
    else if ( !strcmp( month, "Jul" ))
        i = 7;
    else if ( !strcmp( month, "Aug" ))
        i = 8;
    else if ( !strcmp( month, "Sep" ))
        i = 9;
    else if ( !strcmp( month, "Oct" ))
        i = 10;
    else if ( !strcmp( month, "Nov" ))
        i = 11;
    else if ( !strcmp( month, "Dec" ))
        i = 12;
    return i;

}


bool compare_date_fields( http_date req_date, http_date file_date ) {
    if ( file_date.year > req_date.year ) {
        return true;
    }

    if ( file_date.month > req_date.month ) {
        return true;
    }

    if ( file_date.date > req_date.date ) {
        return true;
    }

    if ( file_date.hours > req_date.hours ) {
        return true;
    }
    if ( file_date.minutes > req_date.minutes ) {
        return true;
    }

    if ( file_date.seconds > req_date.seconds ) {
        return true;
    }
    return false;
}

/*
 * type 1 represents Sun, 06 Nov 1994 08:49:37 GMT
 * type 2 represents Sunday, 06-Nov-94 08:49:37 GMT
 * type 3 represents Sun Nov  6 08:49:37 1994
 */
bool compare_date_type_1( char * last_mod_date, char * if_mod_date ) {  //Sun, 06 Nov 1994 08:49:37 GMT"
    http_date file_date;
    http_date req_date;
    char ** file_last_mod_date = dynamic_tokenize_req( last_mod_date, 1 );
    char ** req_last_mod_date = dynamic_tokenize_req( if_mod_date, 1 );

    char ** file_last_mod_time = dynamic_tokenize_time( file_last_mod_date[4], ":" );
    char ** req_last_mod_time = dynamic_tokenize_time( req_last_mod_date[4], ":" );


    file_date.date = atoi( file_last_mod_date[1] );
    file_date.month = get_month( file_last_mod_date[2] );
    file_date.year = atoi( file_last_mod_date[3] );
    file_date.hours = atoi( file_last_mod_time[0] );
    file_date.minutes = atoi( file_last_mod_time[1] );
    file_date.seconds = atoi( file_last_mod_time[2] );

    req_date.date = atoi( req_last_mod_date[1] );
    req_date.month = get_month( req_last_mod_date[2] );
    req_date.year = atoi( req_last_mod_date[3] );
    req_date.hours = atoi( req_last_mod_time[0] );
    req_date.minutes = atoi( req_last_mod_time[1] );
    req_date.seconds = atoi( req_last_mod_time[2] );

    return compare_date_fields( req_date, file_date );

}
/*
 * type 1 represents Sun, 06 Nov 1994 08:49:37 GMT
 * type 2 represents Sunday, 06-Nov-94 08:49:37 GMT
 * type 3 represents Sun Nov  6 08:49:37 1994
 */
bool compare_date_type_2( char * last_mod_date, char * if_mod_date ) { //Sunday, 06-Nov-94 08:49:37 GMT
    http_date file_date;
    http_date req_date;
    char ** file_last_mod_date = dynamic_tokenize_req( last_mod_date, 1 );
    char ** req_last_mod_date = dynamic_tokenize_req( if_mod_date, 1 );

    char ** req_last_mod_calender_date = dynamic_tokenize_time( req_last_mod_date[1], "-" );

    char ** file_last_mod_time = dynamic_tokenize_time( file_last_mod_date[2], ":" );
    char ** req_last_mod_time = dynamic_tokenize_time( req_last_mod_date[2], ":" );

    file_date.date = atoi( file_last_mod_date[1] );
    file_date.month = get_month( file_last_mod_date[2] );
    file_date.year = atoi( file_last_mod_date[3] );
    file_date.hours = atoi( file_last_mod_time[0] );
    file_date.minutes = atoi( file_last_mod_time[1] );
    file_date.seconds = atoi( file_last_mod_time[2] );

    req_date.date = atoi( req_last_mod_calender_date[0] );
    req_date.month = get_month( req_last_mod_calender_date[1] );
    req_date.year = atoi( req_last_mod_calender_date[2] );
    req_date.hours = atoi( req_last_mod_time[0] );
    req_date.minutes = atoi( req_last_mod_time[1] );
    req_date.seconds = atoi( req_last_mod_time[2] );

    if ( req_date.year <= 30 ) {
        req_date.year += 2000;
    } else {
        req_date.year += 1900;
    }

    return compare_date_fields( req_date, file_date );
}

//bool compare_datesv2(char * last_mod_date, char * if_mod_date, char * date_format ) {
//    struct tm if_mod_date_tm;
//    char * if_mod = strptime(if_mod_date, date_format, &if_mod_date_tm);
//    if (if_mod == NULL) {
//        fprintf( stderr, "Time parsing error at check_modified_since\n");
//        exit(EXIT_TIME);
//    }
//
//    char * last_mod = strptime(if_mod_date, date_format, &if_mod_date_tm);
//    if (last_mod == NULL) {
//        fprintf( stderr, "Time parsing error at check_modified_since\n");
//        exit(EXIT_TIME);
//    }
//
//    time_t since_time = mktime(&since_time_tm);
//}


/*
 * type 1 represents Sun, 06 Nov 1994 08:49:37 GMT
 * type 2 represents Sunday, 06-Nov-94 08:49:37 GMT
 * type 3 represents Sun Nov  6 08:49:37 1994
 */
bool compare_date_type_3( char * last_mod_date, char * if_mod_date ) { //"Sun Nov  6 08:49:37 1994"
    http_date file_date;
    http_date req_date;
    char ** file_last_mod_date = dynamic_tokenize_req( last_mod_date, 1 );
    char ** req_last_mod_date = dynamic_tokenize_req( if_mod_date, 1 );

    char ** file_last_mod_time = dynamic_tokenize_time( file_last_mod_date[2], ":" );
    char ** req_last_mod_time = dynamic_tokenize_time( req_last_mod_date[2], ":" );

    file_date.date = atoi( file_last_mod_date[1] );
    file_date.month = get_month( file_last_mod_date[2] );
    file_date.year = atoi( file_last_mod_date[3] );
    file_date.hours = atoi( file_last_mod_time[0] );
    file_date.minutes = atoi( file_last_mod_time[1] );
    file_date.seconds = atoi( file_last_mod_time[2] );

    req_date.date = atoi( req_last_mod_date[2] );
    req_date.month = get_month( req_last_mod_date[1] );
    req_date.year = atoi( req_last_mod_date[4] );
    req_date.hours = atoi( req_last_mod_time[0] );
    req_date.minutes = atoi( req_last_mod_time[1] );
    req_date.seconds = atoi( req_last_mod_time[2] );

    return compare_date_fields( req_date, file_date );
}



/*
 * HTTP_TIME represents Sun, 06 Nov 1994 08:49:37 GMT
 * OTH_1 represents Sunday, 06-Nov-94 08:49:37 GMT
 * OTH_2 represents Sun Nov  6 08:49:37 1994
 */
//bool compare_dates( char * last_mod_date, http_req_t * req ) {
//    bool is_modified_after_asked_date = false;
//    size_t type_of_date = get_type_of_date( last_mod_date );
//    switch ( type_of_date ) {
//        case 1:
//            return is_modified_after_asked_date = check_modified_since( req, HTTP_TIME_FORMAT );
//        case 2:
//            return is_modified_after_asked_date = check_modified_since( req, DATE_FORMAT_OTH_1 );
//        case 0:
//            return is_modified_after_asked_date = check_modified_since( req, DATE_FORMAT_OTH_2 );
//        default:
//            return true;
//    }
//}

//bool file_modified_after_requested_if_modified_date( http_req_t * req ) {
//    char buf[DATE_BLOCK];
//    struct stat file_stat;
//    stat( req->request_URI, &file_stat );
//    struct tm last_modified_tm;
//    gmtime_r( &file_stat.st_mtime, &last_modified_tm );
//
//    int len = strftime( buf, sizeof buf, HTTP_TIME_FORMAT, &last_modified_tm );
//
//    char * last_modified_date = malloc( sizeof( char ) * ( len + 2 ));
//    strcpy( last_modified_date, buf );
//
//    return  compare_dates( req );
//}

void prepare_common_headers( http_res_t * res ) {
    res->server = get_server();
    res->date = get_date();
    res->allow = get_allow();
}

