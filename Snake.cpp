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

// TÍNH NĂNG ĐỒ HỌA: Di chuyển con trỏ vẽ đến vị trí (x, y)
void gotoxy(int column, int line)
{
    COORD coord = {(SHORT)column, (SHORT)line};
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
        A[0].x = 10;
        A[0].y = 10;
        A[1].x = 11;
        A[1].y = 10;
        A[2].x = 12;
        A[2].y = 10;
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
Point TaoThucAn(CONRAN &r, Point moiKhac = {-1, -1})
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
bool checkWallCollision(CONRAN &r)
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
    FILE *f = fopen("highscore.txt", "r");
    int score = 0;

    if (f != NULL)
    {
        fscanf(f, "%d", &score);
        fclose(f);
    }

    return score;
}
// Lưu lại kỷ lục
void saveHighScore(int score)
{
    FILE *f = fopen("highscore.txt", "w");

    if (f != NULL)
    {
        fprintf(f, "%d", score);
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

int main()
{
    CONRAN r;
    int Huong = 0;
    char t;

    int score = 0;
    int highScore = loadHighScore();
    bool gameOver = false;

    // Khởi tạo seed cho rand()
    srand((unsigned)time(NULL));

    Point thucAn = TaoThucAn(r);
    Point moiDacBiet = {-1, -1};
    bool coMoiDacBiet = false;
    int thoiGianMoiDacBiet = 0;
    int soMoiThuongDaAn = 0; // Biến đếm số mồi thường đã ăn

    // Vẽ khung ban đầu và rắn, thức ăn để người chơi chuẩn bị
    system("cls");
    VeKhung();
    r.Ve();
    gotoxy(thucAn.x, thucAn.y);
    cout << "#";

    while (1)
    {
        if (_kbhit())
        {
            t = _getch();
            int newHuong = Huong;
            if (t == 'a')
                newHuong = 2; // trai
            if (t == 'w')
                newHuong = 3; // len
            if (t == 'd')
                newHuong = 0; // phai
            if (t == 's')
                newHuong = 1; // xuong

            // Ngăn chặn đổi chiều 180 độ ngay lập tức
            bool opposite = (Huong == 0 && newHuong == 2) || (Huong == 2 && newHuong == 0) || (Huong == 1 && newHuong == 3) || (Huong == 3 && newHuong == 1);
            if (!opposite)
                Huong = newHuong;
        }

        int tiepTheoX = r.A[0].x;
        int tiepTheoY = r.A[0].y;
        if (Huong == 0)
            tiepTheoX++;
        if (Huong == 1)
            tiepTheoY++;
        if (Huong == 2)
            tiepTheoX--;
        if (Huong == 3)
            tiepTheoY--;
        bool daAnMoi = false;

        // KIỂM TRA ĂN MỒI BÌNH THƯỜNG
        if (tiepTheoX == thucAn.x && tiepTheoY == thucAn.y)
        {
            score += 1;
            r.MocDai();
            thucAn = TaoThucAn(r, moiDacBiet);

            soMoiThuongDaAn++; //  Cứ ăn 1 mồi thường thì cộng biến đếm lên 1
        }

        // KIỂM TRA ĂN MỒI ĐẶC BIỆT
        if (coMoiDacBiet && tiepTheoX == moiDacBiet.x && tiepTheoY == moiDacBiet.y)
        {
            score += 5; // Mồi đặc biệt cho nhiều điểm hơn
            r.MocDai();
            daAnMoi = true;
            coMoiDacBiet = false;
            gotoxy(moiDacBiet.x, moiDacBiet.y);
            cout << " ";
        }

        // KIỂM TRA ĐIỀU KIỆN ĐỂ SINH MỒI ĐẶC BIỆT (thay vì random như cũ)
        if (!coMoiDacBiet && soMoiThuongDaAn >= 5) // Nếu đã ăn đủ 5 mồi thường
        {
            moiDacBiet = TaoThucAn(r, thucAn);
            coMoiDacBiet = true;
            thoiGianMoiDacBiet = 50;

            soMoiThuongDaAn = 0; // Đặt lại đếm về 0 để tính cho vòng 5 con tiếp theo
        }

        if (coMoiDacBiet)
        {
            thoiGianMoiDacBiet--;
            if (thoiGianMoiDacBiet <= 0)
            {
                coMoiDacBiet = false;
                gotoxy(moiDacBiet.x, moiDacBiet.y);
                cout << " ";
            }
        }

        system("cls");

        VeKhung(); // Vẽ khung

        r.DiChuyen(Huong);
        r.Ve();

        gotoxy(thucAn.x, thucAn.y);
        cout << "#"; // Vẽ mồi thường

        if (coMoiDacBiet)
        {
            gotoxy(moiDacBiet.x, moiDacBiet.y);
            cout << "$"; // Vẽ mồi đặc biệt
        }

        // check thua
        if (checkWallCollision(r))
            gameOver = true;
        if (r.DauChamThan())
            gameOver = true;

        Sleep(300);
    }

    return 0;
}
