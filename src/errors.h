#ifndef _DEV_ERRORS_H_
#define _DEV_ERRORS_H_

#define EFS_STATUS unsigned int

#define ERROR_MASK            0xFF00

#define EFS_ERROR             0xD000
#define EFS_INIT_ERROR        (EFS_ERROR+1)
#define EFS_FILE_OPEN_ERROR   (EFS_ERROR+2)
#define EFS_FILE_WRITE_ERROR  (EFS_ERROR+3)
#define EFS_FILE_DELETE_ERROR (EFS_ERROR+4)
#define EFS_OPEN_DIR_ERROR    (EFS_ERROR+5)
#define EFS_MAKE_DIR_ERROR    (EFS_ERROR+6)
#define EFS_NO_FILE_FIND      (EFS_ERROR+7)


#endif  // _DEV_ERRORS_H_
