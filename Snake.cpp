#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <conio.h>
using namespace std;

// Kích thước cố định của màn hình chơi
const int WIDTH = 40;
const int HEIGHT = 20;

// TÍNH NĂNG ĐỒ HỌA: Di chuyển con trỏ vẽ đến vị trí (x, y)
void gotoxy(int column, int line) {
    COORD coord = { (SHORT)column, (SHORT)line };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct Point{
    int x,y;
};

class CONRAN{
public:
    struct Point A[100];
    int DoDai;
    CONRAN(){
        DoDai = 3;
        A[0].x = 10; A[0].y = 10;
        A[1].x = 11; A[1].y = 10;
        A[2].x = 12; A[2].y = 10;
    }
    void Ve(){
        for (int i = 0; i < DoDai; i++){
            gotoxy(A[i].x,A[i].y);
            cout << (i == 0 ? "O" : "x"); // Đầu là O, thân là x
        }
    }
    void XoaDuoi() {
        gotoxy(A[DoDai - 1].x, A[DoDai - 1].y);
        cout << " ";
    }
    void DiChuyen(int Huong){
        XoaDuoi();
        for (int i = DoDai-1; i>0;i--)
            A[i] = A[i-1];
        if (Huong==0) A[0].x = A[0].x + 1;
        if (Huong==1) A[0].y = A[0].y + 1;
        if (Huong==2) A[0].x = A[0].x - 1;
        if (Huong==3) A[0].y = A[0].y - 1;

    }
    // Kiểm tra xem một điểm p có nằm đè lên người con rắn không. Mục đích: Tránh spawn mồi vào thân con rắn.
    bool TrungVaoThan(Point p) {
        for (int i = 0; i < DoDai; i++)
            if (A[i].x == p.x && A[i].y == p.y) return true;
        return false;
    }

    // TÍNH NĂNG VA CHẠM: Kiểm tra đầu đâm vào thân
    bool DauChamThan() {
        for (int i = 1; i < DoDai; i++)
            if (A[0].x == A[i].x && A[0].y == A[i].y) return true;
        return false;
    }
    // Mọc dài thêm một đoạn khi ăn mồi
    void MocDai() {
        if (DoDai < 100) { A[DoDai] = A[DoDai - 1]; DoDai++; }
    }
};

int main()
{
    CONRAN r;
    int Huong = 0;
    char t;

    while (1){
        if (kbhit()){
            t = getch();
            if (t=='a') Huong = 2;
            if (t=='w') Huong = 3;
            if (t=='d') Huong = 0;
            if (t=='x') Huong = 1;
        }
        system("cls");
        r.Ve();
        r.DiChuyen(Huong);
        Sleep(300);
    }

    return 0;
}
