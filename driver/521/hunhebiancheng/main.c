int fun(int a,int b)
{
	return a+b;
}
extern int mycopy(char *psrc,char *pdest);
int main()
{
	char * src = "abcde";
	char dest[10] = {0};
	mycopy(src,dest);
	
	return 9;
}