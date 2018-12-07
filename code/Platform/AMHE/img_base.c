void img_base(int *img,int m,int n,int *basebin,int bin_num,int *baseimg)
	//主要功能为根据bin索引条生成新的图像，img为原图像，m和n分别为图像的列数和行数，basebin为索引条，bin_num为bin数目，baseimg为新图像
{
int i,j;
for(i=0;i<n;i++)
for(j=0;j<m;j++)
baseimg[i*m+j]=basebin[img[i*m+j]];
}