// CH PuzzleView.cpp: CCHPuzzleView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "CH Puzzle.h"
#endif

#include "CH PuzzleDoc.h"
#include "CH PuzzleView.h"
#include <chrono>
#include <memory>

using namespace std::chrono;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCHPuzzleView
IMPLEMENT_DYNCREATE(CCHPuzzleView, CView)

BEGIN_MESSAGE_MAP(CCHPuzzleView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CCHPuzzleView 생성/소멸
CCHPuzzleView::CCHPuzzleView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// [1단계] 리소스에서 비트맵 이미지 로딩 == "리소스에서 비트맵 이미지를 불러와 Image 포인터로 저장"
	Gdiplus::Bitmap* pbitmap = Gdiplus::Bitmap::FromResource( // Bitmap::FromResource는 실행파일 내부 리소스에서 이미지를 로드
		AfxGetInstanceHandle(), (WCHAR*)MAKEINTRESOURCE(IDB_BITMAP2)); // IDB_BITMAP2는 리소스에 등록된 비트맵 ID
	pimage = dynamic_cast<Gdiplus::Image*>(pbitmap); // dynamic_cast는 Bitmap*을 상위 타입인 Image*로 안전하게 캐스팅

	// [2단계] 퍼즐 상태 초기화 == 퍼즐의 각 조각에 1~15번 번호를 할당, 마지막(3,3)은 빈칸(0)으로 설정
 	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			state[i][j] = i * 4 + j + 1;
		}
	}
	state[3][3] = 0; // 4x4 퍼즐 배열 초기화 (마지막은 빈칸 0)

	// [3단계] 메모리 비트맵 생성 및 그래픽스 객체 초기화 == 퍼즐을 그릴 메모리 비트맵과 고품질 렌더링 설정
	pmembitmap = ::new Gdiplus::Bitmap{ board.Width, board.Height };
	Gdiplus::Graphics memgraphics(pmembitmap); // Graphics 객체는 해당 비트맵에 그림을 그릴 수 있게 함
	memgraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

	// [4단계] 보드 배경을 반투명 블랙으로 채움
	Gdiplus::SolidBrush BlackBrush{ Gdiplus::Color(32, 0, 0, 0) }; // Color(32, 0, 0, 0)은 알파=32의 반투명 블랙
	memgraphics.FillRectangle(&BlackBrush,
		Gdiplus::Rect{ 0, 0, board.Width, board.Height }); // // 반투명 검은색으로 보드 전체를 채움

	// [5단계] 원본 이미지의 폭과 높이를 실수형으로 저장
	const Gdiplus::REAL tmpW = static_cast<Gdiplus::REAL>(pimage->GetWidth()),
		tmpH = static_cast<Gdiplus::REAL>(pimage->GetHeight());

	// [6단계] 퍼즐 조각 하나씩 그리기 == "state[i][j]에 따라 원본 이미지의 일부만 잘라서 그리기"
	// 각 퍼즐 조각 위치에 맞게 원본 이미지 일부를 잘라서 memgraphics에 그림
    // state[i][j]를 이용하여 원본 이미지 내의 타일 좌표 계산
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (i * j == 9) continue;
			memgraphics.DrawImage(pimage, // 잘라낼 이미지의 원본 위치: DrawImage()의 5~7번째 인자
				Gdiplus::RectF(static_cast<Gdiplus::REAL>(52) * i, // 출력 위치와 크기: RectF()
					static_cast<Gdiplus::REAL>(52) * j,
					static_cast<Gdiplus::REAL>(50), static_cast<Gdiplus::REAL>(50)),
				tmpW / board.Width * 52 * (state[i][j] / 4),
				tmpH / board.Height * 52 * (state[i][j] % 4),
				tmpW / board.Width * 50, tmpH / board.Height * 50,
				Gdiplus::UnitPixel);
		}
	}
}

CCHPuzzleView::~CCHPuzzleView()
{
}

BOOL CCHPuzzleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CCHPuzzleView 그리기
void CCHPuzzleView::OnDraw(CDC* /*pDC*/)
{
	CCHPuzzleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}

// CCHPuzzleView 인쇄
BOOL CCHPuzzleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CCHPuzzleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CCHPuzzleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

// CCHPuzzleView 진단
#ifdef _DEBUG
void CCHPuzzleView::AssertValid() const
{
	CView::AssertValid();
}

void CCHPuzzleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCHPuzzleDoc* CCHPuzzleView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCHPuzzleDoc)));
	return (CCHPuzzleDoc*)m_pDocument;
}
#endif //_DEBUG

// CCHPuzzleView 메시지 처리기
void CCHPuzzleView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CView::OnPaint()을(를) 호출하지 마십시오.
	Gdiplus::Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

	graphics.DrawImage(pmembitmap, board.X, board.Y);
}

void CCHPuzzleView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nChar)
	{
	case VK_LEFT:
		if (!isMoving && loc0.x < 3) movePiece(CPoint{ loc0.x + 1, loc0.y });
		break;
	case VK_RIGHT:
		if (!isMoving && loc0.x > 0) movePiece(CPoint{ loc0.x - 1, loc0.y });
		break;
	case VK_UP:
		if (!isMoving && loc0.y < 3) movePiece(CPoint{ loc0.x, loc0.y + 1 });
		break;
	case VK_DOWN:
		if (!isMoving && loc0.y > 0) movePiece(CPoint{ loc0.x, loc0.y - 1 });
		break;
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// (1) 따로 조각 이동은 안 일어나고, 눌렀다는 "플래그 설정"만 하는 부분
void CCHPuzzleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	isClicked = true;
	CView::OnLButtonDown(nFlags, point);
}

// (2) 마우스 왼쪽 버튼에서 손을 뗄 때! 클릭한 위치에 따라 조각을 이동
void CCHPuzzleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (isClicked && !isMoving) {
		// 핵심 로직으로, 마우스 클릭 위치(value)에서 퍼즐판 기준점 좌표를 빼고, 한 타일의 크기 52로 나눔.
		auto locInclude = [](int value, int measure) {
			if ((value - measure) / 52 >= 0 && (value - measure) / 52 <= 3)
				return (value - measure) / 52;
			else return -1;  // 보드 바깥 클릭 방지
			};
		// 두 좌표 간 거리 측정용!
		auto abs = [](int value) {
			if (value < 0) return -value;
			return value;
			};

		isClicked = false;

		// 이동 조건을 확인하는 로직
		CPoint loc{ locInclude(point.x, board.X), locInclude(point.y, board.Y) }; // 클릭된 위치가 퍼즐 그리드의 몇 번째 타일인지
		if (loc.x >= 0 && loc.y >= 0 &&  // 클릭 위치가 유효한 보드 내부인지 (>= 0) 
			abs(loc.x - loc0.x) + abs(loc.y - loc0.y) == 1 && !isMoving) // 클릭한 조각이 빈칸(loc0)과 인접한지 확인 && 조각이 안움직일때만 이동가능
			movePiece(loc);
	}
	CView::OnLButtonUp(nFlags, point);
}

void CCHPuzzleView::movePiece(const CPoint& loc)
{
	isMoving = true; // 퍼즐 조작 중으로 표시하여 다른 입력 무시하도록 설정하는 값

	Gdiplus::Graphics memgraphics(pmembitmap); // 메모리 비트맵에 그릴 그래픽 객체 생성
	memgraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gdiplus::SolidBrush WhiteBrush{ Gdiplus::Color::White };
	Gdiplus::SolidBrush BlackBrush{ Gdiplus::Color(32, 0, 0, 0) };

	// 이미지 크기 계산
	// 비율 계산용으로 REAL 타입으로 변환 == REAL은 typedef로 float을 사용!
	const Gdiplus::REAL tmpW = static_cast<Gdiplus::REAL>(pimage->GetWidth()),
		tmpH = static_cast<Gdiplus::REAL>(pimage->GetHeight()),
		tmp2_5 = static_cast<Gdiplus::REAL>(2.5);

	const int n = 13;
	int m = 4;
	// 빈칸 타일이 그려질 시작 위치와 크기 초기화!
	/*Gdiplus::RectF moverect { static_cast<Gdiplus::REAL>(52) * loc.x,
		static_cast<Gdiplus::REAL>(52) * loc.y,
		static_cast<Gdiplus::REAL>(50),
		static_cast<Gdiplus::REAL>(50) };*/
	Gdiplus::RectF moverect{
	52.0f * loc.x + 1.0f,  // 약간의 여유
	52.0f * loc.y + 1.0f,
	50.0f,
	50.0f
	};

	while (m != 0) {
		// 60ms 대기
		system_clock::time_point tp = system_clock::now();
		while (duration_cast<milliseconds>(system_clock::now() - tp)
			<= static_cast<milliseconds>(60));
		--m;  // m을 4로 초기화했으므로 총 4번 반복되어 각 반복마다 60ms 대기 -> 프레임 간 딜레이

		// 기존 위치 잔상 제거 (흰색 → 반투명 검정으로 덮기) == "매우 필수임!!"
		/* 잔상 제거를 안하면? 
		이전 프레임의 내용이 메모리상에 남아있는 상태에서 위에만 덮는 식이 되기 때문에
		1. 움직이는 타일 경로가 줄처럼 남음
		2. 특히 system_clock으로 60ms씩 지연하면서 그리면, 눈에 보일 정도로 잔상이 선명하게 생김
		3. 결과적으로 애니메이션이 아니라 "오염된 화면"처럼 보임
		*/
		// 그러므로, FillRectangle() 구문은 슬라이딩 애니메이션의 필수 전제조건
		memgraphics.FillRectangle(&WhiteBrush,
			Gdiplus::RectF{ moverect.X - (2 * (loc.x - loc0.x)) - tmp2_5,
			moverect.Y - (2 * (loc.y - loc0.y)) - tmp2_5,
			moverect.Width + (2 * (loc.x - loc0.x)) + tmp2_5,
			moverect.Height + (2 * (loc.y - loc0.y)) + tmp2_5 });
		memgraphics.FillRectangle(&BlackBrush,
			Gdiplus::RectF{ moverect.X - (2 * (loc.x - loc0.x)) - tmp2_5,
			moverect.Y - (2 * (loc.y - loc0.y)) - tmp2_5,
			moverect.Width + (2 * (loc.x - loc0.x)) + tmp2_5,
			moverect.Height + (2 * (loc.y - loc0.y)) + tmp2_5 });
		/*Gdiplus::REAL padding = 15.0f;
		Gdiplus::RectF eraseRect{
			moverect.X - padding,
			moverect.Y - padding,
			moverect.Width + 2 * padding,
			moverect.Height + 2 * padding
		};

		memgraphics.FillRectangle(&WhiteBrush, eraseRect);
		memgraphics.FillRectangle(&BlackBrush, eraseRect);*/
		// X, Y 방향으로 13픽셀씩 이동 + 방향은 loc(클릭 위치)와 loc0(빈칸 위치) 차이로 결정
		moverect.X -= (n * (loc.x - loc0.x));
		moverect.Y -= (n * (loc.y - loc0.y));

		// DrawImage()로 퍼즐 조각의 해당 부분을 잘라 moverect 위치에 그림
		// 원본 이미지에서의 타일 위치 계산: 행은 state / 4, 열은 state % 4
		/*memgraphics.DrawImage(pimage,
			moverect,
			tmpW / board.Width * 52 * (state[loc.x][loc.y] / 4),
			tmpH / board.Height * 52 * (state[loc.x][loc.y] % 4),
			tmpW / board.Width * 50, tmpH / board.Height * 50, Gdiplus::UnitPixel);*/
			// 1. 타일 번호에서 행, 열 좌표 구하기
		// 코드 내에서 실수 오차 없이 정확한 타일 위치 계산
		int tileWidth = 186;
		int tileHeight = 186;

		int tile = state[loc.x][loc.y];
		int row = tile / 4;
		int col = tile % 4;

		memgraphics.DrawImage(pimage,
			moverect,
			col * tileWidth,
			row * tileHeight,
			tileWidth,
			tileHeight,
			Gdiplus::UnitPixel);



		// 그려진 비트맵을 윈도우에 반영
		RedrawWindow();
	}

	// state와 loc0 갱신 == 빈칸(loc0)과 클릭된 조각(loc)의 위치 값을 XOR 연산으로 교환
	state[loc0.x][loc0.y] ^= state[loc.x][loc.y];
	state[loc.x][loc.y] ^= state[loc0.x][loc0.y];
	state[loc0.x][loc0.y] ^= state[loc.x][loc.y];
	loc0 = loc;  // 새로운 빈칸 위치는 클릭된 조각의 좌표

	isMoving = false;
}