#include<WinSock2.h>
#include<stdio.h>
#include<string.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

void parseUrl(const  char* url, char* host, char* respath);
void getImageUrl(const char* html, char* imgUrl);


struct Spider
{
	char host[128];
	char respath[128];
	SOCKET fd; 
} spider;

//��ʼ���ṹ��
void Spider_init(struct Spider* spider, const char* url)
{
	memset(spider->host, 0, sizeof(spider->host));
	memset(spider->respath, 0, sizeof(spider->respath));

	parseUrl(url, spider->host, spider->respath);
}
//���ӵ�������
void spider_connect(struct Spider* spider)
{
	//��socket
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	//����socket
	spider->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (spider->fd == SOCKET_ERROR)
	{
		printf("create socket failed");
		return;
	}
	//ͨ��������ȡIP��ַ
	HOSTENT *hent = gethostbyname(spider->host);
	if (!hent)
	{
		printf("Get host failed");
		return;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memcpy(&addr.sin_addr, hent->h_addr, sizeof(IN_ADDR));
	if (SOCKET_ERROR == connect(spider->fd, &addr, sizeof(addr)))
	{
		printf("connect failed %d\n", WSAGetLastError());
		return 0;
	}


	//���ӷ�����

}

//����URL����ȡ����������Դ·��
void parseUrl(const  char* url, char* host, char* respath)
{
	if (!url)
		return;
	const char* ph = strstr(url, "//");
	ph = ph ? ph + 2 : url;
	//if (!ph)
	//{
	//	ph = url;
	//} 
	//else
	//{
	//	ph += 2;
	//}
	const char* pp = strstr(ph, "/");
	if (!pp)
	{
		strcpy(host, ph);
	}
	else
	{
		strncpy(host, ph, pp - ph);
		strcpy(respath, pp);
	}
	strncpy(host, ph, pp - ph);
	strcpy(respath, pp);
}

//��ȡ��ҳ
void getHtml(struct Spider* spider)
{
	//���ӵ�������
	spider_connect(spider);
	//����������������
	char header[128] = { 0 };
	sprintf(header, "GET %s HTTP/1.1\r\n", spider->respath);
	sprintf(header+strlen(header), "Host:%s\r\n", spider->host);
	strcat(header, "connection:Close\r\n");
	strcat(header, "\r\n");

	if (SOCKET_ERROR == send(spider->fd, header, strlen(header), 0))
	{
		printf("send failed %d\n", WSAGetLastError());
		return;
	}

	//��ȡ��ҳ
	char html[1024 * 5] = { 0 };
	int len= recv(spider->fd, html, 1024 * 5, 0);
	if (len == SOCKET_ERROR)
	{
		printf("RECV failed %d\n", WSAGetLastError());
		return;
	}
	else
	{
		FILE* fp = fopen("mayae.html", "w");
		if (!fp)
			return;
		fwrite(html, sizeof(char), strlen(html), fp);
		fclose(fp);
		//printf("%s\n", html);
	}


	char imgUrl[128] = { 0 };
	getImageUrl(html, imgUrl);



	printf("%s\n", imgUrl);
}

//��ȡ��ҳ�е�ͼƬ
void getImageUrl(const char* html,char* imgUrl)
{
	//<img src="http://img.netbian.com/file/2022/0927/181623I3zfc.jpg"
	if (!html || !imgUrl)
		return;
	char* beg = strstr(html, "img src=\"");
	if (!beg)
	{
		return;
	}
	else
	{
		beg += 9;
	}
	//�ҽ�β��˫����
	char* end=strstr(beg,"\"");
	if (!end)
	{
		printf("��ҳ����");
	}
	else
	{
		strncpy(imgUrl, beg, end - beg);
	}
}

int main()
{
	printf("������Ҫ��ȡ����ַ��");
	char url[128] = "http://www.netbian.com/s/yuanchuang/";
	//gets_s(url, 128);//�ַ�������
	puts(url);//�ַ������

	struct Spider sp;
	Spider_init(&sp, url);


	spider_connect(&sp);
	getHtml(&sp);

	//printf("Host:%s\n resPath:%s\n", sp.host, sp.respath);



	getchar();
	return 0;
}