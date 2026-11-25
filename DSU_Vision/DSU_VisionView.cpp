
// DSU_VisionView.cpp: CDSUVisionView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "DSU_Vision.h"
#endif

#include "DSU_VisionDoc.h"
#include "DSU_VisionView.h"

using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDSUVisionView

IMPLEMENT_DYNCREATE(CDSUVisionView, CView)

static const UINT_PTR TIMER_CAMERA = 1;
static const int CAMERA_INDEX = 1;

BEGIN_MESSAGE_MAP(CDSUVisionView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_CAMERA_OUTPUT, &CDSUVisionView::OnCameraOutput)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDSUVisionView 생성/소멸

CDSUVisionView::CDSUVisionView() noexcept
	: m_bCamera(false)
{
}

CDSUVisionView::~CDSUVisionView()
{
}

BOOL CDSUVisionView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CDSUVisionView 그리기

void CDSUVisionView::OnDraw(CDC* pDC)
{
	CDSUVisionDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (m_bCamera)
	{
		DrawCameraFrame(pDC);
		return;
	}

    if (pDoc->m_in_img.empty())
    {
        if (!pDoc->m_strFilePath.IsEmpty())
        {
            string strPath = CT2A(pDoc->m_strFilePath);
            pDoc->m_in_img = imread(strPath, IMREAD_COLOR);
            pDoc->m_out_img = pDoc->m_in_img.clone();
        }
    }

    if (pDoc->m_in_img.empty())
        return;

    int width = pDoc->m_in_img.cols;
    int height = pDoc->m_in_img.rows;

    // 원본 이미지 버퍼 (항상 BGR 3채널)
    Mat inBgr;
    if (pDoc->m_in_img.channels() == 1)
        cvtColor(pDoc->m_in_img, inBgr, COLOR_GRAY2BGR);
    else
        inBgr = pDoc->m_in_img;

    int rowSize = (width * 3 + 3) & ~3;

    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 24;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    vector<BYTE> bufferIn(static_cast<size_t>(rowSize) * height);
    for (int y = 0; y < height; y++)
        memcpy(&bufferIn[y * rowSize], inBgr.ptr(y), width * 3);

    if (pDoc->m_bProcess == FALSE)
    {
        StretchDIBits(pDC->GetSafeHdc(),
            100, 100, width, height,
            0, 0, width, height,
            bufferIn.data(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }
    else
    {
        // 원본 (왼쪽)
        StretchDIBits(pDC->GetSafeHdc(),
            100, 100, width, height,
            0, 0, width, height,
            bufferIn.data(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

        // 처리 결과 (오른쪽) - m_out_img를 BGR로 변환 후 출력
        Mat outBgr;
        if (pDoc->m_out_img.channels() == 1)
            cvtColor(pDoc->m_out_img, outBgr, COLOR_GRAY2BGR);
        else
            outBgr = pDoc->m_out_img;

        vector<BYTE> bufferOut(static_cast<size_t>(rowSize) * height);
        for (int y = 0; y < height; y++)
            memcpy(&bufferOut[y * rowSize], outBgr.ptr(y), width * 3);

        StretchDIBits(pDC->GetSafeHdc(),
            width + 200, 100, width, height,
            0, 0, width, height,
            bufferOut.data(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }
}


// CDSUVisionView 인쇄

BOOL CDSUVisionView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CDSUVisionView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CDSUVisionView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CDSUVisionView 진단

#ifdef _DEBUG
void CDSUVisionView::AssertValid() const
{
	CView::AssertValid();
}

void CDSUVisionView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDSUVisionDoc* CDSUVisionView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDSUVisionDoc)));
	return (CDSUVisionDoc*)m_pDocument;
}
#endif //_DEBUG


// CDSUVisionView 메시지 처리기

void CDSUVisionView::OnCameraOutput()
{
	if (m_bCamera)
	{
		KillTimer(TIMER_CAMERA);
		m_cap.release();
		m_frame.release();
		m_bCamera = false;
	}
	else
	{
		if (!m_cap.open(CAMERA_INDEX, CAP_DSHOW))
		{
			AfxMessageBox(_T("카메라를 열 수 없습니다."));
			return;
		}
		m_cap.set(CAP_PROP_FRAME_WIDTH, 640);
		m_cap.set(CAP_PROP_FRAME_HEIGHT, 480);
		m_bCamera = true;
		SetTimer(TIMER_CAMERA, 33, NULL);
	}
	Invalidate(FALSE);
}

void CDSUVisionView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_CAMERA && m_cap.isOpened())
	{
		m_cap >> m_frame;
		Invalidate(FALSE);
	}
	CView::OnTimer(nIDEvent);
}

void CDSUVisionView::OnDestroy()
{
	if (m_cap.isOpened())
	{
		KillTimer(TIMER_CAMERA);
		m_cap.release();
	}
	CView::OnDestroy();
}

void CDSUVisionView::DrawCameraFrame(CDC* pDC)
{
	if (m_frame.empty())
		return;

	int width = m_frame.cols;
	int height = m_frame.rows;
	int rowSize = (width * 3 + 3) & ~3;

	BITMAPINFO bitmapInfo = {};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = -height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	vector<BYTE> buffer(static_cast<size_t>(rowSize) * height);
	for (int y = 0; y < height; y++)
		memcpy(&buffer[y * rowSize], m_frame.ptr(y), width * 3);

	StretchDIBits(pDC->GetSafeHdc(),
		100, 100, width, height,
		0, 0, width, height,
		buffer.data(), &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}
