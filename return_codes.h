/**
 * @since 2020-11-07
 * @author Karel Chanivecky Garcia
 * BCIT 
 * c_saw_server
 * return_codes.h
 */

#ifndef C_SAW_SERVER_RETURN_CODES_H
#define C_SAW_SERVER_RETURN_CODES_H

#define SUCCESS 0
#define ERRNO_ERROR -1
#define EXIT_WAITPID -2
#define EXIT_FORK -3
#define EXIT_CHILD -3
#define EXIT_EXEC -4
#define EXIT_TIME -5
#define EXIT_PROC_MSG -6
#define EXIT_SOCK_UNLINK -7
#define INVALID_HEADER_FIELD - 8
#define INVALID_URI -9


#define STATUS_OK 200
#define STATUS_NOT_MODIFIED 304
#define STATUS_FORBIDDEN 403
#define STATUS_NOT_FOUND 404
#define STATUS_NOT_IMPLEMENTED 501
#define STATUS_INTERNAL_SERVER_ERROR 505

#endif //C_SAW_SERVER_RETURN_CODES_H
