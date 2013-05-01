//#define DEBUG

#ifdef DEBUG
FILE* log_path;
void create_log(void);
void write_log(char *,...);
#endif

