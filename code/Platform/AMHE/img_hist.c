   void img_hist(int *img,int m,int n,int binnum,int *hist)
	   //主要功能为根据图像生成直方图，img为图像，m和n分别为图像的列数和行数，binNum为bin数目，hist为直方图
   {
   int i,j;
   for(i=0;i<n;i++)           
   for(j=0;j<m;j++)
    hist[img[i*m+j]]+=1;
   }
   