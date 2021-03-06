
// W14View.h: CW14View 类的接口
//

#pragma once
#include "CDib.h"

class CW14View : public CView
{
protected: // 仅从序列化创建
	CW14View();
	DECLARE_DYNCREATE(CW14View)

// 特性
public:
	CW14Doc* GetDocument() const;

// 操作
public:
	CDib mybmp;
	CDib newbmp;
	CSize sizeimage;
	int nWidth, nHeight;

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CW14View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStartOpen();
	afx_msg void OnStartGray();
	afx_msg void OnStartDetectthreshold();
	afx_msg void OnStartOpenoperator();
	afx_msg void OnStartDilation();
	afx_msg void OnStartErosion();
	afx_msg void OnStartClose();
	afx_msg void OnStartInneredgeforbinary();
	afx_msg void OnStartBinaryouteredge();
	afx_msg void OnStartMorphcontour();
	afx_msg void OnStartHitmissthinning();
};

#ifndef _DEBUG  // W14View.cpp 中的调试版本
inline CW14Doc* CW14View::GetDocument() const
   { return reinterpret_cast<CW14Doc*>(m_pDocument); }
#endif

