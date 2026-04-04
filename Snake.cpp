#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <conio.h>
#include <ctime>
using namespace std;

// Kích thước cố định của màn hình chơi
const int WIDTH = 40;
const int HEIGHT = 20;
const int TG = 300; // thời gian mỗi khung (ms)
const int TG_MOI = 7000; // mồi đặc biệt tồn tại 7 giây (ms)
const int DM_MAX = 10; // điểm tối đa mồi đặc biệt
const int DM_MIN = 5;  // điểm tối thiểu mồi đặc biệt

// TÍNH NĂNG ĐỒ HỌA: Di chuyển con trỏ vẽ đến vị trí (x, y)
void gotoxy(int column, int line)
{
    COORD coord = { (SHORT)column, (SHORT)line };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct Point
{
    int x, y;
};

class CONRAN
{
public:
    struct Point A[100];
    int DoDai;
    CONRAN()
    {
        DoDai = 3;
        A[0].x = 12; A[0].y = 10; // Đầu nằm ở 12
        A[1].x = 11; A[1].y = 10; // Thân ở 11
        A[2].x = 10; A[2].y = 10; // Đuôi ở 10
    }
    void Ve()
    {
        for (int i = 0; i < DoDai; i++)
        {
            gotoxy(A[i].x, A[i].y);
            cout << (i == 0 ? "O" : "x"); // Đầu là O, thân là x
        }
    }
    void XoaDuoi()
    {
        gotoxy(A[DoDai - 1].x, A[DoDai - 1].y);
        cout << " ";
    }
    void DiChuyen(int Huong)
    {
        XoaDuoi();
        for (int i = DoDai - 1; i > 0; i--)
            A[i] = A[i - 1];
        if (Huong == 0)
            A[0].x = A[0].x + 1;
        if (Huong == 1)
            A[0].y = A[0].y + 1;
        if (Huong == 2)
            A[0].x = A[0].x - 1;
        if (Huong == 3)
            A[0].y = A[0].y - 1;
    }
    // Kiểm tra xem một điểm p có nằm đè lên người con rắn không. Mục đích: Tránh spawn mồi vào thân con rắn.
    bool TrungVaoThan(Point p)
    {
        for (int i = 0; i < DoDai; i++)
            if (A[i].x == p.x && A[i].y == p.y)
                return true;
        return false;
    }

    // TÍNH NĂNG VA CHẠM: Kiểm tra đầu đâm vào thân
    bool DauChamThan()
    {
        for (int i = 1; i < DoDai; i++)
            if (A[0].x == A[i].x && A[0].y == A[i].y)
                return true;
        return false;
    }
    // Mọc dài thêm một đoạn khi ăn mồi
    void MocDai()
    {
        if (DoDai < 100)
        {
            A[DoDai] = A[DoDai - 1];
            DoDai++;
        }
    }
};

// Tạo thức ăn.
Point TaoThucAn(CONRAN& r, Point moiKhac = { -1, -1 })
{
    Point thucAn;
    bool viTriHopLe;
    do
    {
        viTriHopLe = true;
        thucAn.x = 1 + rand() % (WIDTH - 2);
        thucAn.y = 1 + rand() % (HEIGHT - 2);

        for (int i = 0; i < r.DoDai; i++)
        {
            if (r.A[i].x == thucAn.x && r.A[i].y == thucAn.y)
            {
                viTriHopLe = false;
                break;
            }
        }
        if (thucAn.x == moiKhac.x && thucAn.y == moiKhac.y)
        {
            viTriHopLe = false;
        }
    } while (viTriHopLe == false);
    return thucAn;
}

// Đụng tường -> game over
bool checkWallCollision(CONRAN& r)
{
    int x = r.A[0].x;
    int y = r.A[0].y;

    // Khu vực hợp lệ là 1..WIDTH-2 và 1..HEIGHT-2, nên va chạm xảy ra khi nằm trên biên (0 hoặc WIDTH-1 / 0 hoặc HEIGHT-1)
    if (x <= 0 || x >= WIDTH - 1 || y <= 0 || y >= HEIGHT - 1)
    {
        return true; // thua
    }
    return false;
}

// Lưu lại kỷ lục
int loadHighScore()
{
    FILE* f = NULL;
    fopen_s(&f, "highscore.txt", "r");
    int score = 0;

    if (f != NULL)
    {
        fscanf_s(f, "%d", &score);
        fclose(f);
    }

    return score;
}
// Lưu lại kỷ lục
void saveHighScore(int score)
{
    FILE* f = NULL;
    fopen_s(&f, "highscore.txt", "w");

    if (f != NULL)
    {
        fprintf_s(f, "%d", score);
        fclose(f);
    }
}

// Vẽ khung cho trò chơi
void VeKhung()
{

    // viền trên + dưới
    for (int x = 0; x < WIDTH; x++)
    {
        gotoxy(x, 0);
        cout << "#";

        gotoxy(x, HEIGHT - 1);
        cout << "#";
    }

    // viền trái + phải
    for (int y = 0; y < HEIGHT; y++)
    {
        gotoxy(0, y);
        cout << "#";

        gotoxy(WIDTH - 1, y);
        cout << "#";
    }
}

// Hướng dẫn chơi game
void HuongDanChoi() {
    cout << "=============================\n";
    cout << "        SNAKE GAME\n";
    cout << "=============================\n\n";

    cout << "Huong dan choi:\n";
    cout << "- Dieu khien ran bang cac phim:\n";
    cout << "  W: Len\n";
    cout << "  S: Xuong\n";
    cout << "  A: Trai\n";
    cout << "  D: Phai\n\n";

    cout << "- An thuc an de tang diem.\n";
    cout << "- Moi lan an, ran se dai ra.\n";
    cout << "- Se co cac thuc an dac biet va co thoi gian bien mat. \n";
    cout << "- Khong duoc dam vao tuong.\n";
    cout << "- Khong duoc tu can vao than minh.\n\n";

    cout << "Game se ket thuc khi ban dam vao tuong hoac can chinh ban than.\n\n";
    cout << "Chuc ban choi game vui ve!\n\n";

    cout << "Nhan phim bat ky de bat dau...";
	_getch(); // Dùng cái này thay cho cin.get() bị lỗi không ấn phím awsd được
}

// Hàm ẩn con trỏ chuột để không bị nhấp nháy trên màn hình
void NhanDinhConTro(bool showFlag) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.dwSize = 10;
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

int main()
{
    // Khởi tạo hệ thống
    NhanDinhConTro(false); // Ẩn con trỏ chuột
    srand((unsigned)time(NULL));
    int highScore = loadHighScore();
    HuongDanChoi();
    cout << "\n\nGame bat dau:";

	// Vòng lặp chính của trò chơi, cho phép chơi lại sau khi kết thúc
    while (true)
    {
        system("cls");

        // Khởi tạo lại thông số ván mới
        system("cls");
        CONRAN r;
        int Huong = 0; // Thay đổi hướng của rắn
        char t;


        int score = 0;
        bool gameOver = false;

        Point thucAn = TaoThucAn(r);
        Point moiDacBiet = { -1, -1 };
        bool coMoiDacBiet = false;
        int thoiGianMoiDacBiet = 0;
        int soMoiThuongDaAn = 0; // Biến đếm số mồi thường đã ăn
        int tocDoHienTai = 300; // Tốc độ ban đầu (ms)
        const int TOC_DO_TOI_DA = 50;


        // Vẽ khung ban đầu và rắn, thức ăn để người chơi chuẩn bị
        VeKhung();
        r.Ve();
        gotoxy(thucAn.x, thucAn.y);
        cout << "@";

        while (!gameOver)
        {
            // Kiểm tra phím bấm
            if (_kbhit())
            {
                t = _getch();
                int newHuong = Huong;
                if (t == 'a') newHuong = 2; // trai
                if (t == 'w') newHuong = 3; // len
                if (t == 'd') newHuong = 0; // phai
                if (t == 's') newHuong = 1; // xuong

                // Chặn quay xe 180 độ
                bool opposite = (Huong == 0 && newHuong == 2) || (Huong == 2 && newHuong == 0) ||
                    (Huong == 1 && newHuong == 3) || (Huong == 3 && newHuong == 1);
                if (!opposite) Huong = newHuong;
            }

            // Tính toán vị trí dự kiến
            int tiepTheoX = r.A[0].x;
            int tiepTheoY = r.A[0].y;
            if (Huong == 0) tiepTheoX++;
            if (Huong == 1) tiepTheoY++;
            if (Huong == 2) tiepTheoX--;
            if (Huong == 3) tiepTheoY--;

            // Xóa cái đuôi cũ trước
            r.XoaDuoi();

            // Xử lý ăn mồi thường
            if (tiepTheoX == thucAn.x && tiepTheoY == thucAn.y)
            {
                score += 1;
                r.MocDai();

                if (tocDoHienTai > TOC_DO_TOI_DA) {
                    tocDoHienTai -= 10;
                }

                thucAn = TaoThucAn(r, moiDacBiet);
                soMoiThuongDaAn++;
                gotoxy(thucAn.x, thucAn.y);
                cout << "@";
            }

            // Xử lý ăn mồi đặc biệt
            if (coMoiDacBiet && tiepTheoX == moiDacBiet.x && tiepTheoY == moiDacBiet.y)
            {
                int them = DM_MIN + ((DM_MAX - DM_MIN) * thoiGianMoiDacBiet) / TG_MOI;
                score += (them < DM_MIN) ? DM_MIN : them;
                r.MocDai();
                coMoiDacBiet = false;
                // Xóa ký tự mồi đặc biệt cũ
                gotoxy(moiDacBiet.x, moiDacBiet.y);
                cout << " ";
            }

            // Cập nhật tọa độ và vẽ thân rắn mới
            r.DiChuyen(Huong);
            r.Ve();

            // Logic sinh mồi đặc biệt
            if (!coMoiDacBiet && soMoiThuongDaAn >= 5)
            {
                moiDacBiet = TaoThucAn(r, thucAn);
                coMoiDacBiet = true;
                thoiGianMoiDacBiet = TG_MOI;
                soMoiThuongDaAn = 0;
                gotoxy(moiDacBiet.x, moiDacBiet.y);
                cout << "$";
            }

            if (coMoiDacBiet)
            {
                thoiGianMoiDacBiet -= tocDoHienTai;
                if (thoiGianMoiDacBiet <= 0)
                {
                    coMoiDacBiet = false;
                    gotoxy(moiDacBiet.x, moiDacBiet.y);
                    cout << " ";
                }
            }

            // Score và High Score ở dưới cùng
            gotoxy(0, HEIGHT);
            cout << "Score: " << score << "  |  High Score: " << highScore << "    ";

			// Timer mồi đặc biệt
            gotoxy(0, HEIGHT + 1);
            if (coMoiDacBiet) {
                int giayCon = (thoiGianMoiDacBiet + 999) / 1000;
                cout << "BONUS: " << (giayCon > 0 ? giayCon : 0) << "s  ";
            }
            else {
                cout << "                "; // Xóa dòng timer khi hết mồi
            }

            // check thua
            if (checkWallCollision(r))
                gameOver = true;
            if (r.DauChamThan())
                gameOver = true;
            Sleep(tocDoHienTai);
        }

        // 3. MÀN HÌNH GAME OVER & LỰA CHỌN THOÁT
        if (score > highScore) {
            highScore = score;
            saveHighScore(score);
        }

        gotoxy(WIDTH / 2 - 5, HEIGHT / 2);
        cout << "  GAME OVER!  ";
        gotoxy(WIDTH / 2 - 7, HEIGHT / 2 + 1);
        cout << "Final Score: " << score;

        gotoxy(WIDTH / 2 - 12, HEIGHT / 2 + 3);
        cout << "Nhan [R] de choi lai";
        gotoxy(WIDTH / 2 - 12, HEIGHT / 2 + 4);
        cout << "Nhan [ESC] de thoat game";

        // Vòng lặp chờ người chơi bấm phím hợp lệ
        while (true) {
            if (_kbhit()) {
                char choice = _getch();
                if (choice == 'r' || choice == 'R') {
                    break; // Thoát vòng lặp chờ, quay lại vòng lặp chơi game (while true)
                }
                if (choice == 27) { // 27 là mã ASCII của phím ESC
                    NhanDinhConTro(true);
                    return 0; // Kết thúc hàm main, thoát chương trình hoàn toàn
                }
            }
        }        
	}
    return 0;
}
