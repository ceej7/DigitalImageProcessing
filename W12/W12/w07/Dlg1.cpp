// Dlg1.cpp: 实现文件
//

#include "stdafx.h"
#include "w07.h"
#include "Dlg1.h"
#include "afxdialogex.h"


// Dlg1 对话框

IMPLEMENT_DYNAMIC(Dlg1, CDialogEx)

Dlg1::Dlg1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

Dlg1::~Dlg1()
{
}

void Dlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Dlg1, CDialogEx)
	ON_BN_CLICKED(IDOK, &Dlg1::OnBnClickedOk)
END_MESSAGE_MAP()


// Dlg1 消息处理程序


void Dlg1::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit * box;
	box = (CEdit *)GetDlgItem(IDC_EDIT1);
	CString str;
	box->GetWindowText(str);
	ref1 = atof(str);
	CDialogEx::OnOK();
}
