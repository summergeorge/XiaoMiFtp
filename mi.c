#include <winsock2.h>
#include <process.h>

#define OK 1
#define FOUND 1
#define NOTFOUND 0
#define Status int

/** 小米FTP端口2121
    开始查找的ip = THREAD_HEAD
    查询ip的数量 = THREAD_NUM
**/
#define PORT 2121
#define THREAD_HEAD 100
#define THREAD_NUM 20

int last = 1;
int flag = THREAD_NUM;
HANDLE  ThreadEvent; /** 事件  */

/**
**  查找参数ip地址的PORT端口是否开放
**  返回值：FOUND; NOTFOUND;
**/
Status Search_mi(char* ip)
{
    WSADATA wsaData;
    if(0 != WSAStartup(MAKEWORD(2,0),&wsaData)) exit(-1);/** 初始化 winSocket  */
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    SOCKADDR_IN sa;
    sa.sin_addr.s_addr = inet_addr (ip);

    SetEvent(ThreadEvent); /** 触发事件  */

    sa.sin_family=AF_INET;
    sa.sin_port = htons(PORT);

    int ret = 0,error = -1,len = sizeof(int);
    unsigned long ul = 1;
    struct timeval tout;
    fd_set se;
    ioctlsocket(sock, FIONBIO, &ul); /** 设置为非阻塞模式  */
    if(connect(sock,(SOCKADDR *)&sa,sizeof(sa))== SOCKET_ERROR)
    {
        tout.tv_sec  = 0;
        tout.tv_usec = 800*1000;/** 100毫秒;1秒 == 1000豪秒== 1000微秒;  */
        FD_ZERO(&se);
        FD_SET(sock, &se);
        if( select(sock+1, NULL, &se, NULL, &tout) > 0)
        {
            getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error,&len);
            if(error == 0)
            {
                ret = 1;
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            ret = 0;
        }
    }
    else
    {
        ret = 1;
    }
    if(1 == ret)
    {
        closesocket(sock);
        WSACleanup();
        return FOUND;
    }
    else
    {
        closesocket(sock);
        WSACleanup();
        return NOTFOUND;
    }
}

/** 
**  获取本机IP地址
**  返回值：char* ip;
**/
char* CheckIP(void)
{
    WSADATA wsaData;
    char name[255];/** 主机名  */
    char *ip;

    PHOSTENT hostinfo;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0)
    {
        if(gethostname(name,sizeof(name)) == 0)
        {
            /** 若成功获得主机名，则返回一个指针，指向hostinfo */
            if((hostinfo = gethostbyname(name)) != NULL)
            {
                /** inet_ntoa()函数，将hostinfo结构变量中的h_addr_list转化为标准的点分表示的IP */
                ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
                return ip;
            }
        }
        WSACleanup();
    }
    return NULL;
}

/**
**  获取IP地址前3段
**  返回值：Status;
**/
Status IPhead(char* ip)
{
    char *startip = CheckIP();
    strcat(ip,strtok(startip, "."));
    strcat(ip,".");
    strcat(ip,strtok (NULL, "."));
    strcat(ip,".");
    strcat(ip,strtok (NULL, "."));
    strcat(ip,".");

    /** 如果当前主机ip第四段在10以内，为防止减后溢出，强制从***.***.***.1开始查询  **/

    last = atoi(strtok(NULL,"."));
    if (last > 10)
        last -= 10;


    return OK;
}

unsigned  __stdcall  thread_func(void *ip)
{
    //char cmd[50] = "explorer.exe ftp://";
    char cmd[50] = "ftp://";
    char temp[5];
    strcat(cmd,(char *)ip);
    strcat(cmd,":");
    itoa(PORT,temp,10);
    strcat(cmd,temp);
    if(FOUND == Search_mi(ip))
    {
        //system(cmd);
        // MessageBox(NULL,cmd,"123",0);
//ShellExecute(NULL,"open","explorer.exe",TEXT("ftp://192.168.1.101:2121"),NULL,SW_SHOWNORMAL);
        ShellExecute(NULL,"open","explorer.exe",cmd,NULL,SW_SHOWNORMAL);
    }
    else
    {
        flag--;
    }

    return 0;
}




int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    ThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    char iphead[16] = "";
    IPhead(iphead);/** 获取网关IP地址前3段  */
    HANDLE  handle[THREAD_NUM];
    int i;
    for(i = 0; i < THREAD_NUM; i++)
    {
        char tmp[4] = "",ip[16] = "";
        strcat(ip,iphead);
        itoa(last  + i,tmp,10);
        strcat(ip,tmp);/** 拼接上IP地址最后一段 */

        /** 开THREAD_NUM个线程进行扫描  */
        handle[i] = (HANDLE)_beginthreadex(NULL,0,thread_func,ip,0,NULL);
        WaitForSingleObject(ThreadEvent, INFINITE); /** 等待事件被触发  */
    }
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE,INFINITE);
    CloseHandle(ThreadEvent);/** 销毁事件  */
    if(0 == flag)
    {
        char notfound[] = "请按下面方法检查：\n\n\t1, 手机和电脑 连接同一个路由器\n\t2, 打开小米手机 -> 文件管理 -> 远程管理 -> 启动服务";
        MessageBox(NULL,notfound,TEXT("没有找到小米手机！"),MB_OK|MB_ICONWARNING);
    }
    return 0;
}
