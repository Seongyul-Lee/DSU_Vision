
// DSU_VisionView.h: CDSUVisionView 클래스의 인터페이스
//

#pragma once

#include <opencv2/videoio.hpp>

class CDSUVisionView : public CView
{
protected: // serialization에서만 만들어집니다.
	CDSUVisionView() noexcept;
	DECLARE_DYNCREATE(CDSUVisionView)

// 특성입니다.
public:
	CDSUVisionDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CDSUVisionView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCameraOutput();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

private:
	cv::VideoCapture m_cap;
	cv::Mat m_frame;
	bool m_bCamera;

	void DrawCameraFrame(CDC* pDC);
};

#ifndef _DEBUG  // DSU_VisionView.cpp의 디버그 버전
inline CDSUVisionDoc* CDSUVisionView::GetDocument() const
   { return reinterpret_cast<CDSUVisionDoc*>(m_pDocument); }
#endif

