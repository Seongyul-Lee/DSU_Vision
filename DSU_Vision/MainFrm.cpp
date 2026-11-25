
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "DSU_Vision.h"

#include "MainFrm.h"
#include "DSU_VisionDoc.h"
#include "DSU_VisionView.h"

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_PIXEL_BRIGHT, &CMainFrame::OnPixelBright)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기


void CMainFrame::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("모든 파일 (*.*)|*.*|텍스트 파일 (*.txt)|*.txt||");

	// 필터 설정 (필요에 따라 확장자 변경 가능)
	CDSUVisionView* pView = (CDSUVisionView*)GetActiveView();
	CDSUVisionDoc* pDoc = pView->GetDocument();
	
	// CFileDialog(bOpenFileDialog=TRUE: 파일 열기 모드)
	CFileDialog dlg(TRUE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);

	// 대화상자를 띄우고 '확인(OK)'을 눌렀는지 체크
	if (dlg.DoModal() == IDOK)
	{
		pDoc->m_strFilePath = dlg.GetPathName();

		string strPath = CT2A(pDoc->m_strFilePath);
		pDoc->m_in_img = imread(strPath, IMREAD_COLOR);
		pDoc->m_out_img = pDoc->m_in_img.clone();
		pDoc->m_bProcess = FALSE;

		pDoc->UpdateAllViews(NULL);
	}
}

void CMainFrame::OnPixelBright()
{
	CDSUVisionView* pView = (CDSUVisionView*)GetActiveView();
	CDSUVisionDoc* pDoc = pView->GetDocument();

	if (pDoc->m_in_img.empty())
	{
		AfxMessageBox(_T("먼저 이미지를 열어주세요."));
		return;
	}

	cvtColor(pDoc->m_in_img, pDoc->m_out_img, COLOR_BGR2GRAY);

	pDoc->m_bProcess = TRUE;
	pDoc->UpdateAllViews(NULL);
}
