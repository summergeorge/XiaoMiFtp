#include <winsock2.h>
#include <process.h>

#define OK 1
#define FOUND 1
#define NOTFOUND 0
#define Status int

/** С��FTP�˿�2121
    ��ʼ���ҵ�ip = THREAD_HEAD
    ��ѯip������ = THREAD_NUM
**/
#define PORT 2121
#define THREAD_HEAD 100
#define THREAD_NUM 20

int last = 1;
int flag = THREAD_NUM;
HANDLE  ThreadEvent; /** �¼�  */

/**
**  ���Ҳ���ip��ַ��PORT�˿��Ƿ񿪷�
**  ����ֵ��FOUND; NOTFOUND;
**/
Status Search_mi(char* ip)
{
    WSADATA wsaData;
    if(0 != WSAStartup(MAKEWORD(2,0),&wsaData)) exit(-1);/** ��ʼ�� winSocket  */
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    SOCKADDR_IN sa;
    sa.sin_addr.s_addr = inet_addr (ip);

    SetEvent(ThreadEvent); /** �����¼�  */

    sa.sin_family=AF_INET;
    sa.sin_port = htons(PORT);

    int ret = 0,error = -1,len = sizeof(int);
    unsigned long ul = 1;
    struct timeval tout;
    fd_set se;
    ioctlsocket(sock, FIONBIO, &ul); /** ����Ϊ������ģʽ  */
    if(connect(sock,(SOCKADDR *)&sa,sizeof(sa))== SOCKET_ERROR)
    {
        tout.tv_sec  = 0;
        tout.tv_usec = 800*1000;/** 100����;1�� == 1000����== 1000΢��;  */
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
**  ��ȡ����IP��ַ
**  ����ֵ��char* ip;
**/
char* CheckIP(void)
{
    WSADATA wsaData;
    char name[255];/** ������  */
    char *ip;

    PHOSTENT hostinfo;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) == 0)
    {
        if(gethostname(name,sizeof(name)) == 0)
        {
            /** ���ɹ��������������򷵻�һ��ָ�룬ָ��hostinfo */
            if((hostinfo = gethostbyname(name)) != NULL)
            {
                /** inet_ntoa()��������hostinfo�ṹ�����е�h_addr_listת��Ϊ��׼�ĵ��ֱ�ʾ��IP */
                ip = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
                return ip;
            }
        }
        WSACleanup();
    }
    return NULL;
}

/**
**  ��ȡIP��ַǰ3��
**  ����ֵ��Status;
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

    /** ������ǰ����ip���Ķ���10���ڣ�Ϊ��ֹ����������ǿ�ƴ�***.***.***.1��ʼ��ѯ  **/

    last = atoi(strtok(NULL,"."));
    if (last > 10)
        last -= 10;
    else
        last = 1;    


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
    IPhead(iphead);/** ��ȡ����IP��ַǰ3��  */
    HANDLE  handle[THREAD_NUM];
    int i;
    for(i = 0; i < THREAD_NUM; i++)
    {
        char tmp[4] = "",ip[16] = "";
        strcat(ip,iphead);
        itoa(last  + i,tmp,10);
        strcat(ip,tmp);/** ƴ����IP��ַ����һ�� */

        /** ��THREAD_NUM���߳̽���ɨ��  */
        handle[i] = (HANDLE)_beginthreadex(NULL,0,thread_func,ip,0,NULL);
        WaitForSingleObject(ThreadEvent, INFINITE); /** �ȴ��¼�������  */
    }
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE,INFINITE);
    CloseHandle(ThreadEvent);/** �����¼�  */
    if(0 == flag)
    {
        char notfound[] = "�밴���淽�����飺\n\n\t1, �ֻ��͵��� ����ͬһ��·����\n\t2, ����С���ֻ� -> �ļ����� -> Զ�̹��� -> ��������";
        MessageBox(NULL,notfound,TEXT("û���ҵ�С���ֻ���"),MB_OK|MB_ICONWARNING);
    }
    return 0;
}
