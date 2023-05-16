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

//初始化结构体
void Spider_init(struct Spider* spider, const char* url)
{
	memset(spider->host, 0, sizeof(spider->host));
	memset(spider->respath, 0, sizeof(spider->respath));

	parseUrl(url, spider->host, spider->respath);
}
//连接到服务器
void spider_connect(struct Spider* spider)
{
	//打开socket
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	//创建socket
	spider->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (spider->fd == SOCKET_ERROR)
	{
		printf("create socket failed");
		return;
	}
	//通过域名获取IP地址
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


	//连接服务器

}

//解析URL，获取主机名和资源路径
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

//获取网页
void getHtml(struct Spider* spider)
{
	//连接到服务器
	spider_connect(spider);
	//给服务器发送请求
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

	//获取网页
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

//获取网页中的图片
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
	//找结尾的双引号
	char* end=strstr(beg,"\"");
	if (!end)
	{
		printf("网页错误");
	}
	else
	{
		strncpy(imgUrl, beg, end - beg);
	}
}

int main()
{
	printf("请输入要爬取的网址：");
	char url[128] = "http://www.netbian.com/s/yuanchuang/";
	//gets_s(url, 128);//字符串输入
	puts(url);//字符串输出

	struct Spider sp;
	Spider_init(&sp, url);


	spider_connect(&sp);
	getHtml(&sp);

	//printf("Host:%s\n resPath:%s\n", sp.host, sp.respath);



	getchar();
	return 0;
}