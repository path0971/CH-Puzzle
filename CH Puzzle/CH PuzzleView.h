
// CH PuzzleView.h: CCHPuzzleView 클래스의 인터페이스
//

#pragma once


class CCHPuzzleView : public CView
{
protected: // serialization에서만 만들어집니다.
	CCHPuzzleView() noexcept;
	DECLARE_DYNCREATE(CCHPuzzleView)

// 특성입니다.
public:
	CCHPuzzleDoc* GetDocument() const;

// 작업입니다.
public:
	int state[4][4];
	Gdiplus::Rect board{ 100, 50, 206, 206 };
	Gdiplus::Image* pimage;
	Gdiplus::Bitmap* pmembitmap;
	bool isClicked = false;
	bool isMoving = false;
	CPoint loc0{ 3,3 };
	void movePiece(const CPoint& loc);

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
	virtual ~CCHPuzzleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // CH PuzzleView.cpp의 디버그 버전
inline CCHPuzzleDoc* CCHPuzzleView::GetDocument() const
   { return reinterpret_cast<CCHPuzzleDoc*>(m_pDocument); }
#endif

