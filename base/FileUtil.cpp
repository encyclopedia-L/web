#include "FileUtil.h"
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

AppendFile::AppendFile(string filename)
:fp(fopen(filename.c_str(),"a"))
{
    setbuffer(fp,buffer,sizeof(buffer));
}

~AppendFile()
{
    fclose(fp);
}

void AppendFile::append(const char *logline, const size_t len)
{
    size_t n = this->write(logline,len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t t = this->write(logline+n,remain);
        if(t == 0)
        {
            int err = ferror(fp);
            if(err)
                printf(stderr,"AppendFile::append() failed !\n");
            break;
        }
        n += t;
        remain = len - n;
    }
}

size_t AppendFile::write(const char *logline, size_t len)
{
    return fwrite_unlocked(logline,1,len,fp);
}

void AppendFile::flush()
{
    fflush(fp);
}
