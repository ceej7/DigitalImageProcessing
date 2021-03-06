
// W15View.h: CW15View 类的接口
//

#pragma once
#include"CDib.h"

class CW15View : public CView
{
protected: // 仅从序列化创建
	CW15View();
	DECLARE_DYNCREATE(CW15View)

// 特性
public:
	CW15Doc* GetDocument() const;
	CDib mybmp;
	CDib newbmp;
	CSize sizeimage;
	int nWidth, nHeight;


// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CW15View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStartGray();
	afx_msg void OnStartReverse();
	afx_msg void OnStartMosaic();
	afx_msg void OnStartEmbossment();
	afx_msg void OnStartHierarchy();
	afx_msg void OnStartGraytrans();
	afx_msg void OnOpen();
};

#ifndef _DEBUG  // W15View.cpp 中的调试版本
inline CW15Doc* CW15View::GetDocument() const
   { return reinterpret_cast<CW15Doc*>(m_pDocument); }
#endif

