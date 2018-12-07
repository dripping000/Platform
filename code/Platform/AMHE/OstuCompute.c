int OstuCompute(double *hist,int binNum)
{
	int i,k;
	double sum,sum1,sum2,sb,fmax,m1,m2;
	double n,n1,n2;
	int ThreshValue;                        //otsu阈值

	n =0;
	sum = 0.0;
	sum1 = 0.0;

	for(i=0;i<binNum;i++)
	{
		sum += (double)i * (*(hist + i));
		n += *(hist + i);
	}

	fmax = - 1.0;                          
	n1 = 0;

	for(k=0;k<binNum;k++)                   //计算binNum个otsu函数，求取otsu函数最大时对应的k值
	{
		n1 += *(hist + k);
		if (n1 == 0)
			continue;
		n2 = n - n1;
		if (n2 == 0)
			break;
		sum1 += (double)k * (double)(*(hist + k));
		m1 = sum1/n1;
		sum2 = sum - sum1;
		m2 = sum2/n2;
		sb = (double)n1 * (double)n2 * (m1 - m2) * (m1 - m2);
		if(sb > fmax)
		{
			fmax = sb;
			ThreshValue = k;
		}
	}

	return ThreshValue;
}