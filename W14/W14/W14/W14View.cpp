
// W14View.cpp: CW14View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "W14.h"
#endif

#include "W14Doc.h"
#include "W14View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CW14View

IMPLEMENT_DYNCREATE(CW14View, CView)

BEGIN_MESSAGE_MAP(CW14View, CView)
	// 标准打印命令
	ON_COMMAND(ID_START_OPEN, &CW14View::OnStartOpen)
	ON_COMMAND(ID_START_GRAY, &CW14View::OnStartGray)
	ON_COMMAND(ID_START_DETECTTHRESHOLD, &CW14View::OnStartDetectthreshold)
	ON_COMMAND(ID_START_OPENOPERATOR, &CW14View::OnStartOpenoperator)
	ON_COMMAND(ID_START_DILATION, &CW14View::OnStartDilation)
	ON_COMMAND(ID_START_EROSION, &CW14View::OnStartErosion)
	ON_COMMAND(ID_START_CLOSE, &CW14View::OnStartClose)
	ON_COMMAND(ID_START_INNEREDGEFORBINARY, &CW14View::OnStartInneredgeforbinary)
	ON_COMMAND(ID_START_BINARYOUTEREDGE, &CW14View::OnStartBinaryouteredge)
	ON_COMMAND(ID_START_MORPHCONTOUR, &CW14View::OnStartMorphcontour)
	ON_COMMAND(ID_START_HITMISSTHINNING, &CW14View::OnStartHitmissthinning)
END_MESSAGE_MAP()

// CW14View 构造/析构

CW14View::CW14View()
{
	// TODO: 在此处添加构造代码

}

CW14View::~CW14View()
{
}

BOOL CW14View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CW14View 绘图

void CW14View::OnDraw(CDC* pDC)
{
	CW14Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	mybmp.Draw(pDC, CPoint(0, 0), sizeimage);

	newbmp.Draw(pDC, CPoint(sizeimage.cx + 10, 0), sizeimage);
}

// CW14View 诊断

#ifdef _DEBUG
void CW14View::AssertValid() const
{
	CView::AssertValid();
}

void CW14View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CW14Doc* CW14View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CW14Doc)));
	return (CW14Doc*)m_pDocument;
}
#endif //_DEBUG

// CW14View 消息处理程序


void CW14View::OnStartOpen()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog FileDlg(TRUE, _T("*.bmp"), "", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, "image files (*.bmp) |*.bmp|AVI files (*.avi) |*.avi|All Files (*.*)|*.*||", NULL);
	char title[] = { "Open Image" };
	FileDlg.m_ofn.lpstrTitle = title;
	CFile file;
	if (FileDlg.DoModal() == IDOK)
	{
		if (!file.Open(FileDlg.GetPathName(), CFile::modeRead))
		{
			AfxMessageBox("cannot open the file");
			return;
		}
		if (!mybmp.Read(&file))
		{
			AfxMessageBox("cannot read the file");
			return;
		}
	}

	if (mybmp.m_lpBMIH->biCompression != BI_RGB)
	{
		AfxMessageBox("Can not read compressed file.");
		return;
	}
	sizeimage = mybmp.GetDimensions();
	Invalidate();
}


void CW14View::OnStartGray()
{
	// TODO: 在此添加命令处理程序代码
	// 对图象的象素值进行变换
	// 每列
	for (int x = 0; x < sizeimage.cx; x++)
	{
		// 每行
		for (int y = 0; y < sizeimage.cy; y++)
		{
			RGBQUAD color;
			color = mybmp.GetPixel(x, y);
			//RGB图像转灰度图像 Gray = R*0.299 + G*0.587 + B*0.114
			double gray = color.rgbRed*0.299 + color.rgbGreen*0.587 + color.rgbBlue*0.114;
			color.rgbBlue = (int)gray;
			color.rgbGreen = (int)gray;
			color.rgbRed = (int)gray;
			mybmp.WritePixel(x, y, color);
		}
	}
	Invalidate();
}


void CW14View::OnStartDetectthreshold()
{
	// TODO: 在此添加命令处理程序代码
	//图象的长宽大小
	int nWidth = sizeimage.cx;
	int nHeight = sizeimage.cy;

	//1,初始化
	double	avg = 0.0;// 图象的平均值
	for (int y = 0; y<nHeight; y++)
	{
		for (int x = 0; x<nWidth; x++)
		{
			RGBQUAD color;
			color = mybmp.GetPixel(x, y);
			avg += color.rgbBlue;
		}
	}
	double T = 0;
	T = avg / (nHeight * nWidth);//选择一个初始化的阈值T (通常取灰度值的平均值)


								 //2,使用阈值T 将图像的像素分为两部分: G1包含灰度满足大于T， G2包含灰度满足小于T
								 //  计算G1中所有像素的均值μ1，以及G2中所有像素的均值μ2
	double curThd = T;
	double preThd = curThd;
	do
	{
		preThd = curThd;
		double u1 = 0, u2 = 0;
		int num_u1 = 0, num_u2 = 0;
		for (int y = 0; y<nHeight; y++)
		{
			for (int x = 0; x<nWidth; x++)
			{
				RGBQUAD color;
				color = mybmp.GetPixel(x, y);
				if (color.rgbBlue < preThd)
				{
					u1 += color.rgbBlue;
					num_u1++;
				}
				else
				{
					u2 += color.rgbBlue;
					num_u2++;
				}
			}
		}
		curThd = (u1 / num_u1 + u2 / num_u2) / 2;
	} while (fabs(curThd - preThd) > 1.0);

	for (int y = 0; y<nHeight; y++)
	{
		// x方向
		for (int x = 0; x<nWidth; x++)
		{
			RGBQUAD color;
			color = mybmp.GetPixel(x, y);
			if (color.rgbBlue < curThd)
			{
				color.rgbBlue = 0;
				color.rgbGreen = 0;
				color.rgbRed = 0;
			}
			else
			{
				color.rgbBlue = 255;
				color.rgbGreen = 255;
				color.rgbRed = 255;
			}
			mybmp.WritePixel(x, y, color);
		}
	}
	Invalidate();
}


void CW14View::OnStartOpenoperator()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	CDib tmpbmp;
	tmpbmp.CreateCDib(sizeimage, 24);

	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;

	//使用自定义的结构元素进行腐蚀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//目标图像当前点置为黑色
			color.rgbBlue = (unsigned char)0;
			color.rgbGreen = (unsigned char)0;
			color.rgbRed = (unsigned char)0;
			tmpbmp.WritePixel(i, j, color);

			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 255)
					{
						color.rgbBlue = (unsigned char)255;
						color.rgbGreen = (unsigned char)255;
						color.rgbRed = (unsigned char)255;
						tmpbmp.WritePixel(i, j, color);
						break;
					}
				}
		}

	//使用自定义的结构元素进行膨胀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = tmpbmp.GetPixel(i, j);

			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = tmpbmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 0)
					{
						color.rgbBlue = (unsigned char)0;
						color.rgbGreen = (unsigned char)0;
						color.rgbRed = (unsigned char)0;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}
	Invalidate();

}


void CW14View::OnStartDilation()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
	// 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 0)
					{
						color.rgbBlue = (unsigned char)0;
						color.rgbGreen = (unsigned char)0;
						color.rgbRed = (unsigned char)0;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}
	Invalidate();
}


void CW14View::OnStartErosion()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;

	//使用自定义的结构元素进行腐蚀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//目标图像当前点置为黑色
			color.rgbBlue = (unsigned char)0;
			color.rgbGreen = (unsigned char)0;
			color.rgbRed = (unsigned char)0;
			newbmp.WritePixel(i, j, color);

			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 255)
					{
						color.rgbBlue = (unsigned char)255;
						color.rgbGreen = (unsigned char)255;
						color.rgbRed = (unsigned char)255;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}
	Invalidate();

}


void CW14View::OnStartClose()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	CDib tmpbmp;
	tmpbmp.CreateCDib(sizeimage, 24);

	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;

	//使用自定义的结构元素进行膨胀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 0)
					{
						color.rgbBlue = (unsigned char)0;
						color.rgbGreen = (unsigned char)0;
						color.rgbRed = (unsigned char)0;
						tmpbmp.WritePixel(i, j, color);
						break;
					}
				}
		}

	//使用自定义的结构元素进行腐蚀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = tmpbmp.GetPixel(i, j);

			//目标图像当前点置为黑色
			color.rgbBlue = (unsigned char)0;
			color.rgbGreen = (unsigned char)0;
			color.rgbRed = (unsigned char)0;
			newbmp.WritePixel(i, j, color);

			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = tmpbmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 255)
					{
						color.rgbBlue = (unsigned char)255;
						color.rgbGreen = (unsigned char)255;
						color.rgbRed = (unsigned char)255;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}
	Invalidate();
}


void CW14View::OnStartInneredgeforbinary()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;

	//使用自定义的结构元素进行腐蚀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//目标图像当前点置为黑色
			color.rgbBlue = (unsigned char)0;
			color.rgbGreen = (unsigned char)0;
			color.rgbRed = (unsigned char)0;
			newbmp.WritePixel(i, j, color);

			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 255)
					{
						color.rgbBlue = (unsigned char)255;
						color.rgbGreen = (unsigned char)255;
						color.rgbRed = (unsigned char)255;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}
	//原始数据减去腐蚀结果
	for (int j = 0; j < sizeimage.cy; j++)
		for (int i = 0; i < sizeimage.cx; i++)
		{
			RGBQUAD colorold, colornew, color;
			colorold = mybmp.GetPixel(i, j);
			colornew = newbmp.GetPixel(i, j);
			color.rgbBlue = (unsigned char)(colornew.rgbBlue - colorold.rgbBlue);
			color.rgbGreen = color.rgbBlue;
			color.rgbRed = color.rgbBlue;
			newbmp.WritePixel(i, j, color);
		}
	Invalidate();
}


void CW14View::OnStartBinaryouteredge()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边
	// 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 0)
					{
						color.rgbBlue = (unsigned char)0;
						color.rgbGreen = (unsigned char)0;
						color.rgbRed = (unsigned char)0;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}

	//膨胀结果减去原始数据
	for (int j = 0; j < sizeimage.cy; j++)
		for (int i = 0; i < sizeimage.cx; i++)
		{
			RGBQUAD colorold, colornew, color;
			colorold = mybmp.GetPixel(i, j);
			colornew = newbmp.GetPixel(i, j);
			color.rgbBlue = (unsigned char)(colorold.rgbBlue - colornew.rgbBlue);
			color.rgbGreen = color.rgbBlue;
			color.rgbRed = color.rgbBlue;
			newbmp.WritePixel(i, j, color);
		}

	Invalidate();
}


void CW14View::OnStartMorphcontour()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	CDib tmpbmp;
	tmpbmp.CreateCDib(sizeimage, 24);

	int structure[3][3];
	/*CMorphDlg dlgPara;
	dlgPara.DoModal();
	structure[0][0] = dlgPara.m_nStructure1;
	structure[0][1] = dlgPara.m_nStructure2;
	structure[0][2] = dlgPara.m_nStructure3;
	structure[1][0] = dlgPara.m_nStructure4;
	structure[1][1] = dlgPara.m_nStructure5;
	structure[1][2] = dlgPara.m_nStructure6;
	structure[2][0] = dlgPara.m_nStructure7;
	structure[2][1] = dlgPara.m_nStructure8;
	structure[2][2] = dlgPara.m_nStructure9;*/
	structure[0][0] = 1;
	structure[0][1] = 1;
	structure[0][2] = 1;
	structure[1][0] = 1;
	structure[1][1] = 1;
	structure[1][2] = 1;
	structure[2][0] = 1;
	structure[2][1] = 1;
	structure[2][2] = 1;

	//使用自定义的结构元素进行腐蚀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边 的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像倒数第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//目标图像当前点置为黑色
			color.rgbBlue = (unsigned char)0;
			color.rgbGreen = (unsigned char)0;
			color.rgbRed = (unsigned char)0;
			tmpbmp.WritePixel(i, j, color);

			//如果原图像中对应结构元素中为黑色的那些点中有一个不是黑色，
			//则将目标图像中的当前点赋成白色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 255)
					{
						color.rgbBlue = (unsigned char)255;
						color.rgbGreen = (unsigned char)255;
						color.rgbRed = (unsigned char)255;
						tmpbmp.WritePixel(i, j, color);
						break;
					}
				}
		}

	//使用自定义的结构元素进行膨胀
	// 由于使用3×3的结构元素，为防止越界，所以不处理最左边和最右边的两列像素和最上边和最下边的两列像素
	for (int j = 1; j <sizeimage.cy - 1; j++)
		for (int i = 1; i <sizeimage.cx - 1; i++)
		{
			// 指向源图像第j行，第i个象素的指针		
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);

			//原图像中对应结构元素中为黑色的那些点中只要有一个是黑色，
			//则将目标图像中的当前点赋成黑色
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
				{
					if (structure[m][n] == -1)
						continue;

					color = mybmp.GetPixel(i + n - 1, j + m - 1);
					if ((int)color.rgbBlue == 0)
					{
						color.rgbBlue = (unsigned char)0;
						color.rgbGreen = (unsigned char)0;
						color.rgbRed = (unsigned char)0;
						newbmp.WritePixel(i, j, color);
						break;
					}
				}
		}

	//腐蚀结果减膨胀结果
	for (int j = 0; j < sizeimage.cy; j++)
		for (int i = 0; i < sizeimage.cx; i++)
		{
			RGBQUAD colorold, colornew, color;
			colorold = tmpbmp.GetPixel(i, j);
			colornew = newbmp.GetPixel(i, j);
			color.rgbBlue = (unsigned char)(colorold.rgbBlue - colornew.rgbBlue);
			color.rgbGreen = color.rgbBlue;
			color.rgbRed = color.rgbBlue;
			newbmp.WritePixel(i, j, color);
		}

	Invalidate();
}


void CW14View::OnStartHitmissthinning()
{
	// TODO: 在此添加命令处理程序代码
	newbmp.CreateCDib(sizeimage, 24);
	for (int j = 0; j <sizeimage.cy; j++)
		for (int i = 0; i <sizeimage.cx; i++)
		{
			RGBQUAD color;
			color = mybmp.GetPixel(i, j);
			newbmp.WritePixel(i, j, color);
		}
	unsigned char nCount = 0;	//计数器	

	bool bModified = true;
	while (bModified)
	{
		bModified = false;

		for (int j = 2; j <sizeimage.cy - 2; j++)
			for (int i = 2; i <sizeimage.cx - 2; i++)
			{
				//四个条件
				bool bCondition1 = false;
				bool bCondition2 = false;
				bool bCondition3 = false;
				bool bCondition4 = false;

				//由于使用5×5的结构元素，为防止越界，所以不处理外围的几行和几列像素

				// 指向源图像第j行，第i个象素的指针		
				RGBQUAD color;
				color = newbmp.GetPixel(i, j);

				//目标图像中含有0和255外的其它灰度值
				if (color.rgbBlue != 255 && color.rgbBlue != 0)
				{
					continue;
				}
				if (color.rgbBlue == (unsigned char)255)
				{
					continue;
				}

				int neighbour[5][5] = { 0 };
				//获得当前点相邻的5×5区域内像素值，白色用0代表，黑色用1代表
				for (int m = 0; m < 5; m++)
					for (int n = 0; n < 5; n++)
					{
						color = newbmp.GetPixel(i + n - 2, j + m - 2);
						neighbour[m][n] = (255 - (unsigned char)color.rgbBlue) / 255;
					}

				//逐个判断条件。
				//判断2<=NZ(P1)<=6
				nCount = neighbour[1][1] + neighbour[1][2] + neighbour[1][3] \
					+ neighbour[2][1] + neighbour[2][3] + \
					+ neighbour[3][1] + neighbour[3][2] + neighbour[3][3];
				if (nCount >= 2 && nCount <= 6)
				{
					bCondition1 = true;
				}

				//判断Z0(P1)=1
				nCount = 0;
				if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
					nCount++;
				if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
					nCount++;
				if (neighbour[2][1] == 0 && neighbour[3][1] == 1)
					nCount++;
				if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
					nCount++;
				if (neighbour[3][2] == 0 && neighbour[3][3] == 1)
					nCount++;
				if (neighbour[3][3] == 0 && neighbour[2][3] == 1)
					nCount++;
				if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
					nCount++;
				if (neighbour[1][3] == 0 && neighbour[1][2] == 1)
					nCount++;
				if (nCount == 1)
					bCondition2 = true;

				//判断P2*P4*P8=0 or Z0(p2)!=1
				if (neighbour[1][2] * neighbour[2][1] * neighbour[2][3] == 0)
				{
					bCondition3 = true;
				}
				else
				{
					nCount = 0;
					if (neighbour[0][2] == 0 && neighbour[0][1] == 1)
						nCount++;
					if (neighbour[0][1] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (neighbour[1][1] == 0 && neighbour[2][1] == 1)
						nCount++;
					if (neighbour[2][1] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[2][3] == 1)
						nCount++;
					if (neighbour[2][3] == 0 && neighbour[1][3] == 1)
						nCount++;
					if (neighbour[1][3] == 0 && neighbour[0][3] == 1)
						nCount++;
					if (neighbour[0][3] == 0 && neighbour[0][2] == 1)
						nCount++;
					if (nCount != 1)
						bCondition3 = true;
				}

				//判断P2*P4*P6=0 or Z0(p4)!=1
				if (neighbour[1][2] * neighbour[2][1] * neighbour[3][2] == 0)
				{
					bCondition4 = true;
				}
				else
				{
					nCount = 0;
					if (neighbour[1][1] == 0 && neighbour[1][0] == 1)
						nCount++;
					if (neighbour[1][0] == 0 && neighbour[2][0] == 1)
						nCount++;
					if (neighbour[2][0] == 0 && neighbour[3][0] == 1)
						nCount++;
					if (neighbour[3][0] == 0 && neighbour[3][1] == 1)
						nCount++;
					if (neighbour[3][1] == 0 && neighbour[3][2] == 1)
						nCount++;
					if (neighbour[3][2] == 0 && neighbour[2][2] == 1)
						nCount++;
					if (neighbour[2][2] == 0 && neighbour[1][2] == 1)
						nCount++;
					if (neighbour[1][2] == 0 && neighbour[1][1] == 1)
						nCount++;
					if (nCount != 1)
						bCondition4 = true;
				}
				if (bCondition1 && bCondition2 && bCondition3 && bCondition4)
				{
					color.rgbBlue = (unsigned char)255;
					color.rgbGreen = (unsigned char)255;
					color.rgbRed = (unsigned char)255;
					newbmp.WritePixel(i, j, color);
					bModified = true;
				}
				else
				{
					color.rgbBlue = (unsigned char)0;
					color.rgbGreen = (unsigned char)0;
					color.rgbRed = (unsigned char)0;
					newbmp.WritePixel(i, j, color);
				}
			}
	}
	Invalidate();
}

